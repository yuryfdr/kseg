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


#ifndef KSEGDOCUMENT_H
#define KSEGDOCUMENT_H

#include "my_hash_map.H"
#include <qobject.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qpointer.h>
#include "G_refs.H"
#include "G_undoStack.H"
#include "G_drawstyle.H"
#include "KSegSelectionGroupDialog.H"

class KSegSelectionGroup;

struct ViewTransform;

class KSegDocument : public QObject
{
  friend class KSegConstructionList;
  Q_OBJECT
public:

  KSegDocument();
  virtual ~KSegDocument();

  virtual bool isConstruction() { return false; }

  int getNumRefs() const { return allRefs.size(); }

  friend QDataStream &operator<<(QDataStream &, KSegDocument &);
  friend QDataStream &operator>>(QDataStream &, KSegDocument &);

  //parses a string like "KSeg Document Version 2.3" and returns true if successful
  static bool parseVersionString(const QString &s, bool *isConstruction = NULL,
				 int *majorVersion = NULL, int *minorVersion = NULL);

  virtual void addRef(G_ref *inRef);
  virtual void delRef(G_ref *inRef);

  G_ref *getRef(int which) const { return allRefs[which]; }
  
  void addView() { numViews++; }
  void delView() { numViews--; if(numViews == 0) delete this; }
  int getNumViews() const { return numViews; }

  void draw(QPainter &p, const ViewTransform &trans);
  void print(QPainter &p, const ViewTransform &trans);

  QRect getSize(const ViewTransform &trans);

  G_ref *getNextSel(G_refs inRefs, bool shiftDown);
  G_refs whatAmIOn(int x, int y, const ViewTransform &trans, bool pointsOnly = true);
  G_refs whatLabelsAmIOn(int x, int y, const ViewTransform &trans);

  void addSelect(const QRect &, const ViewTransform &trans); //select all in rectangle.

  void addSel(G_ref *inRef);
  void delSel(G_ref *inRef);
  void clearSel();

  G_refs getSelected() const { return selectedRefs; }
  int selectedCount() const { return selectedRefs.count(); }

  void emitDocumentChanged() { emit documentChanged(); }
  void emitDocumentModified()
  { if(!changed) {changed = true; emit documentModified();} }
  void emitDocumentSaved(const QString &f)
  { filename = f; changed = false; emit documentSaved(f); }

  bool isModified() const { return changed; }

  QString getFilename() const { return filename; }

  QPrinter *getPrinter() const { return printer; }

  G_drawstyle *getDefaultDrawstyle() { return defaultDrawstyle; }
  void setDefaultDrawstyle(G_drawstyle *inDrawstyle)
  { defaultDrawstyle->deleteReference(); defaultDrawstyle = inDrawstyle; }

  void addUndo(G_undo *undo) {
    if(isUndoing) {
      redoStack.push(QSharedPointer<G_undo>(undo));
    }
    else {
      undoStack.push(QSharedPointer<G_undo>(undo));
      if(!isRedoing) redoStack.clear();
    }
  }

  virtual bool isPointFreeable(G_ref *p);

  void topSortAllRefs() { allRefs.topologicalSort(allRefs); }

  QString autoLabel(G_ref *r);

  QColor getCurrentColor(); //for the edit menu
  void setCurrentColor(QColor color); //also for the edit menu
  //
  Qt::PenStyle getCurrentPenstyle();
  void setCurrentPenstyle(Qt::PenStyle penstyle);
  //
  int getCurrentPenWidth();
  void setCurrentPenWidth(int width);
  //
  PointStyle getCurrentPointstyle();
  void setCurrentPointstyle(PointStyle style);
  //
  int getCurrentFontsize();
  void setCurrentFontsize(int size);
  //
  QFont getCurrentFont();
  void setCurrentFont(const QFont &font);

  //these guys return whether something can be done with the current selection
  bool canNewSegment();
  bool canNewRay();
  bool canNewMidpoint();
  bool canNewLine();
  bool canNewPerpendicular();
  bool canNewBisector();
  bool canNewCircle();
  bool canNewArc();
  bool canNewIntersection();
  bool canNewEndpoints();
  bool canNewCenterpoint();
  bool canNewLocus();
  bool canNewPolygon();
  bool canNewArcSector();
  bool canNewArcSegment();
  bool canNewCircleInterior();

  bool canEditDelete(); //selected count > 0
  bool canEditHide();
  bool canEditToggleLabels();
  bool canEditChangeLabel();
  bool canEditShowHidden();
  bool canEditUndo() { return undoStack.count(); }
  bool canEditRedo() { return redoStack.count(); }
  bool canEditChangeNumberOfSamples();

  bool canEditChangeColor();
  bool canEditChangeLinestyle();
  bool canEditChangePointstyle();
  bool canEditChangeFont();

  bool canMeasureDistance();
  bool canMeasureLength();
  bool canMeasureRadius();
  bool canMeasureAngle();
  bool canMeasureRatio();
  bool canMeasureSlope();
  bool canMeasureArea();
  bool canMeasureCalculate();

  bool canTransformChooseVector();
  bool canTransformChooseMirror();
  bool canTransformChooseCenter();
  bool canTransformChooseRatio();
  bool canTransformChooseAngle();
  bool canTransformClearChosen();
  bool canTransformTranslate();
  bool canTransformRotate();
  bool canTransformScale();
  bool canTransformReflect();

  bool canPlay(int which);

public slots:
  void newSegment();
  void newRay();
  void newMidpoint();
  void newLine();
  void newPerpendicular();
  void newBisector();
  void newCircle();
  void newIntersection();
  void newEndpoints();
  void newCenterpoint();
  void newArc();
  void newLocus();
  void newPolygon();
  void newArcSector();
  void newArcSegment();
  void newCircleInterior();

  void editUndo();
  void editRedo();
  void editDelete();
  void editToggleLabels();
  void editHideLabels();
  void editShowLabels();
  void editChangeLabel();
  void editHide();
  void editShowHidden();
  void editChangeNumberOfSamples();
  void editShowSelectionGroupDialog();

  void measureDistance();
  void measureLength();
  void measureRadius();
  void measureAngle();
  void measureRatio();
  void measureSlope();
  void measureArea(); 
  void measureCalculate();

  void transformChooseVector();
  void transformChooseMirror();
  void transformChooseAngle();
  void transformChooseCenter();
  void transformChooseRatio();
  void transformClearChosen();
  void transformTranslate();
  void transformReflect();
  void transformRotate();
  void transformScale();

  void doPlay(int which);

signals:
  void documentChanged();
  void documentSaved(const QString &filename);
  void documentModified();

protected slots:
  void IChanged() { undoStack.opFinished(); }

protected:
  void afterLoadDraw(); //makes sure all the formula objects are
                        //calculated correctly after loading

  bool isUndoing;
  bool isRedoing;

  G_refs allRefs;
  G_refs selectedRefs;

  G_refs t_vector, t_ratio, t_center, t_angle, t_mirror;

  G_undoStack undoStack;
  G_undoStack redoStack;

  hash_map<int, int> labelData; //stores the number of each type of object
                                //created for generating labels

  G_drawstyle *defaultDrawstyle;

  bool changed;
  QString filename;

  QPrinter *printer;

  QPointer<KSegSelectionGroupDialog> selectionGroupDialog;
  vector<KSegSelectionGroup *> selectionGroups;
private:
  int numViews;
};

QDataStream &operator<<(QDataStream &, KSegDocument &);
QDataStream &operator>>(QDataStream &, KSegDocument &);


class G_locusObject;
class KSegSampleChangeDialog : public QDialog
{
Q_OBJECT
public:
  KSegSampleChangeDialog(G_ref *inLocus, KSegDocument *inDoc);

public slots:
  void setIt(int val);
  void reject() { setIt(originalNumSamples); QDialog::reject(); }

protected:
  G_locusObject *locus;
  KSegDocument *doc;
  int originalNumSamples;
  G_refs objectsToUpdate;
};

#endif // KSEGDOCUMENT_H
