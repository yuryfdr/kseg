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


#ifndef G_LABEL_H
#define G_LABEL_H

#include <qstring.h>
#include "G_point.H"
#include "formula/kformula.H"

class G_ref;

//maximum distance of label from object:
#define LABEL_DISTANCE  (2.) //formulas have their own padding


class G_label
{
  friend QDataStream &operator<<(QDataStream &, G_label &);
  friend QDataStream &operator>>(QDataStream &, G_label &);

public:
  G_label(G_ref *inRef) { ref = inRef;  relPos = 0.5;  relCoord = G_point(20, 20); }

  void setString(const QString &inText); //assumes input in ugly form

  QString getText() const { return text; }

  void setText(const QString &inText, bool inUpdatePos = true); //for "internal use"

  QRect getRect();
  QSize getSize();
  void setPos(const G_point &);
  G_point getPos() const;

  void draw(QPainter &, const G_drawstyle &, bool selected);

  KFormula *getFormula() { return &output; } //only to be used by ReferenceFetchers
private:
  G_label() {}

  KFormula output;
  QString text;
  G_point relCoord;
  double relPos;
  G_ref *ref;
};

QDataStream &operator<<(QDataStream &, G_label &);
QDataStream &operator>>(QDataStream &, G_label &);

#endif //G_LABEL_H
