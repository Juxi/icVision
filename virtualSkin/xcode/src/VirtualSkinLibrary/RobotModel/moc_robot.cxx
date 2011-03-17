/****************************************************************************
** Meta object code from reading C++ file 'robot.h'
**
** Created: Tue Mar 15 13:34:59 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/VirtualSkinLibrary/RobotModel/robot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RobotModel__Robot[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   19,   18,   18, 0x0a,
      49,   18,   18,   18, 0x2a,
      96,   75,   70,   18, 0x0a,
     142,  130,   70,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RobotModel__Robot[] = {
    "RobotModel::Robot\0\0pos\0setEncoderPosition(qreal)\0"
    "setEncoderPosition()\0bool\0"
    "partNum,motorNum,pos\0"
    "setEncoderPosition(int,int,qreal)\0"
    "partNum,pos\0setEncoderPosition(int,QVector<qreal>)\0"
};

const QMetaObject RobotModel::Robot::staticMetaObject = {
    { &RenderList::staticMetaObject, qt_meta_stringdata_RobotModel__Robot,
      qt_meta_data_RobotModel__Robot, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RobotModel::Robot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RobotModel::Robot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RobotModel::Robot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RobotModel__Robot))
        return static_cast<void*>(const_cast< Robot*>(this));
    return RenderList::qt_metacast(_clname);
}

int RobotModel::Robot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RenderList::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setEncoderPosition((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 1: setEncoderPosition(); break;
        case 2: { bool _r = setEncoderPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< qreal(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = setEncoderPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QVector<qreal>(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
