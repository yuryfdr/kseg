/****************************************************************************
** Meta object code from reading C++ file 'KSegRecursionDialog.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegRecursionDialog.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegRecursionDialog.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegRecursionDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x09,
      30,   20,   20,   20, 0x09,
      45,   39,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KSegRecursionDialog[] = {
    "KSegRecursionDialog\0\0accept()\0reject()\0"
    "value\0spinboxChanged(int)\0"
};

void KSegRecursionDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegRecursionDialog *_t = static_cast<KSegRecursionDialog *>(_o);
        switch (_id) {
        case 0: _t->accept(); break;
        case 1: _t->reject(); break;
        case 2: _t->spinboxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KSegRecursionDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegRecursionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KSegRecursionDialog,
      qt_meta_data_KSegRecursionDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegRecursionDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegRecursionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegRecursionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegRecursionDialog))
        return static_cast<void*>(const_cast< KSegRecursionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int KSegRecursionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
