#include "filemanager.h"
#include "filemodel.h"

#include <QDebug>

extern Serialcom comm_link;
extern filemodel fc_dir_model;

FileManager::FileManager(QObject *parent)
    : QObject(parent)
    , _currentOperation(kCOIdle)
    , _active_session(0)
    , _missingDownloadedBytes(0)
    , _downloadingMissingParts(false)
{
    connect(&_ackTimer, &QTimer::timeout, this, &FileManager::_ackTimeout);

    _lastOutgoingRequest.hdr.seqNumber = 0;
}

void FileManager::_clearAckTimeout(void)
{
    _ackTimer.stop();
}

void FileManager::_closeDownloadSession(bool success)
{
    _currentOperation = kCOIdle;

    if(success){
        if(_missingDownloadedBytes > 0 || (uint32_t)_readFileAccumulator.length() < _downloadFileSize){
            _downloadingMissingParts = true;
            _requestMissingData();
            return;
        }
        QString downloadFilePath = _readFileDownloadDir.absoluteFilePath(_readFileDownloadFilename);

        QFile file(downloadFilePath);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            qDebug() << "unable to open the file";
            return;
        }

        qint64 bytesWritten = file.write((const char *)_readFileAccumulator, _readFileAccumulator.length());
        if (bytesWritten != _readFileAccumulator.length()) {
            file.close();
            qDebug() << "unable to write data to local file";
            return;
        }
        file.close();
    }

    _readFileAccumulator.clear();

    // Close the open session
    _sendResetCommand();

}

void FileManager::_downloadAckResponse(Request* readAck, bool readFile)
{
    if(readAck->hdr.session != _active_session){
        _closeDownloadSession(false);
        return;
    }

    if(readAck->hdr.offset != _downloadOffset){
        if(readFile){
            _closeDownloadSession(false);
            return;
        }
        else{
            if(readAck->hdr.offset < _downloadOffset){
                _setupAckTimeout();
                return;
            }

            MissingData missingData;
            missingData.offset = _downloadOffset;
            missingData.size = readAck->hdr.offset - _downloadOffset;
            _missingData.push_back(missingData);
            _missingDownloadedBytes += readAck->hdr.offset - _downloadOffset;
            _downloadOffset = readAck->hdr.offset;
            _readFileAccumulator.resize(_downloadOffset);
        }
    }

    if (_downloadingMissingParts) {
        //        Q_ASSERT(_missingData.head().offset == _downloadOffset);
        _missingDownloadedBytes -= readAck->hdr.size;
        _readFileAccumulator.replace(_downloadOffset, readAck->hdr.size, (const char*)readAck->data, readAck->hdr.size);
        if (_missingData.head().size <= readAck->hdr.size) {
            _missingData.pop_front();
        } else {
            _missingData.head().size -= readAck->hdr.size;
            _missingData.head().offset += readAck->hdr.size;
        }
    } else {
        _downloadOffset += readAck->hdr.size;
        _readFileAccumulator.append((const char*)readAck->data, readAck->hdr.size);
    }

    if (_downloadFileSize != 0) {
        emit downloadProgress(100 * ((float)(_readFileAccumulator.length() - _missingDownloadedBytes) / (float)_downloadFileSize));
    }

    if (_downloadingMissingParts) {
        _requestMissingData();
    } else if (readFile || readAck->hdr.burstComplete) {
        // Possibly still more data to read, send next read request

        Request request;
        request.hdr.session = _active_session;
        request.hdr.opcode = readFile ? kCmdReadFile : kCmdBurstReadFile;
        request.hdr.offset = _downloadOffset;
        request.hdr.size = 0;

        _sendRequest(&request);
    } else if (!readFile) {
        // Streaming, so next ack should come automatically
        qDebug() << "read file false";
        _setupAckTimeout();
    }
}

void FileManager::_fillRequestWithString(Request* request, const QString& str)
{
    strncpy((char *)&request->data[0], str.toStdString().c_str(), sizeof(request->data));
    request->hdr.size = static_cast<uint8_t>(strnlen((const char *)&request->data[0], sizeof(request->data)));
}

void FileManager::_openAckResponse(Request* openAck)
{
    //    Q_ASSERT(_currentOperation == kCOOpenRead || _currentOperation == kCOOpenBurst);
    _currentOperation = _currentOperation == kCOOpenRead ? kCORead : kCOBurst;
    _active_session = openAck->hdr.session;
    _downloadFileSize = openAck->openFileLength;

    //start the sequence of read commands
    _downloadOffset = 0;
    _readFileAccumulator.clear();
    _missingDownloadedBytes = 0;
    _downloadingMissingParts = false;
    _missingData.clear();

    Request request;
    request.hdr.session = _active_session;
    request.hdr.opcode = kCORead;
    request.hdr.offset = _downloadOffset;
    request.hdr.size = sizeof(request.data);

    _sendRequest(&request);

}
void FileManager::_handleDirList(Request *request)
{
    bool start = true;
    QString str;
    for(int i=0; i<request->hdr.size; i++){
        char data = char(request->data[i]);
        if(data == '\t'){
            start = false;
            str.remove(0, 1); //remove the first char indicating file or directory
            fc_dir_model.append_item(str);
            str.clear();
        }
        else if(data == NULL){
            start = true;
            if(str.isEmpty())
                continue;
            str.remove(0, 1); //remove the first char indicating file or directory
            fc_dir_model.append_item(str);
            str.clear();
        }
        else if(start){
            str.append(char(request->data[i]));
        }
    }

    _currentOperation = kCOIdle;
}

void FileManager::_sendResetCommand(void)
{
    Request request;
    request.hdr.opcode = kCmdResetSessions;
    request.hdr.size = 0;
    qDebug() << "send reset command";
    _sendRequest(&request);
}

void  FileManager::_requestMissingData()
{
    if(_missingData.empty()){
        _downloadingMissingParts = false;
        _missingDownloadedBytes = 0;
        if((uint32_t)_readFileAccumulator.length() != _downloadFileSize){
            _downloadOffset = _readFileAccumulator.length();
        }
        else{
            _closeDownloadSession(true);
            return;
        }
    }
    else{
        _downloadOffset = _missingData.head().offset;
    }

    Request request;
    _currentOperation = kCORead;
    request.hdr.session = _active_session;
    request.hdr.opcode = kCmdReadFile;
    request.hdr.offset = _downloadOffset;
    request.hdr.size = sizeof(request.data);

    _sendRequest(&request);
}


void FileManager::_sendRequestNoAck(Request* request)
{

    mavlink_msg_file_transfer_protocol_send(comm_link.mav_ch,
                                            0,
                                            comm_link.systemId,
                                            0,
                                            (uint8_t*)request);
}

void FileManager::_sendRequest(Request* request)
{
    _setupAckTimeout();

    request->hdr.seqNumber = ++_lastOutgoingRequest.hdr.seqNumber;
    qDebug() << "send request : " << request->hdr.seqNumber;
    // store the current request
    if (request->hdr.size <= sizeof(request->data)) {
        memcpy(&_lastOutgoingRequest, request, sizeof(RequestHeader) + request->hdr.size);
    } else {
        qDebug() << "request length too long:" << request->hdr.size;
    }
    _sendRequestNoAck(request);
}

void FileManager::_setupAckTimeout(void)
{
    _ackNumTries = 0;
    _ackTimer.setSingleShot(false);
    _ackTimer.start(ackTimerTimeoutMsecs);
}

void FileManager::listDirectory(const QString dirPath)
{
    Request request;

    request.hdr.session = 0;
    _command = kCmdListDirectory;
    request.hdr.opcode = kCmdListDirectory;
    request.hdr.offset = 0;
    request.hdr.size = 0;

    _fillRequestWithString(&request, dirPath);
    _sendRequest(&request);
}

void FileManager::uploadPath(const QString& toPath, const QFileInfo& uploadFile)
{
    qDebug() << "here";
    if(_currentOperation != kCOIdle){
        qDebug() << tr("UAS File manager busy. Try again later");
        return;
    }

    if (toPath.isEmpty()) {
        return;
    }

    if (!uploadFile.isReadable()){
        qDebug() << (tr("File (%1) is not readable for upload").arg(uploadFile.path()));
        return;
    }

    QFile file(uploadFile.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << (tr("Unable to open local file for upload (%1)").arg(uploadFile.absoluteFilePath()));
            return;
        }

    _writeFileAccumulator = file.readAll();
    _writeFileSize = _writeFileAccumulator.size();

    file.close();

    if (_writeFileAccumulator.size() == 0) {
        qDebug() << (tr("Unable to read data from local file (%1)").arg(uploadFile.absoluteFilePath()));
        return;
    }

    _currentOperation = kCOCreate;

    Request request;
    request.hdr.session = 0;
    request.hdr.opcode = kCmdCreateFile;
    request.hdr.offset = 0;
    request.hdr.size = 0;

    qDebug() << uploadFile.fileName();
    _fillRequestWithString(&request, toPath + "/" + "permissionArtifact.xml");
    _sendRequest(&request);
}

void FileManager::deleteFile(const QString& file){
    if(_currentOperation != kCOIdle){
        qDebug() << (tr("UAS File manager busy. Try again later"));
        return;
    }

    _currentOperation = KCORemove;

    Request request;
    request.hdr.session = 0;
    request.hdr.opcode = kCmdRemoveFile;
    request.hdr.offset = 0;
    request.hdr.size = 0;

    _fillRequestWithString(&request, file);
    _sendRequest(&request);
}


void FileManager::downloadFile(const QString filePath, const QDir& downloadDir, bool readFile)
{
    if (filePath.isEmpty()) {
        return;
    }
    qDebug() << downloadDir.absolutePath();
    _readFileDownloadDir.setPath(downloadDir.absolutePath());

    // We need to strip off the file name from the fully qualified path. We can't use the usual QDir
    // routines because this path does not exist locally.
    int i;
    for (i=filePath.size()-1; i>=0; i--) {
        if (filePath[i] == '/') {
            break;
        }
    }
    i++; // move past slash
    _readFileDownloadFilename = filePath.right(filePath.size() - i);

    _currentOperation = readFile ? kCOOpenRead : kCOOpenBurst;
    Request request;

    request.hdr.session = 0;
    request.hdr.opcode = kCmdOpenFileRO;
    _command = kCmdOpenFileRO;
    request.hdr.offset = 0;
    request.hdr.size = 0;

    _fillRequestWithString(&request, filePath);
    _sendRequest(&request);
}

/// Closes out an upload session doing cleanup.
///     @param success true: successful upload completion, false: error during download
void FileManager::_closeUploadSession(bool success)
{
    _currentOperation = kCOIdle;
    _writeFileAccumulator.clear();
    _writeFileSize = 0;

    if (success) {
        qDebug() << "command complete";
    }

    // Close the open session
    _sendResetCommand();
}

/// @brief Respond to the Ack associated with the create command.
void FileManager::_createAckResponse(Request* createAck)
{
    qDebug() << "_createAckResponse";

    _currentOperation = kCOWrite;
    _activeSession = createAck->hdr.session;

    // Start the sequence of write commands from the beginning of the file

    _writeOffset = 0;
    _writeSize = 0;

    _writeFileDatablock();
}

/// @brief Send next write file data block.
void FileManager::_writeFileDatablock(void)
{
    if (_writeOffset + _writeSize >= _writeFileSize){
        _closeUploadSession(true /* success */);
        return;
    }

    _writeOffset += _writeSize;

    Request request;
    request.hdr.session = _activeSession;
    request.hdr.opcode = kCmdWriteFile;
    request.hdr.offset = _writeOffset;

    if(_writeFileSize -_writeOffset > sizeof(request.data) )
        _writeSize = sizeof(request.data);
    else
        _writeSize = _writeFileSize - _writeOffset;

    request.hdr.size = _writeSize;

    memcpy(request.data, &_writeFileAccumulator.data()[_writeOffset], _writeSize);

    _sendRequest(&request);
}

/// @brief Respond to the Ack associated with the write command.
void FileManager::_writeAckResponse(Request* writeAck)
{
    emit uploadProgress(100 * ((float)(_writeOffset + _writeSize)/ (float)_writeFileSize));

    if(_writeOffset + _writeSize >= _writeFileSize){
        _closeUploadSession(true /* success */);
        return;
    }

    if (writeAck->hdr.session != _activeSession) {
        _closeUploadSession(false /* failure */);
        qDebug() << (tr("Write: Incorrect session returned"));
        return;
    }

    if (writeAck->hdr.offset != _writeOffset) {
        _closeUploadSession(false /* failure */);
        qDebug() << (tr("Write: Offset returned (%1) differs from offset requested (%2)").arg(writeAck->hdr.offset).arg(_writeOffset));
        return;
    }

    if (writeAck->hdr.size != sizeof(uint32_t)) {
        _closeUploadSession(false /* failure */);
        qDebug() << (tr("Write: Returned invalid size of write size data"));
        return;
    }


    if( writeAck->writeFileLength !=_writeSize) {
        _closeUploadSession(false /* failure */);
        qDebug() << (tr("Write: Size returned (%1) differs from size requested (%2)").arg(writeAck->writeFileLength).arg(_writeSize));
        return;
    }

    _writeFileDatablock();
}

void FileManager::receiveMessage(Request *request)
{
    uint16_t incomingSeqNumber = request->hdr.seqNumber;
    qDebug() << "session no: " << request->hdr.seqNumber;

    // Make sure we have a good sequence number
    uint16_t expectedSeqNumber = _lastOutgoingRequest.hdr.seqNumber + 1;

    // ignore old/reordered packets (handle wrap-around properly)
    if ((uint16_t)((expectedSeqNumber - 1) - incomingSeqNumber) < (std::numeric_limits<uint16_t>::max()/2)) {
        qDebug() << "Received old packet: expected seq:" << expectedSeqNumber << "got:" << incomingSeqNumber;
        return;
    }

    _clearAckTimeout();

    if(incomingSeqNumber != expectedSeqNumber){
        switch(_currentOperation){
        case kCORead:
            _closeDownloadSession(false);
            break;
        case kCOWrite:
            _closeUploadSession(false /* failure */);
            break;

        case kCOOpenRead:
        case kCOOpenBurst:
        case kCOCreate:
            // We could have an open session hanging around
            _currentOperation = kCOIdle;
            _sendResetCommand();
            break;

        case KCORemove:
            qDebug() << "file removed";

        default:
            // Don't need to do anything special
            _currentOperation = kCOIdle;
            break;
        }
    }

    // Move past the incoming sequence number for next request
    _lastOutgoingRequest.hdr.seqNumber = incomingSeqNumber;

    if (request->hdr.opcode == kRspAck) {
        switch(request->hdr.req_opcode){
        case kCmdOpenFileRO:
            _openAckResponse(request);
            break;
        case kCmdListDirectory:
            _handleDirList(request);
            break;
        case kCmdReadFile:
            _downloadAckResponse(request, true);
            break;
        case kCmdCreateFile:
            _createAckResponse(request);
            break;

        case kCmdWriteFile:
            _writeAckResponse(request);
            break;
        default:
            if(_currentOperation == KCORemove){
                emit fileRemoved();
            }
            _currentOperation = kCOIdle;
            break;
        }
    }
    else if (request->hdr.opcode == kRspNak) {
        uint8_t errorCode = request->data[0];

        _currentOperation = kCOIdle;
        qDebug() << "error code" <<errorCode ;

        if ((request->hdr.req_opcode == kCmdReadFile || request->hdr.req_opcode == kCmdBurstReadFile) && errorCode == kErrEOF) {
            // This is not an error, just the end of the download loop
            _closeDownloadSession(true /* success */);
            return;
        }
        else {
            // Generic Nak handling
            if (request->hdr.req_opcode == kCmdReadFile || request->hdr.req_opcode == kCmdBurstReadFile) {
                // Nak error during download loop, download failed
                _closeDownloadSession(false /* failure */);
            }
        }
    }
}

void FileManager::_ackTimeout(void)
{
    if (++_ackNumTries <= ackTimerMaxRetries) {
        if (_currentOperation == kCOBurst) {
            // for burst downloads try to initiate a new burst
            Request request;
            request.hdr.session = _active_session;
            request.hdr.opcode = kCmdBurstReadFile;
            request.hdr.offset = _downloadOffset;
            request.hdr.size = 0;
            request.hdr.seqNumber = ++_lastOutgoingRequest.hdr.seqNumber;

            _sendRequestNoAck(&request);
        } else {
            qDebug() << "send last outgoing request again";
            _sendRequestNoAck(&_lastOutgoingRequest);
        }
        return;
    }

    _clearAckTimeout();

    switch (_currentOperation) {
        case kCORead:
        case kCOBurst:
            qDebug() << "try to close download session";
            _closeDownloadSession(false /* failure */);
            break;

        case kCOOpenRead:
        case kCOOpenBurst:
            _currentOperation = kCOIdle;
            qDebug() << "send a reset command";
            _sendResetCommand();
            break;
        default:
        {
            OperationState currentOperation = _currentOperation;
            _currentOperation = kCOIdle;
        }
            break;
    }
}
