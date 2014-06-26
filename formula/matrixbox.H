/*
 *  KSeg/formula
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


#ifndef MATRIXBOX_H_INCLUDED
#define MATRIXBOX_H_INCLUDED

#include "box.H"
#include <QPolygon>
#include <QVector>

/**
 * Matrix formula layout helper.
 * @internal
 */
class matrixbox : public box
{
protected:
  int width;
  int height; // number of elements in each direction

  QVector<box *> elems;
  QPolygon elem_pos; // the relative positions of all the elements

public:
  matrixbox();
  matrixbox(int w, int h);

  ~matrixbox();

  void addElem(box *e);

  void calculate(QPainter &p, int setFontsize = -1, QFont *f = 0, QColor *bc = 0, QColor *fc = 0);
  void draw(QPainter &p, int x, int y, QFont *f = 0, QColor *bc = 0, QColor *fc = 0);

  QRect getCursorPos(charinfo i, int x, int y);
};

#endif // MATRIXBOX_H_INCLUDED
