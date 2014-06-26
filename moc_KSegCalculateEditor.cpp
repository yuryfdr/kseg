/****************************************************************************
** Meta object code from reading C++ file 'KSegCalculateEditor.H'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "KSegCalculateEditor.H"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'KSegCalculateEditor.H' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KSegCalculateEditor[] = {

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
      21,   20,   20,   20, 0x09,
      33,   20,   20,   20, 0x09,
      42,   20,   20,   20, 0x09,
      59,   51,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KSegCalculateEditor[] = {
    "KSegCalculateEditor\0\0OKClicked()\0"
    "reject()\0accept()\0newText\0"
    "formulaChanged(QString)\0"
};

void KSegCalculateEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KSegCalculateEditor *_t = static_cast<KSegCalculateEditor *>(_o);
        switch (_id) {
        case 0: _t->OKClicked(); break;
        case 1: _t->reject(); break;
        case 2: _t->accept(); break;
        case 3: _t->formulaChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KSegCalculateEditor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KSegCalculateEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_KSegCalculateEditor,
      qt_meta_data_KSegCalculateEditor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KSegCalculateEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KSegCalculateEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KSegCalculateEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KSegCalculateEditor))
        return static_cast<void*>(const_cast< KSegCalculateEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int KSegCalculateEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_EditorButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EditorButton[] = {
    "EditorButton\0\0valueClicked(int)\0"
    "gotClick()\0"
};

void EditorButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EditorButton *_t = static_cast<EditorButton *>(_o);
        switch (_id) {
        case 0: _t->valueClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->gotClick(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EditorButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorButton::staticMetaObject = {
    { &QToolButton::staticMetaObject, qt_meta_stringdata_EditorButton,
      qt_meta_data_EditorButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorButton))
        return static_cast<void*>(const_cast< EditorButton*>(this));
    return QToolButton::qt_metacast(_clname);
}

int EditorButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void EditorButton::valueClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_EditorToolBar[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_EditorToolBar[] = {
    "EditorToolBar\0"
};

void EditorToolBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EditorToolBar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EditorToolBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_EditorToolBar,
      qt_meta_data_EditorToolBar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EditorToolBar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EditorToolBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EditorToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditorToolBar))
        return static_cast<void*>(const_cast< EditorToolBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int EditorToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
