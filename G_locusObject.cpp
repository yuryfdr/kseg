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

void G_locusObject::update()
{

  if(IS_TRANSFORM(where->getDescendType())) {
    locus = parent(0)->getLocus();
    updateTransform();
  }
  else switch(where->getDescendType()) {
  case G_OBJECT_LOCUS:
    if(parent(1)->getGeoRef()->getType() == G_POINT)
      generatePointLocus();
    else
      generateObjectLocus();
    break;
  }

  if(!locus.isValid()) where->setExists(false);

  return;
}


void G_locusObject::generateObjectLocus()
{
  QVector<G_geometry*> myList;

  G_refs updateQueue;

  int i;

  double p;

  double oldP = ((G_pointObject *)parent(0))->getP();

  updateQueue.topologicalSort(where->getParents()[0],
			      where->getParents()[1]);
  updateQueue.append(where->getParents()[1]);

  for(i = 0; i < maxSamples; i++) {
    p = double(i) / double(maxSamples - 1);

    ((G_pointObject *)parent(0))->setP(p);

    updateQueue.update(true); //true means do not reparse formulas

    if(parent(1)->getGeoRef()->isValid())
      myList.append(parent(1)->getGeoRef()->copy());

  }

  ((G_pointObject *)parent(0))->setP(oldP);
  
  updateQueue.update(true); //true means do not reparse formulas

  locus = G_locus(myList);

  return;
}

