/****************************************************************************
** Meta object code from reading C++ file 'glwidget.h'
**
** Created: Wed Jun 2 10:58:26 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "glwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'glwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GLWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   10,    9,    9, 0x05,
      38,   10,    9,    9, 0x05,
      60,   10,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      82,   10,    9,    9, 0x0a,
     100,   10,    9,    9, 0x0a,
     118,   10,    9,    9, 0x0a,
     141,  136,    9,    9, 0x0a,
     163,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GLWidget[] = {
    "GLWidget\0\0angle\0xRotationChanged(int)\0"
    "yRotationChanged(int)\0zRotationChanged(int)\0"
    "setXRotation(int)\0setYRotation(int)\0"
    "setZRotation(int)\0link\0createGLObject(Link*)\0"
    "callUpdateGL()\0"
};

const QMetaObject GLWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_GLWidget,
      qt_meta_data_GLWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GLWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GLWidget))
        return static_cast<void*>(const_cast< GLWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int GLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: xRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: yRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: zRotationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: setXRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: setYRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: setZRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: createGLObject((*reinterpret_cast< Link*(*)>(_a[1]))); break;
        case 7: callUpdateGL(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void GLWidget::xRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GLWidget::yRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GLWidget::zRotationChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
