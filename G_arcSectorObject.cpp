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

#include "G_object.H"

void G_arcSectorObject::update()
{

  if(IS_TRANSFORM(where->getDescendType())) {
    arcSector = *((G_arcSector *)(parent(0)->getGeoRef()));
    updateTransform();
  }
  else switch(where->getDescendType()) {
  case G_DEFAULT_FILLED:
    arcSector = G_arcSector(parent(0)->getArc());
    break;
  }

  if(!arcSector.isValid()) where->setExists(false);

  return;
}





