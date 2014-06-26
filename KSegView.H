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


#ifndef KSEGVIEW_H
#define KSEGVIEW_H

#include <qpixmap.h>
#include <qtimer.h>
#include <qwidget.h>
#include <qscrollbar.h>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include "G_refs.H"
#include "G_point.H"
#include "G_matrix.H"
#include "G_object.H"

class KSegDocument;
class G_ref;
class G_undoReconstrain;

class KSegView : public QWidget
{
  Q_OBJECT
public:
  KSegView(KSegDocument *inDoc, QWidget *parent = 0);
  virtual ~KSegView();

  enum SelectType {
    BORDER_SELECT,
    BLINKING_SELECT
  };

  enum DragType {
    NO_DRAG,
    MOVE_DRAG,
    RECTANGLE_DRAG,
    LABEL_DRAG,
    RECONSTRAIN_DRAG,
    PAN_DRAG,
    ZOOM_DRAG
  };

  DragType getDrag() { return drag; }

  static SelectType getSelectType() { return selectType; }

  KSegDocument *getDocument() const { return doc; }
  void delDocumentRef(); //called when window is about to be closed

  void updateStatusBar(Qt::ButtonState state);

  static QVector<KSegView*> getAllViews() { return allViews; }

  bool getMenusEnabled() { return menusEnabled; }
  double getOffsetX() { return offsetX; }
  double getOffsetY() { return offsetY; }
  double getZoom() { return zoom; }

  bool getNextPan() { return nextPan; }
  bool getNextZoom() { return nextZoom; }
  
signals:
  void statusBarMessage(const QString &);
  void updateMenus();

public slots:
  void redrawBuffer();
  void documentSaved(const QString &filename);
  void documentModified();
  void setMenusEnabled(bool b) { menusEnabled = b; emit updateMenus(); }

  void viewPan();
  void viewZoom();
  void viewZoomFit();
  void viewOrigZoom();

  void playConstruction(int which);

protected:
  QPixmap buffer;

  QScrollBar *hBar, *vBar;

  void resizeEvent(QResizeEvent *);
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseDoubleClickEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);

protected slots:
  void moved(int);
  void selectTimeout() { if(selectType == BLINKING_SELECT) redrawBuffer(); }

private:
  QTimer selectTimer; // for doing the selection animation
  bool forcePaint;

  KSegDocument *doc;
  bool docReferenceRemoved;

  //refresh stuff
  int lastMouseX, lastMouseY;
  DragType drag;
  G_refs objectsDragged;
  G_refs objectsUpdating;
  hash_map<G_ref *, G_matrix> dragPattern;

  G_point dragOffset; //for stuff like labels and constrained points.
  int dragStartX, dragStartY;
  int dragCurX, dragCurY;

  //undo for during reconstraint
  G_undoReconstrain *reconstrainUndo;

  //scrolling stuff
  double offsetX, offsetY, zoom;

  //for calculate dialog and moving, menu disabling
  bool menusEnabled;

  bool nextZoom, nextPan;

  static SelectType selectType;

  static QVector<KSegView*> allViews;

  void zoomAnimation(double oldOffsetX, double oldOffsetY, double oldZoom);
  
public:
  static void updateAllViews();

  static QPainter *constructionPainter;
  static ViewTransform constructionTransform;

  static int ScrollBarThickness;

};

#endif // KSEGVIEW_H





