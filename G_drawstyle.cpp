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


#include "G_drawstyle.H"
#include "G_refs.H"
#include "G_ref.H"
#include "KSegProperties.H"
#include "assert.h"

QVector<G_drawstyle*> G_drawstyle::styles;

G_drawstyle* G_drawstyle::match(PointStyle point, const QFont *f,
				const QPen *p, const QBrush *b)
{

  QVector<G_drawstyle*>::iterator iter=styles.begin();

  for(; iter < styles.end(); ++iter) {
    if(point != ANY && point != (*iter)->pointStyle) continue;
    if(f != 0 && *f != (*iter)->font) continue;
    if(p != 0 && *p != (*iter)->pen) continue;
    if(b != 0 && *b != (*iter)->brush) continue;
    //if we are here, we found a match.  Add a reference to it and return it.

    (*iter)->num_refs++;
    return *iter;
  }

  //if we are here, we need to create a new drawstyle.

  G_drawstyle* newDrawstyle;
  newDrawstyle = new G_drawstyle();
  if(point != ANY) newDrawstyle->pointStyle = point; else newDrawstyle->pointStyle = LARGE_CIRCLE;
  if(f != 0) newDrawstyle->font = *f; else newDrawstyle->font = QFont("Arial", 20);
  if(p != 0) newDrawstyle->pen = *p; else newDrawstyle->pen = QPen();
  if(b != 0) newDrawstyle->brush = *b; else newDrawstyle->brush =
					      QBrush(Qt::black, Qt::Dense4Pattern);

  return newDrawstyle;
}

G_drawstyle *G_drawstyle::defaultStyle() {
  QFont f("Arial", 20);
  QPen p;
  QBrush b(Qt::black, Qt::Dense4Pattern);

  return match(LARGE_CIRCLE, &f, &p, &b);
}

void G_drawstyle::deleteReference() // do not use the object after calling this guy.
{
  num_refs--;

  if(num_refs == 0) {
    styles.remove(styles.indexOf(this) );
    delete this;
  }
}

G_drawstyle::G_drawstyle()
{
  num_refs = 1;

  pointStyle = SMALL_CIRCLE;

  styles.append(this);
}


QMap<G_drawstyle *, short> G_drawstyle::saveUsed(QDataStream &stream, G_refs &refs)
{
  QMap<G_drawstyle *, short> toBeReturned;
  std::vector<G_drawstyle *> usedDrawstyles;

  int i;
  int numStyles = 0;

  //make the map first
  for(i = 0; i < (int)refs.count(); i++) {
    if(!(toBeReturned.contains(refs[i]->getDrawstyle()))) {
      toBeReturned.insert(refs[i]->getDrawstyle(), numStyles++);
      usedDrawstyles.push_back(refs[i]->getDrawstyle());
    }
  }

  assert(numStyles < 32767); 

  //now save all the drawstyles in the map:
  stream << (short)numStyles;

  for(i = 0; i < numStyles; ++i) {
    G_drawstyle *tmp = usedDrawstyles[i];

    stream << (Q_INT8)tmp->pointStyle;
    stream << tmp->font;
    stream << tmp->pen;
    stream << tmp->brush;
  }

  return toBeReturned;
}

QVector<G_drawstyle *> G_drawstyle::loadUsed(QDataStream &stream)
{
  QVector<G_drawstyle *> toBeReturned;

  short numStyles;
  int i;

  stream >> numStyles;

  toBeReturned.resize(numStyles);

  for(i = 0; i < numStyles; i++) {
    Q_INT8 pointStyle;
    QFont font;
    QBrush brush;
    QPen pen;

    stream >> pointStyle >> font >> pen >> brush;

    toBeReturned[i] = match((PointStyle)pointStyle, &font, &pen, &brush);
  }

  return toBeReturned;
}

QColor G_drawstyle::getBorderColor(const QColor &drawColor)
{
  QColor bg = QColor(KSegProperties::getProperty("BackgroundColor"));

  int rbg, gbg, bbg, rdc, gdc, bdc;

  bg.rgb(&rbg, &gbg, &bbg);
  drawColor.rgb(&rdc, &gdc, &bdc);

  int dred = QMIN(SQR(rbg - 255) + SQR(gbg) + SQR(bbg), SQR(rdc - 255) + SQR(gdc) + SQR(bdc));
  int dblack = QMIN(SQR(rbg) + SQR(gbg) + SQR(bbg), SQR(rdc) + SQR(gdc) + SQR(bdc));
  int dwhite = QMIN(SQR(rbg - 255) + SQR(gbg - 255) + SQR(bbg - 255), \
		    SQR(rdc - 255) + SQR(gdc - 255) + SQR(bdc - 255));
  
  if(dred > dblack && dred > dwhite) return Qt::red;
  if(dblack > dwhite) return Qt::black;
  return Qt::white;
}
