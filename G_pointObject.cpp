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


#include "G_object.H"

void G_pointObject::update()
{

  if(IS_TRANSFORM(where->getDescendType())) {
    point = parent(0)->getPoint();
    updateTransform();
  }
  else switch(where->getDescendType()) {
  case G_FREE_POINT:
    break;
  case G_CONSTRAINED_POINT:
    if(p == BIG) {
      point = parent(0)->getNearestPoint(point);
      p = parent(0)->getCurveRef()->getParamFromPoint(point);
    }
    else {
      point = parent(0)->getCurveRef()->getPointOnCurve(p);
    }
    break;
  case G_INTERSECTION_POINT:
    point = parent(0)->getCurveRef()->getIntersection(parent(1)->getCurveRef());
    break;
  case G_INTERSECTION2_POINT:
    point = parent(0)->getCurveRef()->getIntersection(parent(1)->getCurveRef(), 1);
    break;
  case G_MID_POINT:
    point = parent(0)->getSegment().getMidpoint();
    break;
  case G_END_POINT:
    point = parent(0)->getCurveRef()->getPointOnCurve(0);
    break;
  case G_END2_POINT:
    point = parent(0)->getCurveRef()->getPointOnCurve(1);
    break;
  case G_CENTER_POINT:
    if(where->getParents()[0]->getType() == G_CIRCLE)
      point = parent(0)->getCircle().getCenter();
    else point = parent(0)->getArc().getCenter();
    break;
  }

  if(!point.isValid()) where->setExists(false);

  return;
}

void G_pointObject::save(QDataStream &stream)
{ 
  if(where->getDescendType() == G_FREE_POINT) {
    stream << point; 
  }
  else if(where->getDescendType() == G_CONSTRAINED_POINT) {
    stream << p;
  }
}

void G_pointObject::load(QDataStream &stream) 
{ 
  if(where->getDescendType() == G_FREE_POINT) {
    stream >> point; 
  }
  else if(where->getDescendType() == G_CONSTRAINED_POINT) {
    stream >> p;
  }
}
