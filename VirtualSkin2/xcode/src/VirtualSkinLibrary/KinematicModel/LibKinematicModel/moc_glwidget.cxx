/****************************************************************************
** Meta object code from reading C++ file 'glwidget.h'
**
** Created: Wed Aug 3 14:18:35 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/VirtualSkinLibrary/KinematicModel/LibKinematicModel/glwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KinematicModel__GLWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,
      46,   40,   25,   25, 0x05,
      68,   40,   25,   25, 0x05,
      90,   40,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
     112,   25,   25,   25, 0x0a,
     145,   25,   25,   25, 0x0a,
     179,   25,   25,   25, 0x0a,
     191,   40,   25,   25, 0x0a,
     209,   40,   25,   25, 0x0a,
     227,   40,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KinematicModel__GLWidget[] = {
    "KinematicModel::GLWidget\0\0renderStuff()\0"
    "angle\0xRotationChanged(int)\0"
    "yRotationChanged(int)\0zRotationChanged(int)\0"
    "addDisplayList(PrimitiveObject*)\0"
    "removeDisplayList(GL_DisplayList)\0"
    "update(int)\0setXRotation(int)\0"
    "setYRotation(int)\0setZRotation(int)\0"
};

const QMetaObject KinematicModel::GLWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_KinematicModel__GLWidget,
      qt_meta_data_KinematicModel__GLWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KinematicModel::GLWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KinematicModel::GLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KinematicModel::GLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KinematicModel__GLWidget))
        return static_cast<void*>(const_cast< GLWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int KinematicModel::GLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: renderStuff(); break;
        case 1: xRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: yRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: zRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: addDisplayList((*reinterpret_cast< PrimitiveObject*(*)>(_a[1]))); break;
        case 5: removeDisplayList((*reinterpret_cast< GL_DisplayList(*)>(_a[1]))); break;
        case 6: update((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: setXRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: setYRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: setZRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void KinematicModel::GLWidget::renderStuff()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void KinematicModel::GLWidget::xRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KinematicModel::GLWidget::yRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KinematicModel::GLWidget::zRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
