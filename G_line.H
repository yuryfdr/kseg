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


#ifndef G_LINE_H
#define G_LINE_H

#include "G_point.H"

class G_line : public G_straight
{
public:
  G_line() { }
  G_line(const G_line &l) : G_straight() { p1 = l.p1; dir = l.dir; }
  G_line(const G_point &P1, const G_point &Dir) { p1 = P1; dir = Dir; dir.normalize();  }

  virtual G_geometry *copy() const { return new G_line(*this); }
  
  virtual G_Type getType() const { return G_LINE; }

  //drawing:
  virtual void draw(QPainter &p, const G_drawstyle &d, bool selected);

  //calculations:
  virtual G_point getDirection() const { return dir; }
  virtual QRect getExtents(void) const { return QRect(-1000000, -1000000, 2000000, 2000000); } //always maximum
  virtual G_point getNearestPoint(const G_point &) const;
  virtual bool inRect(const QRect &) const;

  //transformations:
  virtual void translate(const G_point &p) { p1.translate(p); }
  virtual void rotate(const G_point &p, double d) { p1.rotate(p, d); dir.rotate(G_point(0, 0), d); }
  virtual void reflect(const G_straight &s) { dir += p1; p1.reflect(s); dir.reflect(s); dir -= p1; dir.normalize();}
  virtual void scale(const G_point &p, double d) { p1.scale(p, d); }

  //parametrization:
  virtual G_point getPointOnCurve(double p) const;
  virtual double getParamFromPoint(const G_point &p) const;

  //intersection:
  virtual G_point getIntersection(const G_curve *, int which = 0) const;

  bool isValid() const { return p1.isValid() && dir != G_point(0, 0); }
  static G_line inValid() { return G_line(G_point::inValid(), G_point(0, 0)); }

  G_point getP1() const { return p1; }

private:
  G_point p1, dir;
};

#endif //G_LINE_H
