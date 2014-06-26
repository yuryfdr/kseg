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


#ifndef G_ARC_H
#define G_ARC_H

#include "G_point.H"

class G_arc : public G_curve
{
public:
  G_arc() { }
  G_arc(const G_arc &a) : G_curve() { p1 = a.p1; p2 = a.p2; cosangle = a.cosangle; cw = a.cw; }
  G_arc(const G_point &P1, const G_point &P2, const G_point &P3);
  
  virtual G_geometry *copy() const { return new G_arc(*this); }

  virtual G_Type getType() const { return G_ARC; }

  //drawing:
  virtual void draw(QPainter &p, const G_drawstyle &d, bool selected);

  //calculations:
  virtual QRect getExtents(void) const;
  virtual G_point getNearestPoint(const G_point &) const;
  virtual bool inRect(const QRect &) const;

  //transformations:
  virtual void translate(const G_point &p) { p1.translate(p); p2.translate(p); }
  virtual void rotate(const G_point &p, double d) { p1.rotate(p, d); p2.rotate(p, d); }
  virtual void reflect(const G_straight &s) { p1.reflect(s);  p2.reflect(s); cw = !cw;}
  virtual void scale(const G_point &p, double d) { p1.scale(p, d); p2.scale(p, d);}

  //parametrization:
  virtual G_point getPointOnCurve(double p) const;
  virtual double getParamFromPoint(const G_point &p) const;

  //intersection:
  virtual G_point getIntersection(const G_curve *, int which = 0) const;

  bool isValid() const
  { return p1.isValid() && p2.isValid() && cosangle >= -1 && cosangle <= 1; }
  static G_arc inValid()
  { return G_arc(G_point::inValid(), G_point::inValid(), G_point::inValid()); }

  G_point getCenter() const { return p1; }
  double getRadius() const { return (p2 - p1).length(); }
  double getArcLength() const { return acos(cosangle) * 2 * getRadius(); }
  double getArcAngle() const { return acos(cosangle) * 2; }

  G_point getP2() const { return p2; }
  double getCosangle() const { return cosangle; }
  bool getCW() const { return cw; }

private:
  bool pointOnArc(const G_point &p) const
  { return (p - p1).normalize() * (p2 - p1).normalize() >= cosangle; }

  G_point p1, p2;
  double cosangle; //p1 is the center, p2 is the midpoint of the arc,
  // and cosangle is the cosine of half the arc angle.
  bool cw; //for parameter only--to prevent jumping
};

#endif //G_ARC_H
