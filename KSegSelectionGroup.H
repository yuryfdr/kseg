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


#ifndef KSEGSELECTIONGROUP_H
#define KSEGSELECTIONGROUP_H

#include "defs.H"
#include "G_refs.H"
#include "KSegDocument.H"

class KSegSelectionGroup
{
public:
  typedef hash_set<G_ref *>::iterator iter;

  KSegSelectionGroup(KSegDocument *inDoc) : doc(inDoc) {}
  ~KSegSelectionGroup() { for(iter it = refs.begin(); it != refs.end(); ++it) (*it)->removeSelectionGroup(this); }

  void setName(const QString &inName) { name = inName; }
  QString getName() const { return name; }

  void deleted(G_ref *r) { refs.erase(r); }

  bool isEmpty() const { return size() == 0; }

  int size() const {
    int out = 0;
    for(iter it = refs.begin(); it != refs.end(); ++it) if(!(*it)->getDeleted()) out++;
    return out;
  }

  void addRef(G_ref *r) { if(refs.count(r)) return; refs.insert(r); r->addSelectionGroup(this); }
  void delRef(G_ref *r) { if(!refs.count(r)) return; refs.erase(r); r->removeSelectionGroup(this); }

  void selectRefs() { for(iter it = refs.begin(); it != refs.end(); ++it) if(!(*it)->getDeleted()) doc->addSel(*it); }
  void deselectRefs() { for(iter it = refs.begin(); it != refs.end(); ++it) if(!(*it)->getDeleted()) doc->delSel(*it); }

  void save(QDataStream &stream, hash_map<G_ref *, int> &refMap) {
    stream << name;
    stream << size();
    for(iter it = refs.begin(); it != refs.end(); ++it) if(!(*it)->getDeleted())
      stream << refMap[*it];
  }

  void load(QDataStream &stream) {
    int sz, cur;
    stream >> name; stream >> sz;
    while(sz-- > 0) { stream >> cur; addRef(doc->getRef(cur)); }
  }

private:
  KSegDocument *doc;
  QString name;
  hash_set<G_ref *> refs;
};

#endif //KSEGSELECTIONGROUP_H
