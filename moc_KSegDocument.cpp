/****************************************************************************
** Meta object code from reading C++ file 'KSegDocument.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegDocument.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegDocument.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegDocument[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      50,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      41,   32,   13,   13, 0x05,
      64,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      83,   13,   13,   13, 0x0a,
      96,   13,   13,   13, 0x0a,
     105,   13,   13,   13, 0x0a,
     119,   13,   13,   13, 0x0a,
     129,   13,   13,   13, 0x0a,
     148,   13,   13,   13, 0x0a,
     162,   13,   13,   13, 0x0a,
     174,   13,   13,   13, 0x0a,
     192,   13,   13,   13, 0x0a,
     207,   13,   13,   13, 0x0a,
     224,   13,   13,   13, 0x0a,
     233,   13,   13,   13, 0x0a,
     244,   13,   13,   13, 0x0a,
     257,   13,   13,   13, 0x0a,
     272,   13,   13,   13, 0x0a,
     288,   13,   13,   13, 0x0a,
     308,   13,   13,   13, 0x0a,
     319,   13,   13,   13, 0x0a,
     330,   13,   13,   13, 0x0a,
     343,   13,   13,   13, 0x0a,
     362,   13,   13,   13, 0x0a,
     379,   13,   13,   13, 0x0a,
     396,   13,   13,   13, 0x0a,
     414,   13,   13,   13, 0x0a,
     425,   13,   13,   13, 0x0a,
     442,   13,   13,   13, 0x0a,
     470,   13,   13,   13, 0x0a,
     501,   13,   13,   13, 0x0a,
     519,   13,   13,   13, 0x0a,
     535,   13,   13,   13, 0x0a,
     551,   13,   13,   13, 0x0a,
     566,   13,   13,   13, 0x0a,
     581,   13,   13,   13, 0x0a,
     596,   13,   13,   13, 0x0a,
     610,   13,   13,   13, 0x0a,
     629,   13,   13,   13, 0x0a,
     653,   13,   13,   13, 0x0a,
     677,   13,   13,   13, 0x0a,
     700,   13,   13,   13, 0x0a,
     724,   13,   13,   13, 0x0a,
     747,   13,   13,   13, 0x0a,
     770,   13,   13,   13, 0x0a,
     791,   13,   13,   13, 0x0a,
     810,   13,   13,   13, 0x0a,
     828,   13,   13,   13, 0x0a,
     851,  845,   13,   13, 0x0a,
     863,   13,   13,   13, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KSegDocument[] = {
    "KSegDocument\0\0documentChanged()\0"
    "filename\0documentSaved(QString)\0"
    "documentModified()\0newSegment()\0"
    "newRay()\0newMidpoint()\0newLine()\0"
    "newPerpendicular()\0newBisector()\0"
    "newCircle()\0newIntersection()\0"
    "newEndpoints()\0newCenterpoint()\0"
    "newArc()\0newLocus()\0newPolygon()\0"
    "newArcSector()\0newArcSegment()\0"
    "newCircleInterior()\0editUndo()\0"
    "editRedo()\0editDelete()\0editToggleLabels()\0"
    "editHideLabels()\0editShowLabels()\0"
    "editChangeLabel()\0editHide()\0"
    "editShowHidden()\0editChangeNumberOfSamples()\0"
    "editShowSelectionGroupDialog()\0"
    "measureDistance()\0measureLength()\0"
    "measureRadius()\0measureAngle()\0"
    "measureRatio()\0measureSlope()\0"
    "measureArea()\0measureCalculate()\0"
    "transformChooseVector()\0transformChooseMirror()\0"
    "transformChooseAngle()\0transformChooseCenter()\0"
    "transformChooseRatio()\0transformClearChosen()\0"
    "transformTranslate()\0transformReflect()\0"
    "transformRotate()\0transformScale()\0"
    "which\0doPlay(int)\0IChanged()\0"
};

void KSegDocument::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegDocument *_t = static_cast<KSegDocument *>(_o);
        switch (_id) {
        case 0: _t->documentChanged(); break;
        case 1: _t->documentSaved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->documentModified(); break;
        case 3: _t->newSegment(); break;
        case 4: _t->newRay(); break;
        case 5: _t->newMidpoint(); break;
        case 6: _t->newLine(); break;
        case 7: _t->newPerpendicular(); break;
        case 8: _t->newBisector(); break;
        case 9: _t->newCircle(); break;
        case 10: _t->newIntersection(); break;
        case 11: _t->newEndpoints(); break;
        case 12: _t->newCenterpoint(); break;
        case 13: _t->newArc(); break;
        case 14: _t->newLocus(); break;
        case 15: _t->newPolygon(); break;
        case 16: _t->newArcSector(); break;
        case 17: _t->newArcSegment(); break;
        case 18: _t->newCircleInterior(); break;
        case 19: _t->editUndo(); break;
        case 20: _t->editRedo(); break;
        case 21: _t->editDelete(); break;
        case 22: _t->editToggleLabels(); break;
        case 23: _t->editHideLabels(); break;
        case 24: _t->editShowLabels(); break;
        case 25: _t->editChangeLabel(); break;
        case 26: _t->editHide(); break;
        case 27: _t->editShowHidden(); break;
        case 28: _t->editChangeNumberOfSamples(); break;
        case 29: _t->editShowSelectionGroupDialog(); break;
        case 30: _t->measureDistance(); break;
        case 31: _t->measureLength(); break;
        case 32: _t->measureRadius(); break;
        case 33: _t->measureAngle(); break;
        case 34: _t->measureRatio(); break;
        case 35: _t->measureSlope(); break;
        case 36: _t->measureArea(); break;
        case 37: _t->measureCalculate(); break;
        case 38: _t->transformChooseVector(); break;
        case 39: _t->transformChooseMirror(); break;
        case 40: _t->transformChooseAngle(); break;
        case 41: _t->transformChooseCenter(); break;
        case 42: _t->transformChooseRatio(); break;
        case 43: _t->transformClearChosen(); break;
        case 44: _t->transformTranslate(); break;
        case 45: _t->transformReflect(); break;
        case 46: _t->transformRotate(); break;
        case 47: _t->transformScale(); break;
        case 48: _t->doPlay((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 49: _t->IChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KSegDocument::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegDocument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KSegDocument,
      qt_meta_data_KSegDocument, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegDocument::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegDocument))
        return static_cast<void*>(const_cast< KSegDocument*>(this));
    return QObject::qt_metacast(_clname);
}

int KSegDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void KSegDocument::documentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void KSegDocument::documentSaved(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KSegDocument::documentModified()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
static const uint qt_meta_data_KSegSampleChangeDialog[] = {

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
      28,   24,   23,   23, 0x0a,
      39,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_KSegSampleChangeDialog[] = {
    "KSegSampleChangeDialog\0\0val\0setIt(int)\0"
    "reject()\0"
};

void KSegSampleChangeDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegSampleChangeDialog *_t = static_cast<KSegSampleChangeDialog *>(_o);
        switch (_id) {
        case 0: _t->setIt((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->reject(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KSegSampleChangeDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegSampleChangeDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KSegSampleChangeDialog,
      qt_meta_data_KSegSampleChangeDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegSampleChangeDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegSampleChangeDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegSampleChangeDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegSampleChangeDialog))
        return static_cast<void*>(const_cast< KSegSampleChangeDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int KSegSampleChangeDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
