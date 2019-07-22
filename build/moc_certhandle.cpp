/****************************************************************************
** Meta object code from reading C++ file 'certhandle.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../certhandle.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'certhandle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CertHandle_t {
    QByteArrayData data[13];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CertHandle_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CertHandle_t qt_meta_stringdata_CertHandle = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CertHandle"
QT_MOC_LITERAL(1, 11, 17), // "cert_load_success"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 16), // "csr_load_success"
QT_MOC_LITERAL(4, 47, 20), // "key_generate_success"
QT_MOC_LITERAL(5, 68, 16), // "key_load_success"
QT_MOC_LITERAL(6, 85, 13), // "generate_keys"
QT_MOC_LITERAL(7, 99, 6), // "folder"
QT_MOC_LITERAL(8, 106, 13), // "generate_cert"
QT_MOC_LITERAL(9, 120, 9), // "load_cert"
QT_MOC_LITERAL(10, 130, 4), // "file"
QT_MOC_LITERAL(11, 135, 8), // "load_csr"
QT_MOC_LITERAL(12, 144, 9) // "load_keys"

    },
    "CertHandle\0cert_load_success\0\0"
    "csr_load_success\0key_generate_success\0"
    "key_load_success\0generate_keys\0folder\0"
    "generate_cert\0load_cert\0file\0load_csr\0"
    "load_keys"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CertHandle[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    0,   61,    2, 0x06 /* Public */,
       5,    0,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   63,    2, 0x0a /* Public */,
       8,    1,   66,    2, 0x0a /* Public */,
       9,    1,   69,    2, 0x0a /* Public */,
      11,    1,   72,    2, 0x0a /* Public */,
      12,    1,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void CertHandle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CertHandle *_t = static_cast<CertHandle *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cert_load_success(); break;
        case 1: _t->csr_load_success(); break;
        case 2: _t->key_generate_success(); break;
        case 3: _t->key_load_success(); break;
        case 4: _t->generate_keys((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->generate_cert((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->load_cert((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->load_csr((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->load_keys((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CertHandle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CertHandle::cert_load_success)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CertHandle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CertHandle::csr_load_success)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CertHandle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CertHandle::key_generate_success)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CertHandle::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CertHandle::key_load_success)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CertHandle::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CertHandle.data,
      qt_meta_data_CertHandle,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CertHandle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CertHandle::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CertHandle.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CertHandle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CertHandle::cert_load_success()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CertHandle::csr_load_success()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CertHandle::key_generate_success()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CertHandle::key_load_success()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
