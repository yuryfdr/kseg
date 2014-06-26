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
#include "KSegDocument.H"
#include "KSegConstruction.H"
#include "G_object.H"
#include <QPixmap>
#include "G_drawstyle.H"
#include <qpainter.h>
#include "G_undo.H"
#include "my_hash_set.H"
#include <qapplication.h>
#include "KSegSelectionGroup.H"

G_ref::G_ref(KSegDocument *inDoc)
  : label(this)
{
  doc = inDoc;
  where = NULL;
  exists = visible = true;
  labelVisible = given = final = deleted = selected = false;
  drawstyle = G_drawstyle::match();
}

G_ref::~G_ref()
{
  drawstyle->deleteReference();
  if(where != NULL && !deleted) delete where;
  drawstyle = NULL; //so it definitely crashes if deleted twice
  
  set<KSegSelectionGroup *>::iterator it;
  for(it = selectionGroups.begin(); it != selectionGroups.end(); ++it) {
    (*it)->deleted(this);
  }
}

void G_ref::create(G_Type inType, int inDescendType, const G_refs &inParents, KSegDocument *inDoc, bool inAutoLabel)
{

  type = inType;
  descendType = inDescendType;

  parents = inParents;

  doc = inDoc;

  drawstyle->deleteReference();
  drawstyle = inDoc->getDefaultDrawstyle();
  drawstyle->addReference();

  if(type == G_LOOP) visible = false;

  createObject();

  update();

  labelVisible = false;
  if(inAutoLabel)
    label.setString(doc->autoLabel(this));
}


void G_ref::createObject()
{
  deleted = false;
  selected = false;

  if(doc) {
    doc->addRef(this);
    doc->addUndo(new G_undoCreate(this));
  }

  int i;
  for(i = 0; i < (int)parents.count(); i++) {
    parents[i]->children.append(this);
  }

  //create new object
  switch(type) {
  case G_POINT:
    where = new G_pointObject(this);
    break;
  case G_LINE:
    where = new G_lineObject(this);
    break;
  case G_SEGMENT:
    where = new G_segmentObject(this);
    break;
  case G_RAY:
    where = new G_rayObject(this);
    break;
  case G_CIRCLE:
    where = new G_circleObject(this);
    break;
  case G_ARC:
    where = new G_arcObject(this);
    break;
  case G_ARCSECTOR:
    where = new G_arcSectorObject(this);
    break;
  case G_ARCSEGMENT:
    where = new G_arcSegmentObject(this);
    break;
  case G_CIRCLEINTERIOR:
    where = new G_circleInteriorObject(this);
    break;
  case G_POLYGON:
    where = new G_polygonObject(this);
    break;
  case G_LOCUS:
    where = new G_locusObject(this);
    break;
  case G_MEASURE:
    where = new G_measureObject(this);
    break;
  case G_CALCULATE:
    where = new G_calculateObject(this);
    break;
  case G_LOOP:
    where = NULL;
    break;
  default:
    qFatal("Unknown type %d!\n", type);
    where = NULL;
    break;
  }

  exists = true;
}


void G_ref::remove()
{
  int i;

  Q_ASSERT(children.count() == 0);

  //undo stuff:
  doc->addUndo(new G_undoDelete(this));

  doc->delRef(this);

  for(i = 0; i < (int)parents.count(); i++) {
    parents[i]->children.removeRef(this);
  }

  if(where) delete where;

  deleted = true;
}


void G_ref::reconstrain(int inDescendType, const G_refs &inParents, bool topSortAfter)
{
  //topSortAfter is true by default--pasing false is useful if you are doing
  //several reconstraints one after another.
  int i;

  if(inDescendType == descendType && inParents == parents) return;

  for(i = 0; i < (int)parents.count(); i++) {
    parents[i]->children.removeRef(this);
  }

  parents = inParents;

  for(i = 0; i < (int)parents.count(); i++) {
    parents[i]->children.append(this);
  }

  descendType = inDescendType;

  if(topSortAfter) doc->topSortAllRefs();
}


void G_ref::update(bool fromLocus)
{
  if(type == G_LOOP) { visible = false; return; }

  int i;
  for(i = 0; i < (int)parents.count(); i++) {
    if(!parents[i]->getExists()) {
      if(type != G_LOCUS || descendType != G_OBJECT_LOCUS || i != 1) {
	exists = false;
	return;
      }
    }
  }
  exists = true;

  if(fromLocus && (type & G_VALUE)) {
    //if we are coming from a locus, don't parse the formula
    ((G_valueObject *)where)->calculate();
  }
  else where->update();
}


void G_ref::drawLabel(QPainter &p)
{
  if(labelVisible) {
    Q_ASSERT( (type & (G_POINT | G_CURVE)) );

    label.draw(p, *drawstyle, selected);
  }
}

void G_ref::changeDrawstyle(G_drawstyle *d)
{
  if(drawstyle != d) {
    bool sameFont = (drawstyle->getFont() == d->getFont());

    doc->addUndo(new G_undoChangeDrawstyle(this));
    setDrawstyle(d);

    if(!sameFont || !labelVisible) {
      QPixmap tmp(1, 1);
      QPainter tmpp(&tmp);
      label.draw(tmpp, *d, false);
      label.setPos(label.getPos());
    }
    
    if(type & G_TEXT && !isDrawn()) {
      QPixmap tmp(1, 1);
      QPainter tmpp(&tmp);
      where->draw(tmpp);
    }

  }
  else {
    d->deleteReference();
  }
}


void G_ref::setDrawstyle(G_drawstyle *d)
{
  drawstyle->deleteReference();
  drawstyle = d;
}


//which types may be substituted for this type if this is a given
int G_ref::whatCanItBe()
{
  static hash_set<G_ref *> refsConsidered; // prevents infinite recursion

  //if it's a measurement or calculation, it can be either
  if(type & G_VALUE) return G_VALUE;

  //if it's a filled, it can be any filled:
  if(type & G_FILLED) return G_FILLED;
  
  //otherwise, if it's not some curve, it can only be itself
  if(!(type & G_CURVE)) return type;

  if(refsConsidered.count(this)) return G_CURVE; //if it's already been considered

  int soFar = G_CURVE; //stores what we haven't eliminated yet
  //now go through the children and eliminate possibilities
  int i;
  for(i = 0; i < (int)children.size(); ++i) {
    if(soFar == type) { //if we eliminated all possibilities
      return type;
    }

    //only a segment can be used for scaling
    if(type == G_SEGMENT && (children[i]->type & G_GEOMETRIC) &&
       children[i]->descendType == G_SCALED && children[i]->parents[0] != this) {
      soFar &= G_SEGMENT;
      continue;
    }

    //for transforms, determine possibilities recursively
    if(children[i]->type == type && IS_TRANSFORM(children[i]->descendType)) {
      refsConsidered.insert(this);
      soFar &= children[i]->whatCanItBe();
      refsConsidered.erase(this);
      continue;
    }

    //check point children
    if(children[i]->type == G_POINT) {
      //only segments have midpoints
      if(children[i]->descendType == G_MID_POINT) return G_SEGMENT;
      if(children[i]->descendType == G_INTERSECTION2_POINT) {
	if((type & G_STRAIGHT) == 0) { //if it's a curve, it must stay a curve
	  soFar &= (G_CURVE - G_STRAIGHT);
	}
      }
      if(children[i]->descendType == G_END_POINT) soFar &= (G_SEGMENT | G_RAY | G_ARC);
      if(children[i]->descendType == G_END2_POINT) soFar &= (G_SEGMENT | G_ARC);
      if(children[i]->descendType == G_CENTER_POINT) soFar &= (G_CIRCLE | G_ARC);
      continue;
    }

    //check line children
    if(children[i]->type == G_LINE) {
      if(children[i]->descendType == G_PERPENDICULAR_LINE || 
	 children[i]->descendType == G_PARALLEL_LINE) {
	soFar &= G_STRAIGHT;
	continue;
      }
    }

    if(children[i]->type == G_CIRCLE && //only a segment can be a radius of a circle
       children[i]->descendType == G_CENTERRADIUS_CIRCLE) return G_SEGMENT;

    if(children[i]->type == G_MEASURE) {
      if(children[i]->descendType == G_LENGTH_MEASURE) {
	soFar &= (G_ARC | G_CIRCLE | G_SEGMENT);
	continue;
      }
      if(children[i]->descendType == G_RADIUS_MEASURE) {
	soFar &= (G_ARC | G_CIRCLE);
	continue;
      }
      if(children[i]->descendType == G_ANGLE_MEASURE) return G_ARC;
      if(children[i]->descendType == G_RATIO_MEASURE) return G_SEGMENT;
      if(children[i]->descendType == G_SLOPE_MEASURE) {
	soFar &= G_STRAIGHT;
	continue;
      }
    }

    //arc sectors, arc segments and circle interiors are specific:
    if(children[i]->type & G_FILLED) return type; 

    //finally check loops recursively
    if(children[i]->type == G_LOOP) {
      Q_ASSERT(doc->isConstruction());
      KSegConstruction *d = (KSegConstruction *)doc;

      int j;
      for(j = 0; j < int(children[i]->parents.size()); ++j) {
	if(children[i]->parents[j] != this) continue;

	//now the current object corresponds to the jth given in the loop that is
	//the ith child.  Check if it can do that.
	refsConsidered.insert(this);
	soFar &= d->getGiven()[j]->whatCanItBe();
	refsConsidered.erase(this);
      }
    }
  }

  return soFar;
}




QDataStream &operator<<(QDataStream &stream, G_ref &ref)
{
  //parents and draw style have already been saved.

  short info = 0; //saves space

  //the first five bits is the object type
  int tmp = qRound((log(double(ref.type)) / log(2.)));

  info |= (tmp & 31); //bits 0-4

  //the next four bits is the descend type
  info |= ((ref.descendType & 15) << 5); //bits 5-8

  //the next four bits are the visible, label visible, given, and final flags
  info |= (ref.visible << 9); //bit 9
  info |= (ref.labelVisible << 10); //bit 10
  info |= (ref.given << 11); //bit 11
  info |= (ref.final << 12); //bit 12

  stream << info;

  if(ref.type == G_LOOP) return stream;

  stream << ref.label;

  ref.where->save(stream);

  return stream;
}


QDataStream &operator>>(QDataStream &stream, G_ref &ref)
{
  //parents, draw style, and the document have already been loaded.

  short info = 0; //saves space

  stream >> info;

  //the first five bits is the object type
  ref.type = (G_Type)(1 << (info & 31));
  info >>= 5;
  
  //the next four bits is the descend type
  ref.descendType = (info & 15);
  info >>= 4;

  //the next four bits are the visible, label visible, given, and final flags
  ref.visible = info & 1;
  ref.labelVisible = info & 2;
  ref.given = info & 4;
  ref.final = info & 8;

  ref.createObject();

  if(ref.type == G_LOOP) return stream;

  stream >> ref.label;

  ref.where->load(stream);

  return stream;
}



QString G_ref::getNameFromType(G_Type type)
{
  if(type == G_POINT) return qApp->translate("G_ref", "Point", "");
  if(type == G_SEGMENT) return qApp->translate("G_ref", "Segment", "");
  if(type == G_RAY) return qApp->translate("G_ref", "Ray", "");
  if(type == G_LINE) return qApp->translate("G_ref", "Line", "");
  if(type == G_CIRCLE) return qApp->translate("G_ref", "Circle", "");
  if(type == G_ARC) return qApp->translate("G_ref", "Arc", "");
  if(type == G_POLYGON) return qApp->translate("G_ref", "Polygon", "");
  if(type == G_CIRCLEINTERIOR) return qApp->translate("G_ref", "Circle Interior", "");
  if(type == G_ARCSECTOR) return qApp->translate("G_ref", "Arc Sector", "");
  if(type == G_ARCSEGMENT) return qApp->translate("G_ref", "Arc Segment", "");
  if(type == G_LOCUS) return qApp->translate("G_ref", "Locus", "");
  if(type == G_MEASURE) return qApp->translate("G_ref", "Measurement", "");
  if(type == G_CALCULATE) return qApp->translate("G_ref", "Calculation", "");
  if(type == G_ANNOTATION) return qApp->translate("G_ref", "Annotation", "");

  return qApp->translate("G_ref", "Unknown", "");
}
