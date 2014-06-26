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

#include <qapplication.h>
#include <qcursor.h>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QFrame>
#include <QPixmap>
#include <QMouseEvent>
#include <QEvent>
#include "KSegView.H"
#include "KSegConstructionList.H"
#include "G_ref.H"

bool KSegConstructionList::redrawsEnabled = true;
NotInConstructionSearcher KSegConstructionList::nicSearcher;
ImplicitFinalSearcher KSegConstructionList::ifSearcher;
ImplicitInitialSearcher KSegConstructionList::iiSearcher;

KSegConstructionList::KSegConstructionList(KSegDocument *inDoc, QWidget *parent)
  : QFrame(parent), doc(inDoc), current(0), hoffset(0), lastSel(NULL)
{
  connect(doc, SIGNAL(documentChanged()), this, SLOT(redrawBuffer()));
  setBackgroundMode(Qt::PaletteBase);
  vBar = new QScrollBar(Qt::Vertical, this);
  vBar->setMaxValue(0);
  connect(vBar, SIGNAL(valueChanged(int)), this, SLOT(curItemChanged(int)));
  hBar = new QScrollBar(Qt::Horizontal, this);  
  hBar->setMaxValue(0);
  connect(hBar, SIGNAL(valueChanged(int)), this, SLOT(hoffsetChanged(int)));
  buffer.resize(width(), height());
  setBackgroundMode(Qt::NoBackground);

  connect(&scrollTimer, SIGNAL(timeout()), this, SLOT(autoScroll()));
}


void KSegConstructionList::resizeEvent(QResizeEvent *)
{
  hBar->move(0, height() - KSegView::ScrollBarThickness);
  hBar->resize(width() - KSegView::ScrollBarThickness,
	       KSegView::ScrollBarThickness);
  vBar->move(width() - KSegView::ScrollBarThickness, 0);
  vBar->resize(KSegView::ScrollBarThickness, height());
  buffer.resize(width(), height());
  redrawBuffer();
}


void KSegConstructionList::redrawBuffer()
{
  int i;

  if(!redrawsEnabled) return;

  buffer.fill();
  QPainter p(&buffer);

  if(doc->allRefs.size() == 0) {
    current = 0;
    vBar->blockSignals(true);
    vBar->setMaxValue(0);
    vBar->blockSignals(false);
    hBar->setMaxValue(0);
    repaint();
    return;
  }

  vBar->blockSignals(true);
  vBar->setMaxValue(doc->allRefs.size() - 1);
  vBar->blockSignals(false);

  selectionChanged();

  if(current >= int(doc->allRefs.size())) current = doc->allRefs.size() - 1;

  vBar->blockSignals(true);
  vBar->setValue(current);
  vBar->blockSignals(false);

  int curCoord = 0;

  QPixmap pix(1,1);
  QPainter pixp(&pix);

  itemYCoords.clear();

  int maxwidth = 0;

  ListReferenceFetcher *l = new ListReferenceFetcher();
  KFormula form;
  form.setReferenceFetcher(l);

  nicSearcher.reInit();
  ifSearcher.reInit();
  iiSearcher.reInit();

  for(i = current; i < int(doc->allRefs.size()); ++i) {
    KFormula *f = &form;
    nameObject(doc->allRefs[i], form, *l, i);

    f->redraw(pixp);

    int fsize = f->size().height();
    int hsize = f->size().width();
    if(hsize > maxwidth) maxwidth = hsize;

    if(doc->allRefs[i]->getSelected()) {
      p.fillRect(0, curCoord, width(), fsize, QBrush(Qt::blue));
      p.drawRect(0, curCoord, width(), fsize);
    }

    f->setPos((hsize + 1) / 2 - hoffset, (fsize + 1) / 2 + curCoord);
    f->redraw(p);

    curCoord += fsize;
    if(curCoord > height() - KSegView::ScrollBarThickness) break;
    itemYCoords.push_back(curCoord);
  }

  nicSearcher.reInit();
  ifSearcher.reInit();
  iiSearcher.reInit();

  hBar->setPageStep(width() - KSegView::ScrollBarThickness);
  if(maxwidth >= hBar->value() + width() - KSegView::ScrollBarThickness) {
    if(maxwidth > width() - KSegView::ScrollBarThickness) {
      hBar->setMaxValue(maxwidth - width() + KSegView::ScrollBarThickness);
    }
  }
  else {
    hBar->setMaxValue(QMAX(0, hBar->value()));
  }

  repaint();
}


void KSegConstructionList::selectionChanged()
{
  if(scrollTimer.isActive()) return; //don't do this when scrolling.
  if(doc->selectedRefs.size() == 0) { lastSel = NULL; return; }
  int sel = doc->allRefs.find(doc->selectedRefs.back());

  if(doc->allRefs[sel] == lastSel) return;
  lastSel = doc->allRefs[sel];

  if(sel >= current && sel <= current + (int)itemYCoords.size()) return;
  current = sel;
}

void KSegConstructionList::paintEvent(QPaintEvent *)
{
  bitBlt(this, 0, 0, &buffer);
}


void KSegConstructionList::curItemChanged(int newCurrent)
{
  current = newCurrent;
  redrawBuffer();
}


void KSegConstructionList::hoffsetChanged(int newHoffset)
{
  hoffset = newHoffset;
  redrawBuffer();
}


void KSegConstructionList::mouseReleaseEvent(QMouseEvent *)
{
  scrollTimer.stop();
}


void KSegConstructionList::mousePressEvent(QMouseEvent *ev)
{
  if(doc->allRefs.size() == 0) return;
  if(ev->button() != Qt::LeftButton) return;

  if((ev->state() & Qt::ShiftButton) == 0) doc->clearSel();
  int which = getItemFromYCoord(ev->y());
  if(which >= 0) {
    if(doc->allRefs[which]->getSelected()) {
      doc->delSel(doc->allRefs[which]);
    }
    else {
      doc->addSel(doc->allRefs[which]);
    }
  }

  if(ev->y() > itemYCoords.back()) dragStart = doc->allRefs.size() - 1;
  else dragStart = which;

  selectedAtDragStart = doc->getSelected();

  doc->emitDocumentChanged();
}


void KSegConstructionList::mouseMoveEvent(QMouseEvent *ev)
{
  if((ev->state() & Qt::LeftButton) == 0) return;

  if(scrollTimer.isActive() == false) scrollTimer.start(10);

  int which = getItemFromYCoord(ev->y());
  if(ev->y() < 0 && current > 0) { which = --current; }
  if(ev->y() > itemYCoords.back() && current + itemYCoords.size() < doc->allRefs.size()) {
    which = current + itemYCoords.size();
    current++;
  }
  else if(ev->y() > itemYCoords.back() && doc->allRefs.size() > 0) {
    which = doc->allRefs.size() - 1;
  }
  if(which == -1) return;
  int dir = which > dragStart ? 1 : -1;
  int i;

  G_refs sel = doc->getSelected();

  for(i = 0; i < (int)(sel.size()); i++) {
    if(selectedAtDragStart.find(sel[i]) == -1) doc->delSel(sel[i]);
  }

  for(i = dragStart; i * dir <= which * dir; i += dir) {
    if(selectedAtDragStart.find(doc->allRefs[i]) == -1) doc->addSel(doc->allRefs[i]);    
  }

  doc->emitDocumentChanged();

}

void KSegConstructionList::autoScroll()
{
  QApplication::postEvent(this, new QMouseEvent(QEvent::MouseMove,
						mapFromGlobal(QCursor::pos()),
						Qt::LeftButton, Qt::LeftButton));
}


