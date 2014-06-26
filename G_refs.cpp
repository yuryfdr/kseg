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
#include "G_refs.H"
#include "G_refSearcher.H"

#include "my_hash_set.H"

struct topSortState
{
  topSortState(G_ref *e, int c) : element(e), child(c) {}
  
  G_ref *element;
  int child;
};


G_refs::G_refs()
{
  mHoldingRemovals = false; mToBeRemoved = NULL;
}


G_refs::~G_refs()
{
  Q_ASSERT(!mHoldingRemovals);
}


void G_refs::appendUnique(G_ref *r)
{
  if(findRef(r) == -1) append(r);
}
void G_refs::update(bool fromLocus)
{ 
  //if fromLocus is true, formulas are not reparsed.
  int i; 
  for(i = 0; i < (int)count(); i++) at(i)->update(fromLocus);
}

void G_refs::removeRef(G_ref *r)
{
  if(mHoldingRemovals) mToBeRemoved->insert(r);
  else remove(find(r));
}

bool G_refs::operator==(G_refs &r)
{
  if(count() != r.count()) return false;

  int i; 
  for(i = 0; i < (int)count(); i++) if(at(i) != r.at(i)) return false;

  return true;
}

void G_refs::holdRemovals()
{
  Q_ASSERT(!mHoldingRemovals);
  mHoldingRemovals = true;
  mToBeRemoved = new hash_set<G_ref *>();
}

void G_refs::commitRemovals()
{
  Q_ASSERT(mHoldingRemovals);

  int i;
  int offs = 0;
  for(i = 0; i < (int)count(); i++) {
    (*this)[i - offs] = at(i);
    if(mToBeRemoved->count(at(i))) offs++;
  }
  erase(end() - offs, end());

  mHoldingRemovals = false;
  delete mToBeRemoved;
  mToBeRemoved = NULL;
}

void G_refs::topologicalSort(G_refs toBeAdded, const G_ref *target)
{
  //this function creates a list such that every object comes after all its parents.
  //all children of objects in toBeAdded will be inserted into the list.
  //uses a hash table to keep track of what's been added.
  //target is when only a certain object needs to be updated (like for loci).  It's NULL by default.
  int i;
  
  clear();
  hash_set<G_ref *> added;
  vector<topSortState> stateStack;

  AncestorSearcher *as = NULL;

  if(target) as = new AncestorSearcher(target);

  //loops are separate:
  G_refs loops;
  for(i = 0; i < (int)toBeAdded.count(); ++i) {
    if(toBeAdded[i]->getType() == G_LOOP) {
      loops.push_back(toBeAdded[i]);
    }
  }


  //main loop:
  for(i = 0; i < (int)toBeAdded.count(); ++i) {
    if(added.count(toBeAdded[i])) continue;

    added.insert(toBeAdded[i]);
    stateStack.push_back(topSortState(toBeAdded[i], 0));

    while(stateStack.size()) {
      topSortState &state = stateStack.back();

      if(state.child >= (int)state.element->getChildrenConst().count()) {
	//finished!
	if(state.element->getType() == G_LOOP) {
	  loops.appendUnique(state.element);
	}
	else {
	  append(state.element);
	}
	
	stateStack.pop_back();
	continue;
      }

      G_ref *newRef = state.element->getChildrenConst()[state.child];
      state.child++;

      if(added.count(newRef)) continue;
      if(target && !as->search(newRef)) continue;

      added.insert(newRef);
      stateStack.push_back(topSortState(newRef, 0));
    }
  }

  reverse(begin(), end());

  vector<G_ref *>::insert(end(), loops.begin(), loops.end());

  if(target) delete as;
}

