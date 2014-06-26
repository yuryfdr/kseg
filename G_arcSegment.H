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


#ifndef G_ARCSEGMENT_H
#define G_ARCSEGMENT_H

#include "G_arc.H"

class G_arcSegment : public G_filled
{
public:
  G_arcSegment() { }
  G_arcSegment(const G_arcSegment &a) : G_filled() { arc = a.arc; }
  G_arcSegment(const G_arc &a) { arc = a; }
  
  virtual G_geometry *copy() const { return new G_arcSegment(*this); }

  virtual G_Type getType() const { return G_ARCSEGMENT; }

  //drawing:
  virtual void draw(QPainter &p, const G_drawstyle &d, bool selected);

  //calculations:
  virtual QRect getExtents(void) const { return arc.getExtents(); }
  virtual G_point getNearestPoint(const G_point &) const;
  virtual bool inRect(const QRect &) const;

  virtual double getArea() const;

  //transformations:
  virtual void translate(const G_point &p) { arc.translate(p); }
  virtual void rotate(const G_point &p, double d) { arc.rotate(p, d); }
  virtual void reflect(const G_straight &s) { arc.reflect(s); }
  virtual void scale(const G_point &p, double d) { arc.scale(p, d); }

  bool isValid() const { return arc.isValid(); }
  static G_arcSegment inValid() { return G_arcSegment(G_arc::inValid()); }

private:
  G_arc arc;
};

#endif //G_ARCSEGMENT_H
