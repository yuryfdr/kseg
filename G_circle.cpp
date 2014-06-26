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


#include "G_circle.H"
#include "G_drawstyle.H"
#include "KSegView.H"
#include "G_segment.H"
#include "G_line.H"
#include "G_ray.H"

//drawing:
void G_circle::draw(QPainter &p, const G_drawstyle &d, bool selected)
{
  QRect re = p.window();

  if(!inRect(re)) return;

  if(r > DRAW_MAX / 2) {
    //draw big circle...
    double x;
    double step = QMAX(0.001, 5. / r);
    G_point prevPt = getPointOnCurve(0);
    for(x = step; x < 1 + step / 2.; x += step) {
      G_point curPt = getPointOnCurve(x);
      G_segment s(prevPt, curPt);
      s.draw(p, d, selected);
      prevPt = curPt;
    }

    return;
  }

  p.setBrush(QBrush());

  if(/*p.device()->isExtDev() ||*/ p.hasViewXForm()) { //draw at higher accuracy to a printer or image
    p.scale(0.125, .125);
    
    int w = d.getPen().width() * 8;
    if(w == 0) w = 2;
    //if(!p.device()->isExtDev()) w = (d.getPen().width() == 0 ? 4 : 4 * d.getPen().width());

    p.setPen(QPen(d.getPen().color(), w, d.getPen().style()));

    p.drawEllipse(ROUND((p1.getX() - r) * 8), ROUND((p1.getY() - r) * 8),
		  ROUND(r * 16), ROUND(r * 16));

    p.scale(8, 8);

    return;
  }

  if(selected && KSegView::getSelectType() == KSegView::BORDER_SELECT) {
    int width = d.getPen().width() ? d.getPen().width() + 3 : 4;

    p.setPen(QPen(G_drawstyle::getBorderColor(d.getPen().color()), width));

    p.drawEllipse(ROUND(p1.getX() - r), ROUND(p1.getY() - r),
		  ROUND(r * 2), ROUND(r * 2));

  }

  p.setPen(d.getPen());

  if(selected && KSegView::getSelectType() == KSegView::BLINKING_SELECT) {
    QColor c(QTime::currentTime().msec() * 17, 255, 255, QColor::Hsv);

    p.setPen(QPen(c, d.getPen().width(), d.getPen().style()));
  }

  p.drawEllipse(ROUND(p1.getX() - r), ROUND(p1.getY() - r),
		ROUND(r * 2), ROUND(r * 2));

  return;
	     
}

G_point G_circle::getPointOnCurve(double p) const
{
  return p1 + r * G_point(cos(p * 2 * M_PI), sin(p * 2 * M_PI));
}

double G_circle::getParamFromPoint(const G_point &p) const
{ 
  return fmod((atan2((p - p1).getY(), (p - p1).getX())) / (2 * M_PI) + 1, 1);
}


G_point G_circle::getNearestPoint(const G_point &p) const
{
  if(p == p1) return p1 + G_point(r, 0);
  else return p1 + (p - p1).normalize() * r;
}

bool G_circle::inRect(const QRect &rect) const
{
  //either the rectangle contains the center or the boundary is
  //within a distance of less than r from it.


  //first make sure that the rectangle is within the extents:
  if((rect & getExtents()).isEmpty()) return false;

  //now see if the rect entirely contains the circle:
  if(rect.contains(QRect(ROUND(p1.getX() - r), ROUND(p1.getY() - r), ROUND(r * 2), ROUND(r * 2)))) return true;

  //now see if the rect is not entirely within the circle:
  double cur, max = 0, min = DRAW_MAX;

  cur = (G_segment(G_point(rect.topLeft()), G_point(rect.topRight())).getNearestPoint(p1) - p1).length();
  max = min = cur;

  cur = (G_segment(G_point(rect.topLeft()), G_point(rect.bottomLeft())).getNearestPoint(p1) - p1).length();
  min = QMIN(cur, min);

  cur = (G_segment(G_point(rect.bottomRight()), G_point(rect.topRight())).getNearestPoint(p1) - p1).length();
  min = QMIN(cur, min);

  cur = (G_segment(G_point(rect.bottomLeft()), G_point(rect.bottomRight())).getNearestPoint(p1) - p1).length();
  min = QMIN(cur, min);

  max = QMAX(max, (G_point(rect.bottomLeft()) - p1).length());
  max = QMAX(max, (G_point(rect.bottomRight()) - p1).length());
  max = QMAX(max, (G_point(rect.topLeft()) - p1).length());
  max = QMAX(max, (G_point(rect.topRight()) - p1).length());

  if(max < r) return false; //entirely inside the circle

  if(min < r) return true; //some piece is inside the circle.
  else return false;
}


G_point G_circle::getIntersection(const G_curve *c, int which) const
{ 
  //intersection with circle.
  if(c->getType() == G_CIRCLE) {
    G_circle *ci = (G_circle *)c;
    
    G_point tmp = ci->p1 - p1;
    double d = tmp * tmp;
    double x;
    
    if(fabs(d) < SMALL) return G_point::inValid();
    
    x = -(QUAD(r) + SQR(d - SQR(ci->r)) -
	  2 * SQR(r) * (SQR(ci->r) + d));
    
    if(x < -SMALL) return G_point::inValid();
    
    if(fabs(x) <= SMALL) {
      if(which == 0) {
	if(d < SQR(QMAX(r, ci->r))) {
	  return
	    tmp * (r / (r - ci->r)) + p1;
	}
	else {
	  return
	    tmp * (r / (r + ci->r)) + p1;
	}
      }
      return G_point::inValid();
    }

    if(x < 0) x = 0; else x = sqrt(x);
    
    if(which == 1) x = -x;
    
    G_point tmp1;

    tmp1.setX((tmp.getX() * (SQR(r) - SQR(ci->r) + d) - tmp.getY() * x) / (2 * d));
    tmp1.setY((tmp.getY() * (SQR(r) - SQR(ci->r) + d) + tmp.getX() * x) / (2 * d));
    
    return tmp1 + p1;
  }

  //intersection with line
  if(c->getType() == G_LINE) {
    G_line *l = (G_line *)c;

    G_point tmp = l->getNearestPoint(p1);

    double d = SQR(tmp - p1);

    d = SQR(r) - d;

    if(d < SMALL) return G_point::inValid();

    if(d < 0) d = 0; else d = sqrt(d);

    if(d == 0 && which == 1) return G_point::inValid();

    if(which == 1) d = -d;

    return tmp + d * l->getDirection();

  }

  //intersection with ray
  if(c->getType() == G_RAY) {
    G_ray *l = (G_ray *)c;

    G_point tmp = G_line(l->getP1(), l->getDirection()).getNearestPoint(p1);

    double d = SQR(tmp - p1);

    d = SQR(r) - d;

    if(d < SMALL) return G_point::inValid();

    if(d < 0) d = 0; else d = sqrt(d);

    if(d == 0 && which == 1) return G_point::inValid();

    if(which == 1) d = -d;

    G_point re = tmp + d * l->getDirection();

    if((re - l->getP1()) * l->getDirection() < -SMALL) return G_point::inValid();

    if((re - l->getP1()) * l->getDirection() < 0) re = l->getP1();

    return re;

  }

  //intersection with segment
  if(c->getType() == G_SEGMENT) {
    G_segment *l = (G_segment *)c;

    G_point tmp = G_line(l->getP1(), l->getDirection()).getNearestPoint(p1);

    double d = SQR(tmp - p1);

    d = SQR(r) - d;

    if(d < SMALL) return G_point::inValid();

    if(d < 0) d = 0; else d = sqrt(d);

    if(d == 0 && which == 1) return G_point::inValid();

    if(which == 1) d = -d;

    G_point re = tmp + d * l->getDirection();

    G_point nre = l->getNearestPoint(re);

    if((nre - re) * (nre - re) < SMALL * SMALL) return nre;
    
    return G_point::inValid();
  }
  
  return c->getIntersection(this, which);
}
