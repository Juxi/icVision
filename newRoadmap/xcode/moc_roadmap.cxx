/****************************************************************************
** Meta object code from reading C++ file 'roadmap.h'
**
** Created: Tue Jul 24 10:43:21 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../roadmap.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'roadmap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Roadmap[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      35,   32,    8,    8, 0x05,
      84,   82,    8,    8, 0x05,
     123,   32,    8,    8, 0x05,
     164,   82,    8,    8, 0x05,
     198,   82,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     230,   82,    8,    8, 0x0a,
     268,   82,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Roadmap[] = {
    "Roadmap\0\0appendedNode(vertex_t)\0,,\0"
    "appendedEdge(edge_t,QtGraphNode*,QtGraphNode*)\0"
    ",\0update2DPosition(QtGraphNode*,QPointF)\0"
    "newNodeColor(QtGraphNode*,QColor,QColor)\0"
    "newEdgeColor(QtGraphEdge*,QColor)\0"
    "newEdgeWeight(QtGraphEdge*,int)\0"
    "setQtGraphNode(vertex_t,QtGraphNode*)\0"
    "setQtGraphEdge(edge_t,QtGraphEdge*)\0"
};

const QMetaObject Roadmap::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Roadmap,
      qt_meta_data_Roadmap, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Roadmap::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Roadmap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Roadmap::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Roadmap))
        return static_cast<void*>(const_cast< Roadmap*>(this));
    return QObject::qt_metacast(_clname);
}

int Roadmap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: appendedNode((*reinterpret_cast< vertex_t(*)>(_a[1]))); break;
        case 1: appendedEdge((*reinterpret_cast< edge_t(*)>(_a[1])),(*reinterpret_cast< QtGraphNode*(*)>(_a[2])),(*reinterpret_cast< QtGraphNode*(*)>(_a[3]))); break;
        case 2: update2DPosition((*reinterpret_cast< QtGraphNode*(*)>(_a[1])),(*reinterpret_cast< QPointF(*)>(_a[2]))); break;
        case 3: newNodeColor((*reinterpret_cast< QtGraphNode*(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2])),(*reinterpret_cast< QColor(*)>(_a[3]))); break;
        case 4: newEdgeColor((*reinterpret_cast< QtGraphEdge*(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 5: newEdgeWeight((*reinterpret_cast< QtGraphEdge*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: setQtGraphNode((*reinterpret_cast< vertex_t(*)>(_a[1])),(*reinterpret_cast< QtGraphNode*(*)>(_a[2]))); break;
        case 7: setQtGraphEdge((*reinterpret_cast< edge_t(*)>(_a[1])),(*reinterpret_cast< QtGraphEdge*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Roadmap::appendedNode(vertex_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Roadmap::appendedEdge(edge_t _t1, QtGraphNode * _t2, QtGraphNode * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Roadmap::update2DPosition(QtGraphNode * _t1, QPointF _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Roadmap::newNodeColor(QtGraphNode * _t1, QColor _t2, QColor _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Roadmap::newEdgeColor(QtGraphEdge * _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Roadmap::newEdgeWeight(QtGraphEdge * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
