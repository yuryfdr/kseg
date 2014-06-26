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

void G_polygonObject::update()
{

  if(IS_TRANSFORM(where->getDescendType())) {
    polygon = *((G_polygon *)parent(0)->getGeoRef());
    updateTransform();
  }
  else switch(where->getDescendType()) {
  case G_DEFAULT_FILLED: {
    vector<G_point> pts;
    int i;

    for(i = 0; i < (int)where->getParentsConst().size(); ++i) {
      pts.push_back(parent(i)->getPoint());
    }

    polygon = G_polygon(pts);
    break;
  }
  }

  if(!polygon.isValid()) where->setExists(false);

  return;
}



