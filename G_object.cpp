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
#include <stdio.h>

G_object::G_object()
{

}

G_object::~G_object()
{

}

void G_object::tempTransform(const ViewTransform &trans)
{
  G_geometry *geo = getGeoRef();

  geo->translate(G_point(trans.offsetX, trans.offsetY));
  geo->scale(G_point(0, 0), trans.zoom);
}

void G_valueObject::tempTransform(const ViewTransform &trans)
{
  pos.translate(G_point(trans.offsetX, trans.offsetY));
  pos.scale(G_point(0, 0), trans.zoom);
}

void G_object::draw(QPainter &p)
{
  getGeoRef()->draw(p, *(where->getDrawstyle()), where->getSelected());
}

void G_object::update()
{

}

//this function is used by subclasses.
void G_object::updateTransform()
{
  switch(where->getDescendType()) {
  case G_TRANSLATED:
    getGeoRef()->translate(parent(2)->getPoint() - parent(1)->getPoint());
    break;
  case G_ROTATED: {
    double angle;
    if(where->getParents().count() == 3) {
      if(fabs(parent(2)->getNumValue()) >= BIG) {
	where->setExists(false);
	return;
      }
      angle = (360. - parent(2)->getNumValue()) / 180. * M_PI;
    }
    else {
      G_point v1 = parent(4)->getPoint() - parent(3)->getPoint();
      G_point v2 = parent(2)->getPoint() - parent(3)->getPoint();
      angle = asin((v2 % v1) / sqrt(v1.lengthsq() * v2.lengthsq()));

      if(v2 * v1 < 0) angle = M_PI - angle;
    }

    getGeoRef()->rotate(parent(1)->getPoint(), angle);
    break;
  }
  case G_SCALED: {
    double scale;

    if(where->getParents().count() == 3) {
      scale = parent(2)->getNumValue();
      if(fabs(scale) >= BIG) {
	where->setExists(false);
	return;
      }
    }
    else {
      scale = parent(3)->getSegment().length() /
	parent(2)->getSegment().length();
    }

    getGeoRef()->scale(parent(1)->getPoint(), scale);

    break;
  }
  case G_REFLECTED:
    getGeoRef()->reflect(*(parent(1)->getStraightRef()));
    break;
  }

  if(!getGeoRef()->isValid()) where->setExists(false);
}
