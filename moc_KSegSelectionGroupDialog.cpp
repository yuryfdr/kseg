/****************************************************************************
** Meta object code from reading C++ file 'KSegSelectionGroupDialog.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegSelectionGroupDialog.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegSelectionGroupDialog.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegSelectionGroupDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x08,
      35,   25,   25,   25, 0x08,
      46,   25,   25,   25, 0x08,
      60,   25,   25,   25, 0x08,
      71,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_KSegSelectionGroupDialog[] = {
    "KSegSelectionGroupDialog\0\0select()\0"
    "deselect()\0deleteGroup()\0newGroup()\0"
    "updateButtons()\0"
};

void KSegSelectionGroupDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegSelectionGroupDialog *_t = static_cast<KSegSelectionGroupDialog *>(_o);
        switch (_id) {
        case 0: _t->select(); break;
        case 1: _t->deselect(); break;
        case 2: _t->deleteGroup(); break;
        case 3: _t->newGroup(); break;
        case 4: _t->updateButtons(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData KSegSelectionGroupDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegSelectionGroupDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KSegSelectionGroupDialog,
      qt_meta_data_KSegSelectionGroupDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegSelectionGroupDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegSelectionGroupDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegSelectionGroupDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegSelectionGroupDialog))
        return static_cast<void*>(const_cast< KSegSelectionGroupDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int KSegSelectionGroupDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
