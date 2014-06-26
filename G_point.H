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


#ifndef G_POINT_H
#define G_POINT_H

#include "G_geometry.H"
#include <qpoint.h>
#include <math.h>

class QDataStream;

class G_point : public G_geometry
{
public:
  G_point() { x = 0; y = 0; }
  G_point(double X, double Y) { x = X; y = Y; }
  G_point(const G_point &p) : G_geometry() { x = p.x;  y = p.y; } //copy

  G_point(const QPoint &p) { x = p.x(); y = p.y(); } //convert

  virtual G_geometry *copy() const { return new G_point(*this); }

  virtual G_Type getType() const { return G_POINT; }

  double getX() const { return x; }
  double getY() const { return y; }
  
  void setX(double X) { x = X; }
  void setY(double Y) { y = Y; }

  //cast to QPoint
  QPoint toQPoint() const { return QPoint(qRound(x), qRound(y)); }

  //drawing
  void draw(QPainter &, const G_drawstyle&, bool selected = false);

  //calculations
  QRect getExtents(void) const { return QRect(qRound(x - 6), qRound(y - 6), 12, 12); }
  G_point getNearestPoint(const G_point &) const { return *this; }
  bool inRect(const QRect &) const;
  
  //make this point have length 1.
  G_point &normalize() {
    double lsq = x * x + y * y;
    if(!lsq) {x = 1; y = 0; return *this;}
    if(lsq != 1) return (*this /= sqrt(lsq)); else return *this;
  }

  //transformations:
  void translate(const G_point &);
  void rotate(const G_point &, double);
  void reflect(const G_straight &);
  void scale(const G_point &, double);

  //rotate 90 degrees:
  G_point r90() const { return G_point(-y, x); }

  //operators:
  G_point &operator=(const G_point &p) { x = p.x; y = p.y; return *this; }

  G_point &operator+=(const G_point &p) { x += p.x; y += p.y; return *this; }
  G_point &operator-=(const G_point &p) { x -= p.x; y -= p.y; return *this; }
  G_point &operator*=(double d) { x *= d; y *= d; return *this; }
  G_point &operator/=(double d) { x /= d; y /= d; return *this; }

  G_point operator+(const G_point &p) const { return G_point(x + p.x, y + p.y); }
  G_point operator-(const G_point &p) const { return G_point(x - p.x, y - p.y); }
  G_point operator*(double d) const { return G_point(x * d, y * d); }
  friend G_point operator*(double d, const G_point &p);
  G_point operator/(double d) const { return G_point(x / d, y / d); }

  double operator*(const G_point &p) const { return x * p.x + y * p.y; } //dot product
  double operator%(const G_point &p) const { return x * p.y - y * p.x; } //cross product

  bool operator==(const G_point &p) const { return x == p.x && y == p.y; }
  bool operator!=(const G_point &p) const { return x != p.x || y != p.y; }

  friend QDataStream &operator<<(QDataStream &, const G_point &);
  friend QDataStream &operator>>(QDataStream &, G_point &);

  //utility:
  double lengthsq() const { return x * x + y * y; }
  double length() const { return sqrt(x * x + y * y); }
  double angle() const { return atan2(y, x); } // from -pi to pi

  //is it big?
  bool isValid() const { return myFinite(x) && myFinite(y); }

  static G_point inValid() { return G_point(BIG, BIG); }

private:
  double x, y;
};

inline G_point operator*(double d, const G_point &p) { return p * d; }

#endif //G_POINT.H
