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


#include "G_arc.H"
#include "G_drawstyle.H"
#include "KSegView.H"
#include "G_circle.H"
#include "G_segment.H"
#include "G_line.H"
#include "G_ray.H"


//constructor:
G_arc::G_arc(const G_point &P1, const G_point &P2, const G_point &P3)
{
  double x1 = P1.getX();
  double x2 = P2.getX();
  double x3 = P3.getX();
  double y1 = P1.getY();
  double y2 = P2.getY();
  double y3 = P3.getY();
  double dtmp;

  if(!P1.isValid() || !P2.isValid() || !P3.isValid()) {
    p1 = G_point::inValid();
    return;
  }

  dtmp = (2 * ((-x2 * y1) + (x3 * y1) + (x1 * y2) -
	       (x3 * y2) - (x1 * y3) + (x2 * y3)));
  
  if(fabs(dtmp) < SMALL) {
    p1 = G_point::inValid();
    return;
  }

  double x, y;

  x = (x3 * x3 * (y1 - y2) + (x1 * x1 + (y1 - y2) * (y1 - y3)) *
       (y2 - y3) + x2 * x2 * (-y1 + y3)) / dtmp;
  y = -(((x3 - x2) * (-x1 * x1 + x2 * x2 - y1 * y1 + y2 * y2) +
	 (x2 - x1) * (x2 * x2 - x3 * x3 + y2 * y2 - y3 * y3)) / dtmp);
  p1 = G_point(x, y);
  
  G_point tmp = (P1 + P3) / 2. - p1;
  if(tmp.lengthsq() < SMALL * SMALL) {
    tmp = (P1-p1).r90();
  }
  tmp = tmp.normalize();

  cosangle = (P1 - p1).normalize() * tmp;

  p2 = p1 + tmp * (P1 - p1).length();

  if(!pointOnArc(P2)) {
    tmp = G_point(0, 0) - tmp;

    cosangle = (P1 - p1).normalize() * tmp;
    
    p2 = p1 + tmp * (P1 - p1).length();
  }

  cw = (P1 - P2) % (P3 - P2) > 0;
}


//drawing:
void G_arc::draw(QPainter &p, const G_drawstyle &d, bool selected)
{
  double r = getRadius();
  QRect re = p.window();

  if(!inRect(re)) return;

  if(r > DRAW_MAX / 2) {
    //draw big arc...
    double x;
    double step = QMAX(0.001, 2. / getArcLength());
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

  const double angle_scale = -16 * 180. / M_PI;
  
  if(/*p.device()->isExtDev() ||*/ p.hasViewXForm()) { //draw at higher accuracy to a printer or image
    p.scale(0.125, .125);
    
    int w = d.getPen().width() * 8;
    if(w == 0) w = 2;
    //if(!p.device()->isExtDev()) w = (d.getPen().width() == 0 ? 4 : 4 * d.getPen().width());

    p.setPen(QPen(d.getPen().color(), w, d.getPen().style()));

    p.drawArc(ROUND((p1.getX() - r) * 8), ROUND((p1.getY() - r) * 8),
	      ROUND(r * 16), ROUND(r * 16),
	      ROUND(((p2 - p1).angle() - acos(cosangle)) * angle_scale),
	      ROUND(angle_scale * acos(cosangle) * 2));

    p.scale(8, 8);

    return;
  }

  if(selected && KSegView::getSelectType() == KSegView::BORDER_SELECT) {
    int width = d.getPen().width() ? d.getPen().width() + 3 : 4;

    p.setPen(QPen(G_drawstyle::getBorderColor(d.getPen().color()), width));


    p.drawArc(ROUND(p1.getX() - r), ROUND(p1.getY() - r),
	      ROUND(r * 2), ROUND(r * 2),
	      ROUND(((p2 - p1).angle() - acos(cosangle)) * angle_scale),
	      ROUND(angle_scale * acos(cosangle) * 2));

  }

  p.setPen(d.getPen());

  if(selected && KSegView::getSelectType() == KSegView::BLINKING_SELECT) {
    QColor c(QTime::currentTime().msec() * 17, 255, 255, QColor::Hsv);

    p.setPen(QPen(c, d.getPen().width(), d.getPen().style()));
  }

  p.drawArc(ROUND(p1.getX() - r), ROUND(p1.getY() - r),
	    ROUND(r * 2), ROUND(r * 2),
	    ROUND(((p2 - p1).angle() - acos(cosangle)) * angle_scale),
	    ROUND(angle_scale * acos(cosangle) * 2));

  return;
	     
}

G_point G_arc::getPointOnCurve(double p) const
{
  if(cw) p = 1. - p;
  G_point tmp = p2;
  tmp.rotate(p1, (p - 0.5) * 2. * acos(cosangle));
  return tmp;
}

double G_arc::getParamFromPoint(const G_point &p) const
{ 
  double cosa = (p2 - p1) * (p - p1) / getRadius() / (p - p1).length();
  double re = (acos(cosa) / acos(cosangle) / 2.) * SIGN((p2 - p1) % (p - p1)) + 0.5;
  if(cw) re = 1. - re;
  return re;
}


G_point G_arc::getNearestPoint(const G_point &p) const
{
  if(p == p1) return p2;
  if(pointOnArc(p)) 
    return p1 + (p - p1).normalize() * getRadius();
  else {
    G_point tmp = p2;
    tmp.rotate(p1, acos(cosangle) * SIGN((p2 - p1) % (p - p1)));
    return tmp;
  }
}

QRect G_arc::getExtents(void) const
{
  double x1, x2, y1, y2;

  G_point e1, e2;

  e1 = p2;
  e1.rotate(p1, acos(cosangle));
  e2 = p2;
  e2.rotate(p1, -acos(cosangle));

  if(pointOnArc(G_point(-1, 0) + p1)) {
    x1 = p1.getX() - getRadius();
  }
  else x1 = QMIN(e1.getX(), e2.getX());

  if(pointOnArc(G_point(1, 0) + p1)) {
    x2 = p1.getX() + getRadius();
  }
  else x2 = QMAX(e1.getX(), e2.getX());

  if(pointOnArc(G_point(0, -1) + p1)) {
    y1 = p1.getY() - getRadius();
  }
  else y1 = QMIN(e1.getY(), e2.getY());

  if(pointOnArc(G_point(0, 1) + p1)) {
    y2 = p1.getY() + getRadius();
  }
  else y2 = QMAX(e1.getY(), e2.getY());

  return QRect(ROUND(x1 - 6), ROUND(y1 - 6),
	       ROUND((x2 - x1) + 12), ROUND((y2 - y1) + 12));
}


bool G_arc::inRect(const QRect &rect) const
{
  //first make sure that the rectangle is within the extents:
  if((rect & getExtents()).isEmpty()) return false;

  //now see if the rect entirely contains the arc:
  if(rect.contains(getExtents())) return true;

  if(rect.contains(p2.toQPoint())) return true;
  if(rect.contains(getPointOnCurve(0).toQPoint())) return true;
  if(rect.contains(getPointOnCurve(1).toQPoint())) return true;

  //now see if the rect is not entirely within the circle:
  G_segment tmp;
  tmp = G_segment(rect.topRight(), rect.topLeft());
  if(getIntersection(&tmp, 0).isValid() || getIntersection(&tmp, 1).isValid()) return true;
  tmp = G_segment(rect.topRight(), rect.bottomRight());
  if(getIntersection(&tmp, 0).isValid() || getIntersection(&tmp, 1).isValid()) return true;
  tmp = G_segment(rect.topLeft(), rect.bottomLeft());
  if(getIntersection(&tmp, 0).isValid() || getIntersection(&tmp, 1).isValid()) return true;
  tmp = G_segment(rect.bottomRight(), rect.bottomLeft());
  if(getIntersection(&tmp, 0).isValid() || getIntersection(&tmp, 1).isValid()) return true;
  
  return false;
}


G_point G_arc::getIntersection(const G_curve *c, int which) const
{ 
  double r = getRadius();

  //intersection with arc.
  if(c->getType() == G_ARC) {
    G_arc *ar = (G_arc *)c;
    double r2 = ar->getRadius();
    
    G_point tmp = ar->p1 - p1;
    double d = tmp * tmp;
    double x;
    
    if(fabs(d) < SMALL) return G_point::inValid();
    
    x = -(QUAD(r) + SQR(d - SQR(r2)) -
	  2 * SQR(r) * (SQR(r2) + d));
    
    if(x < -SMALL) return G_point::inValid();
    
    if(fabs(x) <= SMALL) {
      if(which == 0) {
	if(d < SQR(QMAX(r, r2))) {
	  G_point re = tmp * (r / (r - r2)) + p1;
	  if(pointOnArc(re) && ar->pointOnArc(re)) return re;
	  else return G_point::inValid();
	}
	else {
	  G_point re = tmp * (r / (r + r2)) + p1;
	  if(pointOnArc(re) && ar->pointOnArc(re)) return re;
	  else return G_point::inValid();
	}
      }
      return G_point::inValid();
    }

    if(x < 0) x = 0; else x = sqrt(x);
    
    if(which == 1) x = -x;
    
    G_point tmp1;

    tmp1.setX((tmp.getX() * (SQR(r) - SQR(r2) + d) - tmp.getY() * x) / (2 * d));
    tmp1.setY((tmp.getY() * (SQR(r) - SQR(r2) + d) + tmp.getX() * x) / (2 * d));
    
    G_point re = tmp1 + p1;
    if(pointOnArc(re) && ar->pointOnArc(re)) return re;
    else return G_point::inValid();
  }

  //intersection with circle.
  if(c->getType() == G_CIRCLE) {
    G_circle *ci = (G_circle *)c;
    
    G_point tmp = ci->getCenter() - p1;
    double d = tmp * tmp;
    double x;
    
    if(fabs(d) < SMALL) return G_point::inValid();
    
    x = -(QUAD(r) + SQR(d - SQR(ci->getRadius())) -
	  2 * SQR(r) * (SQR(ci->getRadius()) + d));
    
    if(x < -SMALL) return G_point::inValid();
    
    if(fabs(x) <= SMALL) {
      if(which == 0) {
	if(d < SQR(QMAX(r, ci->getRadius()))) {
	  G_point re = tmp * (r / (r - getRadius())) + p1;
	  if(pointOnArc(re)) return re;
	  else return G_point::inValid();
	}
	else {
	  G_point re = tmp * (r / (r + getRadius())) + p1;
	  if(pointOnArc(re)) return re;
	  else return G_point::inValid();
	}
      }
      return G_point::inValid();
    }

    if(x < 0) x = 0; else x = sqrt(x);
    
    if(which == 1) x = -x;
    
    G_point tmp1;

    tmp1.setX((tmp.getX() * (SQR(r) - SQR(ci->getRadius()) + d) - tmp.getY() * x) / (2 * d));
    tmp1.setY((tmp.getY() * (SQR(r) - SQR(ci->getRadius()) + d) + tmp.getX() * x) / (2 * d));
    
    if(pointOnArc(tmp1 + p1)) return tmp1 + p1;
    else return G_point::inValid();
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

    G_point re = tmp + d * l->getDirection();
    if(pointOnArc(re)) return re;
    else return G_point::inValid();
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

    if(pointOnArc(re)) return re;
    else return G_point::inValid();

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

    if((nre - re) * (nre - re) >= SMALL * SMALL) return G_point::inValid();
    
    if(pointOnArc(nre)) return nre;
    else return G_point::inValid();
  }
  
  return c->getIntersection(this, which);
}
