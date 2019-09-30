#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QDir>
#include <QQueue>
//#include "serialcom.h"
#include "mavlink/mavlink_types.h"
#include "mav_bridge.h"

#ifdef __GNUC__
  #define PACKED_STRUCT( __Declaration__ ) __Declaration__ __attribute__((packed))
#else
  #define PACKED_STRUCT( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif


class FileManager : public QObject
{
    Q_OBJECT
public:


    explicit FileManager(QObject *parent = nullptr);
    /// @brief This is the fixed length portion of the protocol data.
    /// This needs to be packed, because it's typecasted from mavlink_file_transfer_protocol_t.payload, which starts
    /// at a 3 byte offset, causing an unaligned access to seq_number and offset
    PACKED_STRUCT(
    typedef struct _RequestHeader
        {
            uint16_t    seqNumber;      ///< sequence number for message
            uint8_t     session;        ///< Session id for read and write commands
            uint8_t     opcode;         ///< Command opcode
            uint8_t     size;           ///< Size of data
            uint8_t     req_opcode;     ///< Request opcode returned in kRspAck, kRspNak message
            uint8_t     burstComplete;  ///< Only used if req_opcode=kCmdBurstReadFile - 1: set of burst packets complete, 0: More burst packets coming.
            uint8_t     padding;        ///< 32 bit aligment padding
            uint32_t    offset;         ///< Offsets for List and Read commands
        }) RequestHeader;


    PACKED_STRUCT(
    typedef struct _Request
    {
        RequestHeader hdr;
        // We use a union here instead of just casting (uint32_t)&payload[0] to not break strict aliasing rules
        union {
            // The entire Request must fit into the payload member of the mavlink_file_transfer_protocol_t structure. We use as many leftover bytes
            // after we use up space for the RequestHeader for the data portion of the Request.
            uint8_t data[sizeof(((mavlink_file_transfer_protocol_t*)0)->payload) - sizeof(RequestHeader)];

            // File length returned by Open command
            uint32_t openFileLength;

            // Length of file chunk written by write command
            uint32_t writeFileLength;
        };
    }) Request;

    int session_id;
    static const int ackTimerTimeoutMsecs = 5000;
    static const int ackTimerMaxRetries = 10;

private:
    enum Opcode
    {
        kCmdNone,				///< ignored, always acked
        kCmdTerminateSession,	///< Terminates open Read session
        kCmdResetSessions,		///< Terminates all open Read sessions
        kCmdListDirectory,		///< List files in <path> from <offset>
        kCmdOpenFileRO,			///< Opens file at <path> for reading, returns <session>
        kCmdReadFile,			///< Reads <size> bytes from <offset> in <session>
        kCmdCreateFile,			///< Creates file at <path> for writing, returns <session>
        kCmdWriteFile,			///< Writes <size> bytes to <offset> in <session>
        kCmdRemoveFile,			///< Remove file at <path>
        kCmdCreateDirectory,	///< Creates directory at <path>
        kCmdRemoveDirectory,	///< Removes Directory at <path>, must be empty
        kCmdOpenFileWO,			///< Opens file at <path> for writing, returns <session>
        kCmdTruncateFile,		///< Truncate file at <path> to <offset> length
        kCmdRename,				///< Rename <path1> to <path2>
        kCmdCalcFileCRC32,		///< Calculate CRC32 for file at <path>
        kCmdBurstReadFile,      ///< Burst download session file

        kRspAck = 128,          ///< Ack response
        kRspNak,                ///< Nak response

        // Used for testing only, not part of protocol
        kCmdTestNoAck,          ///< ignored, ack not sent back, should timeout waiting for ack
    };

    enum OperationState
        {
            kCOIdle,		// not doing anything
            kCOAck,			// waiting for an Ack
            kCOList,		// waiting for List response
            kCOOpenRead,    // waiting for Open response followed by Read download
            kCOOpenBurst,   // waiting for Open response, followed by Burst download
            kCORead,		// waiting for Read response
            kCOBurst,		// waiting for Burst response
            kCOWrite,       // waiting for Write response
            kCOCreate,      // waiting for Create response
            KCORemove,      // waiting for remove response
            kCOCreateDir,   // waiting for Create Directory response
        };

    /// @brief Error codes returned in Nak response PayloadHeader.data[0].
    enum ErrorCode
    {
        kErrNone,
        kErrFail,                   ///< Unknown failure
        kErrFailErrno,              ///< errno sent back in PayloadHeader.data[1]
        kErrInvalidDataSize,		///< PayloadHeader.size is invalid
        kErrInvalidSession,         ///< Session is not currently open
        kErrNoSessionsAvailable,	///< All available Sessions in use
        kErrEOF,                    ///< Offset past end of file for List and Read commands
        kErrUnknownCommand,          ///< Unknown command opcode
        kErrFailFileExists,         ///< File exists already
        kErrFailFileProtected       ///< File is write protected
    };

    Opcode _command;
    OperationState  _currentOperation;              ///< Current operation of state machine

    struct MissingData {
        uint32_t offset;
        uint32_t size;
    };

    Request _lastOutgoingRequest;

    QTimer          _ackTimer;                      ///< Used to signal a timeout waiting for an ack
    int             _ackNumTries;                   ///< current number of tries

    uint8_t     _active_session;
    uint32_t    _downloadFileSize;          ///< Size of file being downloaded
    uint32_t    _downloadOffset;
    QByteArray  _readFileAccumulator;
    QDir        _readFileDownloadDir;
    QString     _readFileDownloadFilename;
    uint32_t    _missingDownloadedBytes;    ///< number of missing bytes for burst download
    bool        _downloadingMissingParts;   ///< true if we are currently downloading missing parts
    QQueue<MissingData> _missingData;       ///< missing chunks of downloaded file (for burst downloads)

    uint32_t    _writeOffset;               ///< current write offset
    uint32_t    _writeSize;                 ///< current write data size
    uint32_t    _writeFileSize;             ///< Size of file being uploaded
    QByteArray  _writeFileAccumulator;      ///< Holds file being uploaded
    uint8_t     _activeSession;             ///< currently active session, 0 for none

    void _clearAckTimeout(void);
    void _closeDownloadSession(bool success);
    void _downloadAckResponse(Request* readAck, bool readFile);
    void _fillRequestWithString(Request* request, const QString& str);
    void _handleDirList(Request* request);
    void _openAckResponse(Request* openAck);
    void _requestMissingData(void);
    void _sendResetCommand(void);
    void _sendRequest(Request* request);
    void _sendRequestNoAck(Request* request);
    void _setupAckTimeout(void);
    void _closeUploadSession(bool success);
    void _createAckResponse(Request* createAck);
    void _writeAckResponse(Request* writeAck);
    void _writeFileDatablock(void);

signals:
    void downloadProgress(qreal val);

public slots:
    //get the list of files and directory from UAS at specified path
    void listDirectory(const QString dirPath);

    //download the file from UAS at specified path
    void downloadFile(const QString filePath, const QDir& downloadDir, bool readFile);

    void receiveMessage(Request* request);

    void uploadPath(const QString& toPath, const QFileInfo& uploadFile);

    /// Delete the specified file
    void deleteFile(const QString& file);
private slots:
    void _ackTimeout(void);
};

#endif // FILEMANAGER_H
