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


#ifndef G_OBJECT_H
#define G_OBJECT_H

#include <qdatastream.h>

#include "formula/kformula.H"

#include "G_ref.H"
#include "G_point.H"
#include "G_line.H"
#include "G_segment.H"
#include "G_ray.H"
#include "G_circle.H"
#include "G_arc.H"
#include "G_locus.H"
#include "G_arcSector.H"
#include "G_arcSegment.H"
#include "G_circleInterior.H"
#include "G_polygon.H"

struct ViewTransform {
  ViewTransform() : offsetX(0), offsetY(0), zoom(1) {}
  ViewTransform(double x, double y, double z) : offsetX(x), offsetY(y), zoom(z) {}
  ViewTransform(const ViewTransform &t) : offsetX(t.offsetX), offsetY(t.offsetY), zoom(t.zoom) {}
  double offsetX, offsetY, zoom;
};

class G_object
{
public:
  G_object();
  G_object(G_ref *inWhere) { where = inWhere; } 
  virtual ~G_object();

  virtual void draw(QPainter &p);

  virtual G_point getNearestPoint(const G_point &p) { return getGeoRef()->getNearestPoint(p); }

  virtual void translate(const G_point &p) { getGeoRef()->translate(p); }

  virtual QRect getScrollExtents() const { return QRect(); }
  virtual QRect getSelectExtents() const { return getScrollExtents(); }

  virtual bool inRect(QRect r) { return getGeoRef()->inRect(r); }

  virtual void save(QDataStream &) {}
  virtual void load(QDataStream &) {}

  virtual void tempTransform(const ViewTransform &trans);

  //these guys are to make writing update functions easier:  do not use them unless you know the type!
  G_object *parent(int i) { return where->getParents()[i]->getObject(); }
  virtual G_geometry *getGeoRef() { return NULL; }
  virtual G_straight *getStraightRef() { return (G_straight *)getGeoRef(); }
  virtual G_curve *getCurveRef() { return (G_curve *)getGeoRef(); }
  virtual G_filled *getFilledRef() { return (G_filled *)getGeoRef(); }
  virtual G_point getPoint() const { return G_point(); }
  virtual G_line getLine() const { return G_line(); }
  virtual G_ray getRay() const { return G_ray(); }
  virtual G_segment getSegment() const { return G_segment(); }
  virtual G_circle getCircle() const { return G_circle(); }
  virtual G_arc getArc() const { return G_arc(); }
  virtual G_locus getLocus() const { return G_locus(); }
  virtual double getNumValue() const { return 0; }

protected:
  friend void G_ref::update(bool fromLocus);
  virtual void update();
  void updateTransform();

  G_ref *where;
};

//---------------------------G_pointObject-------------------------------

class G_pointObject : public G_object
{
public:
  G_pointObject(G_ref *inWhere) : G_object(inWhere), p(BIG) { } 
  virtual ~G_pointObject() {}

  virtual G_point getNearestPoint(const G_point &) { return point; }

  virtual void update();

  double getP() const { return p; }
  void setP(double inP) { p = inP; }
  void setPoint(const G_point &x) { point = x; resetP(); }
  void translate(const G_point &x) { point += x; resetP(); }

  virtual QRect getScrollExtents() const { return point.getExtents(); }

  virtual void save(QDataStream &stream);
  virtual void load(QDataStream &stream);

  G_geometry *getGeoRef() { return &point; }
  virtual G_point getPoint() const { return point; }
protected:
  void resetP() { if(where->getDescendType() == G_CONSTRAINED_POINT) p = BIG; }

  G_point point;
  double p; //parameter for constrained points
};

//-------------------------------G_lineObject-------------------------

class G_lineObject : public G_object
{
public:
  G_lineObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_lineObject() {}

  virtual void update();
  virtual QRect getSelectExtents() const { return line.getExtents(); }

  virtual G_line getLine() const { return line; }
  G_geometry *getGeoRef() { return &line; }

protected:
  G_line line;
};

//-----------------------------G_rayObject---------------------------

class G_rayObject : public G_object
{
public:
  G_rayObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_rayObject() {}

  virtual void update();
  virtual QRect getSelectExtents() const { return ray.getExtents(); }
  virtual QRect getScrollExtents() const { return ray.getP1().getExtents(); }

  virtual G_ray getRay() const { return ray; }
  G_geometry *getGeoRef() { return &ray; }

protected:
  G_ray ray;
};

//---------------------------G_segmentObject---------------------------

class G_segmentObject : public G_object
{
public:
  G_segmentObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_segmentObject() {}

  virtual void update();
  virtual QRect getScrollExtents() const { return segment.getExtents(); }

  virtual G_segment getSegment() const { return segment; }
  G_geometry *getGeoRef() { return &segment; }

protected:
  G_segment segment;
};

//-------------------------G_circleObject---------------------------

class G_circleObject : public G_object
{
public:
  G_circleObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_circleObject() {}

  virtual void update();
  virtual QRect getScrollExtents() const { return circle.getExtents(); }

  virtual G_circle getCircle() const { return circle; }
  G_geometry *getGeoRef() { return &circle; }

protected:
  G_circle circle;
};

//----------------------------G_arcObject------------------------------

class G_arcObject : public G_object
{
public:
  G_arcObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_arcObject() {}

  virtual void update();
  virtual QRect getScrollExtents() const { return arc.getExtents(); }

  virtual G_arc getArc() const { return arc; }
  G_geometry *getGeoRef() { return &arc; }

protected:
  G_arc arc;
};

//---------------------------G_locusObject------------------------
class G_locusObject : public G_object
{
public:
  G_locusObject(G_ref *inWhere) : G_object(inWhere), maxSamples(150) { }
  virtual ~G_locusObject() {}

  virtual void update();
  virtual QRect getSelectExtents() const { return locus.getExtents(); }

  virtual void save(QDataStream &stream) { stream << maxSamples; }
  virtual void load(QDataStream &stream) { stream >> maxSamples; }

  virtual G_locus getLocus() const { return locus; }
  G_geometry *getGeoRef() { return &locus; }

  virtual void generateObjectLocus();
  virtual void generatePointLocus();

  virtual void setMaxSamples(int samples) { maxSamples = samples; }
  virtual int getMaxSamples() { return maxSamples; }

  //protected:
public:
  G_locus locus;

  int maxSamples;

};


//---------------------------G_valueObject------------------------
//pure virtual--base for measure and calculation
//partly implemented in G_measureObject.cpp
class G_valueObject : public G_object
{
public:
  G_valueObject(G_ref *inWhere, bool lhsRestricted = false) :
    G_object(inWhere), dirtylhs(true), lhs(lhsRestricted) {}

  virtual void update();

  virtual void calculate() = 0;
  virtual void updatelhs() = 0;

  virtual void draw(QPainter &p);

  virtual void translate(const G_point &p) { pos += p; }
  virtual G_point getNearestPoint(const G_point &p);

  virtual QRect getScrollExtents() const
  { QRect tmp; tmp.setSize(text.size()); tmp.moveCenter(pos.toQPoint()); return tmp; }

  virtual bool inRect(QRect r);

  virtual double getNumValue() const { return value; }

  virtual void tempTransform(const ViewTransform &trans);

  KFormula *getLhs() { return &lhs; } //only to be used by ReferenceFetcher classes.

protected:
  double value;

  bool dirtylhs;

  KFormula lhs;
  KFormula text;
  G_point pos;
};


//---------------------------G_measureObject------------------------
class G_measureObject : public G_valueObject
{
public:
  G_measureObject(G_ref *inWhere) : G_valueObject(inWhere) {}
  virtual ~G_measureObject() {}

  virtual void calculate();
  virtual void updatelhs();

  virtual void save(QDataStream &stream) { stream << pos; }
  virtual void load(QDataStream &stream) { stream >> pos; }
};


//---------------------------G_calculateObject------------------------
class G_calculateObject : public G_valueObject
{
public:
  G_calculateObject(G_ref *inWhere);
  virtual ~G_calculateObject() {}

  virtual void calculate();
  virtual void updatelhs() { lhs.parse(formulaString); dirtylhs = false; }

  void changeFormula(const G_refs& newParents, const QString &newFormulaString);

  void setInitialFormulaString(const QString &newFormulaString)
  { formulaString = newFormulaString; dirtylhs = true;}

  virtual void save(QDataStream &stream) { stream << pos; stream << convertFormulaStringOnSave(formulaString); }
  virtual void load(QDataStream &stream) { stream >> pos; stream >> formulaString; convertFormulaStringOnLoad(); }

  void convertFormulaStringOnLoad();
  QString convertFormulaStringOnSave(QString s) const;

  QString getFormulaString() const { return formulaString; }

protected:
  QString formulaString;
};


//-------------------------G_arcSectorObject---------------------------

class G_arcSectorObject : public G_object
{
public:
  G_arcSectorObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_arcSectorObject() {}

  virtual void update();
  virtual QRect getScrollExtents() const { return arcSector.getExtents(); }

  G_geometry *getGeoRef() { return &arcSector; }

protected:
  G_arcSector arcSector;
};


//-------------------------G_arcSegmentObject---------------------------

class G_arcSegmentObject : public G_object
{
public:
  G_arcSegmentObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_arcSegmentObject() {}

  virtual void update();
  virtual QRect getScrollExtents() const { return arcSegment.getExtents(); }

  G_geometry *getGeoRef() { return &arcSegment; }

protected:
  G_arcSegment arcSegment;
};


//-------------------------G_circleInteriorObject---------------------------

class G_circleInteriorObject : public G_object
{
public:
  G_circleInteriorObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_circleInteriorObject() {}

  virtual void update();
  virtual QRect getScrollExtents() const { return circleInterior.getExtents(); }

  G_geometry *getGeoRef() { return &circleInterior; }

protected:
  G_circleInterior circleInterior;
};

//-------------------------G_polygonObject---------------------------

class G_polygonObject : public G_object
{
public:
  G_polygonObject(G_ref *inWhere) : G_object(inWhere) { } 
  virtual ~G_polygonObject() {}

  virtual void update();
  virtual QRect getScrollExtents() const { return polygon.getExtents(); }

  G_geometry *getGeoRef() { return &polygon; }

protected:
  G_polygon polygon;
};


#endif //G_OBJECT_H
