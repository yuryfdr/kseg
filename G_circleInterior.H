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


#ifndef G_CIRCLEINTERIOR_H
#define G_CIRCLEINTERIOR_H

#include "G_circle.H"

class G_circleInterior : public G_filled
{
public:
  G_circleInterior() { }
  G_circleInterior(const G_circleInterior &c) : G_filled() { circle = c.circle; }
  G_circleInterior(const G_circle &c) { circle = c; }
  
  virtual G_geometry *copy() const { return new G_circleInterior(*this); }

  virtual G_Type getType() const { return G_CIRCLEINTERIOR; }

  //drawing:
  virtual void draw(QPainter &p, const G_drawstyle &d, bool selected);

  //calculations:
  virtual QRect getExtents(void) const { return circle.getExtents(); }
  virtual G_point getNearestPoint(const G_point &) const;
  virtual bool inRect(const QRect &) const;

  virtual double getArea() const { return SQR(circle.getRadius()) * M_PI; }

  //transformations:
  virtual void translate(const G_point &p) { circle.translate(p); }
  virtual void rotate(const G_point &p, double d) { circle.rotate(p, d); }
  virtual void reflect(const G_straight &s) { circle.reflect(s); }
  virtual void scale(const G_point &p, double d) { circle.scale(p, d); }

  bool isValid() const { return circle.isValid(); }
  static G_circleInterior inValid() { return G_circleInterior(G_circle::inValid()); }

private:
  G_circle circle;
};

#endif //G_CIRCLEINTERIOR_H
