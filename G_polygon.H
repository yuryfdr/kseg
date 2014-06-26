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


#ifndef G_POLYGON_H
#define G_POLYGON_H

#include <vector>
#include "G_point.H"


class G_polygon : public G_filled
{
public:
  G_polygon() { }
  G_polygon(const G_polygon &p) : G_filled() { points = p.points; }
  G_polygon(vector<G_point> p) : G_filled() { points = p; }
  
  virtual G_geometry *copy() const { return new G_polygon(*this); }

  virtual G_Type getType() const { return G_POLYGON; }

  //drawing:
  virtual void draw(QPainter &p, const G_drawstyle &d, bool selected);

  //calculations:
  virtual QRect getExtents(void) const;
  virtual G_point getNearestPoint(const G_point &) const;
  virtual bool inRect(const QRect &) const;

  virtual double getArea() const;

  //transformations:
  virtual void translate(const G_point &p);
  virtual void rotate(const G_point &p, double d);
  virtual void reflect(const G_straight &s);
  virtual void scale(const G_point &p, double d);

  bool isValid() const;
  static G_polygon inValid() { return G_polygon(); }

private:
  vector<G_point> points;
};

#endif //G_POLYGON_H
