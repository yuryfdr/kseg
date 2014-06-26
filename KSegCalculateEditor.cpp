/*
 *  KSeg
 *  Copyright (C) 1999-2006 Ilya Baran
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Send comments and/or bug reports to:
 *                 ibaran@mit.edu
 */


#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <QBoxLayout>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <QEvent>
#include <QVBoxLayout>

#include "KSegCalculateEditor.H"
#include "G_object.H"
#include "KSegDocument.H"
#include "KSegWindow.H"

#include "pics/abs.xpm"
#include "pics/editcopy.xpm"
#include "pics/editcut.xpm"
#include "pics/editpaste.xpm"
#include "pics/frac.xpm"
#include "pics/paren.xpm"
#include "pics/redo.xpm"
#include "pics/rsup.xpm"
#include "pics/sqrt.xpm"
#include "pics/undo.xpm"


class CalculationDialogReferenceFetcher : public ReferenceFetcher
{
public:
  CalculationDialogReferenceFetcher(KSegCalculateEditor *inRef) { ref = inRef; }

  double getValue(int reference_number)
  { return ref->getOutputParents()[reference_number]->getObject()->getNumValue(); }

  KFormula *getFormula(int reference_number)
  { return ((G_valueObject *)(ref->getOutputParents()[reference_number]->getObject()))->getLhs(); }

protected:
  KSegCalculateEditor * ref;
};


KSegCalculateEditor::KSegCalculateEditor(KSegView *view, QString initialString,
					 G_refs initialParents, G_ref *inCurRef)
  : QDialog(view, 0, false), outputParents(initialParents),
    curRef(inCurRef), descendantSearcher(inCurRef)
{
  QPushButton *cancel;

  QVBoxLayout *vlayout = new QVBoxLayout(this);

  view->installEventFilter(this);
  view->setMenusEnabled(false);

  //now disable all the other views
  unsigned int i;

  QVector<KSegView*> allViews = KSegView::getAllViews();

  for(i = 0; i < allViews.count(); ++i) {
    if(allViews.at(i) == view) continue;
    allViews.at(i)->topLevelWidget()->setEnabled(false);
  }

  EditorToolBar *tb = new EditorToolBar(this);
  vlayout->addWidget(tb);

  valueDisplay = new QLabel(tr("Value is undefined"), this);
  vlayout->addWidget(valueDisplay);
  
  editor = new KFormulaEdit(this, 0, 0, true);
  tb->connectToFormula(editor);
  setFocusProxy(editor);
  editor->getFormula()->setReferenceFetcher(new CalculationDialogReferenceFetcher(this));
  editor->enableSizeHintSignal(true);
  connect(editor, SIGNAL(formulaChanged(const QString &)),
	  this, SLOT(formulaChanged(const QString &)));
  editor->setText(initialString);

  vlayout->addWidget(editor, 1);
  
  QBoxLayout *hlayout = new QBoxLayout(QBoxLayout::RightToLeft);

  ok = new QPushButton(tr("OK"), this );
  hlayout->addWidget(ok);
  QObject::connect( ok, SIGNAL(clicked()), this, SLOT(OKClicked()) );
  ok->setDefault(true);
  cancel = new QPushButton(tr("Cancel"), this );
  hlayout->addWidget(cancel);
  QObject::connect( cancel, SIGNAL(clicked()), this, SLOT(reject()) );

  vlayout->addLayout(hlayout);

  formulaChanged(initialString);
}


void KSegCalculateEditor::run()
{
  finished = false;
  show();
  while(!finished) qApp->processOneEvent(); 
}

KSegCalculateEditor::~KSegCalculateEditor()
{
  ((KSegView *)parent())->setMenusEnabled(true);

  unsigned int i;

  QVector<KSegView*> allViews = KSegView::getAllViews();

  for(i = 0; i < allViews.count(); ++i) {
    allViews.at(i)->topLevelWidget()->setEnabled(true);
  }
}


void KSegCalculateEditor::OKClicked()
{
  //now delete unnecessary parents.
  unsigned int i;
  for(i = 0; i < outputParents.size(); ++i) {
    if(!(outputString.contains(QChar(int(REFERENCE_NUM(i)))))) {
      removeParent(i);
      i--;
    }
  }

  outputString = editor->text();
  accept();
}


void KSegCalculateEditor::formulaChanged(const QString & /*newTextDummy*/)
{
  QString newText = editor->text();        
  
  outputString = newText;

  updateValue();
}


void KSegCalculateEditor::removeParent(int i)
{
  unsigned int j;

  outputParents.remove(i);

  for(j = 0; j < outputString.length(); j++) {
    if(!IS_REFERENCE(outputString[j].unicode())) continue;

    if(outputString[j].unicode() > REFERENCE_NUM(i)) {
      outputString[j] = outputString[j].unicode() - 1;
    }
  }

  return;
}


void KSegCalculateEditor::updateValue()
{
  int error;
  double value = 0;
  
  unsigned int i;

  for(i = 0; i < outputParents.count(); ++i) {
    if(fabs(outputParents[i]->getObject()->getNumValue()) >= BIG) {
      value = BIG;
    }
  }
  
  if(value != BIG) {
    value = editor->getFormula()->evaluate(QMap<QString,double>(), &error);
  }

  if(error != NO_ERROR) {
    value = BIG;
    ok->setEnabled(false);
  }
  else ok->setEnabled(true);

  QString t;

  if(value != BIG) t = tr("Value is %1").arg(QString::number(value, 'f', 4));
  else t = tr("Value is undefined");

  valueDisplay->setText(t);
 
}

bool KSegCalculateEditor::eventFilter(QObject *o, QEvent *e)
{
  if(o != parent()) return false;

  if(e->type() == QEvent::MouseMove) {
    QMouseEvent *m = (QMouseEvent *)e;
    KSegView *v = (KSegView *)parent();
    
    G_refs tmp =
      v->getDocument()->whatAmIOn(m->x(), m->y(),
				  ViewTransform(v->getOffsetX(), v->getOffsetY(), v->getZoom()), false);
    
    unsigned int i;
    for(i = 0; i < tmp.size(); ++i) {
      if(tmp[i] == curRef) continue;
      if(tmp[i]->getType() & G_VALUE && (curRef == 0 || !descendantSearcher.search(tmp[i]))) {
	parentWidget()->setCursor(Qt::upArrowCursor);

	QString message = tr("Insert %1 %2", "For example, \"Insert Measurement M1\"");

	message = message.arg(G_ref::getNameFromType(tmp[i]->getType()));
	message = message.arg(KFormula::toUgly(tmp[i]->getLabel().getText()));
	
	((KSegWindow *)(parentWidget()->topLevelWidget()))->statusBarMessage(message);
	return true;
      }
    }
    parentWidget()->unsetCursor();
    ((KSegWindow *)(parentWidget()->topLevelWidget()))->statusBarMessage(tr("Ready"));
    return true;
  }
  if(e->type() == QEvent::MouseButtonRelease ||
     e->type() == QEvent::MouseButtonDblClick) {
    return true;
  }
  if(e->type() == QEvent::MouseButtonPress) {
    QMouseEvent *m = (QMouseEvent *)e;
    KSegView *v = (KSegView *)parent();
    
    G_refs tmp =
      v->getDocument()->whatAmIOn(m->x(), m->y(),
				  ViewTransform(v->getOffsetX(), v->getOffsetY(), v->getZoom()), false);
    
    unsigned int i;
    for(i = 0; i < tmp.size(); ++i) {
      if(tmp[i] == curRef) continue;
      if(tmp[i]->getType() & G_VALUE && (curRef == 0 || !descendantSearcher.search(tmp[i]))) {
	insertReference(tmp[i]);
	break;
      }
    }

    setActiveWindow();

    return true;
  }

  return false;
}


void KSegCalculateEditor::insertReference(G_ref *r)
{
  //first try to find r in parents:
  int i = outputParents.find(r);

  if(i != -1) {
    editor->insertChar(REFERENCE_NUM(i));
    return;
  }

  if(outputParents.size() + 1 >= MAX_REFERENCES) return; // just in case

  outputParents.append(r);
  editor->insertChar(REFERENCE_NUM(outputParents.size() - 1));  
}


using namespace Box;

EditorToolBar::EditorToolBar(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  setMinimumHeight(32);
  layout = new QHBoxLayout(this);
  layout->setSpacing(2);
  layout->setMargin(4);
}

void EditorToolBar::insertButton(QPixmap &icon, int key, QString toolTipText)
{
  EditorButton *eb = new EditorButton(icon, this, key);
  layout->addWidget(eb);
  connect(eb, SIGNAL(valueClicked(int)), formula, SLOT(insertChar(int)));
  QToolTip::add(eb, toolTipText);
}

void EditorToolBar::connectToFormula(KFormulaEdit *inFormula)
{
  formula = inFormula;

  QPixmap ecut((const char **)editcut_xpm);
  QPixmap ecopy((const char **)editcopy_xpm);
  QPixmap epaste((const char **)editpaste_xpm);
  QPixmap undo((const char **)undo_xpm);
  QPixmap redo((const char **)redo_xpm);
  QPixmap rsup((const char **)rsup_xpm);
  QPixmap paren((const char **)paren_xpm);
  QPixmap abs((const char **)abs_xpm);
  QPixmap frac((const char **)frac_xpm);
  QPixmap sqrt((const char **)sqrt_xpm);

  insertButton(ecut, CUT_CHAR, tr("Cut (Ctrl + X)"));
  insertButton(ecopy, COPY_CHAR, tr("Copy (Ctrl + C)"));
  insertButton(epaste, PASTE_CHAR, tr("Paste (Ctrl + V)"));
  
  layout->addSpacing(5);
  
  insertButton(undo, UNDO_CHAR, tr("Undo (Ctrl + Z)"));
  insertButton(redo, REDO_CHAR, tr("Redo (Ctrl + R)"));

  layout->addSpacing(5);

  insertButton(rsup, POWER, tr("Power (Ctrl + 6)"));
  insertButton(paren, PAREN, tr("Parentheses (()"));
  insertButton(abs, ABS, tr("Absolute value (|)"));
  insertButton(frac, DIVIDE, tr("Fraction (Ctrl + /)"));
  insertButton(sqrt, SQRT, tr("Root (Ctrl + 2)"));

  layout->addStretch();
}


