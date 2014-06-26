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


#include <stdio.h>
#include <qregexp.h>
#include <qapplication.h>

#include "G_object.H"
#include "G_drawstyle.H"
#include "KSegDocument.H"

void G_measureObject::calculate()
{
  QString tmp;

  switch(where->getDescendType()) {
  case G_DISTANCE_MEASURE:
    value = (
	     parent(0)->getPoint() -
	     parent(1)->getGeoRef()->getNearestPoint(parent(0)->getPoint())
	    ).length();
    break;
  case G_LENGTH_MEASURE:
    if(where->getParents()[0]->getType() == G_ARC) {
      value = parent(0)->getArc().getArcLength();
    }
    else if(where->getParents()[0]->getType() == G_CIRCLE) {
      value = parent(0)->getCircle().getRadius() * 2 * M_PI;
      break;
    }
    else { //parent is a segment
      value = parent(0)->getSegment().length();
    }

    break;
    
  case G_RADIUS_MEASURE:
    if(where->getParents()[0]->getType() == G_ARC) {
      value = parent(0)->getArc().getRadius();
    }
    else { //parent is a circle
      value = parent(0)->getCircle().getRadius();
    }
    break;

  case G_ANGLE_MEASURE:
    if(where->getParents()[0]->getType() == G_ARC) {
      value = parent(0)->getArc().getArcAngle() * 180. / M_PI;
    }
    else { //three points
      value = (parent(2)->getPoint() - parent(1)->getPoint()).angle() -
       	(parent(0)->getPoint() - parent(1)->getPoint()).angle();
      value = fmod(-value + 4 * M_PI, 2 * M_PI) * 180. / M_PI;
    }
    break;
    
  case G_RATIO_MEASURE:
    if(parent(1)->getSegment().length() == 0) {    
      value = BIG * 10;
    }
    else {
      value = parent(0)->getSegment().length() /
	parent(1)->getSegment().length();
    }
      
    break;

  case G_SLOPE_MEASURE:
    {
      G_point p = parent(0)->getStraightRef()->getDirection();
      if(p.getX() == 0) {
	value = BIG * 10;
      }
      else {
	value = -p.getY() / p.getX();
      }
    }

    break;

  case G_AREA_MEASURE:
    value = parent(0)->getFilledRef()->getArea();
    break;

  }

  return;
}


class ParentLabelReferenceFetcher : public ReferenceFetcher
{
public:
  ParentLabelReferenceFetcher(G_ref *inRef) { ref = inRef; }

  virtual KFormula *getFormula(int reference_number)
  { return ref->getParents()[reference_number]->getLabel().getFormula(); }

protected:
  G_ref *ref;
};


void G_measureObject::updatelhs()
{
  QString tmp;

  lhs.setReferenceFetcher(new ParentLabelReferenceFetcher(where));

  switch(where->getDescendType()) {
  case G_DISTANCE_MEASURE:
    tmp = qApp->translate("G_measureObject", "Distance {%1} {%2}", "");
    break;
  case G_LENGTH_MEASURE:
    if(where->getParents()[0]->getType() == G_CIRCLE) {
      tmp = qApp->translate("G_measureObject", "Circumference {%1}", "");
      break;
    }

    tmp = qApp->translate("G_measureObject", "Length {%1}", "");

    break;
    
  case G_RADIUS_MEASURE:
    tmp = qApp->translate("G_measureObject", "Radius {%1}", "");

    break;

  case G_ANGLE_MEASURE:
    if(where->getParents()[0]->getType() == G_ARC) {
      tmp = qApp->translate("G_measureObject", "Arc Angle {%1}", "");
    }
    else { //three points
      tmp = qApp->translate("G_measureObject", "Angle {%1}{%2}{%3}", "");
    }
    break;
    
  case G_RATIO_MEASURE:
    tmp = qApp->translate("G_measureObject", "Ratio {%1}/{%2}", "");

    break;

  case G_SLOPE_MEASURE:
    tmp = qApp->translate("G_measureObject", "Slope {%1}", "");

    break;

  case G_AREA_MEASURE:
    tmp = qApp->translate("G_measureObject", "Area {%1}", "");

    break;
  }

  tmp.replace(QRegExp("%1"), QString((QChar)int(REFERENCE_NUM(0))));
  tmp.replace(QRegExp("%2"), QString((QChar)int(REFERENCE_NUM(1))));
  tmp.replace(QRegExp("%3"), QString((QChar)int(REFERENCE_NUM(2))));

  lhs.parse(tmp);
  dirtylhs = false;

  return;
}


//this class is for having text reference lhs.
class LhsReferenceFetcher : public ReferenceFetcher
{
public:
  LhsReferenceFetcher(G_valueObject *inValObj) { valueObject = inValObj; }
  
  virtual KFormula* getFormula(int) { return valueObject->getLhs(); }
  
protected:
  G_valueObject *valueObject;
};


void G_valueObject::update()
{
  if(dirtylhs) updatelhs();
  calculate();

  text.setReferenceFetcher(new LhsReferenceFetcher(this));

  QString tmp = QChar(int(REFERENCE_NUM(0))) + QString(" = %1");

  if(fabs(value) >= BIG) {
    tmp = tmp.arg(qApp->translate("G_valueObject", "undefined", ""));
  }
  else {
    tmp = tmp.arg(QString::number(value, 'f', 4));
  }

  text.parse(tmp);
	     
  return;
}


bool G_valueObject::inRect(QRect r)
{
  QRect tmp;
  
  tmp.setSize(text.size());
  tmp.moveCenter(pos.toQPoint());

  return tmp.intersects(r);
}


G_point G_valueObject::getNearestPoint(const G_point &p)
{
  QRect tmp;
  
  tmp.setSize(text.size());
  tmp.moveCenter(pos.toQPoint());

  if(tmp.contains(p.toQPoint())) return p;
  else return pos; //TODO! this is wrong, but I'm too bored to do it right now.
}


void G_valueObject::draw(QPainter &p)
{
  QRect r = p.window();

  if((pos - G_point(r.center())).lengthsq() > DRAW_MAX * DRAW_MAX) return;

  text.setPos(ROUND(pos.getX()), ROUND(pos.getY()));

  p.setFont(where->getDrawstyle()->getFont());

  p.setPen(QPen(where->getDrawstyle()->getPen().color()));

  lhs.makeDirty();
  text.makeDirty();

  text.redraw(p);

  if(where->getSelected()) {
    p.setBrush(QBrush());
    p.setPen(QPen(G_drawstyle::getBorderColor(where->getDrawstyle()->getPen().color()), 2));
    p.drawRect(getSelectExtents());
  }

}




