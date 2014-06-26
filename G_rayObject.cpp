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

void G_rayObject::update()
{

  if(IS_TRANSFORM(where->getDescendType())) {
    ray = parent(0)->getRay();
    updateTransform();
  }
  else switch(where->getDescendType()) {
  case G_TWOPOINTS_RAY:
    ray = G_ray(parent(0)->getPoint(), parent(1)->getPoint() - parent(0)->getPoint());
    break;
  case G_BISECTOR_RAY: {
    G_point p1, p2, p3;
    p1 = parent(0)->getPoint();
    p2 = parent(1)->getPoint();
    p3 = parent(2)->getPoint();

    p1 -= p2;
    p3 -= p2;

    if(p1.lengthsq() < SMALL || p2.lengthsq() < SMALL) {
      ray = G_ray::inValid();
      break;
    }

    p1.normalize();
    p3.normalize();

    p1 += p3;

    if(p1.lengthsq() < SMALL) {
      ray = G_ray::inValid();
      break;
    }

    ray = G_ray(p2, p1);

    break;
  }
  }

  if(!ray.isValid()) where->setExists(false);

  return;
}



