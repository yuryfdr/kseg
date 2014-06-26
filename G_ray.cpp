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


#include "G_ray.H"
#include "G_segment.H"
#include "G_line.H"
#include "G_drawstyle.H"
#include "KSegView.H"

//drawing:
void G_ray::draw(QPainter &p, const G_drawstyle &d, bool selected)
{
  QRect r = p.window();

  if(!inRect(r)) return;

  G_segment(p1, p1 + dir * 200000000).draw(p, d, selected);

  return;
	     
}

//the mapping is via the tangent function.  So when p = 0, the location is -inf and when p = 1 it's inf.
G_point G_ray::getPointOnCurve(double p) const
{
  double c;

  c = pow(p, 1./81.) * M_PI / (2 + SMALL);
 
  return p1 + tan(c) * dir;
}

double G_ray::getParamFromPoint(const G_point &p) const
{ 
  double c;

  c = atan((p - p1).length());

  return CUBE(CUBE(CUBE(CUBE(c / M_PI * (2 + SMALL)))));
}


G_point G_ray::getNearestPoint(const G_point &p) const
{
  double x = (p - p1) * dir / (dir * dir);
  if(x <= 0 || !myFinite(x)) return p1;
  return p1 + dir * x;
}

bool G_ray::inRect(const QRect &r) const
{
  if(r.contains(p1.toQPoint())) return true;

  if(p1.getX() < r.left() && dir.getX() < 0) return false;
  if(p1.getX() > r.right() && dir.getX() > 0) return false;

  if(p1.getY() < r.top() && dir.getY() < 0) return false;
  if(p1.getY() > r.bottom() && dir.getY() > 0) return false;

  //the following could be optimized
  G_segment tmp;

  tmp = G_segment(r.topLeft(), r.bottomLeft());
  if(getIntersection(&tmp).isValid()) return true;

  tmp = G_segment(r.topRight(), r.bottomRight());
  if(getIntersection(&tmp).isValid()) return true;

  tmp = G_segment(r.bottomLeft(), r.bottomRight());
  if(getIntersection(&tmp).isValid()) return true;

  tmp = G_segment(r.topRight(), r.topLeft());
  if(getIntersection(&tmp).isValid()) return true;

  return false;
}

G_point G_ray::getIntersection(const G_curve *c, int which) const
{
  if(c->getType() == G_RAY) {
    G_ray *r = (G_ray *)c;

    double s, t, tmp;

    tmp = dir % r->dir;

    if(fabs(tmp) < SMALL) return G_point::inValid();

    s = r->dir % (p1 - r->p1) / tmp;

    t = dir % (p1 - r->p1) / tmp;

    if( s < -SMALL || t < -SMALL)
      return G_point::inValid();
    
    if(s < 0) s = 0;
    
    return p1 + s * dir;
  }


  if(c->getType() == G_SEGMENT) {
    G_segment *s = (G_segment *)c;

    double r, t, tmp;

    tmp = dir % (s->getP2() - s->getP1());

    if(fabs(tmp) < SMALL) return G_point::inValid();

    r = (s->getP2() - s->getP1()) % (p1 - s->getP1()) / tmp;

    t = dir % (p1 - s->getP1()) / tmp;

    if( r < -SMALL || t < -SMALL || t > 1 + SMALL)
      return G_point::inValid();
    
    if(r < 0) r = 0;
    
    return p1 + r * dir;

  }

  if(c->getType() == G_LINE) {
    G_line *l = (G_line *)c;

    double r, tmp;

    tmp = dir % l->getDirection();

    if(fabs(tmp) < SMALL) return G_point::inValid();

    r = ((p1.getY() - l->getP1().getY()) * (l->getDirection().getX()) -
	 (p1.getX() - l->getP1().getX()) * (l->getDirection().getY())) / tmp;
    
    if(r < -SMALL) return G_point::inValid();

    if(r < 0) r = 0;

    return p1 + r * dir;

  }

  return c->getIntersection(this, which);
}
