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


#include "KSegDocument.H"
#include "KSegConstruction.H"
#include "G_ref.H"
#include "G_refSearcher.H"

//this file has all the functions that decide what can be created.


bool KSegDocument::canNewSegment()
{
  if(selectedRefs.count() < 2) return false;

  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) if(selectedRefs[i]->getType() != G_POINT) return false;

  return true;
}


bool KSegDocument::canEditShowHidden()
{
  int i;

  for(i = 0; i < (int)allRefs.count(); i++) {
    if(allRefs[i]->getType() != G_LOOP && allRefs[i]->getVisible() == false) return true;
  }

  return false;
}


bool KSegDocument::canEditHide()
{
  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() == G_LOOP) return false;
  }

  return selectedRefs.count() > 0;
}


bool KSegDocument::canEditDelete()
{
  return selectedRefs.count() > 0;
}

bool KSegDocument::canEditToggleLabels()
{
  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & (G_CURVE | G_POINT)) return true;
  }

  return false;
}


bool KSegDocument::canEditChangeLabel()
{
  if(selectedRefs.count() != 1) return false;
  if(selectedRefs[0]->getType() & (G_CURVE | G_POINT)) return true;
  return false;
}


bool KSegDocument::canEditChangeNumberOfSamples()
{
  if(selectedRefs.count() != 1) return false;
  if(selectedRefs[0]->getType() & G_LOCUS &&
     selectedRefs[0]->getDescendType() == G_OBJECT_LOCUS) return true;
  return false;
}


bool KSegDocument::canEditChangeColor()
{
  if(selectedRefs.count() == 0) return true;

  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++)
    if(!(selectedRefs[i]->getType() & (G_TEXT | G_GEOMETRIC))) return false;
  
  return true;
}


bool KSegDocument::canEditChangeLinestyle()
{
  if(selectedRefs.count() == 0) return true;

  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++)
    if(!(selectedRefs[i]->getType() & (G_CURVE | G_LOCUS))) return false;
  
  return true;
}


bool KSegDocument::canEditChangePointstyle()
{
  if(selectedRefs.count() == 0) return true;

  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++)
    if(!(selectedRefs[i]->getType() & G_POINT)) return false;
  
  return true;
}


bool KSegDocument::canEditChangeFont()
{
  if(selectedRefs.count() == 0) return true;

  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++)
    if(!(selectedRefs[i]->getType() & (G_CURVE | G_POINT | G_TEXT)))
      return false;
  
  return true;
}


bool KSegDocument::canNewRay()
{
  if(selectedRefs.count() < 2) return false;

  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) if(selectedRefs[i]->getType() != G_POINT) return false;

  return true;
}


bool KSegDocument::canNewLocus()
{
  if(selectedRefs.count() != 2) return false;

  if(selectedRefs[0]->getType() == G_POINT &&
     selectedRefs[0]->getDescendType() == G_CONSTRAINED_POINT &&
     selectedRefs[1]->getType() & G_GEOMETRIC) {

    BrokenLocusChainSearcher blcs(selectedRefs[1]);
    if(blcs.search(selectedRefs[0])) return true;  
  }

  if(selectedRefs[1]->getType() == G_POINT &&
     selectedRefs[1]->getDescendType() == G_CONSTRAINED_POINT &&
     selectedRefs[0]->getType() & G_GEOMETRIC) {

    BrokenLocusChainSearcher blcs(selectedRefs[0]);
    if(blcs.search(selectedRefs[1])) return true;  
  }

  return false;
}


bool KSegDocument::canNewBisector()
{
  if(selectedRefs.count() != 3) return false;

  if(selectedRefs[0]->getType() == G_POINT && selectedRefs[1]->getType() == G_POINT &&
     selectedRefs[2]->getType() == G_POINT) return true;

  return false;
}


bool KSegDocument::canNewMidpoint()
{
  if(!selectedRefs.count()) return false;

  int i;

  for(i = 0; i < (int)selectedRefs.count(); i++) if(selectedRefs[i]->getType() != G_SEGMENT) return false;

  return true;
}

bool KSegDocument::canNewEndpoints()
{
  if(selectedRefs.count() != 1) return false;
  return (selectedRefs[0]->getType() & (G_SEGMENT | G_RAY | G_ARC)) != 0;
}

bool KSegDocument::canNewCenterpoint()
{
  if(selectedRefs.count() != 1) return false;
  return (selectedRefs[0]->getType() & (G_CIRCLE | G_ARC)) != 0;
}

bool KSegDocument::canNewPerpendicular()
{
  if(selectedRefs.count() < 2) return false;

  int i;
  int straights = 0, pts = 0;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & G_STRAIGHT) straights++;
    else if(selectedRefs[i]->getType() == G_POINT) pts++;
    else return false;
  }

  if((straights == 1 && pts) || (straights && pts == 1)) return true;

  return false;
}


bool KSegDocument::canNewLine()
{
  if(selectedRefs.count() < 2) return false;

  int i;
  int straights = 0, pts = 0;

  for(i = 0; i < (int)selectedRefs.count(); i++) {
    if(selectedRefs[i]->getType() & G_STRAIGHT) straights++;
    else if(selectedRefs[i]->getType() == G_POINT) pts++;
    else return false;
  }

  if((straights == 1 && pts) || (straights && pts == 1) || (!straights && pts >= 2)) return true;

  return false;
}


bool KSegDocument::canNewCircle()
{
  if(selectedRefs.count() != 2) return false;

  if((selectedRefs[0]->getType() == G_POINT) && (selectedRefs[1]->getType() == G_POINT)) return true;
  if((selectedRefs[0]->getType() == G_POINT) && (selectedRefs[1]->getType() == G_SEGMENT)) return true;
  if((selectedRefs[1]->getType() == G_POINT) && (selectedRefs[0]->getType() == G_SEGMENT)) return true;
 
  return false;
}


bool KSegDocument::canNewArc()
{
  if(selectedRefs.count() != 3) return false;

  if(selectedRefs[0]->getType() != G_POINT) return false;
  if(selectedRefs[1]->getType() != G_POINT) return false;
  if(selectedRefs[2]->getType() != G_POINT) return false;
  
  return true;
}


bool KSegDocument::canNewIntersection()
{
  if(selectedRefs.count() != 2) return false;

  return (selectedRefs[0]->getType() & G_CURVE) && (selectedRefs[1]->getType() & G_CURVE);
}


bool KSegDocument::canNewArcSector()
{
  if(selectedRefs.count() == 1 && selectedRefs[0]->getType() == G_ARC) return true;
  return false;
}


bool KSegDocument::canNewArcSegment()
{
  if(selectedRefs.count() == 1 && selectedRefs[0]->getType() == G_ARC) return true;
  return false;
}


bool KSegDocument::canNewCircleInterior()
{
  if(selectedRefs.count() == 1 && selectedRefs[0]->getType() == G_CIRCLE) return true;
  return false;
}


bool KSegDocument::canNewPolygon()
{
  if(selectedRefs.count() < 3) return false;
  int i;
  for(i = 0; i < (int)selectedRefs.count(); ++i) {
    if(selectedRefs[i]->getType() != G_POINT) return false;
  }
  return true;
}


bool KSegDocument::canMeasureDistance()
{
  if(selectedRefs.count() != 2) return false;
  if(selectedRefs[0]->getType() != G_POINT && selectedRefs[1]->getType() != G_POINT)
    return false;
  //if both are points
  if(selectedRefs[0]->getType() == selectedRefs[1]->getType()) return true;
  
  if(selectedRefs[0]->getType() & G_CURVE || selectedRefs[1]->getType() & G_CURVE)
    return true;

  return false;
}


bool KSegDocument::canMeasureLength()
{
  if(selectedRefs.count() != 1) return false;

  if((selectedRefs[0]->getType() & (G_SEGMENT | G_ARC | G_CIRCLE)) == 0)
    return false;
  
  return true;
}


bool KSegDocument::canMeasureRadius()
{
  if(selectedRefs.count() != 1) return false;

  if((selectedRefs[0]->getType() & (G_ARC | G_CIRCLE)) == 0)
    return false;
  
  return true;
}


bool KSegDocument::canMeasureAngle()
{
  if(selectedRefs.count() == 1 &&
     selectedRefs[0]->getType() == G_ARC) return true;

  if(selectedRefs.count() != 3) return false;
  //if all are points, we are ok
  if((selectedRefs[0]->getType() & selectedRefs[1]->getType() &
      selectedRefs[2]->getType()) == G_POINT) return true;

  return false;
}


bool KSegDocument::canMeasureRatio()
{
  if(selectedRefs.count() != 2) return false;

  if(selectedRefs[0]->getType() != G_SEGMENT) return false;
  if(selectedRefs[1]->getType() != G_SEGMENT) return false;

  return true;
}


bool KSegDocument::canMeasureSlope()
{
  if(selectedRefs.count() != 1) return false;

  if(selectedRefs[0]->getType() & G_STRAIGHT) return true;

  return false;
}


bool KSegDocument::canMeasureArea()
{
  if(selectedRefs.count() != 1) return false;

  if(selectedRefs[0]->getType() & G_FILLED) return true;

  return false;
}


bool KSegDocument::canTransformChooseVector()
{
  if(selectedRefs.count() != 2) return false;

  if(selectedRefs[0]->getType() != G_POINT) return false;
  if(selectedRefs[1]->getType() != G_POINT) return false;

  return true;
}


bool KSegDocument::canTransformChooseMirror()
{
  if(selectedRefs.count() != 1) return false;

  if(selectedRefs[0]->getType() & G_STRAIGHT) return true;

  return false;
}


bool KSegDocument::canTransformChooseCenter()
{
  return (selectedRefs.count() == 1 && selectedRefs[0]->getType() == G_POINT);
}


bool KSegDocument::canTransformChooseRatio()
{
  if(selectedRefs.count() == 1 && selectedRefs[0]->getType() & G_VALUE) return true;
  if(selectedRefs.count() != 2) return false;

  if(selectedRefs[0]->getType() != G_SEGMENT) return false;
  if(selectedRefs[1]->getType() != G_SEGMENT) return false;

  return true;
}


bool KSegDocument::canTransformChooseAngle()
{
  if(selectedRefs.count() == 1 && selectedRefs[0]->getType() & G_VALUE) return true;
  if(selectedRefs.count() != 3) return false;
  if((selectedRefs[0]->getType() & selectedRefs[1]->getType() &
      selectedRefs[2]->getType()) == G_POINT) return true;

  return false;
}


bool KSegDocument::canTransformClearChosen()
{
  return t_vector.count() + t_ratio.count() + t_center.count() +
    t_angle.count() + t_mirror.count() > 0;
}


bool KSegDocument::canTransformTranslate()
{
  if(t_vector.count() == 0) return false;
  if(selectedRefs.count() == 0) return false;

  int i;

  for(i = 0; i < int(selectedRefs.count()); i++) {
    if(!(selectedRefs[i]->getType() & G_GEOMETRIC)) return false;
  }

  return true;
}


bool KSegDocument::canTransformReflect()
{
  if(t_mirror.count() == 0) return false;
  if(selectedRefs.count() == 0) return false;

  int i;

  for(i = 0; i < int(selectedRefs.count()); i++) {
    if(!(selectedRefs[i]->getType() & G_GEOMETRIC)) return false;
  }

  return true;
}


bool KSegDocument::canTransformScale()
{
  if(t_center.count() == 0) return false;
  if(t_ratio.count() == 0) return false;
  if(selectedRefs.count() == 0) return false;

  int i;

  for(i = 0; i < int(selectedRefs.count()); i++) {
    if(!(selectedRefs[i]->getType() & G_GEOMETRIC)) return false;
  }

  return true;
}


bool KSegDocument::canTransformRotate()
{
  if(t_center.count() == 0) return false;
  if(t_angle.count() == 0) return false;
  if(selectedRefs.count() == 0) return false;

  int i;

  for(i = 0; i < int(selectedRefs.count()); i++) {
    if(!(selectedRefs[i]->getType() & G_GEOMETRIC)) return false;
  }

  return true;
}


bool KSegDocument::canMeasureCalculate()
{
  if(selectedRefs.size() == 0) return true;
  if(selectedRefs.size() == 1 && selectedRefs[0]->getType() & G_VALUE) return true;
  return false;
}


bool KSegDocument::canPlay(int which)
{
  return KSegConstruction::getAllConstructions()[which]->doesMatch(selectedRefs);
}


bool KSegConstruction::canConstructionMakeNormal()
{
  int i;
  bool allNormal = true;

  for(i = 0; i < (int)selectedRefs.size(); ++i) {
    if(selectedRefs[i]->getNormal() == false) allNormal = false;

    if(canMakeNormal(selectedRefs[i]) == false) return false;
  }

  return !allNormal;
}


bool KSegConstruction::canConstructionMakeGiven()
{
  int i;
  bool allGiven = true;

  CanMakeGivenSearcher s;

  for(i = 0; i < (int)selectedRefs.size(); ++i) {
    if(selectedRefs[i]->getGiven() == false) allGiven = false;
    else continue;

    if(canMakeNormal(selectedRefs[i]) == false) return false;
    if(canMakeGiven(selectedRefs[i]) == false) return false;
    if(s.search(selectedRefs[i])) return false;
  }

  return !allGiven;
}


bool KSegConstruction::canConstructionMakeFinal()
{
  int i;
  bool allFinal = true;

  CanMakeFinalSearcher s;
  ImplicitInitialSearcher iis;

  for(i = 0; i < (int)selectedRefs.size(); ++i) {
    if(selectedRefs[i]->getFinal() == false) allFinal = false;
    else continue;

    if(canMakeNormal(selectedRefs[i]) == false) return false;
    if(canMakeFinal(selectedRefs[i]) == false) return false;
    if(s.search(selectedRefs[i])) return false;
    if(selectedRefs[i]->getParentsConst().size() != 0 &&
       iis.search(selectedRefs[i]->getParentsConst()) == false) return false;
  }

  return !allFinal;
}

bool KSegConstruction::canConstructionMakeInitial()
{
  int i;
  bool allInitial = true;

  CanMakeInitialAncSearcher s1;
  CanMakeInitialDesSearcher s2;

  for(i = 0; i < (int)selectedRefs.size(); ++i) {
    if(selectedRefs[i]->getInitial() == false) allInitial = false;
    else continue;

    if(canMakeNormal(selectedRefs[i]) == false) return false;
    if(canMakeInitial(selectedRefs[i]) == false) return false;
    if(s1.search(selectedRefs[i])) return false;
    if(s2.search(selectedRefs[i])) return false;

    ImplicitInitialSearcher iis(selectedRefs[i]); //for checking finals
    int j;
    for(j = 0; j < (int)final.size(); ++j) {
      if(final[j] == selectedRefs[i]) continue;
      if(final[j]->getParentsConst().size() && iis.search(final[j]->getParentsConst()) == false)
	return false;
    }
  }

  return !allInitial;
}


bool KSegConstruction::canConstructionRecurse()
{
  int i;

  CanMakeInitialAncSearcher s1;
  NotInConstructionSearcher s2;

  for(i = 0; i < (int)selectedRefs.size(); ++i) {
    if(s1.search(selectedRefs[i]) == true) return false;
    if(selectedRefs[i]->getGiven() == false && s2.search(selectedRefs[i]) == false) return false;
  }

  return selectedRefs.size() > 0 && doesMatch(selectedRefs);
}

