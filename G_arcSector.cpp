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


#include "G_arcSector.H"
#include "G_drawstyle.H"
#include "KSegView.H"
#include "G_segment.H"
#include "G_line.H"
#include "G_ray.H"

//drawing:
void G_arcSector::draw(QPainter &p, const G_drawstyle &d, bool selected)
{ //FIXME!
  double r = arc.getRadius();
  QRect re = p.window();

  if(!inRect(re)) return;

  if(r > DRAW_MAX / 2) {
    //draw big arc...

    return;
  }

  p.setBrush(d.getBrush());
  p.setPen(QPen(Qt::NoPen));

  const double angle_scale = -16 * 180. / M_PI;
  
  if(/*p.device()->isExtDev() ||*/ p.hasViewXForm()) { //draw at higher accuracy to a printer or image
    p.scale(0.125, .125);

    p.drawPie(ROUND((arc.getCenter().getX() - r) * 8),
	      ROUND((arc.getCenter().getY() - r) * 8),
	      ROUND(r * 16), ROUND(r * 16),
	      ROUND(((arc.getP2() - arc.getCenter()).angle() -
		     acos(arc.getCosangle())) * angle_scale),
	      ROUND(angle_scale * acos(arc.getCosangle()) * 2));

    p.scale(8, 8);
    return;
  }

  p.drawPie(ROUND(arc.getCenter().getX() - r),
	    ROUND(arc.getCenter().getY() - r),
	    ROUND(r * 2), ROUND(r * 2),
	    ROUND(((arc.getP2() - arc.getCenter()).angle() -
		   acos(arc.getCosangle())) * angle_scale),
	    ROUND(angle_scale * acos(arc.getCosangle()) * 2));
  
  if(selected) {
    if(KSegView::getSelectType() == KSegView::BORDER_SELECT) {
      p.setBrush(QBrush(G_drawstyle::getBorderColor(d.getBrush().color()), Qt::BDiagPattern));
    }
    if(selected && KSegView::getSelectType() == KSegView::BLINKING_SELECT) {
      QColor c(QTime::currentTime().msec() * 17, 255, 255, QColor::Hsv);
      
      p.setBrush(QBrush(c, Qt::BDiagPattern));
    }
    
    p.drawPie(ROUND(arc.getCenter().getX() - r),
	      ROUND(arc.getCenter().getY() - r),
	      ROUND(r * 2), ROUND(r * 2),
	      ROUND(((arc.getP2() - arc.getCenter()).angle() -
		     acos(arc.getCosangle())) * angle_scale),
	      ROUND(angle_scale * acos(arc.getCosangle()) * 2));    

  }

  p.setBrush(QBrush());

  return;

}


G_point G_arcSector::getNearestPoint(const G_point &p) const
{ //not really--returns point if point is in arcsector, center of sector otherwise.
  if((p - arc.getCenter()).length() > arc.getRadius()) return arc.getCenter();
  if((p - arc.getCenter()).normalize() * (arc.getP2() - arc.getCenter()).normalize() <
     arc.getCosangle()) return arc.getCenter();
  return p;
}

bool G_arcSector::inRect(const QRect &rect) const
{
  if(getNearestPoint(rect.topRight()) == G_point(rect.topRight())) return true;
  if(getNearestPoint(rect.topLeft()) == G_point(rect.topLeft())) return true;
  if(getNearestPoint(rect.bottomRight()) == G_point(rect.bottomRight())) return true;
  if(getNearestPoint(rect.bottomLeft()) == G_point(rect.bottomLeft())) return true;

  if(arc.inRect(rect)) return true;
  if(G_segment(arc.getCenter(), arc.getPointOnCurve(0)).inRect(rect)) return true;
  if(G_segment(arc.getCenter(), arc.getPointOnCurve(1)).inRect(rect)) return true;

  return false;
}
