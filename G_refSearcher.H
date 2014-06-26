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


#ifndef G_REFSEARCHER_H
#define G_REFSEARCHER_H

#include "defs.H"
#include "my_hash_map.H"
#include "G_ref.H"

//----------------------------------------------------------------------------------

class G_refSearcher
{
public:
  virtual ~G_refSearcher() {}
  void reInit() { known_refs.clear(); }
  bool search(const G_refs &refs);
  bool search(G_ref *ref) { G_refs tmp; tmp.append(ref); return search(tmp); }
  
  virtual bool is_found(const G_ref *) = 0; //if it returns true, the search stops
  virtual G_refs search_next(const G_ref *) = 0; //the objects to search further to determine

private:
  hash_map<G_ref *, bool> known_refs;
};

//----------------------------------------------------------------------------------


//search children until r (search true for ancestors of r)
class AncestorSearcher : public G_refSearcher
{
public:
  AncestorSearcher(const G_ref *ref) { r = ref; }

  bool is_found(const G_ref *ref) { return ref == r; }
  G_refs search_next(const G_ref *ref) { return ref->getChildrenConst(); }

private:
  const G_ref *r;
};

//----------------------------------------------------------------------------------


//search parents until r (search true for descendants of r)
class DescendantSearcher : public G_refSearcher
{
public:
  DescendantSearcher(const G_ref *ref) { r = ref; }

  bool is_found(const G_ref *ref) { return ref == r; }
  G_refs search_next(const G_ref *ref) { return ref->getParentsConst(); }

private:
  const G_ref *r;
};


//----------------------------------------------------------------------------------


//search children until final, initial, or bottom-level non-given
//stop at given
class NotInConstructionSearcher : public G_refSearcher
{
public:
  NotInConstructionSearcher() {}

  bool is_found(const G_ref *ref) { 
    return ref->getFinal() || ref->getInitial() ||
      (!(ref->getGiven()) && ref->getChildrenConst().size() == 0); 
  }
  G_refs search_next(const G_ref *ref)
  { if(ref->getGiven()) return G_refs(); else return ref->getChildrenConst(); }
};

//----------------------------------------------------------------------------------


//search parents until final
//stop at initial, given
class ImplicitFinalSearcher : public G_refSearcher
{
public:
  ImplicitFinalSearcher() {}

  bool is_found(const G_ref *ref) { 
    return ref->getFinal();
  }
  G_refs search_next(const G_ref *ref)
  { if(ref->getGiven() || ref->getInitial()) return G_refs(); else return ref->getParentsConst(); }
};

//----------------------------------------------------------------------------------


//search parents until final or initial
//stop at given
class CanMakeGivenSearcher : public G_refSearcher
{
public:
  CanMakeGivenSearcher() {}

  bool is_found(const G_ref *ref) { 
    return ref->getFinal() || ref->getInitial();
  }
  G_refs search_next(const G_ref *ref)
  { if(ref->getGiven()) return G_refs(); else return ref->getParentsConst(); }
};

//----------------------------------------------------------------------------------


//search children until given or initial or loop
//stop at final
class CanMakeFinalSearcher : public G_refSearcher
{
public:
  CanMakeFinalSearcher() {}

  bool is_found(const G_ref *ref) { 
    return ref->getGiven() || ref->getInitial() || ref->getType() == G_LOOP;
  }
  G_refs search_next(const G_ref *ref)
  { if(ref->getFinal()) return G_refs(); else return ref->getChildrenConst(); }
};

//----------------------------------------------------------------------------------


//search parents until final, given, or top-level non-initial
//stop at initial
//pretends that extraInitial is initial.
class ImplicitInitialSearcher : public G_refSearcher
{
public:
  ImplicitInitialSearcher(G_ref *inExtraInitial = NULL) { extraInitial = inExtraInitial; }

  bool is_found(const G_ref *ref) { 
    return ref->getFinal() || ref->getGiven() || (extraInitial != ref && ref->getInitial() == false && ref->getParentsConst().count() == 0);
  }
  G_refs search_next(const G_ref *ref)
  { if(ref->getInitial() || ref == extraInitial) return G_refs(); else return ref->getParentsConst(); }

  G_ref *extraInitial;
};

//----------------------------------------------------------------------------------


//search parents until final
//stop at given or initial
class CanMakeInitialAncSearcher : public G_refSearcher
{
public:
  CanMakeInitialAncSearcher() {}

  bool is_found(const G_ref *ref) { 
    return ref->getFinal();
  }
  G_refs search_next(const G_ref *ref)
  { if(ref->getGiven() || ref->getInitial()) return G_refs(); else return ref->getParentsConst(); }
};

//----------------------------------------------------------------------------------

//search children until given
//stop at final or initial
class CanMakeInitialDesSearcher : public G_refSearcher
{
public:
  CanMakeInitialDesSearcher() {}

  bool is_found(const G_ref *ref) { 
    return ref->getGiven();
  }
  G_refs search_next(const G_ref *ref)
  { if(ref->getFinal() || ref->getInitial()) return G_refs(); else return ref->getChildrenConst(); }
};

//----------------------------------------------------------------------------------

//this class is for making sure that a locus chain is never disrupted by
//a reconstraint (the potentially freed point may be passed as exclude)
//or by givens (whose parents may change when the construction is played)
//it looks for a stable descendence from the search parameter (driver) to
//driven.
class BrokenLocusChainSearcher : public G_refSearcher
{
public:
  BrokenLocusChainSearcher(G_ref *inDriven, G_ref *inExclude = NULL)
    : driven(inDriven), exclude(inExclude) {}

  bool is_found(const G_ref *ref) { 
    return ref == driven;
  }

  G_refs search_next(const G_ref *ref)
  {
    if(driven->getGiven() || ref->getGiven() || ref == exclude) return G_refs();
    else return ref->getChildrenConst();
  }

private:
  G_ref *driven, *exclude;
};


//----------------------------------------------------------------------------------


//this class counts the number of finals (explicit and implicit)
//for the recursion dialog.  The number is stored in count.
//because of the hash table, no final gets counted twice.
class FinalCounter : public G_refSearcher //da-da-daa-dum da-da-di-da-dum :)
{
public:
  FinalCounter() { count = 0; }

  bool is_found(const G_ref *) { count++; return false; }
  G_refs search_next(const G_ref *ref) { return ref->getChildrenConst(); }

  int count;
};

//----------------------------------------------------------------------------------



#endif //G_REFSEARCHER_H
