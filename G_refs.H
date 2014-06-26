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


#ifndef G_REFS_H
#define G_REFS_H

#include "defs.H"
#include <algorithm>
#include <vector>
#include <assert.h>
#include "my_hash_set.H"

class G_ref;

//Here's what happened to this class:
//It started out as a QPtrList but it was
//too slow so I changed it to a vector
//all the weird funcs are for "backward compatibility"
//meaning that I was too lazy to rewrite all
//the code that uses G_refs
class G_refs : public vector<G_ref *>
{
public:
  G_refs();
  ~G_refs();
  
  void appendUnique(G_ref *);

  void assertUnique() { //don't call unless you're debugging (slow!)
    vector<G_ref *> v(*this);
    sort(v.begin(), v.end());
    vector<G_ref *>::iterator it = v.end();
    assert(it == unique(v.begin(), v.end()));
  }

  void topologicalSort(G_refs, const G_ref *target = 0);
  void topologicalSort(G_ref *p, const G_ref *target = 0) { G_refs tmp; tmp.append(p); topologicalSort(tmp, target); }

  void update(bool fromLocus = false);

  void append(G_ref *x) { push_back(x); }
  unsigned int count() const { return size(); }

  void insert(int pos, G_ref *what) { vector<G_ref *>::insert(begin() + pos, what); }
  void remove(int pos) { if(pos != -1) erase(begin() + pos); }
  void removeRef(G_ref *r);

  bool containsRef(const G_ref *r) const { return std::find(begin(), end(), r) != end(); }
  int find(const G_ref *r) const  { assert(!mHoldingRemovals); return rend() - std::find(rbegin(), rend(), r) - 1; }
  int findRef(const G_ref *r) const { return find(r); }

  G_ref *at(int i) { return (*this)[i]; }

  bool operator==(G_refs &r);
  bool operator!=(G_refs &r) { return !(*this == r); }

  void holdRemovals();
  void commitRemovals();

private:
  bool mHoldingRemovals;
  hash_set<G_ref *> *mToBeRemoved;
};

#endif //G_REFS_H

