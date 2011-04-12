/****************************************************************************
** Meta object code from reading C++ file 'renderList.h'
**
** Created: Tue Apr 12 14:53:03 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/VirtualSkinLibrary/RobotModel/renderList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'renderList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RobotModel__RenderList[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      29,   24,   23,   23, 0x05,
      74,   70,   23,   23, 0x05,
      99,   23,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
     114,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RobotModel__RenderList[] = {
    "RobotModel::RenderList\0\0list\0"
    "appendedObject(RobotModel::DisplayList*)\0"
    "idx\0outdatedDisplayList(int)\0"
    "changedState()\0callLists()\0"
};

const QMetaObject RobotModel::RenderList::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RobotModel__RenderList,
      qt_meta_data_RobotModel__RenderList, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RobotModel::RenderList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RobotModel::RenderList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RobotModel::RenderList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RobotModel__RenderList))
        return static_cast<void*>(const_cast< RenderList*>(this));
    return QObject::qt_metacast(_clname);
}

int RobotModel::RenderList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: appendedObject((*reinterpret_cast< RobotModel::DisplayList*(*)>(_a[1]))); break;
        case 1: outdatedDisplayList((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: changedState(); break;
        case 3: callLists(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void RobotModel::RenderList::appendedObject(RobotModel::DisplayList * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RobotModel::RenderList::outdatedDisplayList(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RobotModel::RenderList::changedState()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
