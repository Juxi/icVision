/****************************************************************************
** Meta object code from reading C++ file 'robot.h'
**
** Created: Thu Aug 4 12:25:54 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/VirtualSkinLibrary/KinematicModel/LibKinematicModel/robot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KinematicModel__Robot[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      39,   22,   22,   22, 0x05,
      65,   61,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      99,   95,   22,   22, 0x0a,
     124,   22,   22,   22, 0x2a,
     170,  149,  144,   22, 0x0a,
     203,   95,   22,   22, 0x0a,
     229,   22,   22,   22, 0x2a,
     250,  149,  144,   22, 0x0a,
     296,  284,  144,   22, 0x0a,
     335,   22,   22,   22, 0x0a,
     356,  348,   22,   22, 0x0a,
     367,   22,   22,   22, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_KinematicModel__Robot[] = {
    "KinematicModel::Robot\0\0collisions(int)\0"
    "reflexCollisions(int)\0obs\0"
    "observation(RobotObservation)\0pos\0"
    "setNormalPosition(qreal)\0setNormalPosition()\0"
    "bool\0partNum,motorNum,pos\0"
    "setNormalPosition(int,int,qreal)\0"
    "setEncoderPosition(qreal)\0"
    "setEncoderPosition()\0"
    "setEncoderPosition(int,int,qreal)\0"
    "partNum,pos\0setEncoderPosition(int,QVector<qreal>)\0"
    "updatePose()\0verbose\0home(bool)\0home()\0"
};

const QMetaObject KinematicModel::Robot::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KinematicModel__Robot,
      qt_meta_data_KinematicModel__Robot, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KinematicModel::Robot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KinematicModel::Robot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KinematicModel::Robot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KinematicModel__Robot))
        return static_cast<void*>(const_cast< Robot*>(this));
    return QObject::qt_metacast(_clname);
}

int KinematicModel::Robot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: collisions((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: reflexCollisions((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: observation((*reinterpret_cast< RobotObservation(*)>(_a[1]))); break;
        case 3: setNormalPosition((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 4: setNormalPosition(); break;
        case 5: { bool _r = setNormalPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: setEncoderPosition((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 7: setEncoderPosition(); break;
        case 8: { bool _r = setEncoderPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: { bool _r = setEncoderPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QVector<qreal>(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: updatePose(); break;
        case 11: home((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: home(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void KinematicModel::Robot::collisions(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KinematicModel::Robot::reflexCollisions(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KinematicModel::Robot::observation(RobotObservation _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
