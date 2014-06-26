/****************************************************************************
** Meta object code from reading C++ file 'KSegPreferencesDialog.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegPreferencesDialog.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegPreferencesDialog.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegPreferencesDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x09,
      30,   22,   22,   22, 0x09,
      40,   22,   22,   22, 0x09,
      49,   22,   22,   22, 0x09,
      75,   22,   22,   22, 0x09,
     101,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KSegPreferencesDialog[] = {
    "KSegPreferencesDialog\0\0init()\0destroy()\0"
    "accept()\0foregroundButtonClicked()\0"
    "backgroundButtonClicked()\0"
    "selectButtonClicked()\0"
};

void KSegPreferencesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegPreferencesDialog *_t = static_cast<KSegPreferencesDialog *>(_o);
        switch (_id) {
        case 0: _t->init(); break;
        case 1: _t->destroy(); break;
        case 2: _t->accept(); break;
        case 3: _t->foregroundButtonClicked(); break;
        case 4: _t->backgroundButtonClicked(); break;
        case 5: _t->selectButtonClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData KSegPreferencesDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegPreferencesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KSegPreferencesDialog,
      qt_meta_data_KSegPreferencesDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegPreferencesDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegPreferencesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegPreferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegPreferencesDialog))
        return static_cast<void*>(const_cast< KSegPreferencesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int KSegPreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
