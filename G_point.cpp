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


#include "G_point.H"
#include "G_line.H"
#include <qpainter.h>
#include "G_drawstyle.H"
#include "KSegView.H"
#include <qdatastream.h>

void G_point::draw(QPainter &p, const G_drawstyle &drawstyle, bool selected)
{

  QPoint t = p.xForm(toQPoint());
  if(t.x() > DRAW_MAX || t.x() < -DRAW_MAX || t.y() > DRAW_MAX || t.y() < -DRAW_MAX) return;

  int size;
  switch(drawstyle.getPointStyle()) {
  case SMALL_CIRCLE:
    size = 2; break;
  case MEDIUM_CIRCLE:
    size = 3; break;
  case LARGE_CIRCLE:
    size = 4; break;
  default:
    size = 0;
    break;
  }

  p.setPen(QPen(Qt::NoPen)); //ignore drawstyle settings
  if(selected && KSegView::getSelectType() == KSegView::BORDER_SELECT) {
    p.setPen(QPen(G_drawstyle::getBorderColor(drawstyle.getPen().color()), 2));
    size += 2;
  }
  p.setBrush(QBrush(drawstyle.getPen().color()));

  if(selected && KSegView::getSelectType() == KSegView::BLINKING_SELECT)
    p.setBrush(QBrush(QColor(QTime::currentTime().msec() * 17, 255, 255, QColor::Hsv)));
    
  if(/*p.device()->isExtDev() ||*/ p.hasViewXForm()) { //draw at higher accuracy to a printer or image
    p.scale(0.125, .125);
    size *= 8;
    p.drawEllipse(qRound(x * 8. - size), qRound(y * 8. - size), size * 2, size * 2);
    p.scale(8, 8);

    return;
  }

  p.drawEllipse(qRound(x - size), qRound(y - size), size * 2, size * 2);

  return;
}


bool G_point::inRect(const QRect &r) const
{
  return r.contains(toQPoint());
}

//transformations:
void G_point::translate(const G_point &p)
{
  (*this) += p;
}

void G_point::scale(const G_point &p, double d)
{
  *this = p + (*this - p) * d;
}

void G_point::rotate(const G_point &p, double d)
{
  *this -= p;

  double oldX = x;
  x = x * cos(d) - y * sin(d);
  y = oldX * sin(d) + y * cos(d);

  *this += p;
}

void G_point::reflect(const G_straight &s)
{
  *this =
    G_line(s.getNearestPoint(*this), s.getDirection()).
    getNearestPoint(*this) * 2 - *this;
}

//save and retrieve:
QDataStream &operator<< (QDataStream &d, const G_point &p)
{
  d << (float)p.x << (float)p.y;
  return d;
}
QDataStream &operator>> (QDataStream &d, G_point &p)
{
  float x, y;

  d >> x >> y;
  p.x = x; p.y = y;

  return d;
}



