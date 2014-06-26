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


#include "G_locus.H"


G_locus::G_locus(const G_locus &l) : G_geometry()
{
  int i;
  
  objects.setAutoDelete(true);
  objects.clear();

  Q3PtrListIterator<G_geometry> it(l.objects);

  for(i = 0; i < (int)l.objects.count(); i++) {
    objects.append((*it)->copy());
    ++it;
  }
}


G_locus & G_locus::operator=(const G_locus &l)
{
  int i;
  
  objects.setAutoDelete(true);
  objects.clear();

  Q3PtrListIterator<G_geometry> it(l.objects);

  for(i = 0; i < (int)l.objects.count(); i++) {
    objects.append((*it)->copy());
    ++it;
  }

  return *this;
}


void G_locus::draw(QPainter &p, const G_drawstyle &d, bool selected)
{
  int i;

  for(i = 0; i < (int)objects.count(); i++) {
    objects.at(i)->draw(p,d, selected);
  }
}

QRect G_locus::getExtents(void) const
{
  int i;
  QRect r;

  if(objects.count() == 0) return r;

  Q3PtrListIterator<G_geometry> it(objects);
  r = (*it)->getExtents();

  for(i = 1; i < (int)objects.count(); i++) {
    ++it;
    r |= (*it)->getExtents();
  }

  return r;
}

G_point G_locus::getNearestPoint(const G_point &p) const
{
  int i;

  Q3PtrListIterator<G_geometry> it(objects);
  G_point m = (*it)->getNearestPoint(p);

  //fix it so it's replaced by the first valid point
  if(!m.isValid()) m = G_point(BIG, BIG);

  for(i = 1; i < (int)objects.count(); i++) {
    ++it;

    G_point q = (*it)->getNearestPoint(p);
    if(!q.isValid()) continue;

    if((m - p).lengthsq() > (q - p).lengthsq()) {
      m = q;
    }
  }

  return m;
}

bool G_locus::inRect(const QRect &r) const
{
  int i;

  Q3PtrListIterator<G_geometry> it(objects);

  for(i = 0; i < (int)objects.count(); i++) {
    if((*it)->inRect(r)) return true;
    ++it;
  }

  return false;
}


void G_locus::translate(const G_point &p)
{
  int i;

  for(i = 0; i < (int)objects.count(); i++) {
    objects.at(i)->translate(p);
  }
}

void G_locus::rotate(const G_point &p, double d)
{
  int i;

  for(i = 0; i < (int)objects.count(); i++) {
    objects.at(i)->rotate(p, d);
  }
}

void G_locus::reflect(const G_straight &s)
{
  int i;

  for(i = 0; i < (int)objects.count(); i++) {
    objects.at(i)->reflect(s);
  }
}

void G_locus::scale(const G_point &p, double d)
{
  int i;

  for(i = 0; i < (int)objects.count(); i++) {
    objects.at(i)->scale(p, d);
  }
}
