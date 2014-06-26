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

#include <iostream>
#include <qlayout.h>
#include <qdialog.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "KSegCalculateEditor.H"
#include "KSegView.H"
#include "KSegConstruction.H"
#include "KSegRecursionDialog.H"
#include "G_refSearcher.H"
#include "KSegDocument.H"
#include "G_ref.H"
#include "G_object.H"
#include "KSegSelectionGroupDialog.H"

#include "formula/kformulaedit.H"


//this file contains all the menu handlers by KSegDocument.


void KSegDocument::newSegment()
{
  G_refs tmp;
  G_refs new_refs;
  int i;

  if(!canNewSegment()) {
    Q_ASSERT(canNewSegment());
    return;
  }

  if(selectedRefs.count() == 2) { // two points
    G_ref *s = new G_ref();
    s->create(G_SEGMENT, G_ENDPOINTS_SEGMENT, selectedRefs, this);
    clearSel();
    addSel(s);
    emit documentChanged();
    return;
  }

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    G_ref *s = new G_ref();
    tmp.clear();
    tmp.append(selectedRefs[i]);
    if(i + 1 < (int)selectedRefs.count()) {
      tmp.append(selectedRefs[i + 1]);
    }
    else { tmp.append(selectedRefs[0]); }

    s->create(G_SEGMENT, G_ENDPOINTS_SEGMENT, tmp, this);

    new_refs.append(s);
  }
  clearSel();
  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;
}


void KSegDocument::newRay()
{
  G_refs tmp;
  G_refs new_refs;
  int i;

  if(!canNewRay()) {
    Q_ASSERT(canNewRay());
    return;
  }

  if(selectedRefs.count() == 2) { // two points
    G_ref *s = new G_ref();
    s->create(G_RAY, G_TWOPOINTS_RAY, selectedRefs, this);
    clearSel();
    addSel(s);
    emit documentChanged();
    return;
  }

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    G_ref *s = new G_ref();
    tmp.clear();
    tmp.append(selectedRefs[i]);
    if(i + 1 < (int)selectedRefs.count()) {
      tmp.append(selectedRefs[i + 1]);
    }
    else { tmp.append(selectedRefs[0]); }

    s->create(G_RAY, G_TWOPOINTS_RAY, tmp, this);

    new_refs.append(s);
  }
  clearSel();
  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;
}


void KSegDocument::newBisector()
{
  if(!canNewBisector()) {
    Q_ASSERT(canNewBisector());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_RAY, G_BISECTOR_RAY, selectedRefs, this);

  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newIntersection()
{
  if(!canNewIntersection()) {
    Q_ASSERT(canNewIntersection());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_POINT, G_INTERSECTION_POINT, selectedRefs, this);

  if(!(selectedRefs[0]->getType() & G_STRAIGHT && selectedRefs[1]->getType() & G_STRAIGHT)) {
    G_ref *p1 = new G_ref();

    p1->create(G_POINT, G_INTERSECTION2_POINT, selectedRefs, this);
    
    clearSel();

    if(p->getExists()) addSel(p);
    if(p1->getExists()) addSel(p1);
  }
  else {
    clearSel();

    addSel(p);
  }

  emit documentChanged();
}


void KSegDocument::newEndpoints()
{
  if(!canNewEndpoints()) {
    Q_ASSERT(canNewEndpoints());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_POINT, G_END_POINT, selectedRefs, this);

  if(!(selectedRefs[0]->getType() & G_RAY)) {
    G_ref *p1 = new G_ref();

    p1->create(G_POINT, G_END2_POINT, selectedRefs, this);
    
    clearSel();

    addSel(p);
    addSel(p1);
  }
  else {
    clearSel();

    addSel(p);
  }

  emit documentChanged();
}


void KSegDocument::newCenterpoint()
{
  if(!canNewCenterpoint()) {
    Q_ASSERT(canNewCenterpoint());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_POINT, G_CENTER_POINT, selectedRefs, this);

  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newCircle()
{
  if(!canNewCircle()) {
    Q_ASSERT(canNewCircle());
    return;
  }

  G_refs tmp;

  G_ref *p = new G_ref();

  if(selectedRefs[0]->getType() == G_POINT && selectedRefs[1]->getType() == G_POINT) {
    //by center and point
    p->create(G_CIRCLE, G_CENTERPOINT_CIRCLE, selectedRefs, this);
  }
  else {
    //by center and radius
    if(selectedRefs[0]->getType() == G_POINT) {
      tmp = selectedRefs;
    }
    else { // reverse the order
      tmp.append(selectedRefs[1]);
      tmp.append(selectedRefs[0]);
    }
    
    p->create(G_CIRCLE, G_CENTERRADIUS_CIRCLE, tmp, this);
  }
  
  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newArc()
{
  if(!canNewArc()) {
    Q_ASSERT(canNewArc());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_ARC, G_THREEPOINTS_ARC, selectedRefs, this);

  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newArcSector()
{
  if(!canNewArcSector()) {
    Q_ASSERT(canNewArcSector());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_ARCSECTOR, G_DEFAULT_FILLED, selectedRefs, this);

  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newArcSegment()
{
  if(!canNewArcSegment()) {
    Q_ASSERT(canNewArcSegment());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_ARCSEGMENT, G_DEFAULT_FILLED, selectedRefs, this);

  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newCircleInterior()
{
  if(!canNewCircleInterior()) {
    Q_ASSERT(canNewCircleInterior());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_CIRCLEINTERIOR, G_DEFAULT_FILLED, selectedRefs, this);

  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newPolygon()
{
  if(!canNewPolygon()) {
    Q_ASSERT(canNewPolygon());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_POLYGON, G_DEFAULT_FILLED, selectedRefs, this);

  clearSel();

  addSel(p);

  emit documentChanged();
}


void KSegDocument::newLocus()
{
  if(!canNewLocus()) {
    Q_ASSERT(canNewLocus());
    return;
  }

  G_ref *p = new G_ref();

  DescendantSearcher ds(selectedRefs[0]);

  if(ds.search(selectedRefs[1])) {
    p->create(G_LOCUS, G_OBJECT_LOCUS, selectedRefs, this);
  }
  else {
    G_refs tmp;

    tmp.append(selectedRefs[1]);
    tmp.append(selectedRefs[0]);

    p->create(G_LOCUS, G_OBJECT_LOCUS, tmp, this);
  }

  clearSel();

  if(p->getExists()) {
    addSel(p);
  }

  emit documentChanged();
}


void KSegDocument::newMidpoint()
{
  G_refs tmp;
  G_refs new_refs;
  int i;

  if(!canNewMidpoint()) {
    Q_ASSERT(canNewMidpoint());
    return;
  }

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    G_ref *p = new G_ref();
    tmp.clear();
    tmp.append(selectedRefs[i]);

    p->create(G_POINT, G_MID_POINT, tmp, this);

    new_refs.append(p);
  }
  clearSel();

  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;

}


void KSegDocument::newPerpendicular()
{
  G_refs tmp, new_refs, pts, straights;
  int i;

  if(!canNewPerpendicular()) {
    Q_ASSERT(canNewPerpendicular());
    return;
  }


  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & G_STRAIGHT) straights.append(selectedRefs[i]);
    else if(selectedRefs[i]->getType() == G_POINT) pts.append(selectedRefs[i]);
  }

  if(pts.count() == 1) {
    for(i = 0; i < (int)straights.count(); i++) {
      G_ref *p = new G_ref();
      tmp.clear();
      tmp.append(pts[0]);
      tmp.append(straights[i]);
      
      p->create(G_LINE, G_PERPENDICULAR_LINE, tmp, this);
      
      new_refs.append(p);
    }
  }
  else { // straights.count() must be 1.
    for(i = 0; i < (int)pts.count(); i++) {
      G_ref *p = new G_ref();
      tmp.clear();
      tmp.append(pts[i]);
      tmp.append(straights[0]);
      
      p->create(G_LINE, G_PERPENDICULAR_LINE, tmp, this);
      
      new_refs.append(p);
    }
  }

  clearSel();

  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;

}


void KSegDocument::newLine()
{
  G_refs tmp, new_refs, pts, straights;
  int i;

  if(!canNewLine()) {
    Q_ASSERT(canNewLine());
    return;
  }


  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & G_STRAIGHT) straights.append(selectedRefs[i]);
    else if(selectedRefs[i]->getType() == G_POINT) pts.append(selectedRefs[i]);
  }

  if(pts.count() == 1) {
    for(i = 0; i < (int)straights.count(); i++) {
      G_ref *p = new G_ref();
      tmp.clear();
      tmp.append(pts[0]);
      tmp.append(straights[i]);
      
      p->create(G_LINE, G_PARALLEL_LINE, tmp, this);
      
      new_refs.append(p);
    }
  }
  else if(straights.count() == 0) {
    if(selectedRefs.count() == 2) { // two points
      G_ref *s = new G_ref();
      s->create(G_LINE, G_TWOPOINTS_LINE, selectedRefs, this);
      clearSel();
      addSel(s);
      emit documentChanged();
      return;
    }

    for(i = 0; i < (int)selectedRefs.count(); i++) {
      G_ref *s = new G_ref();
      tmp.clear();
      tmp.append(selectedRefs[i]);
      if(i + 1 < (int)selectedRefs.count()) {
	tmp.append(selectedRefs[i + 1]);
      }
      else { tmp.append(selectedRefs[0]); }
      
      s->create(G_LINE, G_TWOPOINTS_LINE, tmp, this);
      
      new_refs.append(s);
    }    
  }
  else { // straights.count() must be 1.
    for(i = 0; i < (int)pts.count(); i++) {
      G_ref *p = new G_ref();
      tmp.clear();
      tmp.append(pts[i]);
      tmp.append(straights[0]);
      
      p->create(G_LINE, G_PARALLEL_LINE, tmp, this);
      
      new_refs.append(p);
    }
  }

  clearSel();

  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;

}


void KSegDocument::editDelete()
{
  if(!canEditDelete()) {
    Q_ASSERT(canEditDelete());
    return;
  }

  G_refs toBeDeleted;

  toBeDeleted.topologicalSort(selectedRefs);

  //if this is a construction and we are deleting any givens, we must
  //also delete all loops:
  int i;

  if(isConstruction()) {
    for(i = toBeDeleted.count() - 1; i >= 0; i--) {
      if(toBeDeleted[i]->getGiven()) break;
    }
    
    if(i >= 0) { //if there was a given
      KSegConstruction *me = (KSegConstruction *)this;

      for(i = me->getLoops().count() - 1; i >= 0; i--) {
	toBeDeleted.appendUnique(me->getLoops()[i]);
      }
    }
  }

  allRefs.holdRemovals();

  for(i = toBeDeleted.count() - 1; i >= 0; i--) {
    toBeDeleted[i]->getChildren().clear(); //for performance
  }

  for(i = toBeDeleted.count() - 1; i >= 0; i--) {
    toBeDeleted[i]->remove();
  }

  allRefs.commitRemovals();

  emit documentChanged();
}


void KSegDocument::editUndo()
{
  if(!canEditUndo()) {
    Q_ASSERT(canEditUndo());
    return;
  }

  isUndoing = true;

  undoStack.DoIt();

  isUndoing = false;

  redoStack.opFinished();

  emit documentChanged();
}


void KSegDocument::editRedo()
{
  if(!canEditRedo()) {
    Q_ASSERT(canEditRedo());
    return;
  }

  isRedoing = true;

  redoStack.DoIt();

  isRedoing = false;

  emit documentChanged();
}


void KSegDocument::editToggleLabels()
{
  int i;

  if(!canEditToggleLabels()) {
    Q_ASSERT(canEditToggleLabels());
    return;
  }

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & (G_CURVE | G_POINT)) {
      selectedRefs[i]->setLabelVisible(!(selectedRefs[i]->getLabelVisible()));
    }
  }

  emit documentChanged();
  emitDocumentModified();
}


void KSegDocument::editHideLabels()
{
  int i;

  if(!canEditToggleLabels()) {
    Q_ASSERT(canEditToggleLabels());
    return;
  }

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & (G_CURVE | G_POINT)) {
      selectedRefs[i]->setLabelVisible(false);
    }
  }

  emit documentChanged();
  emitDocumentModified();
}


void KSegDocument::editShowLabels()
{
  int i;

  if(!canEditToggleLabels()) {
    Q_ASSERT(canEditToggleLabels());
    return;
  }

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & (G_CURVE | G_POINT)) {
      selectedRefs[i]->setLabelVisible(true);
    }
  }

  emit documentChanged();
  emitDocumentModified();
}


void KSegDocument::editChangeLabel()
{

  QDialog labelDlg(0, 0, true);
  QPushButton *ok, *cancel;
  KFormulaEdit *edit;
  
  QVBoxLayout vlayout(&labelDlg);

  edit = new KFormulaEdit(&labelDlg);
  edit->setText(selectedRefs[0]->getLabel().getText());

  vlayout.addWidget(edit);
  
  QHBoxLayout hlayout;
  vlayout.addLayout(&hlayout);

  ok = new QPushButton(qApp->translate("KSegDocument", "OK", ""), &labelDlg );
  hlayout.addWidget(ok);
  QObject::connect( ok, SIGNAL(clicked()), &labelDlg, SLOT(accept()) );
  ok->setDefault(true);
  cancel = new QPushButton(qApp->translate("KSegDocument", "Cancel", ""), &labelDlg );
  hlayout.addWidget(cancel);
  QObject::connect( cancel, SIGNAL(clicked()), &labelDlg, SLOT(reject()) );

  labelDlg.resize(QSize(300, 200).expandedTo(edit->getFormula()->size() +
					     QSize(20, 20 + ok->height())));
  
  labelDlg.exec();
  if(labelDlg.result() == QDialog::Rejected) return;
  
  addUndo(new G_undoChangeLabel(selectedRefs[0]));
  selectedRefs[0]->getLabel().setText(edit->text());

  emit documentChanged();
  emitDocumentModified();
}


void KSegDocument::editHide()
{
  int i;
  if(!canEditHide()) {
    Q_ASSERT(canEditHide());
    return;
  }

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    addUndo(new G_undoShowHide(selectedRefs[i]));

    selectedRefs[i]->setVisible(!selectedRefs[i]->getVisible());
    if(selectedRefs[i]->getVisible() == false) {
      delSel(selectedRefs[i]);
      i--;
    }

  }

  emit documentChanged();
  emitDocumentModified();
}


void KSegDocument::editShowHidden()
{
  int i;
  if(!canEditShowHidden()) {
    Q_ASSERT(canEditShowHidden());
    return;
  }

  clearSel();

  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getVisible() == false && allRefs[i]->getType() != G_LOOP) {
      addUndo(new G_undoShowHide(allRefs[i]));

      allRefs[i]->setVisible(true);
      addSel(allRefs[i]);
    }
  }

  emit documentChanged();
  emitDocumentModified();
}


void KSegDocument::editChangeNumberOfSamples()
{
  if(!canEditChangeNumberOfSamples()) {
    Q_ASSERT(canEditChangeNumberOfSamples());
  }

  KSegSampleChangeDialog d(selectedRefs[0], this);
  d.exec();
}

KSegSampleChangeDialog::KSegSampleChangeDialog(G_ref *inLocus,
					       KSegDocument *inDoc)
  : QDialog(0, 0, true), doc(inDoc)
{
  locus = (G_locusObject *)(inLocus->getObject());
  originalNumSamples = locus->getMaxSamples();
  QLabel *l = new QLabel(tr("Samples in locus:"), this);
  l->setGeometry(10, 10, 100, 30);
  QSpinBox *box = new QSpinBox(15, 40000, 5, this);
  box->setValue(locus->getMaxSamples());
  box->setGeometry(130, 10, 100, 30);
  connect(box, SIGNAL(valueChanged(int)), SLOT(setIt(int)));
  QPushButton *ok, *cancel;
  ok = new QPushButton(tr("OK"), this );
  ok->setGeometry( 10,60, 100,30 );
  connect( ok, SIGNAL(clicked()), SLOT(accept()) );
  cancel = new QPushButton(tr("Cancel"), this );
  cancel->setGeometry( 130,60, 100,30 );
  connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
  objectsToUpdate.topologicalSort(inLocus);
}

void KSegSampleChangeDialog::setIt(int val)
{
  locus->setMaxSamples(val);
  objectsToUpdate.update();
  doc->emitDocumentChanged();
}

void KSegDocument::measureDistance()
{
  if(!canMeasureDistance()) {
    Q_ASSERT(canMeasureDistance());
    return;
  }

  G_ref *p = new G_ref();
  G_refs tmp;

  if(selectedRefs[0]->getType() == G_POINT) tmp = selectedRefs;
  else {
    tmp.append(selectedRefs[1]);
    tmp.append(selectedRefs[0]);
  }

  p->create(G_MEASURE, G_DISTANCE_MEASURE, tmp, this);

  //move it close to the (first) object
  p->getObject()->translate(G_point(selectedRefs[0]->getObject()->getScrollExtents().
				    center()) +
			    G_point(INTRAND(-100, 100), INTRAND(-100, 100)));

  clearSel();

  addSel(p);

  emit documentChanged();  
}


void KSegDocument::measureLength()
{
  if(!canMeasureLength()) {
    Q_ASSERT(canMeasureLength());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_MEASURE, G_LENGTH_MEASURE, selectedRefs, this);

  //move it close to the object
  p->getObject()->translate(G_point(selectedRefs[0]->
				    getObject()->getScrollExtents().center()) +
			    G_point(INTRAND(-100, 100), INTRAND(-100, 100)));

  clearSel();

  addSel(p);

  emit documentChanged();  
}


void KSegDocument::measureRadius()
{
  if(!canMeasureRadius()) {
    Q_ASSERT(canMeasureRadius());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_MEASURE, G_RADIUS_MEASURE, selectedRefs, this);

  //move it close to the object
  p->getObject()->translate(G_point(selectedRefs[0]->
				    getObject()->getScrollExtents().center()) +
			    G_point(INTRAND(-100, 100), INTRAND(-100, 100)));

  clearSel();

  addSel(p);

  emit documentChanged();  
}

void KSegDocument::measureAngle()
{
  if(!canMeasureAngle()) {
    Q_ASSERT(canMeasureAngle());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_MEASURE, G_ANGLE_MEASURE, selectedRefs, this);

  //move it close to the (first) object
  p->getObject()->translate(G_point(selectedRefs[0]->
				    getObject()->getScrollExtents().center()) +
			    G_point(INTRAND(-100, 100), INTRAND(-100, 100)));

  clearSel();

  addSel(p);

  emit documentChanged();  
}


void KSegDocument::measureRatio()
{
  if(!canMeasureRatio()) {
    Q_ASSERT(canMeasureRatio());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_MEASURE, G_RATIO_MEASURE, selectedRefs, this);

  //move it close to the (first) object
  p->getObject()->translate(G_point(selectedRefs[0]->
				    getObject()->getScrollExtents().center()) +
			    G_point(INTRAND(-100, 100), INTRAND(-100, 100)));

  clearSel();

  addSel(p);

  emit documentChanged();  
}


void KSegDocument::measureSlope()
{
  if(!canMeasureSlope()) {
    Q_ASSERT(canMeasureSlope());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_MEASURE, G_SLOPE_MEASURE, selectedRefs, this);

  //move it close to the (first) object
  if(selectedRefs[0]->getType() != G_LINE) {
    p->getObject()->translate(G_point(selectedRefs[0]->
				      getObject()->getScrollExtents().center()) +
			      G_point(INTRAND(-100, 100), INTRAND(-100, 100)));
  }
  else {
    p->getObject()->translate(selectedRefs[0]->getObject()->
			      getLine().getP1() +
			      G_point(INTRAND(-100, 100), INTRAND(-100, 100)));
  }

  clearSel();

  addSel(p);

  emit documentChanged();  
}


void KSegDocument::measureArea()
{
  if(!canMeasureArea()) {
    Q_ASSERT(canMeasureArea());
    return;
  }

  G_ref *p = new G_ref();

  p->create(G_MEASURE, G_AREA_MEASURE, selectedRefs, this);

  //move it close to the (first) object
  p->getObject()->translate(G_point(selectedRefs[0]->
				    getObject()->getScrollExtents().center()) +
			    G_point(INTRAND(-100, 100), INTRAND(-100, 100)));

  clearSel();

  addSel(p);

  emit documentChanged();  
}


void KSegDocument::transformChooseVector()
{
  if(!canTransformChooseVector()) {
    Q_ASSERT(canTransformChooseVector());
    return;
  }

  t_vector = selectedRefs;
  
  emit documentChanged();
}


void KSegDocument::transformChooseMirror()
{
  if(!canTransformChooseMirror()) {
    Q_ASSERT(canTransformChooseMirror());
    return;
  }

  t_mirror = selectedRefs;
  
  emit documentChanged();
}


void KSegDocument::transformChooseRatio()
{
  if(!canTransformChooseRatio()) {
    Q_ASSERT(canTransformChooseRatio());
    return;
  }

  t_ratio = selectedRefs;
  
  emit documentChanged();
}


void KSegDocument::transformChooseCenter()
{
  if(!canTransformChooseCenter()) {
    Q_ASSERT(canTransformChooseCenter());
    return;
  }

  t_center = selectedRefs;
  
  emit documentChanged();
}


void KSegDocument::transformChooseAngle()
{
  if(!canTransformChooseAngle()) {
    Q_ASSERT(canTransformChooseAngle());
    return;
  }

  t_angle = selectedRefs;
  
  emit documentChanged();
}


void KSegDocument::transformClearChosen()
{
  if(!canTransformClearChosen()) {
    Q_ASSERT(canTransformClearChosen());
    return;
  }

  t_vector.clear(); t_ratio.clear(); t_center.clear(); t_angle.clear(); t_mirror.clear();
  emit documentChanged();
}


void KSegDocument::transformTranslate()
{
  if(!canTransformTranslate()) {
    Q_ASSERT(canTransformTranslate());
    return;
  }

  G_refs parents;
  G_refs new_refs;
  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    G_ref *p = new G_ref();
    parents = t_vector;
    parents.insert(0, selectedRefs[i]);

    p->create(parents[0]->getType(), G_TRANSLATED, parents, this);

    new_refs.append(p);
  }
  clearSel();

  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;
}


void KSegDocument::transformReflect()
{
  if(!canTransformReflect()) {
    Q_ASSERT(canTransformReflect());
    return;
  }

  G_refs parents;
  G_refs new_refs;
  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    G_ref *p = new G_ref();
    parents = t_mirror;
    parents.insert(0, selectedRefs[i]);

    p->create(parents[0]->getType(), G_REFLECTED, parents, this);

    new_refs.append(p);
  }
  clearSel();

  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;
}


void KSegDocument::transformRotate()
{
  if(!canTransformRotate()) {
    Q_ASSERT(canTransformRotate());
    return;
  }

  G_refs parents;
  G_refs new_refs;
  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    G_ref *p = new G_ref();
    parents = t_angle;
    parents.insert(0, t_center[0]);
    parents.insert(0, selectedRefs[i]);

    p->create(parents[0]->getType(), G_ROTATED, parents, this);

    new_refs.append(p);
  }
  clearSel();

  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;
}


void KSegDocument::transformScale()
{
  if(!canTransformScale()) {
    Q_ASSERT(canTransformScale());
    return;
  }

  G_refs parents;
  G_refs new_refs;
  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    G_ref *p = new G_ref();
    parents = t_ratio;
    parents.insert(0, t_center[0]);
    parents.insert(0, selectedRefs[i]);

    p->create(parents[0]->getType(), G_SCALED, parents, this);

    new_refs.append(p);
  }
  clearSel();

  for(i = 0; i < (int)new_refs.count(); i++) addSel(new_refs[i]);
  emit documentChanged();
  return;
}


void KSegDocument::measureCalculate()
{
  if(!canMeasureCalculate()) {
    Q_ASSERT(canMeasureCalculate());
    return;
  }

  //find the appropriate view which originated the command:
  KSegView *curView = 0;

  QVector<KSegView*> allViews = KSegView::getAllViews();
  unsigned int i;

  for(i = 0; i < allViews.count(); ++i) {
    if(allViews.at(i)->isActiveWindow() && allViews.at(i)->getDocument() == this) {
      curView = allViews.at(i);
      break;
    }
  }
  if(curView == 0) for(i = 0; i < allViews.count(); ++i) {
    if(allViews.at(i)->getDocument() == this) {
      curView = allViews.at(i);
      break;
    }
  }

  Q_ASSERT(curView != 0);

  QString str;
  if(selectedRefs.size() != 0) str = QString(QChar(int(REFERENCE_NUM(0))));

  KSegCalculateEditor *e = new KSegCalculateEditor(curView, str, selectedRefs);

  e->run();
  if(e->result() == QDialog::Rejected) { delete e; return; }

  G_ref *p = new G_ref();

  p->create(G_CALCULATE, G_REGULAR_CALCULATE, e->getOutputParents(), this);
  ((G_calculateObject *)(p->getObject()))->setInitialFormulaString(e->getOutputString());

  delete e;

  p->update();

  p->getObject()->translate(G_point(INTRAND(100, 400), INTRAND(100, 300)));
  clearSel();

  addSel(p);
  emit documentChanged();
  return;
}

void KSegDocument::editShowSelectionGroupDialog()
{
  KSegView *curView = 0;
  QVector<KSegView*> allViews = KSegView::getAllViews();
  unsigned int i;

  for(i = 0; i < allViews.count(); ++i) {
    if(allViews.at(i)->isActiveWindow() && allViews.at(i)->getDocument() == this) {
      curView = allViews.at(i);
      break;
    }
  }
  if(curView == 0) for(i = 0; i < allViews.count(); ++i) {
    if(allViews.at(i)->getDocument() == this) {
      curView = allViews.at(i);
      break;
    }
  }

  Q_ASSERT(curView != 0);

  if(!selectionGroupDialog.isNull()) {
    if(curView != selectionGroupDialog->parent()) delete selectionGroupDialog;
    else {
      selectionGroupDialog->show();
      selectionGroupDialog->setActiveWindow();
      return;
    }
  }

  selectionGroupDialog = new KSegSelectionGroupDialog(curView, selectionGroups, this);

  selectionGroupDialog->show();
  selectionGroupDialog->setActiveWindow();
}

void KSegDocument::doPlay(int which)
{
  if(!canPlay(which)) {
    Q_ASSERT(canPlay(which));
    return;
  }

  KSegConstruction *c = KSegConstruction::getAllConstructions()[which];

  int depth = KSegRecursionDialog::getRecurseDepth(c);
  if(depth == -1) return;

  emit documentChanged(); //force a repaint

  c->allRefs.topologicalSort(c->allRefs);

  c->play(selectedRefs, this, depth);

  clearSel();

  emit documentChanged();
  return;
}


void KSegConstruction::constructionMakeNormal()
{
  if(!canConstructionMakeNormal()) {
    Q_ASSERT(canConstructionMakeNormal());
    return;
  }

  int i;
  for(i = 0; i < (int)(selectedRefs.size()); ++i) {
    makeNormal(selectedRefs[i]);
  }

  emit documentChanged();
  return;
}


void KSegConstruction::constructionMakeGiven()
{
  if(!canConstructionMakeGiven()) {
    Q_ASSERT(canConstructionMakeGiven());
    return;
  }

  int i;
  for(i = 0; i < (int)(selectedRefs.size()); ++i) {
    makeGiven(selectedRefs[i]);
  }

  emit documentChanged();
  return;
}


void KSegConstruction::constructionMakeFinal()
{
  if(!canConstructionMakeFinal()) {
    Q_ASSERT(canConstructionMakeFinal());
    return;
  }

  int i;
  for(i = 0; i < (int)(selectedRefs.size()); ++i) {
    makeFinal(selectedRefs[i]);
  }

  emit documentChanged();
  return;
}


void KSegConstruction::constructionMakeInitial()
{
  if(!canConstructionMakeInitial()) {
    Q_ASSERT(canConstructionMakeInitial());
    return;
  }

  int i;
  for(i = 0; i < (int)(selectedRefs.size()); ++i) {
    makeInitial(selectedRefs[i]);
  }

  emit documentChanged();
  return;
}


void KSegConstruction::constructionRecurse()
{
  if(!canConstructionRecurse()) {
    Q_ASSERT(canConstructionRecurse());
    return;
  }
  
  G_ref *p = new G_ref();

  vector<int> perm;
  G_refs permuted;
  bool tmp = match(selectedRefs, given, perm);
  Q_ASSERT(tmp);
  int i;
  for(i = 0; i < (int)selectedRefs.count(); ++i) {
    permuted.append(selectedRefs[perm[i]]);
  }

  p->create(G_LOOP, 0, permuted, this);

  clearSel();

  addSel(p);

  emit documentChanged();  
}

