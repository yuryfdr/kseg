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


#include <stdio.h>

#include "G_object.H"
#include "G_undo.H"
#include "KSegDocument.H"


class CalculationReferenceFetcher : public ReferenceFetcher
{
public:
  CalculationReferenceFetcher(G_ref *inRef) { ref = inRef; }

  double getValue(int reference_number)
  { return ref->getParents()[reference_number]->getObject()->getNumValue(); }

  KFormula *getFormula(int reference_number)
  { return ((G_valueObject *)(ref->getParents()[reference_number]->getObject()))->getLhs(); }

protected:
  G_ref * ref;
};


G_calculateObject::G_calculateObject(G_ref *inWhere)
  :G_valueObject(inWhere, true)
{
  lhs.setReferenceFetcher(new CalculationReferenceFetcher(where));
}


void G_calculateObject::changeFormula(const G_refs& newParents, const QString &newFormulaString)
{
  if(where->getParents() == newParents && newFormulaString == formulaString) return;

  where->getDocument()->addUndo(new G_undoChangeCalculation(where));

  where->reconstrain(G_REGULAR_CALCULATE, newParents);
  formulaString = newFormulaString;

  dirtylhs = true;

  update();
}


void G_calculateObject::calculate()
{
  //first check to make sure its parents are defined
  unsigned int i;

  for(i = 0; i < where->getParents().count(); ++i) {
    if(fabs(parent(i)->getNumValue()) >= BIG) {
      value = BIG * 10;
      return;
    }
  }

  lhs.setReferenceFetcher(new CalculationReferenceFetcher(where));

  int error;

  value = lhs.evaluate(QMap<QString,double>(), &error);

  if(error != NO_ERROR) value = BIG * 10;
}

void G_calculateObject::convertFormulaStringOnLoad()
{
  int i;
  
  for(i = 0; i < (int)formulaString.length(); ++i) {
    if(formulaString[i].unicode() > OLD_UNUSED_OFFSET && formulaString[i].unicode() < OLD_UNUSED_OFFSET + 300)
      formulaString[i] = QChar(formulaString[i].unicode() + (UNUSED_OFFSET - OLD_UNUSED_OFFSET));
  }
}

QString G_calculateObject::convertFormulaStringOnSave(QString tmp) const
{
  for(int i = 0; i < (int)tmp.length(); ++i)
    if(tmp[i].unicode() > UNUSED_OFFSET &&
       tmp[i].unicode() < UNUSED_OFFSET + 300)
      tmp[i] = QChar((int)(tmp[i].unicode()) +
		     (OLD_UNUSED_OFFSET - UNUSED_OFFSET));

  return tmp;
}
