/****************************************************************************
** Meta object code from reading C++ file 'KSegMenuMaintainer.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegMenuMaintainer.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegMenuMaintainer.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegMenuMaintainer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x0a,
      37,   29,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KSegMenuMaintainer[] = {
    "KSegMenuMaintainer\0\0update()\0visible\0"
    "toolbarVisibilityChanged(bool)\0"
};

void KSegMenuMaintainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegMenuMaintainer *_t = static_cast<KSegMenuMaintainer *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->toolbarVisibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KSegMenuMaintainer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegMenuMaintainer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KSegMenuMaintainer,
      qt_meta_data_KSegMenuMaintainer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegMenuMaintainer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegMenuMaintainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegMenuMaintainer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegMenuMaintainer))
        return static_cast<void*>(const_cast< KSegMenuMaintainer*>(this));
    return QObject::qt_metacast(_clname);
}

int KSegMenuMaintainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_KSegMainMenuMaintainer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KSegMainMenuMaintainer[] = {
    "KSegMainMenuMaintainer\0\0update()\0"
};

void KSegMainMenuMaintainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegMainMenuMaintainer *_t = static_cast<KSegMainMenuMaintainer *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData KSegMainMenuMaintainer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegMainMenuMaintainer::staticMetaObject = {
    { &KSegMenuMaintainer::staticMetaObject, qt_meta_stringdata_KSegMainMenuMaintainer,
      qt_meta_data_KSegMainMenuMaintainer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegMainMenuMaintainer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegMainMenuMaintainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegMainMenuMaintainer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegMainMenuMaintainer))
        return static_cast<void*>(const_cast< KSegMainMenuMaintainer*>(this));
    return KSegMenuMaintainer::qt_metacast(_clname);
}

int KSegMainMenuMaintainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KSegMenuMaintainer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
