/****************************************************************************
** Meta object code from reading C++ file 'KSegConstructionList.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegConstructionList.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegConstructionList.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegConstructionList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x09,
      48,   37,   21,   21, 0x09,
      79,   68,   21,   21, 0x09,
      99,   21,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KSegConstructionList[] = {
    "KSegConstructionList\0\0redrawBuffer()\0"
    "newCurrent\0curItemChanged(int)\0"
    "newHoffset\0hoffsetChanged(int)\0"
    "autoScroll()\0"
};

void KSegConstructionList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegConstructionList *_t = static_cast<KSegConstructionList *>(_o);
        switch (_id) {
        case 0: _t->redrawBuffer(); break;
        case 1: _t->curItemChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->hoffsetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->autoScroll(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KSegConstructionList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegConstructionList::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_KSegConstructionList,
      qt_meta_data_KSegConstructionList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegConstructionList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegConstructionList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegConstructionList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegConstructionList))
        return static_cast<void*>(const_cast< KSegConstructionList*>(this));
    return QFrame::qt_metacast(_clname);
}

int KSegConstructionList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
