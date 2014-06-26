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


#include "G_line.H"
#include "G_drawstyle.H"
#include "KSegView.H"

//drawing:
void G_line::draw(QPainter &p, const G_drawstyle &d, bool selected)
{
  QRect r = p.window();
  
  G_point x = getNearestPoint(G_point(0, 0));
  if(SQR(x - G_point(r.center())) > DRAW_MAX * DRAW_MAX) return;

  G_segment(x - dir * DRAW_MAX, x + dir * DRAW_MAX).draw(p, d, selected);
  return;
}

//the mapping is via the tangent function.  So when p = 0, the location is -inf and when p = 1 it's inf.
G_point G_line::getPointOnCurve(double p) const
{
  double c;

  //  c = (p - 0.5) * M_PI / (1 + SMALL);

  c = (p - 0.5) * 2;

  if(c >= 0) c = pow(c, 1./81.) / 2.;
  else c = -pow(-c, 1./81.) / 2.;

  c = c * M_PI / (1 + SMALL);
 
  return p1 + tan(c) * dir;
}

double G_line::getParamFromPoint(const G_point &p) const
{ 
  double c;

  c = atan((p - p1).length()) * SIGN((p - p1) * dir);

  //  return c / M_PI * (1 + SMALL) + 0.5;
  return CUBE(CUBE(CUBE(CUBE(c / M_PI * (1 + SMALL) * 2.)))) / 2. + 0.5;
}


G_point G_line::getNearestPoint(const G_point &p) const
{
  return p1 + ((p - p1) * dir) * dir;
}

bool G_line::inRect(const QRect &r) const
{
  //a point is in the rectangle only if at least one pair of opposite vertices
  //are on opposite sides of the line.

  double d1 = (G_point(r.topLeft()) - p1) % dir;
  double d2 = (G_point(r.bottomRight()) - p1) % dir;
  double d3 = (G_point(r.topRight()) - p1) % dir;
  double d4 = (G_point(r.bottomLeft()) - p1) % dir;

  return ((d1 < 0) != (d2 < 0)) || ((d3 < 0) != (d4 < 0));
}

G_point G_line::getIntersection(const G_curve *c, int which) const
{
  if(c->getType() == G_LINE) {
    G_line *l = (G_line *)c;

    double r, tmp;

    tmp = dir % l->dir;

    if(fabs(tmp) < SMALL) return G_point::inValid();

    r = ((p1.getY() - l->p1.getY()) * (l->dir.getX()) -
	 (p1.getX() - l->p1.getX()) * (l->dir.getY())) / tmp;
    
    if((p1 + r * dir).length() > 20000000)
      return G_point::inValid();

    return p1 + r * dir;

  }
  else return c->getIntersection(this, which);
}
