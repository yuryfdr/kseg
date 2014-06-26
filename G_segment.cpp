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


#include "G_segment.H"
#include "G_line.H"
#include "G_drawstyle.H"
#include "KSegView.H"

//drawing:
void G_segment::draw(QPainter &p, const G_drawstyle &d, bool selected)
{
  QRect r = p.window();

  if(!inRect(r)) return;

  G_point tmp1, tmp2;
  
  if(fabs(p1.getX()) < DRAW_MAX && fabs(p1.getY()) < DRAW_MAX) {
    tmp1 = p1;
  }
  else {
    tmp1 = getNearestPoint(G_point(0, 0));
    tmp1 += (p1 - tmp1) * (DRAW_MAX) / (p1 - tmp1).length();
  }

  if(fabs(p2.getX()) < DRAW_MAX && fabs(p2.getY()) < DRAW_MAX) {
    tmp2 = p2;
  }
  else {
    tmp2 = getNearestPoint(G_point(0, 0));
    tmp2 += (p2 - tmp2) * (DRAW_MAX) / (p2 - tmp2).length();
  }

  //Now Draw!

  if(/*p.device()->isExtDev() ||*/ p.hasViewXForm()) { //draw at higher accuracy to a printer
    tmp1 *= 8.;
    tmp2 *= 8.;
    p.scale(0.125, .125);
    
    int w = d.getPen().width() * 8;
    if(w == 0) w = 2;
    //if(!p.device()->isExtDev()) w = (d.getPen().width() == 0 ? 4 : 4 * d.getPen().width());

    p.setPen(QPen(d.getPen().color(), w, d.getPen().style()));
    p.drawLine(tmp1.toQPoint(), tmp2.toQPoint());

    p.scale(8, 8);

    return;
  }

  if(selected && KSegView::getSelectType() == KSegView::BORDER_SELECT) {
    int width = d.getPen().width() ? d.getPen().width() + 3 : 4;

    p.setPen(QPen(G_drawstyle::getBorderColor(d.getPen().color()), width));

    p.drawLine(tmp1.toQPoint(), tmp2.toQPoint());
    
  }

  p.setPen(d.getPen());
  if(selected && KSegView::getSelectType() == KSegView::BLINKING_SELECT) {
    QColor c(QTime::currentTime().msec() * 17, 255, 255, QColor::Hsv);

    p.setPen(QPen(c, d.getPen().width(), d.getPen().style()));
  }

  p.drawLine(tmp1.toQPoint(), tmp2.toQPoint());

  return;
	     
}

G_point G_segment::getPointOnCurve(double p) const
{
  return p1 + (p2 - p1) * p;
}

double G_segment::getParamFromPoint(const G_point &p) const
{ 
  return sqrt(((p - p1) * (p - p1)) / ((p2 - p1) * (p2 - p1)));
}


G_point G_segment::getNearestPoint(const G_point &p) const
{
  G_point diff = p2 - p1;
  if(p2 == p1) return p1;
  double x = (p - p1) * diff / (diff * diff);
  if(x <= 0) return p1;
  if(x >= 1) return p2;
  return p1 + diff * x;
}

bool G_segment::inRect(const QRect &r) const
{
  if(p1.getX() < r.right() && p1.getX() > r.left() &&
     p1.getY() < r.bottom() && p1.getY() > r.top()) return true;

  if(p2.getX() < r.right() && p2.getX() > r.left() &&
     p2.getY() < r.bottom() && p2.getY() > r.top()) return true;

  if(p1.getX() < r.left() && p2.getX() < r.left()) return false;
  if(p1.getX() > r.right() && p2.getX() > r.right()) return false;

  if(p1.getY() < r.top() && p2.getY() < r.top()) return false;
  if(p1.getY() > r.bottom() && p2.getY() > r.bottom()) return false;

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

G_point G_segment::getIntersection(const G_curve *c, int which) const
{
  if(c->getType() == G_SEGMENT) {
    G_segment *s = (G_segment *)c;

    double r, t, tmp;

    tmp = (p2 - p1) % (s->p2 - s->p1);

    if(fabs(tmp) < SMALL) return G_point::inValid();

    r = (s->p2 - s->p1) % (p1 - s->p1) / tmp;

    t = (p2 - p1) % (p1 - s->p1) / tmp;

    if( r < -SMALL || r > 1 + SMALL || t < -SMALL || t > 1 + SMALL)
      return G_point::inValid();
    
    if(r < 0) r = 0; if(r > 1) r = 1;
    
    return p1 + r * (p2 - p1);

  }

  if(c->getType() == G_LINE) {
    G_line *l = (G_line *)c;

    double r, tmp;

    tmp = (p2 - p1) % l->getDirection();

    if(fabs(tmp) < SMALL) return G_point::inValid();

    r = l->getDirection() % (p1 - l->getP1()) / tmp;

    if(r < -SMALL || r > 1 + SMALL)
      return G_point::inValid();

    if(r < 0) r = 0; if(r > 1) r = 1;

    return p1 + r * (p2 - p1);

  }

  return c->getIntersection(this, which);
}

QRect G_segment::getExtents(void) const {
  int left, right, top, bottom;

  if(p1.getX() < p2.getX()) {
    left = ROUND(p1.getX());
    right = ROUND(p2.getX());
  }
  else {
    left = ROUND(p2.getX());
    right = ROUND(p1.getX());
  }

  if(p1.getY() < p2.getY()) {
    top = ROUND(p1.getY());
    bottom = ROUND(p2.getY());
  }
  else {
    top = ROUND(p2.getY());
    bottom = ROUND(p1.getY());
  }

  return QRect(left - 5, top - 5, right - left + 10, bottom - top + 10);
}
