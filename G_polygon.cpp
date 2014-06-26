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


#include "G_polygon.H"
#include "G_drawstyle.H"
#include "KSegView.H"
#include "G_segment.H"
#include "G_line.H"
#include "G_ray.H"
#include <QPolygon>

//drawing:
void G_polygon::draw(QPainter &p, const G_drawstyle &d, bool selected)
{ //FIXME!
  QPolygon pts(points.size());
  int i;
  for(i = 0; i < (int)points.size(); i++) {
    if(/*p.device()->isExtDev() ||*/ p.hasViewXForm()) pts[i] = (points[i] * 8).toQPoint();
    else pts[i] = points[i].toQPoint();
  }

  p.setBrush(d.getBrush());
  p.setPen(QPen(Qt::NoPen));

  if(/*p.device()->isExtDev() ||*/ p.hasViewXForm()) { //draw at higher accuracy to a printer or image
    p.scale(0.125, .125);
    p.drawPolygon(pts);
    p.scale(8, 8);
    return;
  }

  p.drawPolygon(pts);
  
  if(selected) {
    if(KSegView::getSelectType() == KSegView::BORDER_SELECT) {
      p.setBrush(QBrush(G_drawstyle::getBorderColor(d.getBrush().color()), Qt::BDiagPattern));
    }
    if(selected && KSegView::getSelectType() == KSegView::BLINKING_SELECT) {
      QColor c(QTime::currentTime().msec() * 17, 255, 255, QColor::Hsv);
      
      p.setBrush(QBrush(c, Qt::BDiagPattern));
    }
    
    p.drawPolygon(pts);
  }

  p.setBrush(QBrush());

  return;
}


G_point G_polygon::getNearestPoint(const G_point &p) const
{
  QPolygon pts(points.size());
  int i;
  for(i = 0; i < (int)points.size(); i++) {
    pts[i] = points[i].toQPoint();
  }

  if(QRegion(pts).contains(p.toQPoint())) return p;

  return points[0];
}


bool G_polygon::inRect(const QRect &rect) const
{
  if(getNearestPoint(rect.topRight()) == G_point(rect.topRight())) return true;
  if(getNearestPoint(rect.topLeft()) == G_point(rect.topLeft())) return true;
  if(getNearestPoint(rect.bottomRight()) == G_point(rect.bottomRight())) return true;
  if(getNearestPoint(rect.bottomLeft()) == G_point(rect.bottomLeft())) return true;

  int i;
  for(i = 0; i < (int)points.size(); i++) {
    if(G_segment(points[i], points[(i + 1) % points.size()]).inRect(rect)) return true;
  }

  return false;
}


double G_polygon::getArea() const
{
  double area = 0;
  int i;
  for(i = 0; i < (int)points.size(); i++) {
    area += points[i] % points[(i + 1) % points.size()];
  }
  return fabs(area / 2);
}


QRect G_polygon::getExtents() const
{
  int i;
  QRect e;
  for(i = 0; i < (int)points.size(); i++) {
    e |= QRect(points[i].toQPoint(), QSize(1, 1));
  }

  return e;
}


bool G_polygon::isValid() const
{
  int i;

  for(i = 0; i < (int)points.size(); i++) {
    if(points[i].isValid() == false) return false;
  }

  return points.size() > 2;
}


void G_polygon::translate(const G_point &p)
{
  int i;
  for(i = 0; i < (int)points.size(); i++) {
    points[i].translate(p);
  }
}


void G_polygon::rotate(const G_point &p, double d)
{
  int i;
  for(i = 0; i < (int)points.size(); i++) {
    points[i].rotate(p, d);
  }
}


void G_polygon::reflect(const G_straight &s)
{
  int i;
  for(i = 0; i < (int)points.size(); i++) {
    points[i].reflect(s);
  }
}


void G_polygon::scale(const G_point &p, double d)
{
  int i;
  for(i = 0; i < (int)points.size(); i++) {
    points[i].scale(p, d);
  }
}


