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


#include "KSegRecursionDialog.H"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "G_refSearcher.H"

int KSegRecursionDialog::lastOutput = 0;

KSegRecursionDialog::KSegRecursionDialog(KSegConstruction *construction)
  : QDialog(0, 0, true), c(construction)
{
  QVBoxLayout *vbl = new QVBoxLayout(this);
  vbl->addSpacing(10);

  label = new QLabel(this);

  QHBoxLayout *hbl0 = new QHBoxLayout(vbl);
  
  hbl0->addSpacing(10);
  hbl0->addWidget(new QLabel(tr("Enter recursion depth:"), this));
  hbl0->addSpacing(10);
  spinBox = new QSpinBox(0, 10000, 1, this);
  spinBox->setValue(lastOutput);
  connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(spinboxChanged(int)));
  hbl0->addWidget(spinBox);
  hbl0->addSpacing(10);

  vbl->addSpacing(10);
  QHBoxLayout *hbl1 = new QHBoxLayout(vbl);
  hbl1->addSpacing(10);
  hbl1->addWidget(label);
  hbl1->addSpacing(10);
  vbl->addSpacing(10);

  QHBoxLayout *hbl = new QHBoxLayout(vbl);

  ok = new QPushButton(tr("OK"), this );
  hbl->addSpacing(10);
  hbl->addWidget(ok);
  QObject::connect( ok, SIGNAL(clicked()), this, SLOT(accept()) );
  ok->setDefault(true);
  cancel = new QPushButton(tr("Cancel"), this );
  hbl->addSpacing(10);
  hbl->addWidget(cancel);
  hbl->addSpacing(10);
  QObject::connect( cancel, SIGNAL(clicked()), this, SLOT(reject()) );

  vbl->addSpacing(10);

  resize(20, 20); //set to minimum size.

  //now find finalObjects and objectsConstructedAtEachLevel:
  FinalCounter fc;
  fc.search(c->final);
  finalObjects = fc.count;
  initialObjects = 0;

  int i, nicObjects = 0;
  NotInConstructionSearcher nicSearch;
  ImplicitInitialSearcher iiSearch;
  for(i = 0; i < int(c->allRefs.size()); ++i) {
    if(nicSearch.search(c->allRefs[i]) == false) nicObjects++;
    if(iiSearch.search(c->allRefs[i]) == false) initialObjects++;
  }

  objectsConstructedAtEachLevel = c->allRefs.size() - nicObjects -
    finalObjects - initialObjects - c->loops.size();
  //hope I didn't forget to subtract anything :)

  spinboxChanged(lastOutput);
}


void KSegRecursionDialog::spinboxChanged(int value)
{
  QString outText;

  int num = getNumberOfObjects(value);
  if(num == -1) {
    ok->setEnabled(false);
    outText = tr("Will produce too many objects.");
  }
  else {
    ok->setEnabled(true);
    outText = tr("Will produce %1 objects.");
    outText = outText.arg(QString::number(num));
  }

  label->setText(outText);
}


//because of combinatorial explosion, we must be damn careful here
//to prevent overflow.  Hence the function is so long.
int KSegRecursionDialog::getNumberOfObjects(int depth)
{
  //if this is so, way too many objects will be produced
  if(c->loops.size() > 1 && depth > 30 && objectsConstructedAtEachLevel + finalObjects > 0)
    return -1;

  //first tackle an easy case:
  if(c->loops.size() == 1) {
    double retval = depth + 1; //use a double to prevent overflow
    retval *= objectsConstructedAtEachLevel;
    retval += finalObjects + initialObjects;
    if(retval < MAXCONSTRUCTOBJECTS) return int(retval + 0.5);
    else return -1;
  }

  if(depth == 0) {
    int retval = int(finalObjects + objectsConstructedAtEachLevel +
		     initialObjects + 0.5);
    if(retval < MAXCONSTRUCTOBJECTS) return retval;
    else return -1;
  }

  //do a check with logs to prevent overflow
  double log_ocael = log(objectsConstructedAtEachLevel + 0.0000001);
  double log_final = log(finalObjects + 0.0000001);

  //total number of levels is
  // (loops^(depth + 1) - 1) / (loops - 1), approximately loops^(depth+1) / (loops - 1)
  //the logarithm of that is (depth+1)*log(loops) - log(loops - 1);
  double log_levels = double(depth + 1) * log(double(c->loops.size())) -
    log(double(c->loops.size()) - 1);

  //total number of bottom levels is loops^(depth)
  //the log of that is (depth)*log(loops);
  double log_bottomlevels = double(depth) * log(double(c->loops.size()));

  double log_max = QMAX(log_levels + log_ocael, log_bottomlevels + log_final) + log(2.);

  if(log_max > log(double(MAXCONSTRUCTOBJECTS) * 10.)) return -1;

  //now that the checks all passed, we can use the full formula
  double num_obj = (pow(double(c->loops.size()), depth + 1) - 1.) /
    (double(c->loops.size()) - 1.) * objectsConstructedAtEachLevel +
    pow(double(c->loops.size()), depth) * finalObjects + initialObjects;
    
  if(int(num_obj + 0.5) > MAXCONSTRUCTOBJECTS) return -1;

  return int(num_obj + 0.5);
}



