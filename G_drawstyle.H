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


#ifndef G_DRAWSTYLE_H
#define G_DRAWSTYLE_H

#include <qfont.h>
#include <qpen.h>
#include <qbrush.h>
#include <qdatastream.h>
#include <qmap.h>

#include "defs.H"

enum PointStyle
{
  ANY = 0,
  SMALL_CIRCLE,
  MEDIUM_CIRCLE,
  LARGE_CIRCLE
};

class G_refs;

class G_drawstyle
{
public:
  //the only way to get a G_drawstyle object
  static G_drawstyle* match(PointStyle point = ANY, const QFont *f = 0,
			    const QPen *p = 0, const QBrush *b = 0);

  static G_drawstyle* defaultStyle();
  
  void deleteReference(); // call this function once for every match after you don't need the pointer anymore
  void addReference() { num_refs++; } //use this for creating a "copy" of a drawstyle.

  PointStyle getPointStyle() const { return pointStyle; }
  const QFont &getFont() const { return font; }
  const QPen &getPen() const { return pen; }
  const QBrush &getBrush() const { return brush; }

  static QMap<G_drawstyle*, short> saveUsed(QDataStream &stream, G_refs &refs);
  static QVector<G_drawstyle *> loadUsed(QDataStream &stream);

  static QColor getBorderColor(const QColor &drawColor);
private:
  //constructor
  G_drawstyle();

  //data members
  PointStyle pointStyle;
  QFont font;
  QPen pen;
  QBrush brush;

  //sharing data:
  int num_refs;
  static QVector<G_drawstyle*> styles;
};

#endif //G_DRAWSTYLE_H
