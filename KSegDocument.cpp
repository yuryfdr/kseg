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


#include <zlib.h>
#include <qbuffer.h>
#include <QPixmap>

#include "KSegDocument.H"
#include "KSegConstruction.H"
#include "KSegProperties.H"
#include "G_ref.H"
#include "G_object.H"
#include "G_refSearcher.H"
#include "KSegSelectionGroup.H"


KSegDocument::KSegDocument() : isUndoing(false), isRedoing(false),
		 undoStack(this), redoStack(this), changed(false)
{
  connect(this, SIGNAL(documentChanged()), this, SLOT(IChanged()));
  printer = new QPrinter();
  printer->setOrientation(QPrinter::Landscape);
  defaultDrawstyle = G_drawstyle::defaultStyle();
  setCurrentColor(QColor(KSegProperties::getProperty("ForegroundColor")));
  numViews = 0;
}

KSegDocument::~KSegDocument()
{
  delete printer;
  defaultDrawstyle->deleteReference();

  int i;

  for(i = 0; i < (int)(allRefs.size()); ++i) {
    delete allRefs[i]; //delete all the remaining objects
  }
}

void KSegDocument::addRef(G_ref *inRef)
{ 
  allRefs.append(inRef);
  if (inRef->getSelected()) { selectedRefs.append(inRef); }
  emitDocumentModified();
}

void KSegDocument::delRef(G_ref *inRef)
{
  allRefs.removeRef(inRef);
  selectedRefs.removeRef(inRef);

  if(t_vector.containsRef(inRef)) t_vector.clear();
  if(t_ratio.containsRef(inRef)) t_ratio.clear();
  if(t_mirror.containsRef(inRef)) t_mirror.clear();
  if(t_angle.containsRef(inRef)) t_angle.clear();
  if(t_center.containsRef(inRef)) t_center.clear();
  emitDocumentModified();
}


struct VT {
  VT(G_refs &r, const ViewTransform &inTrans) : refs(r), trans(inTrans) {
    for(int i = 0; i < (int)refs.size(); ++i) {
      if(refs[i]->isDrawn()) refs[i]->getObject()->tempTransform(trans);
    }
  }
  
  ~VT() {
    ViewTransform inv(-trans.offsetX * trans.zoom, -trans.offsetY * trans.zoom, 1. / trans.zoom);
    
    for(int i = 0; i < (int)refs.size(); ++i) {
      if(refs[i]->isDrawn()) refs[i]->getObject()->tempTransform(inv);
    }
  }

private:
  G_refs &refs;
  ViewTransform trans;
};


void KSegDocument::draw(QPainter &p, const ViewTransform &trans)
{
  VT tempTrans(allRefs, trans);
  int i;

  //first draw the markings on marked objects:
  {
    QPen markPen(QColor(255, 255, 0), 5);
    G_drawstyle *markStyle = G_drawstyle::match(ANY, NULL, &markPen);

    //draw the vector
    if(t_vector.count() && t_vector[0]->isDrawn() && t_vector[1]->isDrawn()) {
      G_segment s(t_vector[0]->getObject()->getPoint(),
		  t_vector[1]->getObject()->getPoint());
      
      s.draw(p, *markStyle, false);

      G_point dir = s.getDirection();

      dir.rotate(G_point(0, 0), 0.7);

      G_segment(s.getP2(), s.getP2() - dir * 25).
	draw(p, *markStyle, false);

      dir.rotate(G_point(0, 0), -1.4);

      G_segment(s.getP2(), s.getP2() - dir * 25).
	draw(p, *markStyle, false);
    }

    //draw the mirror
    if(t_mirror.count() && t_mirror[0]->isDrawn()) {
      G_line l(t_mirror[0]->getObject()->getNearestPoint(G_point(0, 0)),
	       t_mirror[0]->getObject()->getStraightRef()->getDirection());

      l.draw(p, *markStyle, false);
    }

    //draw the center
    if(t_center.count() && t_center[0]->isDrawn()) {
      G_circle(t_center.at(0)->getObject()->getPoint(), 6).
	draw(p, *markStyle, false);
    }

    //draw the angle marks
    if(t_angle.count() == 1 && t_angle[0]->isDrawn()) {
      //if the marked angle is a value object
      QPixmap pm(1, 1);
      QPainter pp(&pm);

      //draw it so the size is right
      t_angle[0]->getObject()->draw(pp);

      QRect r = t_angle[0]->getObject()->getSelectExtents();
      
      G_point tl(r.topLeft()), br(r.bottomRight());
      G_point bl(r.bottomLeft()), tr(r.topRight());
      
      G_segment(tl, bl).draw(p, *markStyle, false);
      G_segment(tl, tr).draw(p, *markStyle, false);
      G_segment(br, bl).draw(p, *markStyle, false);
      G_segment(br, tr).draw(p, *markStyle, false);      
    }
    else if(t_angle.count() && t_angle[0]->isDrawn() &&
	    t_angle[1]->isDrawn() && t_angle[2]->isDrawn()) {
      G_point p1, p2, p3, p4;
      p1 = t_angle[0]->getObject()->getPoint();
      p2 = t_angle[1]->getObject()->getPoint();
      p3 = t_angle[2]->getObject()->getPoint();
      
      double radius = QMIN((p1 - p2).length(), (p2 - p3).length());

      p1 = p2 + radius * (p1 - p2).normalize();
      p3 = p2 + radius * (p3 - p2).normalize();
      p4 = p2 + radius * ((p1 + p3) / 2. - p2).normalize();

      G_arc(p1, p4, p3).draw(p, *markStyle, false);

      G_point dir = (p3 - p2).r90().normalize();

      if((p3 - p1) * dir < 0) dir = dir * -1;

      dir.rotate(G_point(0, 0), 0.7);

      G_segment(p3, p3 - dir * 25).
	draw(p, *markStyle, false);

      dir.rotate(G_point(0, 0), -1.4);

      G_segment(p3, p3 - dir * 25).
	draw(p, *markStyle, false);
    }

    //draw the ratio marks
    if(t_ratio.count() == 1 && t_ratio[0]->isDrawn()) {
      //if the marked ratio is a value object
      QPixmap pm(1, 1);
      QPainter pp(&pm);

      //draw it so the size is right
      t_ratio[0]->getObject()->draw(pp);

      QRect r = t_ratio[0]->getObject()->getSelectExtents();
      
      G_point tl(r.topLeft()), br(r.bottomRight());
      G_point bl(r.bottomLeft()), tr(r.topRight());
      
      G_segment(tl, bl).draw(p, *markStyle, false);
      G_segment(tl, tr).draw(p, *markStyle, false);
      G_segment(br, bl).draw(p, *markStyle, false);
      G_segment(br, tr).draw(p, *markStyle, false);      
    }
    else if(t_ratio.count() && t_ratio[0]->isDrawn() && t_ratio[1]->isDrawn()) {
      QPen ratioPen(QColor(255, 255, 0), 3);
      G_drawstyle *ratioStyle = G_drawstyle::match(ANY, NULL, &ratioPen);

      for(i = 0; i < 5; i++) {
	G_point p1, p2;

	p1 = t_ratio[0]->getObject()->getSegment().
	  getPointOnCurve(double(i) / 4.);
	p2 = t_ratio[1]->getObject()->getSegment().
	  getPointOnCurve(double(i) / 4.);

	G_segment(p1, p2).draw(p, *ratioStyle, false);

	G_point dir = (p2 - p1).normalize();

	dir.rotate(G_point(0, 0), 0.7);
	
	G_segment(p2, p2 - dir * 20).
	  draw(p, *ratioStyle, false);
	
	dir.rotate(G_point(0, 0), -1.4);
	
	G_segment(p2, p2 - dir * 20).
	  draw(p, *ratioStyle, false);
      }

      ratioStyle->deleteReference();
    }

    markStyle->deleteReference();
  }
    
  //then draw the labels:
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->isDrawn()) allRefs[i]->drawLabel(p);
  }

  //then draw the unselected Filled stuff
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() & G_FILLED && allRefs[i]->getSelected() == false &&
       allRefs[i]->isDrawn()) {
      allRefs[i]->getObject()->draw(p);
    }
  }

  //then draw selected Filled stuff
  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & G_FILLED && selectedRefs[i]->isDrawn()) {
      selectedRefs[i]->getObject()->draw(p);
    }
  }

  //then draw the unselected other stuff
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() != G_POINT && (allRefs[i]->getType() & G_FILLED) == 0
       && allRefs[i]->getSelected() == false &&
       allRefs[i]->isDrawn()) {
      allRefs[i]->getObject()->draw(p);
    }
  }


  //then draw selected other stuff
  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() != G_POINT && (selectedRefs[i]->getType() & G_FILLED) == 0 &&
       selectedRefs[i]->isDrawn()) {
      selectedRefs[i]->getObject()->draw(p);
    }
  }

  //then draw the unselected points:
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() == G_POINT && allRefs[i]->getSelected() == false &&
       allRefs[i]->isDrawn()) {
      allRefs[i]->getObject()->draw(p);
    }
  }


  //finally draw the selected points:
  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() == G_POINT && selectedRefs[i]->isDrawn()) {
      selectedRefs[i]->getObject()->draw(p);
    }
  }

}


void KSegDocument::print(QPainter &p, const ViewTransform &trans)
{ //kind of like draw, but doesn't draw markings or selection.
  VT tempTrans(allRefs, trans);
  int i;

  //first draw the labels as if unselected:
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->isDrawn()) {
      bool oldSelected = allRefs[i]->getSelected();
      allRefs[i]->setSelected(false); //we don't want selection to be drawn.
      allRefs[i]->drawLabel(p);
      allRefs[i]->setSelected(oldSelected);
    }
  }

  //then draw all the filled stuff unselected
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() & G_FILLED == 0) continue;
    if(allRefs[i]->isDrawn() == false) continue;
    bool oldSelected = allRefs[i]->getSelected();
    allRefs[i]->setSelected(false); //we don't want selection to be drawn.
    allRefs[i]->getObject()->draw(p);
    allRefs[i]->setSelected(oldSelected);
  }

  //then draw everything except filled and points unselected
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() == G_POINT) continue;
    if(allRefs[i]->getType() & G_FILLED) continue;
    if(allRefs[i]->isDrawn() == false) continue;
    bool oldSelected = allRefs[i]->getSelected();
    allRefs[i]->setSelected(false); //we don't want selection to be drawn.
    allRefs[i]->getObject()->draw(p);
    allRefs[i]->setSelected(oldSelected);
  }

  //then draw all the points unselected
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() != G_POINT) continue;
    if(allRefs[i]->isDrawn() == false) continue;
    bool oldSelected = allRefs[i]->getSelected();
    allRefs[i]->setSelected(false); //we don't want selection to be drawn.
    allRefs[i]->getObject()->draw(p);
    allRefs[i]->setSelected(oldSelected);
  }

  afterLoadDraw();
}


void KSegDocument::afterLoadDraw()
{
  QPixmap pm(1, 1);
  QPainter p(&pm);

  int i;

  for(i = 0; i < (int)allRefs.count(); i++) {
    allRefs[i]->getLabel().draw(p, *(allRefs[i]->getDrawstyle()), false);
  }  
}



QRect KSegDocument::getSize(const ViewTransform &trans)
{
  VT tempTrans(allRefs, trans);
  int i;
  QRect tmp = QRect();

  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->isDrawn()) {
      tmp |= allRefs[i]->getObject()->getScrollExtents();
    }
  }

  return tmp;
}

G_refs KSegDocument::whatLabelsAmIOn(int x, int y, const ViewTransform &trans)
{
  VT tempTrans(allRefs, trans);
  int i;
  G_refs tmp;

  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->isDrawn() && allRefs[i]->getLabelVisible()) {
      if(allRefs[i]->getLabelRect().contains(QPoint(x, y))) {
	tmp.append(allRefs[i]);
      }
    }
  }

  return tmp;
}

G_refs KSegDocument::whatAmIOn(int x, int y, const ViewTransform &trans, bool pointsOnly)
{
  VT tempTrans(allRefs, trans);
  int i;
  G_refs tmp;


  //points first
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() == G_POINT && allRefs[i]->isDrawn()) {
      G_point p = (allRefs[i]->getObject()->getNearestPoint(G_point(x, y)) - G_point(x, y));
      if(p * p <= 36) {
	tmp.append(allRefs[i]);
      }
    }    
  }

  if(pointsOnly && tmp.count()) return tmp;

  //then everything else except filled stuff
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() != G_POINT && (allRefs[i]->getType() & G_FILLED) == 0
       && allRefs[i]->isDrawn()) {
      if(allRefs[i]->getObject()->getSelectExtents().contains(QPoint(x, y))) {
	G_point p = (allRefs[i]->getObject()->getNearestPoint(G_point(x, y)) - G_point(x, y));
	if(p * p <= 25) {
	  tmp.append(allRefs[i]);
	}
      }
    }    
  }

  if(tmp.count()) return tmp;

  //then the filled stuff
  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() & G_FILLED && allRefs[i]->isDrawn()) {
      if(allRefs[i]->getObject()->getSelectExtents().contains(QPoint(x, y))) {
	G_point p = (allRefs[i]->getObject()->getNearestPoint(G_point(x, y)) - G_point(x, y));
	if(p * p <= 25) {
	  tmp.append(allRefs[i]);
	}
      }
    }    
  }

  return tmp;
}

void KSegDocument::addSelect(const QRect &r, const ViewTransform &trans)
{
  VT tempTrans(allRefs, trans);
  int i;

  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->isDrawn() && !allRefs[i]->getSelected() && allRefs[i]->getObject()->inRect(r))
      addSel(allRefs[i]);
  }
}

G_ref *KSegDocument::getNextSel(G_refs inRefs, bool shiftDown)
{

  if(inRefs.count() == 0) return NULL;

  int i;
  bool flag;

  flag = true; //whether all are selected

  for(i = 0; i < (int)inRefs.count(); i++) {
    if(!inRefs[i]->getSelected()) {
      flag = false;
      break;
    }
  }

  if(flag) {
    if(!shiftDown) return NULL; //don't touch anything if all is selected.
    else return inRefs[0]; //if all are selected and shift is down, deselect the first one.
  }
  

  //the idea is to be able to cycle selection by clicking multiple times.

  //if shift is not down, select the "next" object: find an unselected group
  //then a selected group after that one and select the first unselected element
  //after the selected group.  Looks like this:
  //USSUU -> UUUSU, USUSU -> UUSUU, UU -> SU, UUS -> SUU 
  if(!shiftDown) {
    //find the end of the unselected group:
    for(i = 0; i < (int)inRefs.count(); i++) {
      if(inRefs[i]->getSelected()) break;
    }

    if(i >= (int)inRefs.count() - 1) return inRefs[0];

    //find the end of the selected group:
    for(; i < (int)inRefs.count(); i++) {
      if(!(inRefs[i]->getSelected())) break;
    }

    if(i >= (int)inRefs.count()) return inRefs[0];
    else return inRefs[i];
    
  }

  //if shift is down, if the first object is selected or no object is selected,
  //select the first unselected object.  otherwise, deselect the first selected object:
  //SSSUU -> SSSSU, USSS -> UUSS, UUUU -> SUUU, UUSUS -> UUUUS
  if(shiftDown) {
    if(inRefs[0]->getSelected()) {
      for(i = 0; i < (int)inRefs.count(); i++) {
	if(!(inRefs[i]->getSelected())) return inRefs[i]; // select the first unselected object
      }
    }

    for(i = 0; i < (int)inRefs.count(); i++) {
      if(inRefs[i]->getSelected()) return inRefs[i]; //deselect the first selected object
    }

    //if none are selected, select the first one.
    return inRefs[0];
  }

  return NULL; //to keep the compiler from complaining--this will never happen.
}

void KSegDocument::addSel(G_ref *inRef)
{
  if(inRef->getSelected()) return;
  //selectedRefs should not contain it yet if it's not selected.
  selectedRefs.append(inRef);
  inRef->setSelected(true);
}

void KSegDocument::delSel(G_ref *inRef)
{
  selectedRefs.removeRef(inRef);
  inRef->setSelected(false);
}

void KSegDocument::clearSel()
{
  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    selectedRefs[i]->setSelected(false);
  }

  selectedRefs.clear();
}


//checks if freeing point p would disrupt a locus chain
bool KSegDocument::isPointFreeable(G_ref *p)
{
  int i;

  //if one of its kids is an object locus, return false.
  for(i = 0; i < (int)p->getChildren().count(); i++) {
    if(p->getChildren()[i]->getType() == G_LOCUS && 
       p->getChildren()[i]->getDescendType() == G_OBJECT_LOCUS)
      return false;
  }

  AncestorSearcher as(p);
  DescendantSearcher ds(p);

  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() != G_LOCUS) continue;
    if(allRefs[i]->getDescendType() != G_OBJECT_LOCUS) continue;

    if(ds.search(allRefs[i]) && as.search(allRefs[i]->getParents()[0])) {
      BrokenLocusChainSearcher blcs(allRefs[i]->getParents()[1], p);

      if(blcs.search(allRefs[i]->getParents()[0]) == false)
	return false;
    }
  }

  return true;
}


QString KSegDocument::autoLabel(G_ref *r)
{
  int t = r->getType(); // easier to type
  int n = labelData[t];

  QString labl;

  if((t & G_GEOMETRIC) && IS_TRANSFORM(r->getDescendType())) {
    labl = r->getParents()[0]->getLabel().getText();
    labl.append("'");
    
    return labl;
  }

  switch(t) {
  case G_POINT:
    while(n >= 0) {
      labl.prepend('A' + n % 26);
      n = n / 26 - 1;
    }
    break;
    
  case G_SEGMENT:
    labl = QString("s_(%1)").arg(n + 1);
    break;

  case G_CIRCLE:
    labl = QString("c_(%1)").arg(n + 1);
    break;

  case G_LINE:
    labl = QString("l_(%1)").arg(n + 1);
    break;

  case G_RAY:
    labl = QString("r_(%1)").arg(n + 1);
    break;

  case G_ARC:
    labl = QString("a_(%1)").arg(n + 1);
    break;

  case G_MEASURE:
    labl = QString("M%1").arg(n + 1);
    break;

  case G_CALCULATE:
    labl = QString("C%1").arg(n + 1);
    break;

  case G_ARCSECTOR:
    labl = QString("Sec%1").arg(n + 1);
    break;

  case G_ARCSEGMENT:
    labl = QString("Seg%1").arg(n + 1);
    break;

  case G_CIRCLEINTERIOR:
    labl = QString("CI%1").arg(n + 1);
    break;

  case G_POLYGON:
    labl = QString("P%1").arg(n + 1);
    break;

  case G_LOCUS:
    labl = r->getParents()[1]->getLabel().getText();
    return labl;
    break;
  }

  labelData[t]++;
  return KFormula::fromUgly(labl);
}


QColor KSegDocument::getCurrentColor()
{
  if(!canEditChangeColor()) {
    Q_ASSERT(canEditChangeColor());
  }

  if(selectedRefs.count() == 0)
    return defaultDrawstyle->getPen().color();

  QColor color, color2;
  int i;

  if(selectedRefs[0]->getType() & G_FILLED)
    color = selectedRefs[0]->getDrawstyle()->getBrush().color();
  else
    color = selectedRefs[0]->getDrawstyle()->getPen().color();

  for(i = 0; i < (int)selectedRefs.count(); ++i) {
    if(selectedRefs[i]->getType() & G_FILLED)
      color2 = selectedRefs[i]->getDrawstyle()->getBrush().color();
    else
      color2 = selectedRefs[i]->getDrawstyle()->getPen().color();    

    if(color2 != color) return QColor();
    // invalid color returned to indicate different colors in selection
  }

  return color;
}

void KSegDocument::setCurrentColor(QColor color)
{
  if(!canEditChangeColor()) {
    Q_ASSERT(canEditChangeColor());
  }

#define CHANGE_STYLE(x)                                                          \
  {                                                                              \
    QPen p = (x)->getPen();                                                      \
    QBrush b = (x)->getBrush();                                                  \
    p.setColor(color);                                                           \
    b.setColor(color);                                                           \
    G_drawstyle *oldX = (x);                                                     \
    (x) = G_drawstyle::match((x)->getPointStyle(), &((x)->getFont()), &p, &b);   \
    oldX->deleteReference();                                                     \
  }

  if(selectedRefs.count() == 0) {
    CHANGE_STYLE(defaultDrawstyle)
    return;
  }

  int i;

  for(i = 0; i < (int)selectedRefs.count(); ++i) {
    G_drawstyle *x = selectedRefs[i]->getDrawstyle();

    QPen p = (x)->getPen();                                                      
    QBrush b = (x)->getBrush();                              
    p.setColor(color);
    b.setColor(color);
    G_drawstyle *newstyle =
      G_drawstyle::match((x)->getPointStyle(), &((x)->getFont()), &p, &b);   
    selectedRefs[i]->changeDrawstyle(newstyle);
  }

  emit documentChanged();
  emitDocumentModified();

#undef CHANGE_STYLE
}


Qt::PenStyle KSegDocument::getCurrentPenstyle()
{
  if(!canEditChangeLinestyle()) {
    Q_ASSERT(canEditChangeLinestyle());
  }

  if(selectedRefs.count() == 0)
    return defaultDrawstyle->getPen().style();

  Qt::PenStyle style, style2;
  int i;

  style = selectedRefs[0]->getDrawstyle()->getPen().style();

  for(i = 1; i < (int)(selectedRefs.count()); ++i) {
    style2 = selectedRefs[i]->getDrawstyle()->getPen().style();

    if(style != style2) return Qt::NoPen;
  }

  return style;
}


void KSegDocument::setCurrentPenstyle(Qt::PenStyle style)
{
  if(selectedRefs.count() == 0) {
    QPen p = defaultDrawstyle->getPen();
    p.setStyle(style);
    G_drawstyle *old = defaultDrawstyle;

    defaultDrawstyle = G_drawstyle::match(old->getPointStyle(), &(old->getFont()),
					  &p, &(old->getBrush()));
    old->deleteReference();
    return;
  }

  int i;

  for(i = 0; i < (int)(selectedRefs.count()); ++i) {
    G_drawstyle *x = selectedRefs[i]->getDrawstyle();

    QPen p = (x)->getPen();                                                      
    p.setStyle(style);

    G_drawstyle *newstyle =
      G_drawstyle::match((x)->getPointStyle(), &((x)->getFont()), &p,
			 &(x->getBrush()));   
    selectedRefs[i]->changeDrawstyle(newstyle);
  }

  emit documentChanged();
  emitDocumentModified();
  
}



int KSegDocument::getCurrentPenWidth()
{
  if(!canEditChangeLinestyle()) {
    Q_ASSERT(canEditChangeLinestyle());
  }

  if(selectedRefs.count() == 0)
    return defaultDrawstyle->getPen().width();

  int width, width2;
  int i;

  width = selectedRefs[0]->getDrawstyle()->getPen().width();

  for(i = 1; i < (int)(selectedRefs.count()); ++i) {
    width2 = selectedRefs[i]->getDrawstyle()->getPen().width();

    if(width != width2) return -1;
  }

  return width;
}


void KSegDocument::setCurrentPenWidth(int width)
{
  if(selectedRefs.count() == 0) {
    QPen p = defaultDrawstyle->getPen();
    p.setWidth(width);
    G_drawstyle *old = defaultDrawstyle;

    defaultDrawstyle = G_drawstyle::match(old->getPointStyle(), &(old->getFont()),
					  &p, &(old->getBrush()));
    old->deleteReference();
    return;
  }

  int i;

  for(i = 0; i < (int)(selectedRefs.count()); ++i) {
    G_drawstyle *x = selectedRefs[i]->getDrawstyle();

    QPen p = (x)->getPen();                                                      
    p.setWidth(width);

    G_drawstyle *newstyle =
      G_drawstyle::match((x)->getPointStyle(), &((x)->getFont()), &p,
			 &(x->getBrush()));   
    selectedRefs[i]->changeDrawstyle(newstyle);
  }

  emit documentChanged();
  emitDocumentModified();
  
}


PointStyle KSegDocument::getCurrentPointstyle()
{
  if(!canEditChangePointstyle()) {
    Q_ASSERT(canEditChangePointstyle());
  }

  if(selectedRefs.count() == 0)
    return defaultDrawstyle->getPointStyle();

  PointStyle style, style2;
  int i;

  style = selectedRefs[0]->getDrawstyle()->getPointStyle();

  for(i = 1; i < (int)(selectedRefs.count()); ++i) {
    style2 = selectedRefs[i]->getDrawstyle()->getPointStyle();

    if(style != style2) return ANY;
  }

  return style;
}


void KSegDocument::setCurrentPointstyle(PointStyle style)
{
  if(selectedRefs.count() == 0) {
    G_drawstyle *old = defaultDrawstyle;

    defaultDrawstyle = G_drawstyle::match(style, &(old->getFont()),
					  &(old->getPen()), &(old->getBrush()));
    old->deleteReference();
    return;
  }

  int i;

  for(i = 0; i < (int)(selectedRefs.count()); ++i) {
    G_drawstyle *x = selectedRefs[i]->getDrawstyle();

    G_drawstyle *newstyle =
      G_drawstyle::match(style, &((x)->getFont()), &((x)->getPen()),
			 &(x->getBrush()));   
    selectedRefs[i]->changeDrawstyle(newstyle);
  }

  emit documentChanged();
  emitDocumentModified();
  
}


int KSegDocument::getCurrentFontsize()
{
  if(!canEditChangeFont()) {
    Q_ASSERT(canEditChangeFont());
  }

  if(selectedRefs.count() == 0)
    return defaultDrawstyle->getFont().pointSize();

  int size, size2;
  int i;

  size = selectedRefs[0]->getDrawstyle()->getFont().pointSize();

  for(i = 1; i < (int)(selectedRefs.count()); ++i) {
    size2 = selectedRefs[i]->getDrawstyle()->getFont().pointSize();

    if(size != size2) return -1;
  }

  return size;
}


void KSegDocument::setCurrentFontsize(int size)
{
  if(selectedRefs.count() == 0) {
    QFont f = defaultDrawstyle->getFont();
    f.setPointSize(size);
    G_drawstyle *old = defaultDrawstyle;

    defaultDrawstyle = G_drawstyle::match(old->getPointStyle(), &f,
					  &(old->getPen()), &(old->getBrush()));
    old->deleteReference();
    return;
  }

  int i;

  for(i = 0; i < (int)(selectedRefs.count()); ++i) {
    G_drawstyle *x = selectedRefs[i]->getDrawstyle();

    QFont f = (x)->getFont();                                                      
    f.setPointSize(size);

    G_drawstyle *newstyle =
      G_drawstyle::match((x)->getPointStyle(), &f, &((x)->getPen()),
			 &(x->getBrush()));   
    selectedRefs[i]->changeDrawstyle(newstyle);
  }

  emit documentChanged();
  emitDocumentModified();
  
}


QFont KSegDocument::getCurrentFont()
{
  if(!canEditChangeFont()) {
    Q_ASSERT(canEditChangeFont());
  }

  if(selectedRefs.count() == 0)
    return defaultDrawstyle->getFont();

  QFont font, font2;
  int i;

  font = selectedRefs[0]->getDrawstyle()->getFont();

  for(i = 1; i < (int)(selectedRefs.count()); ++i) {
    font2 = selectedRefs[i]->getDrawstyle()->getFont();

    if(font != font2) return QFont();
  }

  return font;
}


void KSegDocument::setCurrentFont(const QFont &font)
{
  if(selectedRefs.count() == 0) {
    G_drawstyle *old = defaultDrawstyle;

    defaultDrawstyle = G_drawstyle::match(old->getPointStyle(), &font,
					  &(old->getPen()), &(old->getBrush()));
    old->deleteReference();
    return;
  }

  int i;


  for(i = 0; i < (int)(selectedRefs.count()); ++i) {
    G_drawstyle *x = selectedRefs[i]->getDrawstyle();

    G_drawstyle *newstyle =
      G_drawstyle::match((x)->getPointStyle(), &font, &((x)->getPen()),
			 &(x->getBrush()));   
    selectedRefs[i]->changeDrawstyle(newstyle);
  }

  emit documentChanged();
  emitDocumentModified();
}



//these guys aren't member functions!
QDataStream &operator<<(QDataStream &streamx, KSegDocument &doc)
{
  int i;

  if(doc.isConstruction()) {
    streamx << QString("KSeg Construction Version 0.2");

    //save the given types so that constructions can be quickly extracted and
    //it's easy to check if they can be applied
    KSegConstruction *d = (KSegConstruction *)(&doc);
    streamx << (int)d->given.size();
    for(i = 0; i < (int)d->given.size(); ++i) {
      streamx << d->given[i]->whatCanItBe();
    }
  }
  else {
    streamx << QString("KSeg Document Version 0.2");
  }

  QByteArray array;
  QBuffer buf(&array);

  buf.open(QIODevice::WriteOnly);
  QDataStream stream(&buf);
  stream.setVersion(3);

  QMap<G_drawstyle *, short> styleMap = G_drawstyle::saveUsed(stream, doc.allRefs);

  stream << doc.allRefs.count();

  hash_map<G_ref *, int> refMap;

  doc.allRefs.topologicalSort(doc.allRefs);    

  //this assumes all the stuff is topologically sorted

  for(i = 0; i < (int)doc.allRefs.count(); i++) {
    refMap[doc.allRefs[i]] = i;

    stream << styleMap[doc.allRefs[i]->getDrawstyle()];

    stream << (short)(doc.allRefs[i]->getParents().count());
    int j;
    for(j = 0; j < (int)(doc.allRefs[i]->getParents().count()); j++) {
      stream << refMap[doc.allRefs[i]->getParents()[j]];
    }

    stream << *(doc.allRefs[i]);
  }

  if(doc.isConstruction()) { //save the order of the givens
    KSegConstruction *d = (KSegConstruction *)(&doc);
    stream << (int)d->given.size();
    for(i = 0; i < (int)d->given.size(); ++i) {
      stream << refMap[d->given[i]];
    }
  }

  //now save the selection groups
  int selGroupCount = 0;
  for(i = 0; i < (int)doc.selectionGroups.size(); ++i) {
    if(doc.selectionGroups[i]->size() > 0) selGroupCount++;
  }

  stream << selGroupCount;
  for(i = 0; i < (int)doc.selectionGroups.size(); ++i) {
    if(doc.selectionGroups[i]->size() > 0) doc.selectionGroups[i]->save(stream, refMap);
  }

  //done saving data

  buf.close();

  streamx << array;

  return streamx;
}


QDataStream &operator>>(QDataStream &streamx, KSegDocument &doc)
{
  QString version;

  streamx >> version;

  int majVer, minVer;
  bool isConstr;

  KSegDocument::parseVersionString(version, &isConstr, &majVer, &minVer);

  if(isConstr && (majVer > 0 || minVer > 0)) {
    //ignore the information for quick check:
    unsigned int i;
    int dummy;
    
    streamx >> i;
    while(i--) streamx >> dummy;
  }

  QByteArray array;

  if(majVer == 0 && minVer < 2) { //old files come compressed
    unsigned long origSize;
    int origTmp;

    streamx >> origTmp;
    origSize = origTmp;

    QByteArray compressed;

    streamx >> compressed;
    
    array.resize(origSize + 2);

    uncompress((Bytef *)array.data(), &origSize, (Bytef *)compressed.data(), compressed.size());
  }
  else { //now space is cheap
    streamx >> array;
  }

  QBuffer buf(&array);

  buf.open(QIODevice::ReadOnly);
  QDataStream stream(&buf);
  stream.setVersion(3);

  QVector<G_drawstyle *> styleArray = G_drawstyle::loadUsed(stream);

  int numRefs, i;

  stream >> numRefs;

  for(i = 0; i < numRefs; i++) {
    G_ref *ref = new G_ref(&doc);

    short style;
    stream >> style;
    
    styleArray[style]->addReference(); //add an extra reference for this style
    ref->setDrawstyle(styleArray[style]);

    short numParents;
    stream >> numParents;

    int j;
    for(j = 0; j < numParents; j++) {
      int parent;
      stream >> parent;

      ref->getParents().append(doc.allRefs[parent]);
    }

    stream >> *ref;
    
  }

  for(i = 0; i < (int)(styleArray.size()); ++i) {
    styleArray[i]->deleteReference(); //there's one extraneous reference
    //for each style necessary for the style to exist before it is assigned
    //to any objects.  Now is the time to delete these extraneous references.
  }

  if(isConstr) { //load the order of the givens
    KSegConstruction *d = (KSegConstruction *)(&doc);
    
    int numGivens;
    stream >> numGivens;

    for(i = 0; i < numGivens; ++i) {
      int given;
      stream >> given;
      d->given.append(doc.allRefs[given]);
    }
  }  

  if(majVer > 0 || minVer > 1) { //load selection groups
    int numGroups;
    stream >> numGroups;
    doc.selectionGroups.resize(numGroups);
    for(i = 0; i < numGroups; ++i) {
      doc.selectionGroups[i] = new KSegSelectionGroup(&doc);
      doc.selectionGroups[i]->load(stream);
    }
  }

  buf.close();
  
  doc.allRefs.update();

  doc.afterLoadDraw();

  doc.undoStack.clear();

  return streamx;
}


//parses a string like "KSeg Document Version 2.3" and returns true if successful
bool KSegDocument::parseVersionString(const QString &s, bool *isConstruction,
				      int *majorVersion, int *minorVersion)
{
  if(s.startsWith("KSeg Document Version ") &&
     s.length() == QString("KSeg Document Version 0.0").length()) {
    if(isConstruction != NULL) *isConstruction = false;
  }
  else if(s.startsWith("KSeg Construction Version ") &&
     s.length() == QString("KSeg Construction Version 0.0").length()) {
    if(isConstruction != NULL) *isConstruction = true;
  }
  else return false;

  bool ok;
  int minV, majV;
  
  minV = s.right(1).toInt(&ok);
  if(!ok) return false;
  
  majV = s.right(3).left(1).toInt(&ok);
  if(!ok) return false;
  
  if(minorVersion != NULL) *minorVersion = minV;
  if(majorVersion != NULL) *majorVersion = majV;

  return true;
}


