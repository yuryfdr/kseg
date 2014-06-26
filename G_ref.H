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


#ifndef G_REF_H
#define G_REF_H

#include <qstring.h>
#include <qdatastream.h>
#include "my_hash_set.H"
#include <set>

#include "defs.H"
#include "G_refs.H"
#include "G_point.H"
#include "G_label.H"

class G_refs;
class G_object;
class G_point;
class G_drawstyle;
class KSegDocument;
class KSegSelectionGroup;

class G_ref
{
public:
  G_ref(KSegDocument *inDoc = 0);
  ~G_ref();

  friend QDataStream &operator<<(QDataStream &, G_ref &);
  friend QDataStream &operator>>(QDataStream &, G_ref &);

  void create(G_Type inType, int inDescentType, const G_refs &inParents,
	      KSegDocument *inDoc, bool inAutoLabel = true);
  void createObject(); //for loading and undeleting
  void remove();
  void reconstrain(int inDescentType, const G_refs &inParents, bool topSortAfter = true);

  //these guys are fairly dangerous--don't write to them!
  G_refs &getParents() { return parents; }
  G_refs &getChildren() { return children; }

  //the safe versions--use these if possible.
  const G_refs &getParentsConst() const { return parents; }
  const G_refs &getChildrenConst() const { return children; }

  G_object *getObject() const { return where; }
  G_Type getType() const { return type; }
  int getDescendType() const { return descendType; }

  bool getExists() const { return exists; }
  bool getVisible() const { return visible; }
  bool getSelected() const { return selected; }
  bool getLabelVisible() const { return labelVisible; }
  bool getDeleted() const { return deleted; }
  bool getNormal() const { return !given && !final; }
  bool getGiven() const { return given && !final; }
  bool getFinal() const { return final && !given; }
  bool getInitial() const { return given && final; }

  bool isDrawn() const { return exists && visible; }

  G_drawstyle *getDrawstyle() const { return drawstyle; }

  void setExists(bool b) { exists = b; }
  void setVisible(bool b) { visible = b; }
  void setSelected(bool b) { selected = b; }
  void setLabelVisible(bool b) { labelVisible = b; }
  void setNormal() { given = final = false; }
  void setGiven() { given = true; final = false; }
  void setFinal() { final = true; given = false; }
  void setInitial() { final = given = true; }
  void setDrawstyle(G_drawstyle *d);
  void changeDrawstyle(G_drawstyle *d);

  void update(bool fromLocus = false);

  bool isDirectParent(int which); //is parent "which" a direct parent of this?

  G_label &getLabel() { return label; }

  G_point getLabelPos() { return label.getPos(); }
  void setLabelPos(const G_point &pos) { label.setPos(pos); }
  QRect getLabelRect() { return label.getRect(); }
  void drawLabel(QPainter &p); //checks if it should be drawn too
  KSegDocument *getDocument() { return doc; }

  int whatCanItBe(); //returns what may be used in place of this object as a given
                     //in a script

  QString toString()
  { return QString("type = ") + getNameFromType(type) + QString(" name = ") + label.getText(); }

  static QString getNameFromType(G_Type type);

  void addSelectionGroup(KSegSelectionGroup *g) { selectionGroups.insert(g); }
  void removeSelectionGroup(KSegSelectionGroup *g) { selectionGroups.erase(g); }

private:
  G_object *where;

  G_Type type;
  int descendType;

  G_refs parents;
  G_refs children;

  bool exists;
  bool visible;
  bool selected;
  bool labelVisible;
  bool deleted;
  bool given;
  bool final; // given and final true means the object is initial--saves space

  G_drawstyle *drawstyle;
  KSegDocument *doc;

  G_label label;

  set<KSegSelectionGroup *> selectionGroups; //used to inform selection groups of object's death
};

QDataStream &operator<<(QDataStream &, G_ref &);
QDataStream &operator>>(QDataStream &, G_ref &);

namespace HASH_NAMESPACE {
template <>
struct hash<G_ref *>
{
  size_t operator ()(const G_ref *r) const { return (size_t)r; }
};
}

#endif //G_REF_H
