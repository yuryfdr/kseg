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

#include "KSegConstruction.H"
#include "KSegView.H"
#include "G_refs.H"
#include "G_ref.H"
#include "G_object.H"
#include "my_hash_map.H"
#include "G_refSearcher.H"

std::vector<KSegConstruction *> KSegConstruction::constructions;

KSegConstruction::KSegConstruction()
{
  constructions.push_back(this);
}

KSegConstruction::~KSegConstruction()
{
  constructions.erase(std::find(constructions.begin(),
				constructions.end(), this));
}


void KSegConstruction::addRef(G_ref *ref)
{
  KSegDocument::addRef(ref);

  //givens will be added into the right place by themselves
  if(ref->getFinal()) final.append(ref);
  if(ref->getInitial()) initial.append(ref);
  if(ref->getType() == G_LOOP) loops.append(ref);
}

void KSegConstruction::delRef(G_ref *ref)
{
  KSegDocument::delRef(ref);

  if(ref->getGiven()) given.removeRef(ref);
  if(ref->getFinal()) final.removeRef(ref);
  if(ref->getInitial()) initial.removeRef(ref);  
  if(ref->getType() == G_LOOP) loops.removeRef(ref);
}

void KSegConstruction::play(G_refs givens, KSegDocument *doc, int iter, bool first)
{
  static hash_map<G_ref *, G_ref *> initialMap; //map of the initial objects
  static NotInConstructionSearcher nicSearcher;
  static ImplicitInitialSearcher iiSearcher;

  if(first) {
    initialMap.clear();
    nicSearcher.reInit();
    iiSearcher.reInit();
  }
 
  ViewTransform trans = KSegView::constructionTransform;
  ViewTransform invTrans(-trans.offsetX * trans.zoom, -trans.offsetY * trans.zoom, 1. / trans.zoom);
 
  hash_map<G_ref *, G_ref *> currentMap = initialMap;

  currentMap.resize(allRefs.size());

  int i, j;

  //match the givens:
  vector<int> givenPerm;
  bool tmp = match(givens, given, givenPerm);
  Q_ASSERT(tmp);

  for(i = 0; i < (int)given.count(); i++) {
    currentMap[given[i]] = givens[givenPerm[i]];
  }

  QRect win = KSegView::constructionPainter->window();
  if(win.width() > 100 && win.height() > 100) {
    win.setSize(QSize(win.width() - 60, win.height() - 60));
    win.moveBy(30, 30);
  }

  //main loop over all the refs in this construction
  for(i = 0; i < (int)allRefs.count(); i++) {

    //if loop
    if(allRefs[i]->getType() == G_LOOP) {
      if(iter) {
	G_refs newGivens;
	
	for(j = 0; j < int(allRefs[i]->getParents().count()); j++) {
	  newGivens.append(currentMap[allRefs[i]->getParents()[j]]);
	}
	
	play(newGivens, doc, iter - 1, false);
	currentMap.insert(initialMap.begin(), initialMap.end());
      }
      
      continue;
    }

    //should it be constructed?
    if(allRefs[i]->getGiven()) continue;
    if(allRefs[i]->getFinal() && iter != 0) continue;
    if(currentMap.count(allRefs[i]) != 0) continue;
    if(nicSearcher.search(allRefs[i]) == false) continue;
    
    //construct it!
    G_refs newParents;

    //set up the parents
    bool construct = true; //will be marked false if any parents are missing
                           // like because they are final

    for(j = 0; j < int(allRefs[i]->getParents().count()); j++) {
      if(currentMap.count(allRefs[i]->getParents()[j]) == 0) {
	construct = false;
	break;
      }
      newParents.append(currentMap[allRefs[i]->getParents()[j]]);      
    }

    if(!construct) continue;

    G_ref *r = new G_ref();

    //creation!
    //check if it's a transform becuase then the type depends on the parent:
    if((allRefs[i]->getType() & G_CURVE) && IS_TRANSFORM(allRefs[i]->getDescendType())) {
      r->create(newParents[0]->getType(), allRefs[i]->getDescendType(), newParents, doc, false);
    }
    else { //normal creation
      r->create(allRefs[i]->getType(), allRefs[i]->getDescendType(), newParents, doc, false);
    }

    if(r->getType() == G_CALCULATE) { //we must transfer the formula string
      ((G_calculateObject *)(r->getObject()))->
	setInitialFormulaString(((G_calculateObject *)(allRefs[i]->getObject()))->getFormulaString());
      r->update();
    }

    if(r->getType() == G_LOCUS) { //we need to keep the number of samples the same
	((G_locusObject *)(r->getObject()))->
	  setMaxSamples(((G_locusObject *)(allRefs[i]->getObject()))->getMaxSamples());
    }

    //set the right draw style and attributes
    allRefs[i]->getDrawstyle()->addReference();
    r->setDrawstyle(allRefs[i]->getDrawstyle());
    
    r->setVisible(allRefs[i]->getVisible());
    r->setLabelVisible(allRefs[i]->getLabelVisible());
    r->getLabel().setText(allRefs[i]->getLabel().getText(), false);
    r->setLabelPos(allRefs[i]->getLabelPos());

    //randomize positions of points and measurements:
    if(r->getType() == G_POINT) {
      if(r->getDescendType() == G_CONSTRAINED_POINT) {
	((G_pointObject *)(r->getObject()))->setP(INTRAND(0, 1000) / 1000.);
	r->update();
      }
      if(r->getDescendType() == G_FREE_POINT) {
	r->getObject()->translate(G_point(INTRAND(win.left(), win.right()),
					  INTRAND(win.top(), win.bottom())));
      }
    }

    if(r->getType() & G_TEXT) {
	r->getObject()->translate(G_point(INTRAND(win.left(), win.right()),
					  INTRAND(win.top(), win.bottom())));
    }

    if(r->isDrawn()) {
      r->getObject()->tempTransform(trans);
      r->getObject()->draw(*(KSegView::constructionPainter));
      r->getObject()->tempTransform(invTrans);
    }

    //put it in the maps.
    currentMap[allRefs[i]] = r;
    if(allRefs[i]->getInitial() || iiSearcher.search(allRefs[i]) == false) {
      initialMap[allRefs[i]] = r;
    }
  }

  if(first) { //conserve memory
    initialMap.clear();
    nicSearcher.reInit();
  }
}


bool KSegConstruction::doesMatch(const G_refs& which)
{
  vector<int> v; //dummy
  return match(which, given, v);
}


bool KSegConstruction::canMakeNormal(G_ref *which)
{
  if(which->getType() == G_LOOP) return false;
  if(which->getNormal() || which->getFinal() || which->getInitial()) return true;
  //otherwise the ref is a given.  Check if there are loops in the document
  if(loops.size() > 0) return false;

  return true;
}


bool KSegConstruction::canMakeGiven(G_ref *which)
{
  if(which->getType() == G_LOOP) return false;
  if(which->getGiven()) return true;

  //if there are loops, return false
  if(loops.size() > 0) return false;

  int i;

  //if it's the driven point of a locus, can't make it given
  for(i = 0; i < int(which->getChildrenConst().size()); ++i) {
    if(which->getChildrenConst()[i]->getType() != G_LOCUS) continue;
    if(which->getChildrenConst()[i]->getDescendType() != G_OBJECT_LOCUS) continue;
    if(which->getChildrenConst()[i]->getParentsConst()[1] != which) continue;
    return false;
  }

  //now check if making it given would disrupt a locus chain
  for(i = 0; i < (int)allRefs.count(); ++i) {
    if(allRefs[i]->getType() != G_LOCUS ||
       allRefs[i]->getDescendType() != G_OBJECT_LOCUS) continue;

    BrokenLocusChainSearcher blcs(allRefs[i]->getParentsConst()[1], which);

    if(blcs.search(allRefs[i]->getParentsConst()[0]) == false) return false;
  }

  return true;
}


bool KSegConstruction::canMakeFinal(G_ref * /*which*/)
{
  return true;
}


bool KSegConstruction::canMakeInitial(G_ref * /*which*/)
{
  return true;
}


void KSegConstruction::makeNormal(G_ref *which)
{
  if(which->getNormal()) return;

  //insert undo here
  addUndo(new G_undoChangeConstructMode(which));
  
  if(which->getGiven()) given.removeRef(which);
  if(which->getFinal()) final.removeRef(which);
  if(which->getInitial()) initial.removeRef(which);

  which->setNormal();
}


void KSegConstruction::makeGiven(G_ref *which, int where)
{
  if(which->getGiven()) return;

  //insert undo here
  addUndo(new G_undoChangeConstructMode(which));
  
  if(which->getFinal()) final.removeRef(which);
  if(which->getInitial()) initial.removeRef(which);

  which->setGiven();
  if(where == -1) given.append(which);
  else given.insert(where, which);
}


void KSegConstruction::makeFinal(G_ref *which)
{
  if(which->getFinal()) return;

  //insert undo here
  addUndo(new G_undoChangeConstructMode(which));
  
  if(which->getGiven()) given.removeRef(which);
  if(which->getInitial()) initial.removeRef(which);

  which->setFinal();
  final.append(which);
}


void KSegConstruction::makeInitial(G_ref *which)
{
  if(which->getInitial()) return;

  //insert undo here
  addUndo(new G_undoChangeConstructMode(which));
  
  if(which->getGiven()) given.removeRef(which);
  if(which->getFinal()) final.removeRef(which);

  which->setInitial();
  initial.append(which);
}


ScanType KSegConstruction::scanConstructionFile(QFile &f, const G_refs &selected)
{
  vector<int> givens;

    //check to make sure the file won't crash us.
  if(!f.open(QIODevice::ReadOnly) || f.size() < 5)
    return INVALID;

  char block[4];
  
  f.readBlock(block, 4);
  
  if(block[0] || block[1] || block[2] || block[3] + 5 > (int)f.size()) {
    f.close();
    return INVALID;
  }

  f.ungetch(block[3]);
  f.ungetch(block[2]);
  f.ungetch(block[1]);
  f.ungetch(block[0]);
  
  QDataStream stream(&f);
  
  QString tmp;
  
  stream >> tmp;

  int majVer, minVer;
  bool isConstr, ok;

  ok = KSegDocument::parseVersionString(tmp, &isConstr, &majVer, &minVer);

  if(!ok || !isConstr || (majVer == 0 && minVer == 0)) { //constructions version 0.0 have no given info at start
    f.close();
    return INVALID;
  }

  int num;

  stream >> num;

  while(num--) {
    int tmp;

    stream >> tmp;
    
    givens.push_back(tmp);
  }

  vector<int> v; //dummy

  if(match(selected, givens, v))
    return MATCHED;
  else return UNMATCHED;
}

//find a permutation of sel which matches given.
//set perm to that permutation or return false
//if it doesn't exist.
bool KSegConstruction::match(const G_refs& sel, const vector<int>& given, vector<int> &perm) {
  if(sel.size() != given.size()) return false;

  vector<int> revperm(sel.size());
  perm.resize(sel.size());

  int i, j, k;

  for(i = 0; i < (int)sel.size(); ++i) {
    perm[i] = -1;
    revperm[i] = -1;
  }

  int unmatched = 0;

  //start with a simple greedy match pass

  for(i = 0; i < (int)sel.size(); ++i) {
    bool atLeastOne = false;

    for(j = 0; j < (int)sel.size(); ++j) {
      if((sel[j]->getType() & given[i])) {
	atLeastOne = true;

	if(revperm[j] != -1) continue;

	perm[i] = j;
	revperm[j] = i;
	break;
      }      
    }

    if(!atLeastOne) return false; //there was something that doesn't fit to anything
    if(j == (int)sel.size()) unmatched++; //if reached end of loop, didn't match
  }

  if(unmatched == 0) return true;

  //length 3 augmenting path search
  for(i = 0; i < (int)sel.size(); ++i) {
    if(perm[i] != -1) continue;
    for(j = 0; j < (int)sel.size(); ++j) {
      if(!(sel[j]->getType() & given[i])) continue;

      Q_ASSERT(revperm[j] != -1);

      int x = revperm[j];
      Q_ASSERT(x != i);
      
      //try to match it with something
      for(k = 0; k < (int)sel.size(); ++k) {
	if(j == k || revperm[k] != -1) continue;
	if(!(sel[k]->getType() & given[x])) continue;

	//we have a match:
	perm[i] = j;
	revperm[j] = i;
	perm[x] = k;
	revperm[k] = x;
	unmatched--;

	if(unmatched == 0) return true;
	k = j = sel.size(); //this breaks out of the loops
	i = -1; //reset the outer loop
      }
    }
  }

  return unmatched == 0;
}


bool KSegConstruction::match(const G_refs& sel, const G_refs &given, vector<int> &perm) {
  vector<int> g;
  int i;

  for(i = 0; i < (int)given.size(); ++i) {
    g.push_back(given[i]->whatCanItBe());
  }

  return match(sel, g, perm);
}


