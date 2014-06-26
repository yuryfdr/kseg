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
//Added by qt3to4:
#include <Q3PtrList>

//structure for storing adaptively sampled points
//in a "linked heap"
struct G_l_pt {
  double x;	// location (from 0 to 1)
  G_point loc;
  int next, prev;
  double good;
};


// swap two elements, preserving the links
// the heap functions should only move
// elements using this swap.
void swap(G_l_pt * x, int i, int j)
{
	G_l_pt tmp;

	tmp = x[j];

	if(x[i].prev != -1) x[x[i].prev].next = j;
	if(x[i].next != -1) x[x[i].next].prev = j;

	if(tmp.prev != -1) x[tmp.prev].next = i;
	if(tmp.next != -1) x[tmp.next].prev = i;

	tmp = x[j];

	x[j] = x[i];

	x[i] = tmp;

	return;
	
}

int floatup(G_l_pt * ptr, int n)
{ // float the last element up the heap.
	int i, j;

	for(i = n - 1; i > 0;) {
		j = (i - 1) / 2;
		if(ptr[i].good > ptr[j].good) break;

		swap(ptr, i, j);

		i = j;
	}

	return i;
}

int decroot(G_l_pt * ptr, int n, int i)
{
  //the ith element in a heap of size n is out of place--update the heap
  
	if(i > 0 && ptr[i].good < ptr[(i - 1) / 2].good) { //check
		return floatup(ptr, i + 1);
	}

	//float the root down the heap.  takes O(lg n) time

	while(i * 2 + 1 < n) {
		if(i * 2 + 2 == n) {
			if(ptr[i].good > ptr[i * 2 + 1].good) {
				swap(ptr, i, i * 2 + 1);
				i = i * 2 + 1;
			}
			break;
		}

		if(ptr[i].good <= ptr[i * 2 + 1].good) {
			if(ptr[i].good > ptr[i * 2 + 2].good) {
				swap(ptr, i, i * 2 + 2);
				i = i * 2 + 2;
				continue;
			}
			break;
		}

		if(ptr[i * 2 + 1].good < ptr[i * 2 + 2].good) {
			swap(ptr, i, i * 2 + 1);
			i = i * 2 + 1;
		}
		else {
			swap(ptr, i, i * 2 + 2);
			i = i * 2 + 2;
		}

	}

	return i;
}


//this function returns the goodness less than 0--bad, 1--good
double getGood(G_point p1, G_point p2, G_point p3)
{
  if(p1.isValid() == false && p2.isValid() == false) return 1;
  if(p3.isValid() == false && p2.isValid() == false) return 1;
  if(p1.isValid() == false && p3.isValid() == false) return 1;

  if(p1.isValid() == false) return 1 - (p3 - p2).length() / 500.;
  if(p2.isValid() == false) return 1 - (p1 - p3).length() / 1000.;
  if(p3.isValid() == false) return 1 - (p1 - p2).length() / 500;

  G_arc a(p1, p2, p3);
  if(a.isValid() == false) return 1;
  if(a.getRadius() == 0) return 1;
  return  2 - a.getArcLength() / ((p3 - p2).length() + (p2 - p1).length());
}



void G_locusObject::generatePointLocus()
{
  G_refs updateQueue;

  int i;

  double p;

  double oldP = ((G_pointObject *)parent(0))->getP();

  updateQueue.topologicalSort(where->getParents()[0],
			      where->getParents()[1]);
  updateQueue.append(where->getParents()[1]);

  //storage:

  G_l_pt *points = new G_l_pt[maxSamples];

  //number of them used so far:
  int numSamples = 0;

  //compute how many initial (dumb) samples should be used
  int initialSamples = int(sqrt(double(maxSamples) / 2.));
  if(initialSamples < 10) initialSamples = 10;
  if(initialSamples > maxSamples) initialSamples = maxSamples;

  //initialize the linked heap
  for(i = 0; i < initialSamples; i++) {
    p = double(i) / double(initialSamples - 1);

    if(where->getParents()[0]->getParents()[0]->getType() == G_LINE) {
      p = (p - 0.5) * .999 + 0.5;
    }
    if(where->getParents()[0]->getParents()[0]->getType() == G_RAY) {
      p = p * .999 + 0.001;
    }
    
    ((G_pointObject *)parent(0))->setP(p);
    updateQueue.update(true); //true means do not reparse formulas
    
    if(where->getParents()[1]->getExists())
      points[i].loc = *((G_point *)(parent(1)->getGeoRef()));
    else points[i].loc = G_point::inValid();
    points[i].x = p;

    points[i].prev = i - 1;
    points[i].next = i + 1;

    numSamples++;
  }

  points[numSamples - 1].next = -1;

  points[0].good = points[numSamples - 1].good = 1;

  for(i = 1; i < numSamples - 1; i++) {
    points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			     points[points[i].next].loc);
  }

  //now set up the heap
  for(i = 1; i < numSamples; i++) {
    floatup(points, i);
  }

  int discontCount = 0;

  //-----------now adaptively sample
  while(numSamples < maxSamples - 1) {
    if(points[0].good > 0.9999) break; // if the worst one is this good, we're done.

    //now we add two new points:
    p = (points[0].x + points[points[0].prev].x) / 2.;
    
    ((G_pointObject *)parent(0))->setP(p);
    updateQueue.update(true); //true means do not reparse formulas
    
    if(where->getParents()[1]->getExists())
      points[numSamples].loc = *((G_point *)(parent(1)->getGeoRef()));
    else points[numSamples].loc = G_point::inValid();

    points[numSamples].x = p;

    //now set up the links for the first added point:
    points[numSamples].prev = points[0].prev;
    points[numSamples].next = 0;
    points[points[0].prev].next = numSamples;
    points[0].prev = numSamples;

    //now the second point:
    p = (points[0].x + points[points[0].next].x) / 2.;
    
    ((G_pointObject *)parent(0))->setP(p);
    updateQueue.update(true); //true means do not reparse formulas
    
    if(where->getParents()[1]->getExists())
      points[numSamples + 1].loc = *((G_point *)(parent(1)->getGeoRef()));
    else points[numSamples + 1].loc = G_point::inValid();

    points[numSamples + 1].x = p;

    //now set up the links for the second added point:
    points[numSamples + 1].next = points[0].next;
    points[numSamples + 1].prev = 0;
    points[points[0].next].prev = numSamples + 1;
    points[0].next = numSamples + 1;

    double oldGood = points[0].good;

    //now compute the new goodnesses and resort the heap:
    i = 0;
    points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			     points[points[i].next].loc);
    decroot(points, numSamples, i);

    i = points[numSamples].prev;
    if(points[i].prev != -1) {
      points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			       points[points[i].next].loc);
      decroot(points, numSamples, i);
    }

    i = points[numSamples + 1].next;
    if(points[i].next != -1) {
      points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			       points[points[i].next].loc);
      decroot(points, numSamples, i);
    }
    

    i = numSamples;
    points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			     points[points[i].next].loc);
    floatup(points, i);

    i = numSamples + 1;
    points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			     points[points[i].next].loc);
    floatup(points, i);

    numSamples += 2;

    if(oldGood > points[0].good) {
      discontCount++;
    }
    else discontCount = 0; //number of times the goodness decreased--for finding discontinuities

    if(discontCount > log((double)maxSamples) / 1.5) {
      points[0].loc = G_point::inValid();
      points[points[0].next].loc = G_point::inValid();
      points[points[0].prev].loc = G_point::inValid();
      discontCount = 0;

      i = 0;
      points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			       points[points[i].next].loc);
      i = points[0].next;
      points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			       points[points[i].next].loc);
      i = points[0].prev;
      points[i].good = getGood(points[points[i].prev].loc, points[i].loc,
			       points[points[i].next].loc);
      
      for(i = 1; i < numSamples; i++) {
	floatup(points, i);
      }
    }
  }


  //---------now clean up
  ((G_pointObject *)parent(0))->setP(oldP);
  
  updateQueue.update(true); //true means do not reparse formulas

  //find the "first" memeber of the heap
  for(i = 0; i < numSamples; i++) {
    if(points[i].prev == -1) break; //it's the one without a previous member.
  }

  Q3PtrList<G_geometry> myList;

  while(--numSamples) { //make numSamples - 1 segments
    int j = points[i].next;
    
    if(points[i].loc.isValid() && points[j].loc.isValid())
      myList.append(new G_segment(points[i].loc, points[j].loc));

    i = j;
  }

  locus = G_locus(myList);
 

  delete[] points;
}
