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

#ifndef KSEGCONSTRUCTIONLIST_H
#define KSEGCONSTRUCTIONLIST_H

#include <algorithm>
#include <vector>
#include <qscrollbar.h>
#include <QFrame>
#include <qtimer.h>
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include "formula/kformula.H"
#include "KSegDocument.H"
#include "G_refSearcher.H"


class ListReferenceFetcher : public ReferenceFetcher, public vector<KFormula *>
{
public:
  virtual ~ListReferenceFetcher() {}
  virtual KFormula *getFormula(int ref_num) { return (*this)[ref_num]; }
}; //generic reference fetcher


class KSegConstructionList : public QFrame
{
  Q_OBJECT
public:
  KSegConstructionList(KSegDocument *inDoc, QWidget *parent = 0);

  static void enableRedraws() { redrawsEnabled = true; }
  static void disableRedraws() { redrawsEnabled = false; }

protected slots:
  void redrawBuffer();
  void curItemChanged(int newCurrent);
  void hoffsetChanged(int newHoffset);
  void autoScroll();

protected:
  QPixmap buffer;
  
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);
  void mousePressEvent(QMouseEvent *ev);
  void mouseReleaseEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  KSegDocument *doc;

  int getItemFromYCoord(int y)
  { if(y < 0 || y > itemYCoords.back()) return -1;
  return lower_bound(itemYCoords.begin(), itemYCoords.end(), y) - itemYCoords.begin() + current; }

  void selectionChanged(); //alters the part that's displayed
  
  int current;
  int hoffset;
  QScrollBar *hBar, *vBar;

  std::vector<int> itemYCoords;
  G_ref *lastSel;
  int dragStart;
  G_refs selectedAtDragStart;
  QTimer scrollTimer;

  static NotInConstructionSearcher nicSearcher;
  static ImplicitFinalSearcher ifSearcher;
  static ImplicitInitialSearcher iiSearcher;

  static void nameObject(G_ref *ref, KFormula &f, ListReferenceFetcher &l, int index);
  static bool redrawsEnabled;
};

#endif //KSEGCONSTRUCTIONLIST_H
