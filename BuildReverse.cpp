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

#include <qapplication.h>

#include "defs.H"
#include "G_matrix.H"
#include "G_ref.H"
#include "G_refs.H"
#include "G_object.H"
#include "my_hash_map.H"
#include <vector>


class edge {
public:
  static G_matrix identity;

  G_ref *ref;

  edge(G_ref *inRef) {
    ref = inRef;
  }

  virtual const G_matrix &getMatrix() const { return identity; }
};

G_matrix edge::identity = G_matrix();


class matrixEdge : public edge {
public:
  G_matrix m;

  matrixEdge(G_ref *inRef, G_matrix inM) : edge(inRef), m(inM) {}

  const G_matrix &getMatrix() const { return m; }

};


// This class stores the graph of dependencies between objects.
// A constraint like:
// "If Point P moves by vector V, Segment S must move by the vector
//  M * V" would be represented by two matrixEdge's of M and M^-1
// between P and S.  If a bunch of edges form a cycle such that
// the product of matrices around that cycle is not the identity,
// that graph is inconsistent and needs to be repaired.  Since fixed
// edges are always associated with the identity matrix, a graph
// only containing fixed edges is always consistent.

class mygraph {
private:
  hash_map<G_ref *, vector<edge *> > map1;
  vector<vector<pair<G_ref *, edge *> > > changeableEdges;

public:
  void createFixedEdge(G_ref *r1, G_ref *r2) {
    map1[r1].push_back(new edge(r2));
    map1[r2].push_back(new edge(r1));
  }


  hash_map<G_ref *, G_matrix> getOutput(G_ref *initial) {
    hash_map<G_ref *, G_matrix> m; //will be returned
    //walks through the graph via bfs and multiplies the matrices through

    hash_set<G_ref *> visited;
    vector<pair<G_ref *, G_matrix> > toVisit;

    toVisit.push_back(make_pair(initial, G_matrix()));
    visited.insert(initial);

    int i = 0;
    while(i < (int)toVisit.size()) {

      int j = 0;
      if(toVisit[i].first->getParents().size() == 0 &&
	 toVisit[i].second * G_point(0, 0) == G_point(0, 0)) {
	if((toVisit[i].first->getType() & G_TEXT) == 0) m[toVisit[i].first] = toVisit[i].second;
      }

      while(j < (int)(map1[toVisit[i].first].size())) {
	edge *e = (map1[toVisit[i].first])[j];

	if(visited.count(e->ref) == 0) {
	  visited.insert(e->ref);

	  toVisit.push_back(make_pair(e->ref, e->getMatrix() * toVisit[i].second));
	}

	j++;
      }

      i++;
    }

    return m;
    
  }

  G_matrix getPath(G_ref *r1, G_ref *r2) {
    hash_set<G_ref *> visited;
    vector<pair<G_ref *, G_matrix> > toVisit;

    toVisit.push_back(make_pair(r1, G_matrix()));
    visited.insert(r1);

    if(r1 == r2) return G_matrix();

    int i = 0;
    while(i < (int)toVisit.size()) {
      int j = 0;

      while(j < (int)map1[toVisit[i].first].size()) {
	edge *e = (map1[toVisit[i].first])[j];

	if(e->ref == r2) return e->getMatrix() * toVisit[i].second;

	if(visited.count(e->ref) == 0) {
	  visited.insert(e->ref);

	  toVisit.push_back(make_pair(e->ref, e->getMatrix() * toVisit[i].second));
	}

	j++;
      }

      i++;
    }

    return G_matrix::getNullMatrix();
  }


  bool createChangeableEdge(G_ref *r1, G_ref *r2, G_matrix forw, G_matrix rev, int level) {
    G_matrix m = getPath(r1, r2);
    if(!(m == G_matrix::getNullMatrix())) { //check validity
      if(m.almostEqual(forw) == false) return false;
    }

    if(r1 == r2) return true;

    edge *e1 = new matrixEdge(r2, forw);
    edge *e2 = new matrixEdge(r1, rev);

    map1[r1].push_back(e1);
    map1[r2].push_back(e2);

    if(level >= (int)changeableEdges.size()) changeableEdges.resize(level + 1);
    changeableEdges[level].push_back(make_pair(r1, e1));
    changeableEdges[level].push_back(make_pair(r2, e2));

    return true;
  }

  void clear() { //resets everything:
    map1.clear();
    changeableEdges.clear();
  }

  void clearLevel(int level) {
    int i;

    if(level >= (int)changeableEdges.size()) return;

    for(i = 0; i < (int)changeableEdges[level].size(); ++i) {
      G_ref *r = changeableEdges[level][i].first;
      edge *e = changeableEdges[level][i].second;

      map1[r].erase(find(map1[r].begin(), map1[r].end(), e));
    }

    changeableEdges[level].clear();
  }

};


static mygraph graph1;

bool setupFixedRef(G_ref *ref);
int setupChangeableRef(G_ref *ref, int state, int level);
void setupFixedChangeableRef(G_ref *ref); // for going back in case of timeout

static hash_set<G_ref *> done;
static G_refs changeable;
static G_ref *startRef;

// This function sets up the parts of the graph that
// will definitely not change.  It is linear time.
void BuildReverseFixed(G_ref *ref)
{
  if(done.count(ref) > 0) return; //already done.
  done.insert(ref);

  if(setupFixedRef(ref)) changeable.push_back(ref);

  int i;
  for(i = 0; i < (int)ref->getParents().size(); ++i) {
    BuildReverseFixed(ref->getParents()[i]);
  }
}


// This function tries to set up a consistent set
// of edges among those that can change.  It does
// it with a simple exponential-time (in the
// number of changeable edges) brute-force
// algorithm, but if it finds it is taking too
// long, it reverts to a simpler linear-time
// algorithm which will potentially make more
// objects move than necessary.
void BuildReverseChangeable()
{
  int i;

  bool timeOut = false;

  vector<int> states;
  
  QTime t;
  
  t.start();

  i = 0;
  while(i < (int)changeable.size()) {
    if(t.elapsed() > 1000) { //TIMEOUT!  go back to fixed algorithm.
      timeOut = true;
      break;
    }

    if((int)states.size() <= i) states.push_back(0);
    
    int result = setupChangeableRef(changeable[i], states[i], i);
    
    if(result) {
      states[i] = result;
      i++;
    }
    else {
      graph1.clearLevel(i);
      states.pop_back();
      i--;
    }
  }

  if(!timeOut) return;

  while(i >= 0) {
    graph1.clearLevel(i);
    i--;
  }

  for(i = 0; i < (int)changeable.size(); ++i) {
    setupFixedChangeableRef(changeable[i]);
  }
}


// This is the main function.
// It first sets up the part of the graph
// that's fixed, then the part of the graph
// that's variable, taking care of special
// cases, such as constrained points and text
// objects in the process.
hash_map<G_ref *, G_matrix> BuildReverse(const G_refs &refs)
{
  int i;

  done.clear();
  graph1.clear();
  changeable.clear();

  if(refs.size() == 0) return hash_map<G_ref *, G_matrix>();

  //check if the input consists of a single constrained point
  if(refs.size() == 1 && refs[0]->getType() == G_POINT &&
     refs[0]->getDescendType() == G_CONSTRAINED_POINT) {
    hash_map<G_ref *, G_matrix> out;
    out[refs[0]] = G_matrix();
    return out;
  }

  G_refs texts, refs2;

  //separate the texts (which have no impact on the structure)
  //from the refs
  for(i = 0; i < (int)refs.size(); ++i) {
    if(refs[i]->getType() & G_TEXT) {
      texts.push_back(refs[i]);
    }
    else {
      refs2.push_back(refs[i]);
    }
  }

  if(refs2.size() > 0) {
    startRef = refs2[0];

    for(i = 0; i < (int)refs2.size() - 1; ++i) {
      graph1.createFixedEdge(refs2[i], refs2[i + 1]);
    }
    
    for(i = 0; i < (int)refs2.size(); ++i) {
      BuildReverseFixed(refs2[i]);
    }

    BuildReverseChangeable();
    
    hash_map<G_ref *, G_matrix> toBeReturned = graph1.getOutput(refs2[0]);
    
    for(i = 0; i < (int)texts.size(); ++i) {
      toBeReturned[texts[i]] = G_matrix();
    }
    
    return toBeReturned;
  }

  //else if only texts are present

  hash_map<G_ref *, G_matrix> toBeReturned;

  for(i = 0; i < (int)texts.size(); ++i) {
    toBeReturned[texts[i]] = G_matrix();
  }
    
  return toBeReturned;
}



bool setupFixedRef(G_ref *ref) //returns whether there are changeable edges for it
{
  if(ref->getParents().size() == 0) return false;
  
  if(ref->getType() & G_GEOMETRIC && IS_TRANSFORM(ref->getDescendType())) {
    if(ref->getDescendType() == G_TRANSLATED) {
      graph1.createFixedEdge(ref, ref->getParents()[0]);
      graph1.createFixedEdge(ref->getParents()[1], ref->getParents()[2]);
    }
    else if(ref->getDescendType() == G_ROTATED) {
      if(ref->getParents().count() == 5) { //by three points
	graph1.createFixedEdge(ref->getParents()[2], ref->getParents()[3]);
	graph1.createFixedEdge(ref->getParents()[2], ref->getParents()[4]);
      }

      return true;
    }
    else if(ref->getDescendType() == G_SCALED) {
      return true;
    }
    else if(ref->getDescendType() == G_REFLECTED) {
      return true;
    }
  }
  else if(ref->getType() == G_CIRCLE && ref->getDescendType() == G_CENTERRADIUS_CIRCLE) {
    graph1.createFixedEdge(ref, ref->getParents()[0]);
  }
  else if(ref->getType() == G_LINE && (ref->getDescendType() == G_PARALLEL_LINE ||
				       ref->getDescendType() == G_PERPENDICULAR_LINE)) {
    graph1.createFixedEdge(ref, ref->getParents()[0]);
  }
  else if(ref->getType() == G_LOCUS) {
    graph1.createFixedEdge(ref, ref->getParents()[0]);
    graph1.createFixedEdge(ref, ref->getParents()[1]);
  }
  else if(ref->getType() & G_TEXT) {
    int i;

    if(ref->getType() == G_MEASURE &&
       (ref->getDescendType() == G_DISTANCE_MEASURE ||
	ref->getDescendType() == G_ANGLE_MEASURE)) { //these have dependent parents
      for(i = 0; i < (int)ref->getParents().size() - 1; ++i) {
	graph1.createFixedEdge(ref->getParents()[i + 1], ref->getParents()[i]);
      }
    }
  }
  else {
    int i;

    for(i = 0; i < (int)ref->getParents().size(); ++i) {
      graph1.createFixedEdge(ref, ref->getParents()[i]);
    }
  }

  return false;
}


void setupFixedChangeableRef(G_ref *ref)
{
  //this works for all--rotated, translated, scaled.
  graph1.createFixedEdge(ref, ref->getParents()[0]);
  graph1.createFixedEdge(ref, ref->getParents()[1]);
}


int setupChangeableRef(G_ref *ref, int state, int level)
{
  bool success;

  graph1.clearLevel(level);
  
  if(ref->getDescendType() == G_ROTATED) {

    if(state < 1) {
      double angle;

      if(ref->getParents().count() == 5) { //by three points
	angle = (ref->getParents()[4]->getObject()->getPoint() -
		 ref->getParents()[3]->getObject()->getPoint()).angle() -
	  (ref->getParents()[2]->getObject()->getPoint() -
	   ref->getParents()[3]->getObject()->getPoint()).angle();
      }
      else { //value-based
	angle = ref->getParents()[2]->getObject()->getNumValue() / -180. * M_PI;
      }
	
      success = graph1.createChangeableEdge(ref, ref->getParents()[0], G_matrix::rotate(angle),
					    G_matrix::rotate(-angle), level);
      success = success && graph1.createChangeableEdge(startRef, ref->getParents()[1],
						      G_matrix::translate(1,1), G_matrix::translate(-1,-1),
						      level);
      
      if(success) return 1;
      
      graph1.clearLevel(level);
    }

    if(state < 2) {
      success = graph1.createChangeableEdge(ref, ref->getParents()[0], G_matrix(), G_matrix(), level);
      success = success && graph1.createChangeableEdge(ref, ref->getParents()[1],
						       G_matrix(), G_matrix(), level);
      
      if(success) return 2;
      
      graph1.clearLevel(level);
    }
    
    return 0;
  }

  if(ref->getDescendType() == G_SCALED) {
    if(state < 1) {
      double scale;

      if(ref->getParents().count() == 4) { //by two segments
	scale = ref->getParents()[3]->getObject()->getSegment().length() /
	  ref->getParents()[2]->getObject()->getSegment().length();
      }
      else { //value based
	scale = ref->getParents()[2]->getObject()->getNumValue();
      }

      success = graph1.createChangeableEdge(ref, ref->getParents()[0], G_matrix::scale(1. / (scale + SMALL)),
					    G_matrix::rotate(scale), level);
      success = success && graph1.createChangeableEdge(startRef, ref->getParents()[1],
						      G_matrix::translate(1,1), G_matrix::translate(-1,-1),
						      level);
      
      if(success) return 1;
      
      graph1.clearLevel(level);
    }

    if(state < 2) {
      success = graph1.createChangeableEdge(ref, ref->getParents()[0], G_matrix(), G_matrix(), level);
      success = success && graph1.createChangeableEdge(ref, ref->getParents()[1],
						       G_matrix(), G_matrix(), level);
      
      if(success) return 2;
      
      graph1.clearLevel(level);
    }
    
    return 0;
  }

  if(ref->getDescendType() == G_REFLECTED) {
    if(state < 1) {
      G_straight *reflect = ref->getParents()[1]->getObject()->getStraightRef();
      G_matrix m = G_matrix::reflect(reflect->getDirection().getX(),
				     reflect->getDirection().getY());

      success = graph1.createChangeableEdge(ref, ref->getParents()[0], m, m, level);
      success = success && graph1.createChangeableEdge(startRef, ref->getParents()[1],
						      G_matrix::translate(1,1), G_matrix::translate(-1,-1),
						      level);
      
      if(success) return 1;
      
      graph1.clearLevel(level);
    }

    if(state < 2) {
      success = graph1.createChangeableEdge(ref, ref->getParents()[0], G_matrix(), G_matrix(), level);
      success = success && graph1.createChangeableEdge(ref, ref->getParents()[1],
						       G_matrix(), G_matrix(), level);
      
      if(success) return 2;
      
      graph1.clearLevel(level);
    }
    
    return 0;
  }

  return 0;
}
