/****************************************************************************
** Meta object code from reading C++ file 'model.h'
**
** Created: Tue Mar 15 16:55:21 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/VirtualSkinLibrary/RobotModel/model.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RobotModel__Model[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      35,   18,   18,   18, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_RobotModel__Model[] = {
    "RobotModel::Model\0\0newStateReady()\0"
    "collision()\0"
};

const QMetaObject RobotModel::Model::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_RobotModel__Model,
      qt_meta_data_RobotModel__Model, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RobotModel::Model::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RobotModel::Model::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RobotModel::Model::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RobotModel__Model))
        return static_cast<void*>(const_cast< Model*>(this));
    return QThread::qt_metacast(_clname);
}

int RobotModel::Model::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newStateReady(); break;
        case 1: collision(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void RobotModel::Model::newStateReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void RobotModel::Model::collision()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
