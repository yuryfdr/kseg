/****************************************************************************
** Meta object code from reading C++ file 'KSegView.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegView.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegView.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      36,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      50,    9,    9,    9, 0x0a,
      74,   65,    9,    9, 0x0a,
      97,    9,    9,    9, 0x0a,
     118,  116,    9,    9, 0x0a,
     140,    9,    9,    9, 0x0a,
     150,    9,    9,    9, 0x0a,
     161,    9,    9,    9, 0x0a,
     175,    9,    9,    9, 0x0a,
     196,  190,    9,    9, 0x0a,
     218,    9,    9,    9, 0x09,
     229,    9,    9,    9, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KSegView[] = {
    "KSegView\0\0statusBarMessage(QString)\0"
    "updateMenus()\0redrawBuffer()\0filename\0"
    "documentSaved(QString)\0documentModified()\0"
    "b\0setMenusEnabled(bool)\0viewPan()\0"
    "viewZoom()\0viewZoomFit()\0viewOrigZoom()\0"
    "which\0playConstruction(int)\0moved(int)\0"
    "selectTimeout()\0"
};

void KSegView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegView *_t = static_cast<KSegView *>(_o);
        switch (_id) {
        case 0: _t->statusBarMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->updateMenus(); break;
        case 2: _t->redrawBuffer(); break;
        case 3: _t->documentSaved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->documentModified(); break;
        case 5: _t->setMenusEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->viewPan(); break;
        case 7: _t->viewZoom(); break;
        case 8: _t->viewZoomFit(); break;
        case 9: _t->viewOrigZoom(); break;
        case 10: _t->playConstruction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->moved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->selectTimeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KSegView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KSegView,
      qt_meta_data_KSegView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegView))
        return static_cast<void*>(const_cast< KSegView*>(this));
    return QWidget::qt_metacast(_clname);
}

int KSegView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void KSegView::statusBarMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KSegView::updateMenus()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
