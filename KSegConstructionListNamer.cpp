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

#include "G_ref.H"
#include "KSegConstruction.H"
#include "KSegConstructionList.H"

#define INSERT_REF(r, str) \
{ str = str.arg(QString("%1 %2"). \
                arg(G_ref::getNameFromType((r)->getType())). \
                arg(QChar(REFERENCE_NUM(curRefNum++)))); \
  l.push_back((r)->getLabel().getFormula()); }

#define INSERT_LABEL(r, str) \
{ str = str.arg(QChar(REFERENCE_NUM(curRefNum++))); \
  l.push_back((r)->getLabel().getFormula()); }

#define END fstr += QString("."); f.parse(fstr); return


void KSegConstructionList::nameObject(G_ref *ref, KFormula &f,
				      ListReferenceFetcher &l, int index)
{
  int i;
  int curRefNum = 0;

  KSegConstruction *doc = (KSegConstruction *)(ref->getDocument());

  l.clear();

  QString fstr;
  fstr.sprintf("%d. ", index + 1);

  //first take care of loops
  if(ref->getType() == G_LOOP) {
    QString tmp = "%1";

    for(i = 0; i < (int)ref->getParents().size(); ++i) {
      if(i > 0) tmp += QString(", %1");
      
      INSERT_REF(ref->getParents()[i], tmp);
    }

    fstr += tr("Loop on %1").arg(tmp);

    END;
  }

  QString tmp;
  if(ref->getGiven()) tmp = tr("Given") + "{}({%2} %1%3%4";
  else if(ref->getFinal()) tmp = tr("Final") + " %1%2";
  else if(ref->getInitial()) tmp = tr("Initial") + " %1%2";
  else tmp = "%1%2";

  INSERT_REF(ref, tmp);

  //now take care of givens
  if(ref->getGiven()) {
    i = doc->getGiven().find(ref);
    tmp = tmp.arg(i + 1);

    i = ref->whatCanItBe() - ref->getType(); //all other possibilities
    QString possibilities = "";
    if(i != 0) {

      possibilities += " {}({" + tr("or") + " ";
      int j;
      for(j = 1; j <= i; j = j * 2) {
	if(j & i) {
	  possibilities += G_ref::getNameFromType(G_Type(j)) + QString(", ");
	}
      }
      possibilities.remove(possibilities.length() - 2, 2);
      possibilities += QString("}");
    }

    tmp = tmp.arg(possibilities);
  }

  //object may be (final), descended from final or (not in construction)
  //having only givens as anc, or (initial), having only initial parents.

  bool implicit_final, not_in_construction, does_not_exist, not_shown,
    implicit_initial;

  implicit_final = !ref->getFinal() && ifSearcher.search(ref);
  implicit_initial = !ref->getInitial() && iiSearcher.search(ref) == false;
  
  not_in_construction = !(nicSearcher.search(ref)) && !ref->getGiven();

  does_not_exist = !(ref->getExists());
  not_shown = !(ref->getVisible());

  QString attr = "";

  if(implicit_final || not_in_construction || does_not_exist || not_shown || implicit_initial) {
    attr += QString(" {}({");
    
    if(implicit_final) attr += (tr("final") + ", ");
    if(implicit_initial) attr += (tr("initial") + ", ");
    if(not_in_construction) attr += (tr("not constructed") + ", ");
    if(does_not_exist) attr += (tr("doesn't exist") + ", ");
    if(not_shown) attr += (tr("hidden") + ", ");

    attr.remove(attr.length() - 2, 2);
    attr += QString("}");
  }

  tmp = tmp.arg(attr);

  if(ref->getGiven()) {
    fstr += tmp;
    END;
  }

  //take care of the transforms:
  if(ref->getType() & G_GEOMETRIC && IS_TRANSFORM(ref->getDescendType())) {

    if(ref->getDescendType() == G_TRANSLATED) {
      fstr += tr("%1 is the translation of %2 by the vector from %3 to %4");
      fstr = fstr.arg(tmp);
      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      INSERT_REF(ref->getParents()[2], fstr);
    }

    if(ref->getDescendType() == G_ROTATED) {
      fstr += tr("%1 is the rotation of %2 about %3 by angle %4");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      if(ref->getParents().count() == 5) {
	QString tmp2 = "%1 %2 %3";

	INSERT_LABEL(ref->getParents()[2], tmp2);
	INSERT_LABEL(ref->getParents()[3], tmp2);
	INSERT_LABEL(ref->getParents()[4], tmp2);
	
	fstr = fstr.arg(tmp2);
      }
      else INSERT_LABEL(ref->getParents()[2], fstr);
    }

    if(ref->getDescendType() == G_SCALED) {
      if(ref->getParents().count() == 4) 
	fstr += tr("%1 is the image of %2 scaled about %3 by the ratio %4:%5");
      else
	fstr += tr("%1 is the image of %2 scaled about %3 by ratio %4");

      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      INSERT_LABEL(ref->getParents()[2], fstr);
      if(ref->getParents().count() == 4) {
	INSERT_LABEL(ref->getParents()[3], fstr);
      }
    }

    if(ref->getDescendType() == G_REFLECTED) {
      fstr += tr("%1 is the reflection of %2 about %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
    }

    END;
  }


  switch(ref->getType()) {
  case G_POINT: //points---------------
    switch(ref->getDescendType()) {
    case G_FREE_POINT:
      fstr += tr("%1 is free");
      fstr = fstr.arg(tmp);
      break;

    case G_CONSTRAINED_POINT:
      fstr += tr("%1 is on %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_INTERSECTION_POINT:
      fstr += tr("%1 is the intersection of %2 and %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      break;

    case G_INTERSECTION2_POINT:
      fstr += tr("%1 is the 2nd intersection of %2 and %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      break;

    case G_MID_POINT:
      fstr += tr("%1 is the midpoint of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_END_POINT:
      fstr += tr("%1 is the endpoint of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_END2_POINT:
      fstr += tr("%1 is the 2nd endpoint of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_CENTER_POINT:
      fstr += tr("%1 is the center of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;
    }
    break;

  case G_SEGMENT:
    fstr += tr("%1 has endpoints %2 and %3");
    fstr = fstr.arg(tmp);

    INSERT_LABEL(ref->getParents()[0], fstr);
    INSERT_LABEL(ref->getParents()[1], fstr);
    break;

  case G_RAY:
    if(ref->getDescendType() == G_TWOPOINTS_RAY) {
      fstr += tr("%1 is from %2 through %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
    }
    else if(ref->getDescendType() == G_BISECTOR_RAY) {
      fstr += tr("%1 bisects angle %2 %3 %4");
      fstr = fstr.arg(tmp);

      INSERT_LABEL(ref->getParents()[0], fstr);
      INSERT_LABEL(ref->getParents()[1], fstr);
      INSERT_LABEL(ref->getParents()[2], fstr);
    }
    break;

  case G_LINE:
    switch(ref->getDescendType()) {
    case G_TWOPOINTS_LINE:
      fstr += tr("%1 is between %2 and %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      break;

    case G_PARALLEL_LINE:
      fstr += tr("%1 is parallel to %2 through %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[1], fstr);
      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_PERPENDICULAR_LINE:
      fstr += tr("%1 is perpendicular to %2 through %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[1], fstr);
      INSERT_REF(ref->getParents()[0], fstr);
      break;
    }
    break;

  case G_CIRCLE:
    if(ref->getDescendType() == G_CENTERPOINT_CIRCLE) {
      fstr += tr("%1 is centered at %2 and passes through %3");
    }
    else {
      fstr += tr("%1 has center at %2 and radius %3");
    }

    fstr = fstr.arg(tmp);

    INSERT_REF(ref->getParents()[0], fstr);
    INSERT_REF(ref->getParents()[1], fstr);
    break;

  case G_ARC:
    fstr += tr("%1 passes through points %2, %3, and %4");
    fstr = fstr.arg(tmp);

    INSERT_LABEL(ref->getParents()[0], fstr);
    INSERT_LABEL(ref->getParents()[1], fstr);
    INSERT_LABEL(ref->getParents()[2], fstr);
    break;

  case G_LOCUS:
    fstr += tr("%1 is traced by %2, driven by %3");
    fstr = fstr.arg(tmp);

    INSERT_REF(ref->getParents()[1], fstr);
    INSERT_REF(ref->getParents()[0], fstr);
    break;

  case G_CALCULATE: {
    if(ref->getParents().size() == 0) {
      fstr += "%1";
      fstr = fstr.arg(tmp);
      break;
    }

    fstr += tr("%1 involves %2");
    fstr = fstr.arg(tmp);

    QString tmp2 = "%1";

    for(i = 0; i < (int)ref->getParents().size(); ++i) {
      if(i > 0) tmp2 += QString(", %1");
      
      INSERT_REF(ref->getParents()[i], tmp2);
    }

    fstr = fstr.arg(tmp2);
    break;
  }

  case G_MEASURE:
    switch(ref->getDescendType()) {
    case G_DISTANCE_MEASURE:
      fstr += tr("%1 is the distance between %2 and %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      break;

    case G_LENGTH_MEASURE:
      fstr += tr("%1 is the length of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_RADIUS_MEASURE:
      fstr += tr("%1 is the radius of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_ANGLE_MEASURE:
      if(ref->getParents()[0]->getType() == G_ARC) { //arc angle
	fstr += tr("%1 is the angle of %2");
	fstr = fstr.arg(tmp);

	INSERT_REF(ref->getParents()[0], fstr);
      }
      else { // three points
	fstr += tr("%1 is the angle between points %1, %2, and %3");
	fstr = fstr.arg(tmp);

	INSERT_LABEL(ref->getParents()[0],fstr);
	INSERT_LABEL(ref->getParents()[1],fstr);
	INSERT_LABEL(ref->getParents()[2],fstr);
      }
      break;

    case G_RATIO_MEASURE:
      fstr += tr("%1 is the ratio of %2 to %3");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      INSERT_REF(ref->getParents()[1], fstr);
      break;

    case G_SLOPE_MEASURE:
      fstr += tr("%1 is the slope of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;

    case G_AREA_MEASURE:
      fstr += tr("%1 is the area of %2");
      fstr = fstr.arg(tmp);

      INSERT_REF(ref->getParents()[0], fstr);
      break;
    }
    break;

  case G_ARCSECTOR:
  case G_ARCSEGMENT:
  case G_CIRCLEINTERIOR:
    fstr += tr("%1 is defined by %2");
    fstr = fstr.arg(tmp);

    INSERT_REF(ref->getParents()[0], fstr);
    break;

  case G_POLYGON: {
    fstr += tr("%1 has vertices at points %2");
    fstr = fstr.arg(tmp);

    QString tmp2 = "%1";
    
    int i;
    for(i = 0; i < (int)ref->getParents().size(); ++i) {
      if(i > 0) tmp2 += QString(", %1");
      INSERT_LABEL(ref->getParents()[i], tmp2);
    }

    fstr = fstr.arg(tmp2);
    break;
  }
  
  default:
    break;
  }


  END;
}
