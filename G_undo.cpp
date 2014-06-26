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


#include "G_undo.H"
#include "G_ref.H"
#include "KSegDocument.H"
#include "KSegConstruction.H"
#include "G_object.H"


KSegDocument *G_undo::doc() const
{
  if(ref) return ref->getDocument();
  else return 0;
}


void G_undoCreate::execute()
{
  ref->remove();

  return;
}


G_undoDelete::G_undoDelete(G_ref *ref)
  : G_undo(ref)
{ 
  
  QDataStream s(&objectData, QIODevice::WriteOnly);

  if(ref->getType() != G_LOOP)
    ref->getObject()->save(s);

  if(ref->getGiven()) {
    s << ((KSegConstruction *)(ref->getDocument()))->getGiven().find(ref);
  }
}


void G_undoDelete::execute()
{
  ref->createObject();

  QDataStream s(&objectData, QIODevice::ReadOnly);
  if(ref->getType() != G_LOOP)
    ref->getObject()->load(s);

  if(ref->getGiven()) { //restore the given to the right place
    int posInGiven;
    s >> posInGiven;
    
    KSegConstruction *c = (KSegConstruction *)(ref->getDocument());

    c->insertIntoGiven(ref, posInGiven);
  }

  ref->update();

  ref = NULL;

  return;
}


G_undoDelete::~G_undoDelete()
{
  if(ref != NULL) delete ref;
  ref = NULL;
}


void G_undoShowHide::execute()
{
  doc()->addUndo(new G_undoShowHide(ref));
  ref->setVisible(!(ref->getVisible()));
}

G_undoChangeLabel::G_undoChangeLabel(G_ref *ref)
  : G_undo(ref)
{
  old = ref->getLabel().getText();
}

void G_undoChangeLabel::execute()
{
  doc()->addUndo(new G_undoChangeLabel(ref));
  ref->getLabel().setText(old);
}

G_undoReconstrain::G_undoReconstrain(G_ref *ref)
  : G_undo(ref)
{
  oldParents = ref->getParents();
  oldType = ref->getDescendType();

  if(ref->getType() == G_POINT) {
    if(oldType == G_FREE_POINT) {
      oldPos = ref->getObject()->getPoint();
    }
    else if(oldType == G_CONSTRAINED_POINT) {
      oldPos = G_point(((G_pointObject *)(ref->getObject()))->getP(), 0);
    }
  }
}

void G_undoReconstrain::execute()
{
  doc()->addUndo(new G_undoReconstrain(ref));

  ref->reconstrain(oldType, oldParents, false);

  if(ref->getType() == G_POINT) {
    if(ref->getDescendType() == G_FREE_POINT) {
      ((G_pointObject *)(ref->getObject()))->setPoint(oldPos);
    }
    else if(ref->getDescendType() == G_CONSTRAINED_POINT) {
      //the first "coordinate" is the parameter
      ((G_pointObject *)(ref->getObject()))->setP(oldPos.getX());
    }
  }

  G_refs tmp;

  tmp.topologicalSort(ref);

  tmp.update();
}

bool G_undoReconstrain::isChanged(G_ref *r)
{
  return r->getDescendType() != oldType || r->getParents() != oldParents;
}


G_undoChangeCalculation::G_undoChangeCalculation(G_ref *ref)
  : G_undo(ref)
{
  oldParents = ref->getParents();
  oldFormula = ((G_calculateObject *)(ref->getObject()))->getFormulaString();
}

void G_undoChangeCalculation::execute()
{
  ((G_calculateObject *)(ref->getObject()))->changeFormula(oldParents, oldFormula);

  G_refs tmp;

  tmp.topologicalSort(ref);

  tmp.update();
}


G_undoChangeDrawstyle::G_undoChangeDrawstyle(G_ref *ref)
  : G_undo(ref)
{
  old = ref->getDrawstyle();
  old->addReference();
}

void G_undoChangeDrawstyle::execute()
{
  ref->changeDrawstyle(old);
}


G_undoChangeConstructMode::G_undoChangeConstructMode(G_ref *ref)
  : G_undo(ref)
{
  oldGiven = oldFinal = false;
  if(ref->getGiven()) {
    oldGiven = true;
    whereInGivens = ((KSegConstruction *)(ref->getDocument()))->getGiven().find(ref);
  }
  if(ref->getFinal()) oldFinal = true;
  if(ref->getInitial()) oldGiven = oldFinal = true;
}

void G_undoChangeConstructMode::execute()
{
  if(!oldGiven && !oldFinal) {
    ((KSegConstruction *)(ref->getDocument()))->makeNormal(ref);
    return;
  }
  if(oldGiven && !oldFinal) {
    ((KSegConstruction *)(ref->getDocument()))->makeGiven(ref, whereInGivens);
    return;
  }
  if(!oldGiven && oldFinal) {
    ((KSegConstruction *)(ref->getDocument()))->makeFinal(ref);
    return;
  }
  if(!oldGiven && !oldFinal) {
    ((KSegConstruction *)(ref->getDocument()))->makeInitial(ref);
    return;
  }
}
