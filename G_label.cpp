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


#include <qpixmap.h>

#include "G_label.H"
#include "G_drawstyle.H"
#include "G_ref.H"
#include "G_object.H"

void G_label::draw(QPainter &p, const G_drawstyle &d, bool selected)
{
  p.setFont(d.getFont());
  if(!selected) p.setPen(QPen(d.getPen().color()));
  else p.setPen(QPen(QColor(255, 0, 0)));

  G_point pt = getPos();

  if(!pt.isValid()) { output.setPos(0, 0);  output.redraw(p); return; }

  output.makeDirty();

  output.setPos(ROUND(pt.getX()), ROUND(pt.getY()));
  output.redraw(p);
}

void G_label::setString(const QString &inText) //assumes input in ugly form
{ 
  text = KFormula::fromUgly(inText);
  output.parse(text);

  QPixmap tmp(1,1);
  QPainter tmpp(&tmp);
  tmpp.setFont(ref->getDrawstyle()->getFont());
  output.redraw(tmpp);

  setPos(getPos());
}

void G_label::setText(const QString &inText, bool inUpdatePos) //for "internal use"
{ 
  text = inText; 
  output.parse(text); 
  
  QPixmap tmp(1,1);
  QPainter tmpp(&tmp);
  tmpp.setFont(ref->getDrawstyle()->getFont());
  output.redraw(tmpp);
  
  if(inUpdatePos)
    setPos(getPos());
}

QRect G_label::getRect()
{
  QRect tmp;

  tmp.setSize(getSize());
  tmp.moveCenter(getPos().toQPoint());

  return tmp;
}


QSize G_label::getSize()
{
  return output.size();
}


G_point G_label::getPos() const
{
  if(ref->getExists() == false) return G_point::inValid();
  if(ref->getType() & G_CURVE) {
    return relCoord + ref->getObject()->getCurveRef()->getPointOnCurve(relPos);
  }
  else if(ref->getType() == G_POINT) {
    return relCoord + ref->getObject()->getPoint();
  }
  else return G_point::inValid();
}


void G_label::setPos(const G_point &inPoint)
{
  int i;

  relPos = 0.5;
  relCoord = G_point(0, 0);

  if(!inPoint.isValid()) return;

  relCoord = inPoint - getPos();

  QRect r = getRect();

  //if the object is totally inside the label rectangle, we're all set.
  if(r.contains(ref->getObject()->getSelectExtents())) return;

  G_point minPoint, curPoint;
  G_point minOffset, curOffset;
  double minDist = 999999;

  curOffset = G_point(r.topLeft());

  int step = 3;

  //scan top side
  for(i = 0; i <= r.width(); i += step) {
    curPoint = ref->getObject()->getNearestPoint(curOffset);
    if((curPoint - curOffset).length() < minDist) {
      minDist = (curPoint - curOffset).length();
      minPoint = curPoint;
      minOffset = curOffset;
    }

    curOffset = curOffset + G_point(step, 0);
  }

  //scan right side
  for(i = 0; i <= r.height(); i += step) {
    curPoint = ref->getObject()->getNearestPoint(curOffset);
    if((curPoint - curOffset).length() < minDist) {
      minDist = (curPoint - curOffset).length();
      minPoint = curPoint;
      minOffset = curOffset;
    }

    curOffset = curOffset + G_point(0, step);
  }

  //scan bottom side
  for(i = 0; i <= r.width(); i += step) {
    curPoint = ref->getObject()->getNearestPoint(curOffset);
    if((curPoint - curOffset).length() < minDist) {
      minDist = (curPoint - curOffset).length();
      minPoint = curPoint;
      minOffset = curOffset;
    }

    curOffset = curOffset - G_point(step, 0);
  }

  //scan left side
  for(i = 0; i <= r.height(); i += step) {
    curPoint = ref->getObject()->getNearestPoint(curOffset);
    if((curPoint - curOffset).length() < minDist) {
      minDist = (curPoint - curOffset).length();
      minPoint = curPoint;
      minOffset = curOffset;
    }

    curOffset = curOffset - G_point(0, step);
  }

  G_point relc = minOffset - minPoint;

  if(minDist > LABEL_DISTANCE) {
    relc.normalize();
    relc = relc * LABEL_DISTANCE;
  }

  relCoord = relc + (G_point(r.center()) - minOffset);

  if(ref->getType() & G_CURVE) {
    relPos = ref->getObject()->getCurveRef()->getParamFromPoint(minPoint);
  }

  return;
}


QDataStream &operator<<(QDataStream &stream, G_label &label)
{
  QString tmp = label.text;

  //modify tmp so old versions can read it
  for(int i = 0; i < (int)tmp.length(); ++i)
    if(tmp[i].unicode() > UNUSED_OFFSET &&
       tmp[i].unicode() < UNUSED_OFFSET + 300)
      tmp[i] = QChar((int)(tmp[i].unicode()) +
		     (OLD_UNUSED_OFFSET - UNUSED_OFFSET));

  stream << tmp << label.relCoord;

  if(label.ref->getType() & G_CURVE) {
    stream << label.relPos;
  }

  return stream;
}


QDataStream &operator>>(QDataStream &stream, G_label &label)
{
  stream >> label.text >> label.relCoord;

  if(label.ref->getType() & G_CURVE) {
    stream >> label.relPos;
  }

  //modify text from old files
  for(int i = 0; i < (int)label.text.length(); ++i)
    if(label.text[i].unicode() > OLD_UNUSED_OFFSET &&
       label.text[i].unicode() < OLD_UNUSED_OFFSET + 300)
      label.text[i] = QChar(label.text[i].unicode() +
			    (UNUSED_OFFSET - OLD_UNUSED_OFFSET));

  label.output.parse(label.text);

  return stream;
}

