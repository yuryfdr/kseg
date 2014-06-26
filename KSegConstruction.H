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

#ifndef KSEGCONSTRUCTION_H
#define KSEGCONSTRUCTION_H

#include <vector>
#include <qfile.h>
#include "KSegDocument.H"

enum ScanType
{
  INVALID,
  UNMATCHED,
  MATCHED
};

class KSegConstruction : public KSegDocument
{
  Q_OBJECT
public:
  friend class KSegRecursionDialog;
  friend class G_undoChangeConstructMode;
  friend QDataStream &operator<<(QDataStream &, KSegDocument &);
  friend QDataStream &operator>>(QDataStream &, KSegDocument &);

  KSegConstruction();
  virtual ~KSegConstruction();

  virtual void addRef(G_ref *ref); //override KSegDocument procedures
  virtual void delRef(G_ref *ref);

  virtual bool isConstruction() { return true; }

  void play(G_refs givens, KSegDocument *doc, int iter = 0, bool first = true);

  bool doesMatch(const G_refs& which); //does "which" match the givens?

  const G_refs &getGiven() { return given; }
  const G_refs &getFinal() { return final; }
  const G_refs &getLoops() { return loops; }
  bool hasLoops() { return loops.size() > 0; }

  static const std::vector<KSegConstruction *>& getAllConstructions()
  { return constructions; }

  //this should only be called by G_undoDelete::execute()
  void insertIntoGiven(G_ref *r, int pos) { given.insert(pos, r); }

  bool canConstructionMakeNormal();
  bool canConstructionMakeGiven();
  bool canConstructionMakeFinal();
  bool canConstructionMakeInitial();
  bool canConstructionRecurse();

  static ScanType scanConstructionFile(QFile &f, const G_refs &selection);
  static bool match(const G_refs& sel, const vector<int>& given, vector<int> &permutation);
  static bool match(const G_refs& sel, const G_refs &given, vector<int> &permutation);

public slots:
  void constructionMakeNormal();
  void constructionMakeGiven();
  void constructionMakeFinal();
  void constructionMakeInitial();
  void constructionRecurse();

protected:
  G_refs given, final, initial, loops;

  bool canMakeNormal(G_ref *which);
  bool canMakeGiven(G_ref *which);
  bool canMakeFinal(G_ref *which);
  bool canMakeInitial(G_ref *which);
  void makeNormal(G_ref *which);
  void makeGiven(G_ref *which, int where = -1);
  void makeFinal(G_ref *which);
  void makeInitial(G_ref *which);

private:
  static std::vector<KSegConstruction *> constructions;
};

#endif //KSEGCONSTRUCTION_H
