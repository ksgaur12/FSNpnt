/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
///     @brief PX4 Firmware Upgrade operations which occur on a separate thread.
///     @author Don Gagne <don@thegagnes.com>

#include "px4firmwareupgradethreadcontroller.h"
#include "bootloader.h"

#include <QTimer>
#include <QDebug>
#include <QSerialPort>
#include "QGC.h"

PX4FirmwareUpgradeThreadWorker::PX4FirmwareUpgradeThreadWorker(PX4FirmwareUpgradeThreadController* controller) :
    _controller(controller),
    _bootloader(NULL),
    _bootloaderPort(NULL),
    _timerRetry(NULL),
    _foundBoard(false),
    _findBoardFirstAttempt(true),
    _foundBootloader(false)
{
    Q_ASSERT(_controller);

    connect(_controller, &PX4FirmwareUpgradeThreadController::_initThreadWorker,            this, &PX4FirmwareUpgradeThreadWorker::_init);
    connect(_controller, &PX4FirmwareUpgradeThreadController::_startFindBoardLoopOnThread,  this, &PX4FirmwareUpgradeThreadWorker::_startFindBoardLoop);
    connect(_controller, &PX4FirmwareUpgradeThreadController::_flashOnThread,               this, &PX4FirmwareUpgradeThreadWorker::_flash);
    connect(_controller, &PX4FirmwareUpgradeThreadController::_rebootOnThread,              this, &PX4FirmwareUpgradeThreadWorker::_reboot);
    connect(_controller, &PX4FirmwareUpgradeThreadController::_cancel,                      this, &PX4FirmwareUpgradeThreadWorker::_cancel);
}

PX4FirmwareUpgradeThreadWorker::~PX4FirmwareUpgradeThreadWorker()
{
    if (_bootloaderPort) {
        // deleteLater so delete happens on correct thread
        _bootloaderPort->deleteLater();
    }
}

/// @brief Initializes the PX4FirmwareUpgradeThreadWorker with the various child objects which must be created
///         on the worker thread.
void PX4FirmwareUpgradeThreadWorker::_init(void)
{
    // We create the timers here so that they are on the right thread

    Q_ASSERT(_timerRetry == NULL);
    _timerRetry = new QTimer(this);
    Q_CHECK_PTR(_timerRetry);
    _timerRetry->setSingleShot(true);
    _timerRetry->setInterval(_retryTimeout);
    connect(_timerRetry, &QTimer::timeout, this, &PX4FirmwareUpgradeThreadWorker::_findBoardOnce);

    Q_ASSERT(_bootloader == NULL);
    _bootloader = new Bootloader(this);
    connect(_bootloader, &Bootloader::updateProgress, this, &PX4FirmwareUpgradeThreadWorker::_updateProgress);
}

void PX4FirmwareUpgradeThreadWorker::_cancel(void)
{
    if (_bootloaderPort) {
        _bootloaderPort->close();
        _bootloaderPort->deleteLater();
        _bootloaderPort = NULL;
    }
}

void PX4FirmwareUpgradeThreadWorker::_startFindBoardLoop(void)
{
    _foundBoard = false;
    _findBoardFirstAttempt = true;
    _findBoardOnce();
}

void PX4FirmwareUpgradeThreadWorker::_findBoardOnce(void)
{

    QGCSerialPortInfo               portInfo;
    QGCSerialPortInfo::BoardType_t  boardType;
    QString                         boardName;

    if (_findBoardFromPorts(portInfo, boardType, boardName)) {
        if (!_foundBoard) {
            _foundBoard = true;
            _foundBoardPortInfo = portInfo;
            emit foundBoard(_findBoardFirstAttempt, portInfo, boardType, boardName);
            if (!_findBoardFirstAttempt) {
                if (boardType == QGCSerialPortInfo::BoardTypeSiKRadio) {
                    _3drRadioForceBootloader(portInfo);
                    return;
                } else {
                    _findBootloader(portInfo, false /* radio mode */, true /* errorOnNotFound */);
                    return;
                }
            }
        }
        if(!_foundBootloader){
            _findBootloader(portInfo, false, true);
        }
    } else {
        if (_foundBoard) {
            _foundBoard = false;
            emit boardGone();
        } else if (_findBoardFirstAttempt) {
            emit noBoardFound();
        }
    }
    if(_foundBootloader){
        return;
    }

    qDebug() <<"here";

    _findBoardFirstAttempt = false;
    _timerRetry->start();
}

bool PX4FirmwareUpgradeThreadWorker::_findBoardFromPorts(QGCSerialPortInfo& portInfo, QGCSerialPortInfo::BoardType_t& boardType, QString& boardName)
{
    for (QGCSerialPortInfo info: QGCSerialPortInfo::availablePorts()) {
        info.getBoardInfo(boardType, boardName);
        if (info.canFlash()) {
            portInfo = info;
            return true;
        }
    }

    return false;
}

void PX4FirmwareUpgradeThreadWorker::_3drRadioForceBootloader(const QGCSerialPortInfo& portInfo)
{
    // First make sure we can't get the bootloader

    if (_findBootloader(portInfo, true /* radio Mode */, false /* errorOnNotFound */)) {
        return;
    }

    // Couldn't find the bootloader. We'll need to reboot the radio into bootloader.

    QSerialPort port(portInfo);

    port.setBaudRate(QSerialPort::Baud57600);

    emit status(tr("Putting radio into command mode"));

    // Wait a little while for the USB port to initialize. 3DR Radio boot is really slow.
    QGC::SLEEP::msleep(2000);
    port.open(QIODevice::ReadWrite);

    if (!port.isOpen()) {
        emit error(tr("Unable to open port: %1 error: %2").arg(portInfo.systemLocation()).arg(port.errorString()));
        return;
    }

    // Put radio into command mode
    QGC::SLEEP::msleep(2000);
    port.write("+++", 3);
    if (!port.waitForReadyRead(1500)) {
        emit error(tr("Unable to put radio into command mode"));
        return;
    }
    QByteArray bytes = port.readAll();
    if (!bytes.contains("OK")) {
        emit error(tr("Unable to put radio into command mode"));
        return;
    }

    emit status(tr("Rebooting radio to bootloader"));

    port.write("AT&UPDATE\r\n");
    if (!port.waitForBytesWritten(1500)) {
        emit error(tr("Unable to reboot radio (bytes written)"));
        return;
    }
    if (!port.waitForReadyRead(1500)) {
        emit error(tr("Unable to reboot radio (ready read)"));
        return;
    }
    port.close();
    QGC::SLEEP::msleep(2000);

    // The bootloader should be waiting for us now

    _findBootloader(portInfo, true /* radio mode */, true /* errorOnNotFound */);
}

bool PX4FirmwareUpgradeThreadWorker::_findBootloader(const QGCSerialPortInfo& portInfo, bool radioMode, bool errorOnNotFound)
{
    uint32_t bootloaderVersion = 0;
    uint32_t boardID;
    uint32_t flashSize = 0;

    _bootloaderPort = new QSerialPort();
    if (radioMode) {
        _bootloaderPort->setBaudRate(QSerialPort::Baud115200);
    }

    // Wait a little while for the USB port to initialize.
    bool openFailed = true;
    for (int i=0; i<10; i++) {
        if (_bootloader->open(_bootloaderPort, portInfo.systemLocation())) {
            openFailed = false;
            break;
        } else {
            QGC::SLEEP::msleep(100);
        }
    }

    if (radioMode) {
        QGC::SLEEP::msleep(2000);
    }

    if (openFailed) {
        if (errorOnNotFound) {
            emit error(_bootloader->errorString());
        }
        _bootloaderPort->deleteLater();
        _bootloaderPort = NULL;
        return false;
    }

    if (_bootloader->sync(_bootloaderPort)) {
        bool success;

        if (radioMode) {
            success = _bootloader->get3DRRadioBoardId(_bootloaderPort, boardID);
        } else {
            success = _bootloader->getPX4BoardInfo(_bootloaderPort, bootloaderVersion, boardID, flashSize);
            qDebug() <<"here";
        }
        if (success) {
            _foundBootloader = true;
            emit foundBootloader(bootloaderVersion, boardID, flashSize);
            return true;
        }
    }

    _bootloaderPort->close();
    _bootloaderPort->deleteLater();
    _bootloaderPort = NULL;

    if (errorOnNotFound) {
        emit error(_bootloader->errorString());
    }

    return false;
}

void PX4FirmwareUpgradeThreadWorker::_reboot(void)
{
    if (_bootloaderPort) {
        if (_bootloaderPort->isOpen()) {
            _bootloader->reboot(_bootloaderPort);
        }
        _bootloaderPort->deleteLater();
        _bootloaderPort = NULL;
    }
}

void PX4FirmwareUpgradeThreadWorker::_flash(void)
{
    if (_erase()) {
        emit status(tr("Programming new version..."));
//        if (_bootloader->program(_bootloaderPort, _controller->image())) {
//            emit status("Program complete");
        }
        //else {
//            _bootloaderPort->deleteLater();
//            _bootloaderPort = NULL;
//            emit error(_bootloader->errorString());
//            return;
//        }

//        emit status(tr("Verifying program..."));

//        if (_bootloader->verify(_bootloaderPort, _controller->image())) {
//            emit status(tr("Verify complete"));
//        } else {
//            emit error(_bootloader->errorString());
//            return;
//        }
//    }

//    emit _reboot();

    emit flashComplete();
}

bool PX4FirmwareUpgradeThreadWorker::_erase(void)
{

    emit eraseStarted();
    emit status(tr("Erasing previous program..."));

    if (_bootloader->erase(_bootloaderPort)) {
        emit status(tr("Erase complete"));
        emit eraseComplete();
        return true;
    } else {

        emit error(_bootloader->errorString());
        return false;
    }
}

PX4FirmwareUpgradeThreadController::PX4FirmwareUpgradeThreadController(QObject* parent) :
    QObject(parent)
{
    _worker = new PX4FirmwareUpgradeThreadWorker(this);
    Q_CHECK_PTR(_worker);

    _workerThread = new QThread(this);
    Q_CHECK_PTR(_workerThread);
    _worker->moveToThread(_workerThread);

    connect(_worker, &PX4FirmwareUpgradeThreadWorker::updateProgress,       this, &PX4FirmwareUpgradeThreadController::_updateProgress);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::foundBoard,           this, &PX4FirmwareUpgradeThreadController::_foundBoard);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::noBoardFound,         this, &PX4FirmwareUpgradeThreadController::_noBoardFound);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::boardGone,            this, &PX4FirmwareUpgradeThreadController::_boardGone);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::foundBootloader,      this, &PX4FirmwareUpgradeThreadController::_foundBootloader);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::bootloaderSyncFailed, this, &PX4FirmwareUpgradeThreadController::_bootloaderSyncFailed);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::error,                this, &PX4FirmwareUpgradeThreadController::_error);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::status,               this, &PX4FirmwareUpgradeThreadController::_status);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::eraseStarted,         this, &PX4FirmwareUpgradeThreadController::_eraseStarted);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::eraseComplete,        this, &PX4FirmwareUpgradeThreadController::_eraseComplete);
    connect(_worker, &PX4FirmwareUpgradeThreadWorker::flashComplete,        this, &PX4FirmwareUpgradeThreadController::_flashComplete);

    _workerThread->start();

    emit _initThreadWorker();
}

PX4FirmwareUpgradeThreadController::~PX4FirmwareUpgradeThreadController()
{
    _workerThread->quit();
    _workerThread->wait();

    delete _workerThread;
}

void PX4FirmwareUpgradeThreadController::startFindBoardLoop(void)
{
    emit _startFindBoardLoopOnThread();
}

void PX4FirmwareUpgradeThreadController::cancel(void)
{
    emit _cancel();
}

void PX4FirmwareUpgradeThreadController::flash(const FirmwareImage* image)
{
    _image = image;
    emit _flashOnThread();
}
