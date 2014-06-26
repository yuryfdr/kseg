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


#ifndef G_UNDO_H
#define G_UNDO_H

class G_ref;
class KSegDocument;

#include "G_point.H"
#include "G_refs.H"

//----------------
class G_undo
{
public:
  G_undo(G_ref *inRef) { ref = inRef; lastOne = false; }
  virtual ~G_undo() {}

  void setLastOne(bool last) { lastOne = last; }
  bool isLastOne() const { return lastOne; }

  virtual void execute() {} //undoes the action
  virtual bool isReconstrainUndo() { return false; }

protected:
  G_ref *ref; // the affected object

  KSegDocument *doc() const; //returns the document based on ref->doc

private:
  G_undo() {}
  bool lastOne;

};

//----------------
class G_undoCreate : public G_undo
{
public:
  G_undoCreate(G_ref *ref) : G_undo(ref) {}

  virtual void execute();

};

//----------------
class G_undoDelete : public G_undo
{
public:
  G_undoDelete(G_ref *ref);
  virtual ~G_undoDelete();

  virtual void execute();
protected:
  QByteArray objectData;
};

//----------------
class G_undoShowHide : public G_undo
{
public:
  G_undoShowHide(G_ref *ref) : G_undo(ref) {}

  virtual void execute();  
};

//------------------
class G_undoChangeLabel : public G_undo
{
public:
  G_undoChangeLabel(G_ref *ref);

  virtual void execute();
private:
  QString old;
};

//-------------------
class G_undoReconstrain : public G_undo
{
public:
  G_undoReconstrain(G_ref *ref);

  virtual void execute();
  virtual bool isReconstrainUndo() { return true; }

  bool isChanged(G_ref *r);
  G_point getOldPos() const { return oldPos; }
private:
  G_refs oldParents;
  int oldType;
  G_point oldPos;
};

//-------------------
class G_undoChangeCalculation : public G_undo
{
public:
  G_undoChangeCalculation(G_ref *ref);

  virtual void execute();

private:
  G_refs oldParents;
  QString oldFormula;
};

//------------------
class G_undoChangeDrawstyle : public G_undo
{
public:
  G_undoChangeDrawstyle(G_ref *ref);

  virtual void execute();
private:
  G_drawstyle *old;
};

//------------------
class G_undoChangeConstructMode : public G_undo
{
public:
  G_undoChangeConstructMode(G_ref *ref);

  virtual void execute();
private:
  bool oldGiven, oldFinal;
  int whereInGivens;
};

#endif //G_UNDO_H
