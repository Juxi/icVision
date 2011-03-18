/****************************************************************************
** Meta object code from reading C++ file 'responseObserver.h'
**
** Created: Tue Mar 15 13:35:37 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/VirtualSkinLibrary/responseObserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'responseObserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VirtualSkin__ResponseObserver[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      34,   31,   30,   30, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_VirtualSkin__ResponseObserver[] = {
    "VirtualSkin::ResponseObserver\0\0,,\0"
    "setPosition(int,int,qreal)\0"
};

const QMetaObject VirtualSkin::ResponseObserver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VirtualSkin__ResponseObserver,
      qt_meta_data_VirtualSkin__ResponseObserver, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VirtualSkin::ResponseObserver::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VirtualSkin::ResponseObserver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VirtualSkin::ResponseObserver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VirtualSkin__ResponseObserver))
        return static_cast<void*>(const_cast< ResponseObserver*>(this));
    if (!strcmp(_clname, "yarp::os::IObserver"))
        return static_cast< yarp::os::IObserver*>(const_cast< ResponseObserver*>(this));
    return QObject::qt_metacast(_clname);
}

int VirtualSkin::ResponseObserver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void VirtualSkin::ResponseObserver::setPosition(int _t1, int _t2, qreal _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
