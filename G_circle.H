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


#ifndef G_CIRCLE_H
#define G_CIRCLE_H

#include "G_point.H"

class G_circle : public G_curve
{
public:
  G_circle() { }
  G_circle(const G_circle &c) : G_curve() { p1 = c.p1; r = c.r; }
  G_circle(const G_point &P1, double radius) { p1 = P1; r = radius;  }
  
  virtual G_geometry *copy() const { return new G_circle(*this); }

  virtual G_Type getType() const { return G_CIRCLE; }

  //drawing:
  virtual void draw(QPainter &p, const G_drawstyle &d, bool selected);

  //calculations:
  virtual QRect getExtents(void) const
  { return QRect(ROUND(p1.getX() - r - 6), ROUND(p1.getY() - r - 6),
		 ROUND(r * 2 + 12), ROUND(r * 2 + 12)); }

  virtual G_point getNearestPoint(const G_point &) const;
  virtual bool inRect(const QRect &) const;

  //transformations:
  virtual void translate(const G_point &p) { p1.translate(p); }
  virtual void rotate(const G_point &p, double d) { p1.rotate(p, d); }
  virtual void reflect(const G_straight &s) { p1.reflect(s); }
  virtual void scale(const G_point &p, double d) { p1.scale(p, d); r *= d;}

  //parametrization:
  virtual G_point getPointOnCurve(double p) const;
  virtual double getParamFromPoint(const G_point &p) const;

  //intersection:
  virtual G_point getIntersection(const G_curve *, int which = 0) const;

  bool isValid() const { return p1.isValid() && r >= 0; }
  static G_circle inValid() { return G_circle(G_point::inValid(), 0); }

  G_point getCenter() const { return p1; }
  double getRadius() const { return r; }

private:
  G_point p1;
  double r;
};

#endif //G_CIRCLE_H
