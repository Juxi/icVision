/****************************************************************************
** Meta object code from reading C++ file 'model.h'
**
** Created: Fri Jul 22 13:16:25 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/VirtualSkinLibrary/KinematicModel/LibKinematicModel/model.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KinematicModel__Model[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,
      56,   22,   22,   22, 0x05,
     100,   89,   22,   22, 0x05,
     119,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
     141,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KinematicModel__Model[] = {
    "KinematicModel::Model\0\0"
    "addedPrimitive(PrimitiveObject*)\0"
    "removedPrimitive(GL_DisplayList)\0"
    "collisions\0computedState(int)\0"
    "encounteredObstacle()\0renderWorld()\0"
};

const QMetaObject KinematicModel::Model::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_KinematicModel__Model,
      qt_meta_data_KinematicModel__Model, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KinematicModel::Model::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KinematicModel::Model::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KinematicModel::Model::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KinematicModel__Model))
        return static_cast<void*>(const_cast< Model*>(this));
    return QThread::qt_metacast(_clname);
}

int KinematicModel::Model::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addedPrimitive((*reinterpret_cast< PrimitiveObject*(*)>(_a[1]))); break;
        case 1: removedPrimitive((*reinterpret_cast< GL_DisplayList(*)>(_a[1]))); break;
        case 2: computedState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: encounteredObstacle(); break;
        case 4: renderWorld(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void KinematicModel::Model::addedPrimitive(PrimitiveObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KinematicModel::Model::removedPrimitive(GL_DisplayList _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KinematicModel::Model::computedState(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KinematicModel::Model::encounteredObstacle()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
