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


#include <qscrollbar.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qbitmap.h>
#include <qdatetime.h>
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include "KSegView.H"
#include "G_point.H"
#include "G_line.H"
#include "G_segment.H"
#include "G_drawstyle.H"
#include "G_label.H"
#include "KSegDocument.H"
#include "KSegConstruction.H"
#include "KSegCalculateEditor.H"
#include "G_object.H"
#include "KSegConstructionList.H"
#include "G_refSearcher.H"
#include "KSegProperties.H"

#include "pics/zoom_cursor.xpm"


//the following class and the function after are for determining if A can be
//reconstrained to B.

class ATypeFinder : public G_refSearcher
{
public:
  enum AType {
    Normal,
    Given,
    Initial,
    Final
  };

  ATypeFinder(G_ref *inB) { B = inB; type = Normal; }

  bool is_found(const G_ref *ref)
  {
    if(ref == B) return true;
    if(ref->getGiven()) type = Given;
    if(ref->getInitial() && type != Given) type = Initial;
    if(ref->getFinal() && type == Normal) type = Final;
    return false;
  }

  G_refs search_next(const G_ref *ref) { return ref->getChildrenConst(); }

  AType type;
  G_ref *B;
};

//----------------------------------------------------------------------------------


bool canReconstrain(G_ref *A, G_ref *B)
{
  if(B->getType() & (G_POINT | G_CURVE) == 0) return false;

  if(B->getDocument()->isConstruction() && B->getType() == G_POINT && A->getGiven() &&
     ((KSegConstruction *)(B->getDocument()))->hasLoops()) return false;

  ATypeFinder atf(B);
  if(atf.search(A) == true) return false;

  if(atf.type == ATypeFinder::Normal || B->getDocument()->isConstruction() == false) return true;  

  KSegConstruction *c = (KSegConstruction *)(B->getDocument());

  ImplicitInitialSearcher iis(A);
  if(iis.search(B) == false) {
    int j;
    for(j = 0; j < (int)c->getFinal().size(); ++j) {
      if(c->getFinal()[j]->getParentsConst().size() && iis.search(c->getFinal()[j]->getParentsConst()) == false)
	return false;
    }

  }


  if(atf.type == ATypeFinder::Given) {
    CanMakeGivenSearcher cmgs;
    if(cmgs.search(B) == true) return false;
  }

  if(atf.type == ATypeFinder::Initial) {
    CanMakeInitialAncSearcher cmias;
    if(cmias.search(B) == true) return false;
  }

  return true;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


KSegView::SelectType KSegView::selectType;
QVector<KSegView*> KSegView::allViews;
QPainter *KSegView::constructionPainter;
ViewTransform KSegView::constructionTransform;

int KSegView::ScrollBarThickness = 20;

KSegView::KSegView(KSegDocument *inDoc, QWidget *parent)
  : QWidget(parent)
{
  allViews.append(this);

  menusEnabled = true;
  nextZoom = false;
  nextPan = false;

  doc = inDoc;

  doc->addView();
  docReferenceRemoved = false;

  connect(doc, SIGNAL(documentChanged()), this, SLOT(redrawBuffer()));
  connect(doc, SIGNAL(documentModified()), this, SLOT(documentModified()));
  connect(doc, SIGNAL(documentSaved(const QString &)),
	  this, SLOT(documentSaved(const QString &)));

  setMinimumSize(ScrollBarThickness * 4, ScrollBarThickness * 4);
  buffer = QPixmap(width(), height());

  //buffer.setOptimization(QPixmap::BestOptim);

  forcePaint = true;

  if(KSegProperties::getProperty("SelectType").lower() == "borderselect")
    selectType = BORDER_SELECT;
  else if(KSegProperties::getProperty("SelectType").lower() == "blinkingselect")
    selectType = BLINKING_SELECT;

  hBar = new QScrollBar(Qt::Horizontal, this);
  vBar = new QScrollBar(Qt::Vertical, this);

  if(ScrollBarThickness == 20) ScrollBarThickness = hBar->sizeHint().height();

  setMouseTracking(true);

  drag = NO_DRAG;

  offsetX = offsetY = 0;
  zoom = 1;

  connect(hBar, SIGNAL(valueChanged(int)), this, SLOT(moved(int)));
  connect(vBar, SIGNAL(valueChanged(int)), this, SLOT(moved(int)));

  setBackgroundMode(Qt::NoBackground);

  connect(&selectTimer, SIGNAL(timeout()), this, SLOT(selectTimeout()));
}

//document deletion works like this: if we're not deleting the last
//view (there are other views pointing to the document), we call
//delView in delDocumentRef (to decrease the reference count) and set
//docReferenceRemoved to mark that the reference count of the document
//has been decreased.  Otherwise, we only delete the document when it
//comes time to destroy the view.
KSegView::~KSegView()
{
  if(!docReferenceRemoved) doc->delView();
  allViews.remove(allViews.indexOf(this));
}

void KSegView::delDocumentRef()
{
  if(doc->getNumViews() > 1) {
    doc->delView();
    docReferenceRemoved = true;
  }
}

static bool inMoved = false; //recursion prevention
void KSegView::moved(int)
{
  if(inMoved) return;
  inMoved = true;

  offsetX -= ((double)hBar->value()) / zoom;
  offsetY -= ((double)vBar->value()) / zoom;

  redrawBuffer();
  inMoved = false;
}

void KSegView::resizeEvent(QResizeEvent *e)
{
  hBar->move(0, height() - ScrollBarThickness);
  hBar->resize(width() - ScrollBarThickness, ScrollBarThickness);
  vBar->move(width() - ScrollBarThickness, 0);
  vBar->resize(ScrollBarThickness, height());

  buffer.resize(e->size() - QSize(ScrollBarThickness, ScrollBarThickness));

  redrawBuffer();
}

void KSegView::redrawBuffer()
{
  QPainter p;

  if(drag == NO_DRAG || (drag != LABEL_DRAG && objectsUpdating.count() == 0)) {
    if(drag != ZOOM_DRAG && drag != PAN_DRAG && !inMoved) emit updateMenus();
  }

  if(selectType == BLINKING_SELECT) {
    if(doc->getSelected().count() > 0) {
      if(!selectTimer.isActive()) selectTimer.start(319);
    }
    else {
      if(selectTimer.isActive()) selectTimer.stop();
    }
  }
  else if(selectTimer.isActive()) selectTimer.stop();

  buffer.fill(QColor(KSegProperties::getProperty("BackgroundColor")));

  p.begin(&buffer);

  doc->draw(p, ViewTransform(offsetX, offsetY, zoom));

  p.end();

  if(forcePaint) repaint(false);
}

void KSegView::paintEvent(QPaintEvent *)
{
  if(doc == NULL) return;

  if(drag == RECTANGLE_DRAG) {
    QPainter p(&buffer);

    //quickly draw a flicker-free focus rectangle by doing a xor draw before
    //and after blitting.
    //p.drawWinFocusRect(QRect(dragStartX, dragStartY,dragCurX - dragStartX, dragCurY - dragStartY).normalize());
    QRect rect=QRect(dragStartX, dragStartY,dragCurX - dragStartX, dragCurY - dragStartY).normalize();
    p.drawRect(rect);
    bitBlt(this, 0, 0, &buffer);
    p.end();
    forcePaint=false;
    redrawBuffer();
    forcePaint=true;
    //p.setCompositionMode(QPainter::CompositionMode_Xor );
    //p.drawRect(rect);
    //p.drawWinFocusRect(QRect(dragStartX, dragStartY,dragCurX - dragStartX, dragCurY - dragStartY).normalize());

  }
  else {
    QRect tmp = doc->getSize(ViewTransform(offsetX, offsetY, zoom));
    
    if(tmp.isEmpty() || tmp.isNull()) tmp = QRect(1, 1, 2, 2);

    hBar->setRange(QMIN(0, tmp.left()), QMAX(0, tmp.right() - width() + ScrollBarThickness));
    hBar->setSteps(10, width() - ScrollBarThickness);
    hBar->setValue(0);

    vBar->setRange(QMIN(0, tmp.top()), QMAX(0, tmp.bottom() - height() + ScrollBarThickness));
    vBar->setSteps(10, height() - ScrollBarThickness);
    vBar->setValue(0);

    bitBlt(this, 0, 0, &buffer);
  }

}


void KSegView::documentSaved(const QString &filename)
{
  topLevelWidget()->setCaption(QString("kseg: ") + filename);
}

void KSegView::documentModified()
{
  QString caption = "kseg: ";
  if(getDocument()->getFilename().isEmpty()) caption += tr("Untitled");
  else caption += getDocument()->getFilename();
  caption += '*';

  if(topLevelWidget()->caption() != caption) topLevelWidget()->setCaption(caption);
}

//---------------------MOUSE PRESS EVENT-----------------------------
void KSegView::mousePressEvent(QMouseEvent *e)
{
  if(doc == NULL) return;

  dragCurX = dragStartX = lastMouseX = e->x();
  dragCurY = dragStartY = lastMouseY = e->y();

  if(drag != NO_DRAG) return;

  if(nextPan || e->button() == Qt::MidButton) {
    drag = PAN_DRAG; if(!nextPan) { nextPan = true; updateStatusBar(Qt::NoButton); }
    emit updateMenus();
    return;
  }
  if(nextZoom) { drag = ZOOM_DRAG; emit updateMenus(); return; }

  if(e->button() == Qt::LeftButton) {//----------------LEFT BUTTON PRESSED
    G_refs tmp = doc->whatAmIOn(e->x(), e->y(), ViewTransform(offsetX, offsetY, zoom));

    if(e->state() & Qt::ControlModifier && !(e->state() & Qt::ShiftModifier)) { //CTRL pressed--reconstrain
      if(tmp.count() == 0 || tmp[0]->getType() != G_POINT) return;

      if(!doc->isPointFreeable(tmp[0])) return;

      reconstrainUndo = new G_undoReconstrain(tmp[0]);

      tmp[0]->reconstrain(G_FREE_POINT, G_refs());

      drag = RECONSTRAIN_DRAG;
      objectsDragged.append(tmp[0]);
      doc->clearSel();
      doc->addSel(tmp[0]);

      setCursor(Qt::pointingHandCursor);

      doc->emitDocumentChanged();

      updateStatusBar(e->state());

      return;
    }

    if(tmp.count() == 0 || ((e->state() & Qt::ControlModifier) && (e->state() & Qt::ShiftModifier))) {
      tmp = doc->whatLabelsAmIOn(e->x(), e->y(), ViewTransform(offsetX, offsetY, zoom));
      
      if(tmp.count() == 0) {
	if(!(e->state() & Qt::ShiftModifier)) doc->clearSel();

	drag = RECTANGLE_DRAG;
	doc->emitDocumentChanged();
	return;
      }

      setCursor(Qt::pointingHandCursor);

      drag = LABEL_DRAG;
      objectsDragged.append(tmp[0]);
      objectsDragged[0]->getObject()->tempTransform(ViewTransform(offsetX, offsetY, zoom));
      dragOffset = tmp[0]->getLabelPos() - G_point(e->x(), e->y());
      objectsDragged[0]->getObject()->tempTransform(ViewTransform(-offsetX * zoom, -offsetY * zoom, 1. / zoom));
      return;
    }


    G_ref *r = doc->getNextSel(tmp, e->state() & Qt::ShiftModifier);

    if(r) {
      if(!(e->state() & Qt::ShiftModifier)) doc->clearSel();

      if(r->getSelected()) doc->delSel(r); else doc->addSel(r);
    }

    //for a constrained point, grab the offset
    if(doc->getSelected().count() == 1 && doc->getSelected()[0]->getType() == G_POINT) {
      dragOffset = doc->getSelected()[0]->getObject()->getPoint() -
	(G_point(e->x(), e->y()) / zoom - G_point(offsetX, offsetY));
    }

    setCursor(Qt::pointingHandCursor);

    if(!(e->state() & Qt::ShiftModifier)) drag = MOVE_DRAG;
    else drag = NO_DRAG;
  }
  else if(e->button() == Qt::RightButton) { //-----------RIGHT BUTTON PRESSED
    G_refs tmp = doc->whatAmIOn(e->x(), e->y(), ViewTransform(offsetX, offsetY, zoom));

    int i;
    if(tmp.count() > 2) return;

    for(i = 0; i < (int)tmp.count(); i++) {
      if(!(tmp[i]->getType() & G_CURVE)) {
	return;
      }
    }


    G_ref *p;

    if(tmp.count() == 2) { //create intersection point
      G_point int1Pt = tmp[0]->getObject()->getCurveRef()->
	getIntersection(tmp[1]->getObject()->getCurveRef());
      G_point int2Pt = tmp[0]->getObject()->getCurveRef()->
	getIntersection(tmp[1]->getObject()->getCurveRef(), 1);

      int1Pt = zoom * (int1Pt + G_point(offsetX, offsetY));
      int2Pt = zoom * (int2Pt + G_point(offsetX, offsetY));

      if((G_point(e->x(), e->y()) - int1Pt).length() < 5) {
	if(!(e->state() & Qt::ShiftModifier)) doc->clearSel();

	p = new G_ref();
	p->create(G_POINT, G_INTERSECTION_POINT, tmp, doc);
	p->update();
	doc->addSel(p);

      }
      else if((G_point(e->x(), e->y()) - int2Pt).length() < 5) {
	if(!(e->state() & Qt::ShiftModifier)) doc->clearSel();

	p = new G_ref();
	p->create(G_POINT, G_INTERSECTION2_POINT, tmp, doc);
	p->update();
	doc->addSel(p);

      }
      else return;
    }

    if(tmp.count() == 1) { //create constrained point
      if(!(e->state() & Qt::ShiftModifier)) doc->clearSel();

      p = new G_ref();
      p->create(G_POINT, G_CONSTRAINED_POINT, tmp, doc);
      ((G_pointObject*)(p->getObject()))->setPoint(G_point(e->x(), e->y()) / zoom - G_point(offsetX, offsetY));
      p->update();
      doc->addSel(p);
    }

    if(tmp.count() == 0) { //create free point
      if(!(e->state() & Qt::ShiftModifier)) doc->clearSel();
	
      p = new G_ref();
      p->create(G_POINT, G_FREE_POINT, G_refs(), doc);
      ((G_pointObject*)(p->getObject()))->setPoint(G_point(e->x(), e->y()) / zoom - G_point(offsetX, offsetY));
      doc->addSel(p);
    }
  }

  updateStatusBar(e->state());
  doc->emitDocumentChanged();
}

//--------------------------MOUSE MOVE EVENT-----------------------------
void KSegView::mouseMoveEvent(QMouseEvent *e)
{
  if(doc == NULL) return;

  if(drag != NO_DRAG) {
    KSegConstructionList::disableRedraws();
  }

  if(drag == NO_DRAG) { //display status message
    updateStatusBar(e->state());
  }

  if(e->x() == lastMouseX && e->y() == lastMouseY) return;

  if(drag == RECTANGLE_DRAG) {
    dragCurX = e->x();
    dragCurY = e->y();

    repaint(false);
  }

  if(drag == LABEL_DRAG) {
    objectsDragged[0]->getObject()->tempTransform(ViewTransform(offsetX, offsetY, zoom));
    objectsDragged[0]->setLabelPos(dragOffset + G_point(e->x(), e->y()));
    objectsDragged[0]->getObject()->tempTransform(ViewTransform(-offsetX * zoom, -offsetY * zoom, 1. / zoom));
    doc->emitDocumentChanged();
    doc->emitDocumentModified();
  }

  if(drag == RECONSTRAIN_DRAG) {
    if(objectsUpdating.count() == 0) {
      objectsUpdating.topologicalSort(objectsDragged);
    }
    
    int i;

    for(i = 0; i < (int)objectsDragged.count(); i++) {
      objectsDragged[i]->getObject()->translate(G_point(e->x() - lastMouseX, e->y() - lastMouseY) / zoom);
    }
    objectsUpdating.update();
    
    doc->emitDocumentChanged();
    doc->emitDocumentModified();
    updateStatusBar(e->state());
  }

  if(drag == MOVE_DRAG) {
    if(objectsDragged.count() == 0 && doc->getSelected().count()) {
      dragPattern = BuildReverse(doc->getSelected());
      objectsDragged.clear();
      hash_map<G_ref *, G_matrix>::iterator it;
      for(it = dragPattern.begin(); it != dragPattern.end(); ++it) {
	objectsDragged.append((*it).first);
      }
      objectsUpdating.topologicalSort(objectsDragged);
    }

    if(objectsDragged.count() == 1 && objectsDragged[0]->getType() == G_POINT &&
       objectsDragged[0]->getDescendType() == G_CONSTRAINED_POINT) { // a constrained point is moving.
      ((G_pointObject *)(objectsDragged[0]->getObject()))->
	setPoint(dragOffset + G_point(e->x(), e->y()) / zoom - G_point(offsetX, offsetY));
      
      objectsUpdating.update();
      lastMouseX = e->x(); lastMouseY = e->y();
      
      doc->emitDocumentChanged();
      doc->emitDocumentModified();
    }
    else {
      hash_map<G_ref *, G_matrix>::iterator it;
      for(it = dragPattern.begin(); it != dragPattern.end(); ++it) {
	(*it).first->getObject()->translate((*it).second *
					    G_point(e->x() - lastMouseX, e->y() - lastMouseY) / zoom);
      }

      objectsUpdating.update();
      
      doc->emitDocumentChanged();
      doc->emitDocumentModified();
    }
  }

  if(drag == PAN_DRAG) {
    offsetX += ((double)(e->x() - lastMouseX)) / zoom;
    offsetY += ((double)(e->y() - lastMouseY)) / zoom;

    redrawBuffer();
  }

  if(drag == ZOOM_DRAG) {
    double diff = e->y() - lastMouseY;
    double scale = pow(2., -diff / 100.);

    offsetX += (1. / scale - 1) * ((double)dragStartX) / zoom;
    offsetY += (1. / scale - 1) * ((double)dragStartY) / zoom;
    zoom *= scale;

    redrawBuffer();
    updateStatusBar(Qt::NoButton);
  }

  lastMouseX = e->x(); lastMouseY = e->y();
}

//--------------------------MOUSE RELEASE EVENT-----------------------------
void KSegView::mouseReleaseEvent(QMouseEvent *e)
{
  if(doc == NULL) return;

  KSegConstructionList::enableRedraws();

  if(drag == NO_DRAG) return;

  if(e->stateAfter() & (Qt::LeftButton | Qt::RightButton | Qt::MidButton) != 0) return;

  if(drag == MOVE_DRAG && doc->isConstruction()) doc->emitDocumentChanged();

  if(drag == RECTANGLE_DRAG) {
    doc->addSelect(QRect(dragStartX, dragStartY, dragCurX - dragStartX,
			 dragCurY - dragStartY).normalize(), ViewTransform(offsetX, offsetY, zoom));

    drag = NO_DRAG;
    doc->emitDocumentChanged();
    return;
  }
  if(drag == RECONSTRAIN_DRAG) {
    if(objectsUpdating.count() == 0) {
      objectsUpdating.topologicalSort(objectsDragged);
    }

    G_point releasePt = zoom * (objectsDragged[0]->getObject()->getPoint() + G_point(offsetX, offsetY));

    G_refs tmp = doc->whatAmIOn(ROUND(releasePt.getX()), ROUND(releasePt.getY()),
				ViewTransform(offsetX, offsetY, zoom), false);

    int i;

    //remove objects to which it cannot be reconstrained
    //here do check for various scripting constraints.
    tmp.removeRef(objectsDragged[0]);

    for(i = 0; i < (int)tmp.count(); i++) {
      if(canReconstrain(objectsDragged[0], tmp[i]) == false) {
	tmp.remove(i);
	i--;
      }
    }

    if(tmp.count() != 0 && tmp[0]->getType() == G_POINT) {
      while(objectsDragged[0]->getChildren().count()) {
	G_refs oldpars;
	G_ref *curChild;

	curChild = objectsDragged[0]->getChildren()[0];
	oldpars = curChild->getParents();

	for(i = 0; i < (int)oldpars.count(); i++) {
	  if(oldpars[i] == objectsDragged[0]) {
	    oldpars.insert(i, tmp[0]);
	    oldpars.remove(i + 1);
	  }
	}

	doc->addUndo(new G_undoReconstrain(curChild));

	curChild->reconstrain(curChild->getDescendType(), oldpars, false); //defer topSort
      }

      if(reconstrainUndo->isChanged(objectsDragged[0])) doc->addUndo(reconstrainUndo);
      else {
	//the next two lines make sure that the undo from deletion
	//has the right position of the point if the original was free.
	((G_pointObject *)(objectsDragged[0]->getObject()))->
	  setPoint(reconstrainUndo->getOldPos());
      }
      reconstrainUndo = 0;
      
      objectsUpdating.update();
      objectsDragged[0]->remove();

      //do a topSort to take care of the reconstraints.
      doc->topSortAllRefs();
      doc->emitDocumentChanged();
    }

    if(tmp.count() == 2 && tmp[0]->getType() & G_CURVE && tmp[1]->getType() & G_CURVE) {
      //make it an intersection point
      G_point int1Pt = tmp[0]->getObject()->getCurveRef()->
	getIntersection(tmp[1]->getObject()->getCurveRef());
      G_point int2Pt = tmp[0]->getObject()->getCurveRef()->
	getIntersection(tmp[1]->getObject()->getCurveRef(), 1);

      int1Pt = zoom * (int1Pt + G_point(offsetX, offsetY));
      int2Pt = zoom * (int2Pt + G_point(offsetX, offsetY));

      if((releasePt - int1Pt).length() < 5) {
	objectsDragged[0]->reconstrain(G_INTERSECTION_POINT, tmp);
	objectsUpdating.update();
	doc->emitDocumentChanged();
      }
      else if((releasePt - int2Pt).length() < 5) {
	objectsDragged[0]->reconstrain(G_INTERSECTION2_POINT, tmp);
	objectsUpdating.update();
	doc->emitDocumentChanged();
      }
    }

    if(tmp.count() == 1 && tmp[0]->getType() & G_CURVE) { //create constrained point

      objectsDragged[0]->reconstrain(G_CONSTRAINED_POINT, tmp);
      ((G_pointObject *)(objectsDragged[0]->getObject()))->setP(BIG);
      objectsUpdating.update();
      doc->emitDocumentChanged();
    }

    if(reconstrainUndo &&
       reconstrainUndo->isChanged(objectsDragged[0])) doc->addUndo(reconstrainUndo);

  }

  if(drag == ZOOM_DRAG) nextZoom = false;
  if(drag == PAN_DRAG) nextPan = false;

  drag = NO_DRAG;
  objectsDragged.clear();
  objectsUpdating.clear();
  updateStatusBar(e->state());
  emit updateMenus();
}

//-------------------------MOUSE DOUBLE CLICK EVENT-----------------------------
void KSegView::mouseDoubleClickEvent(QMouseEvent *e)
{
  if(doc == NULL) return;

  if(e->button() == Qt::LeftButton) {
    G_refs tmp = doc->whatAmIOn(e->x(), e->y(), ViewTransform(offsetX, offsetY, zoom));

    if(tmp.count() == 0 || ((e->state() & Qt::ControlModifier) && (e->state() & Qt::ShiftModifier))) {
      tmp = doc->whatLabelsAmIOn(e->x(), e->y(), ViewTransform(offsetX, offsetY, zoom));

      if(tmp.count() == 0) return;

      doc->clearSel();
      doc->addSel(tmp[0]);

      doc->emitDocumentChanged();

      if(doc->canEditChangeLabel()) doc->editChangeLabel();
      return;
    }
    if(tmp[0]->getType() & G_VALUE) { //start or edit a calculation
      if(tmp[0]->getType() == G_CALCULATE) { //edit
	KSegCalculateEditor *ed;
	G_calculateObject *formula = (G_calculateObject *)(tmp[0]->getObject());
	ed = new KSegCalculateEditor(this, formula->getFormulaString(),
				     tmp[0]->getParents(), tmp[0]);
	ed->run();
	if(ed->result() == QDialog::Rejected) { delete ed; return; }
	formula->changeFormula(ed->getOutputParents(), ed->getOutputString());
	G_refs dependents; dependents.topologicalSort(tmp[0]); dependents.update();
	doc->emitDocumentChanged();
	delete ed;
	return;
      }
      else { //measure
	doc->clearSel();
	doc->addSel(tmp[0]);
	doc->measureCalculate();
      }
    }
    if(tmp[0]->getType() & G_LOCUS) { //change number of samples in locus
      if(tmp[0]->getDescendType() == G_OBJECT_LOCUS) {
	doc->clearSel();
	doc->addSel(tmp[0]);
	doc->editChangeNumberOfSamples();
      }
    }
  }
}

//-------------------------UPDATE STATUS BAR-----------------------------
void KSegView::updateStatusBar(Qt::ButtonState state)
{
  QPoint e = mapFromGlobal(QCursor::pos());

  if(nextPan) {
    emit statusBarMessage(tr("Pan"));
    setCursor(Qt::SizeAllCursor);
    return;
  }

  if(nextZoom) {
    emit statusBarMessage(tr("Zoom"));
    QPixmap p((const char **)zoom_cursor_xpm);
    QBitmap b;
    b = p;
    setCursor(QCursor(b, b, 9, 9));
    return;
  }

  if(drag == NO_DRAG) {
    G_refs tmp = doc->whatAmIOn(e.x(), e.y(), ViewTransform(offsetX, offsetY, zoom));

    if(tmp.count() == 0 || ((state & Qt::ControlModifier) && (state & Qt::ShiftModifier))) {
      tmp = doc->whatLabelsAmIOn(e.x(), e.y(), ViewTransform(offsetX, offsetY, zoom));

      if(tmp.count() == 0) {
	unsetCursor();
	emit statusBarMessage(tr("Ready"));
	return;
      }

      setCursor(Qt::upArrowCursor);

      QString message;

      message = tr("Move label of %1 %2", "E.g. \"Move label of Point P1\"");

      message = message.arg(G_ref::getNameFromType(tmp[0]->getType()));
      message = message.arg(KFormula::toUgly(tmp[0]->getLabel().getText()));

      emit statusBarMessage(message);
      return;
    }

    setCursor(Qt::upArrowCursor);

    QString message;


    G_ref *r = doc->getNextSel(tmp, state & Qt::ShiftModifier);

    if(r) {
      if(!(state & Qt::ShiftModifier)) {
	message = tr("Select %1 %2");
      }
      else {
	if(r->getSelected()) {
	  message = tr("Deselect %1 %2"); 
	}
	else {
	  if(doc->selectedCount() > 0) message = tr("Also select %1 %2");
	  else message = tr("Select %1 %2");
	}
      }

      message = message.arg(G_ref::getNameFromType(r->getType()));
      message = message.arg(KFormula::toUgly(r->getLabel().getText()));
    }
    else message = tr("Move Selected Objects");

    emit statusBarMessage(message);

  }

  if(drag == RECONSTRAIN_DRAG) {
    QString message;
    QString messageZero = tr("Reconstrain %1 %2 to be free");
    QString messageOne = tr("Reconstrain %1 %2 to %3 %4",
			    "E.g., \"Reconstrain Point P1 to Line l1\"");
    QString messageTwo = tr("Reconstrain %1 %2 to the intersection of %3 %4 and %5 %6");

    messageZero = messageZero.arg(G_ref::getNameFromType(objectsDragged[0]->getType()));
    messageOne = messageOne.arg(G_ref::getNameFromType(objectsDragged[0]->getType()));
    messageTwo = messageTwo.arg(G_ref::getNameFromType(objectsDragged[0]->getType()));

    messageZero = messageZero.arg(KFormula::toUgly(objectsDragged[0]->getLabel().getText()));
    messageOne = messageOne.arg(KFormula::toUgly(objectsDragged[0]->getLabel().getText()));
    messageTwo = messageTwo.arg(KFormula::toUgly(objectsDragged[0]->getLabel().getText()));

    
    G_point releasePt = zoom * (objectsDragged[0]->getObject()->getPoint() + G_point(offsetX, offsetY));

    G_refs tmp = doc->whatAmIOn(ROUND(releasePt.getX()), ROUND(releasePt.getY()),
				ViewTransform(offsetX, offsetY, zoom), false);

    int i;

    //remove objects to which it cannot be reconstrained
    //here do check for various scripting constraints.
    tmp.removeRef(objectsDragged[0]);

    //objects after the third do not matter since
    //they cannot possibly play any part in the
    //reconstraint
    for(i = 0; i < 3 && i < (int)tmp.count(); i++) {
      if(canReconstrain(objectsDragged[0], tmp[i]) == false) {
	tmp.remove(i);
	i--;
      }
    }

    if((tmp.count() != 0 && tmp[0]->getType() == G_POINT) ||
       (tmp.count() == 1 && tmp[0]->getType() & G_CURVE)) {
      message = messageOne;
      message = message.arg(G_ref::getNameFromType(tmp[0]->getType()));
      message = message.arg(KFormula::toUgly(tmp[0]->getLabel().getText()));
    }
    else if(tmp.count() == 2 && tmp[0]->getType() & G_CURVE && tmp[1]->getType() & G_CURVE) {
      //check if it's really near the intersection or just near both curves
      G_point int1Pt = tmp[0]->getObject()->getCurveRef()->
	getIntersection(tmp[1]->getObject()->getCurveRef());
      G_point int2Pt = tmp[0]->getObject()->getCurveRef()->
	getIntersection(tmp[1]->getObject()->getCurveRef(), 1);

      int1Pt = zoom * (int1Pt + G_point(offsetX, offsetY));
      int2Pt = zoom * (int2Pt + G_point(offsetX, offsetY));

      if((releasePt - int1Pt).length() < 5 || (releasePt - int2Pt).length() < 5) {
	message = messageTwo;
	message = message.arg(G_ref::getNameFromType(tmp[0]->getType()));
	message = message.arg(KFormula::toUgly(tmp[0]->getLabel().getText()));
	message = message.arg(G_ref::getNameFromType(tmp[1]->getType()));
	message = message.arg(KFormula::toUgly(tmp[1]->getLabel().getText()));
      }
      else message = messageZero;
    }
    else {
      message = messageZero;
    }

    emit statusBarMessage(message);
  }
}


void KSegView::playConstruction(int which)
{
  QPainter p;

  constructionPainter = &p;
  constructionTransform = ViewTransform(offsetX, offsetY, zoom);

  p.begin(this);
  p.setClipRect(buffer.rect());
  
  doc->doPlay(which);

  p.end();
}

void KSegView::updateAllViews()
{
  int i;

  if(KSegProperties::getProperty("SelectType").lower() == "borderselect")
    selectType = BORDER_SELECT;
  else if(KSegProperties::getProperty("SelectType").lower() == "blinkingselect")
    selectType = BLINKING_SELECT;
  
  for(i = 0; i < (int)allViews.count(); ++i) {
    allViews.at(i)->redrawBuffer();
  }
}

void KSegView::viewPan()
{
  nextPan = !nextPan;
  if(nextZoom) nextZoom = false;

  updateStatusBar(Qt::NoButton);
  emit updateMenus();
}

void KSegView::viewZoom()
{
  nextZoom = !nextZoom;
  if(nextPan) nextPan = false;

  updateStatusBar(Qt::NoButton);
  emit updateMenus();
}

void KSegView::viewZoomFit()
{
  QRect tmp = doc->getSize(ViewTransform(0, 0, 1));

  double oldOffsetX = offsetX, oldOffsetY = offsetY, oldZoom = zoom;

  if(tmp.isEmpty() || tmp.isNull()) return;

  QRect r = rect();
  r.setWidth(r.width() - ScrollBarThickness);
  r.setHeight(r.height() - ScrollBarThickness);
  
  zoom = QMIN(((double)r.width()) / (1.05 * (double)tmp.width()),
	      ((double)r.height()) / (1.05 * (double)tmp.height()));

  QPoint rc = r.center();
  QPoint tc = tmp.center();

  offsetX = ((double)rc.x()) / zoom - tc.x();
  offsetY = ((double)rc.y()) / zoom - tc.y();

  zoomAnimation(oldOffsetX, oldOffsetY, oldZoom);
  updateStatusBar(Qt::NoButton);
}


void KSegView::viewOrigZoom()
{
  double oldOffsetX = offsetX, oldOffsetY = offsetY, oldZoom = zoom;

  offsetX = 0;
  offsetY = 0;
  zoom = 1;

  zoomAnimation(oldOffsetX, oldOffsetY, oldZoom);
  updateStatusBar(Qt::NoButton);
}

void KSegView::zoomAnimation(double oldOffsetX, double oldOffsetY, double oldZoom)
{
  QTime t;
  t.start();

  double newOffsetX = offsetX, newOffsetY = offsetY, newZoom = zoom;

  int zoomTime = 700;
  
  DragType oldDrag = drag;
  drag = ZOOM_DRAG; //to prevent menus from updating

  if(fabs(offsetX - oldOffsetX) + fabs(offsetY - oldOffsetY) + fabs(zoom - oldZoom) > 0.0001)
  while(t.elapsed() < zoomTime) {
    double p = ((double)t.elapsed()) / ((double)zoomTime);

    zoom = exp(p * log(newZoom) + (1 - p) * log(oldZoom));
    if(fabs(oldZoom - newZoom) > 0.000001) p = (zoom - oldZoom) / (newZoom - oldZoom);
    offsetX = ((1 - p) * oldOffsetX * oldZoom + p * (newOffsetX * newZoom)) / zoom;
    offsetY = ((1 - p) * oldOffsetY * oldZoom + p * (newOffsetY * newZoom)) / zoom;
    redrawBuffer();
    QApplication::syncX();
  }

  offsetX = newOffsetX;
  offsetY = newOffsetY;
  zoom = newZoom;
  redrawBuffer();

  drag = oldDrag;
}



