/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Jul 23 13:08:46 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   11,   11,   11, 0x08,
      68,   60,   11,   11, 0x08,
      82,   11,   11,   11, 0x28,
      92,   11,   11,   11, 0x08,
     109,   11,   11,   11, 0x08,
     119,   11,   11,   11, 0x08,
     132,   11,   11,   11, 0x08,
     145,   11,   11,   11, 0x08,
     160,   11,   11,   11, 0x08,
     177,   11,   11,   11, 0x08,
     199,   11,   11,   11, 0x08,
     218,   11,   11,   11, 0x08,
     225,   11,   11,   11, 0x08,
     242,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0event\0resizedMainWindow(QResizeEvent*)\0"
    "newMap()\0display\0loadMap(bool)\0loadMap()\0"
    "stealthLoadMap()\0saveMap()\0connectMap()\0"
    "projectMap()\0importNesMap()\0"
    "connectToRobot()\0disconnectFromRobot()\0"
    "multiEdgeExplore()\0goTo()\0stopController()\0"
    "setVelocity()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resizedMainWindow((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        case 1: newMap(); break;
        case 2: loadMap((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: loadMap(); break;
        case 4: stealthLoadMap(); break;
        case 5: saveMap(); break;
        case 6: connectMap(); break;
        case 7: projectMap(); break;
        case 8: importNesMap(); break;
        case 9: connectToRobot(); break;
        case 10: disconnectFromRobot(); break;
        case 11: multiEdgeExplore(); break;
        case 12: goTo(); break;
        case 13: stopController(); break;
        case 14: setVelocity(); break;
        default: ;
        }
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::resizedMainWindow(QResizeEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
