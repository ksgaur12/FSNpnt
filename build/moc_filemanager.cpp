/****************************************************************************
** Meta object code from reading C++ file 'filemanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../filemanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filemanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FileManager_t {
    QByteArrayData data[21];
    char stringdata0[203];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FileManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FileManager_t qt_meta_stringdata_FileManager = {
    {
QT_MOC_LITERAL(0, 0, 11), // "FileManager"
QT_MOC_LITERAL(1, 12, 16), // "downloadProgress"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 3), // "val"
QT_MOC_LITERAL(4, 34, 13), // "listDirectory"
QT_MOC_LITERAL(5, 48, 7), // "dirPath"
QT_MOC_LITERAL(6, 56, 12), // "downloadFile"
QT_MOC_LITERAL(7, 69, 8), // "filePath"
QT_MOC_LITERAL(8, 78, 4), // "QDir"
QT_MOC_LITERAL(9, 83, 11), // "downloadDir"
QT_MOC_LITERAL(10, 95, 8), // "readFile"
QT_MOC_LITERAL(11, 104, 14), // "receiveMessage"
QT_MOC_LITERAL(12, 119, 8), // "Request*"
QT_MOC_LITERAL(13, 128, 7), // "request"
QT_MOC_LITERAL(14, 136, 10), // "uploadPath"
QT_MOC_LITERAL(15, 147, 6), // "toPath"
QT_MOC_LITERAL(16, 154, 9), // "QFileInfo"
QT_MOC_LITERAL(17, 164, 10), // "uploadFile"
QT_MOC_LITERAL(18, 175, 10), // "deleteFile"
QT_MOC_LITERAL(19, 186, 4), // "file"
QT_MOC_LITERAL(20, 191, 11) // "_ackTimeout"

    },
    "FileManager\0downloadProgress\0\0val\0"
    "listDirectory\0dirPath\0downloadFile\0"
    "filePath\0QDir\0downloadDir\0readFile\0"
    "receiveMessage\0Request*\0request\0"
    "uploadPath\0toPath\0QFileInfo\0uploadFile\0"
    "deleteFile\0file\0_ackTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FileManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   52,    2, 0x0a /* Public */,
       6,    3,   55,    2, 0x0a /* Public */,
      11,    1,   62,    2, 0x0a /* Public */,
      14,    2,   65,    2, 0x0a /* Public */,
      18,    1,   70,    2, 0x0a /* Public */,
      20,    0,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QReal,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 8, QMetaType::Bool,    7,    9,   10,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 16,   15,   17,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void,

       0        // eod
};

void FileManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FileManager *_t = static_cast<FileManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->downloadProgress((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 1: _t->listDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->downloadFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QDir(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->receiveMessage((*reinterpret_cast< Request*(*)>(_a[1]))); break;
        case 4: _t->uploadPath((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QFileInfo(*)>(_a[2]))); break;
        case 5: _t->deleteFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->_ackTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QFileInfo >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FileManager::*)(qreal );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FileManager::downloadProgress)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FileManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FileManager.data,
      qt_meta_data_FileManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FileManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FileManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FileManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void FileManager::downloadProgress(qreal _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
