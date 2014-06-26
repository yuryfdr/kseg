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


#ifndef G_SEGMENT_H
#define G_SEGMENT_H

#include "G_point.H"

class G_segment : public G_straight
{
public:
  G_segment() { }
  G_segment(const G_segment &s) : G_straight() { p1 = s.p1; p2 = s.p2; }
  G_segment(const G_point &P1, const G_point &P2) { p1 = P1; p2 = P2;  }
  
  virtual G_geometry *copy() const { return new G_segment(*this); }

  virtual G_Type getType() const { return G_SEGMENT; }

  //drawing:
  virtual void draw(QPainter &p, const G_drawstyle &d, bool selected);

  //calculations:
  virtual G_point getDirection() const { return (p2 - p1).normalize(); }
  virtual QRect getExtents(void) const;

  virtual G_point getNearestPoint(const G_point &) const;
  virtual bool inRect(const QRect &) const;

  //transformations:
  virtual void translate(const G_point &p) { p1.translate(p); p2.translate(p); }
  virtual void rotate(const G_point &p, double d) { p1.rotate(p, d); p2.rotate(p, d); }
  virtual void reflect(const G_straight &s) { p1.reflect(s); p2.reflect(s);}
  virtual void scale(const G_point &p, double d) { p1.scale(p, d); p2.scale(p, d); }

  //parametrization:
  virtual G_point getPointOnCurve(double p) const;
  virtual double getParamFromPoint(const G_point &p) const;

  //intersection:
  virtual G_point getIntersection(const G_curve *, int which = 0) const;

  bool isValid() const { return p1.isValid() && p2.isValid() && p2 != p1; }
  static G_segment inValid() { return G_segment(G_point::inValid(), G_point::inValid()); }

  G_point getP1() const { return p1; }
  G_point getP2() const { return p2; }

  G_point getMidpoint() const { return (p1 + p2) / 2; }
  double length() const { return (p2 - p1).length(); }

private:
  G_point p1, p2;
};

#endif //G_SEGMENT_H
