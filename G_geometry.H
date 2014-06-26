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


#ifndef G_GEOMETRY_H
#define G_GEOMETRY_H

#include "defs.H"
#include <qrect.h>
#include <qpainter.h>
#include <qdatetime.h>

class G_point;
class G_straight;
class G_drawstyle;
class QPainter;

class G_geometry
{
public:

  virtual ~G_geometry() {}

  virtual G_Type getType() const = 0;

  virtual G_geometry *copy() const = 0;

  //drawing:
  virtual void draw(QPainter &, const G_drawstyle &, bool selected = false) = 0;

  //calculations:
  virtual QRect getExtents(void) const = 0;
  virtual G_point getNearestPoint(const G_point &) const = 0;
  virtual bool inRect(const QRect &) const = 0;

  //transformations:
  virtual void translate(const G_point &) = 0;
  virtual void rotate(const G_point &, double) = 0;
  virtual void reflect(const G_straight &) = 0;
  virtual void scale(const G_point &, double) = 0;

  virtual bool isValid() const { return false; } //for checking bad geometry--non-existent points, etc.
};


class G_filled : public G_geometry
{
public:
  virtual double getArea() const = 0;
};


class G_curve : public G_geometry
{
public:
  // curve-specific:
  //p is in the range from 0 to 1.  should return all reasonable points on the curve.
  virtual G_point getPointOnCurve(double p) const = 0;
  virtual double getParamFromPoint(const G_point &p) const = 0; //returns p for given point.

  virtual G_point getIntersection(const G_curve *c, int which = 0) const = 0;
};


class G_straight : public G_curve
{
public:
  //other stuff:
  virtual G_point getDirection() const = 0;
};


#endif //G_GEOMETRY_H









