/****************************************************************************
** Meta object code from reading C++ file 'robotFilter.h'
**
** Created: Thu Jul 28 13:27:22 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/VirtualSkinLibrary/robotFilter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robotFilter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VirtualSkin__RobotFilter[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      39,   25,   25,   25, 0x0a,
      56,   25,   25,   25, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_VirtualSkin__RobotFilter[] = {
    "VirtualSkin::RobotFilter\0\0reflexDone()\0"
    "takeControl(int)\0openFilter()\0"
};

const QMetaObject VirtualSkin::RobotFilter::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_VirtualSkin__RobotFilter,
      qt_meta_data_VirtualSkin__RobotFilter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VirtualSkin::RobotFilter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VirtualSkin::RobotFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VirtualSkin::RobotFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VirtualSkin__RobotFilter))
        return static_cast<void*>(const_cast< RobotFilter*>(this));
    return QThread::qt_metacast(_clname);
}

int VirtualSkin::RobotFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reflexDone(); break;
        case 1: takeControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: openFilter(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void VirtualSkin::RobotFilter::reflexDone()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE