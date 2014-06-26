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

#include "G_refSearcher.H"


bool G_refSearcher::search(const G_refs &refs)
{
  int i;

  bool flag;

  //first check if any given refs are true or if all given refs are false
  flag = true;
  for(i = 0; i < int(refs.size()); ++i) {
    hash_map<G_ref *, bool>::const_iterator it = known_refs.find(refs[i]);
    if(it != known_refs.end()) {
	if((*it).second == true) return true;
    }
    else flag = false;
  }
  if(flag) return false;

  //now do the recursive search
  for(i = 0; i < int(refs.size()); ++i) {
    hash_map<G_ref *, bool>::const_iterator it = known_refs.find(refs[i]);
  
    if(it != known_refs.end()) { // if we already know about this ref
      if((*it).second == true) return true;
      else continue;
    }

    if(is_found(refs[i])) { //if the current object is what we're looking for, we're done.
      known_refs[refs[i]] = true;
      return true;
    }

    //temporaries have a life at least until the end of the block
    const G_refs &next = search_next(refs[i]);
    bool result;
    
    if(next.size()) result = search(next); //recursively search
    else result = false;
    
    known_refs[refs[i]] = result;

    if(result == true) return true;
  }

  return false;
}

