/****************************************************************************
** Meta object code from reading C++ file 'detect4g.h'
**
** Created: Sat Apr 1 14:01:55 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "detect4g.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'detect4g.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Detect4G[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Detect4G[] = {
    "Detect4G\0"
};

const QMetaObject Detect4G::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Detect4G,
      qt_meta_data_Detect4G, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Detect4G::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Detect4G::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Detect4G::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Detect4G))
        return static_cast<void*>(const_cast< Detect4G*>(this));
    return QThread::qt_metacast(_clname);
}

int Detect4G::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
