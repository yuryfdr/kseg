/****************************************************************************
** Meta object code from reading C++ file 'MatrixDialog.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "formula/MatrixDialog.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MatrixDialog.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MatrixDialog[] = {

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
      20,   14,   13,   13, 0x09,
      34,   14,   13,   13, 0x09,
      49,   14,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MatrixDialog[] = {
    "MatrixDialog\0\0value\0setWidth(int)\0"
    "setHeight(int)\0setFill(bool)\0"
};

void MatrixDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MatrixDialog *_t = static_cast<MatrixDialog *>(_o);
        switch (_id) {
        case 0: _t->setWidth((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setHeight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setFill((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MatrixDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MatrixDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MatrixDialog,
      qt_meta_data_MatrixDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MatrixDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MatrixDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MatrixDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MatrixDialog))
        return static_cast<void*>(const_cast< MatrixDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MatrixDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
