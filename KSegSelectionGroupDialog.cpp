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

#include <iostream>

#include <qtooltip.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <QHBoxLayout>
#include <QShowEvent>
#include <QLabel>
#include <QVBoxLayout>

#include "KSegView.H"
#include "KSegDocument.H"
#include "KSegSelectionGroupDialog.H"
#include "KSegSelectionGroup.H"

KSegSelectionGroupDialog::KSegSelectionGroupDialog(KSegView *view,
						   vector<KSegSelectionGroup *> &inGroups,
						   KSegDocument *inDoc)
  : QDialog(view), justMade(true), doc(inDoc), groups(inGroups)
{
  QHBoxLayout *hbl = new QHBoxLayout(this);

  groupLBox = new QListWidget(this);
  groupLBox->setMinimumSize(140, 300);
  hbl->addWidget(groupLBox);

  hbl->addSpacing(8);
  
  QVBoxLayout *vbl = new QVBoxLayout();

  selectBut = new QPushButton(tr("Select"), this);
  deselectBut = new QPushButton(tr("Deselect"), this);
  newBut = new QPushButton(tr("New Group"), this);
  deleteBut = new QPushButton(tr("Delete Group"),this);
  closeBut = new QPushButton(tr("Close"), this);
  affectInvisible = new QCheckBox(tr("Affect\nInvisible\nItems"), this);

  QToolTip::add(selectBut, tr("Hold SHIFT and click to add to current selection"));
  selectBut->installEventFilter(this);

  QToolTip::add(affectInvisible, tr("Whether the \"All <Type>\" groups also (de)select invisible objects"));

  vbl->addSpacing(8);
  vbl->addWidget(selectBut);
  vbl->addWidget(deselectBut);
  vbl->addWidget(newBut);
  vbl->addWidget(deleteBut);
  vbl->addWidget(closeBut);
  vbl->addSpacing(16);

  vbl->addWidget(affectInvisible);

  vbl->addItem(new QSpacerItem(0, 0));
  hbl->addLayout(vbl);
  hbl->addSpacing(8);  

  //now set up items properties

  connect(groupLBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(doc, SIGNAL(documentChanged()), this, SLOT(updateButtons()));

  connect(selectBut, SIGNAL(clicked()), this, SLOT(select()));
  connect(deselectBut, SIGNAL(clicked()), this, SLOT(deselect()));
  connect(newBut, SIGNAL(clicked()), this, SLOT(newGroup()));
  connect(deleteBut, SIGNAL(clicked()), this, SLOT(deleteGroup()));
  connect(closeBut, SIGNAL(clicked()), this, SLOT(close()));

  groupLBox->setSelectionMode(QAbstractItemView::ExtendedSelection);

  groupLBox->addItem("*" + tr("All Points"));
  groupLBox->addItem("*" + tr("All Segments"));
  groupLBox->addItem("*" + tr("All Rays"));
  groupLBox->addItem("*" + tr("All Lines"));
  groupLBox->addItem("*" + tr("All Circles"));
  groupLBox->addItem("*" + tr("All Arcs"));
  groupLBox->addItem("*" + tr("All Polygons"));
  groupLBox->addItem("*" + tr("All Circle Interiors"));
  groupLBox->addItem("*" + tr("All Arc Sectors"));
  groupLBox->addItem("*" + tr("All Arc Segments"));
  groupLBox->addItem("*" + tr("All Loci"));
  groupLBox->addItem("*" + tr("All Measurements"));
  groupLBox->addItem("*" + tr("All Calculations"));

  numFixedGroups = groupLBox->count();

  int i;
  for(i = 0; i < (int)groups.size(); ++i) {
    groupLBox->addItem(" " + groups[i]->getName());
  }
}

void KSegSelectionGroupDialog::showEvent(QShowEvent *)
{
  QWidget *top = ((QWidget *)parent())->topLevelWidget();
  if(justMade) move(max(0, top->x() - frameGeometry().width() - 15), top->y());
  justMade = false;

  updateButtons();
}

void KSegSelectionGroupDialog::updateButtons()
{
  int i;

  bool anySelected = false;
  bool onlyNonFixedSelected = false;

  for(i = 0; i < (int)groupLBox->count(); ++i) {
    if(!groupLBox->item(i)->isSelected()) continue;

    if(i >= numFixedGroups && !anySelected) {
      anySelected = true;
      onlyNonFixedSelected = true;
      break;
    }

    anySelected = true;
  }

  selectBut->setEnabled(anySelected);
  deselectBut->setEnabled(anySelected);
  deleteBut->setEnabled(onlyNonFixedSelected);
  newBut->setEnabled(doc->getSelected().size() > 0);

  for(i = numFixedGroups; i < (int)groupLBox->count(); ++i) {
    QString s = groupLBox->item(i)->text();
    if(groups[i - numFixedGroups]->size() == 0) { //empty
      if(s[0] != '-') {
	s[0] = '-';
	groupLBox->item(i)->setText(s);
      }
    }
    else if(s[0] != ' ') {
      s[0] = ' ';
      groupLBox->item(i)->setText(s);
    }
  }
}

void KSegSelectionGroupDialog::select()
{
  int i;

  int filter = 0;
  bool alsoInvisible = affectInvisible->isChecked();

  for(i = 0; i < numFixedGroups; ++i) {
    if(groupLBox->item(i)->isSelected()) {
      filter |= (1 << i);
    }
  }

  if(!shiftDown) doc->clearSel();

  for(i = 0; i < doc->getNumRefs(); ++i) {
    if(doc->getRef(i)->getType() & filter) {
      if(!alsoInvisible && !doc->getRef(i)->getVisible()) continue;
      doc->addSel(doc->getRef(i));
    }
  }

  for(i = numFixedGroups; i < (int)groupLBox->count(); ++i) {
    if(groupLBox->item(i)->isSelected()) {
      groups[i - numFixedGroups]->selectRefs();
    }
  }

  doc->emitDocumentChanged();

  ((QWidget *)parent())->topLevelWidget()->setActiveWindow();
}

void KSegSelectionGroupDialog::deselect()
{
  int i;

  int filter = 0;
  bool alsoInvisible = affectInvisible->isChecked();

  for(i = 0; i < numFixedGroups; ++i) {
    if(groupLBox->item(i)->isSelected()) {
      filter |= (1 << i);
    }
  }

  for(i = 0; i < doc->getNumRefs(); ++i) {
    if(doc->getRef(i)->getType() & filter) {
      if(!alsoInvisible && !doc->getRef(i)->getVisible()) continue;
      doc->delSel(doc->getRef(i));
    }
  }

  for(i = numFixedGroups; i < (int)groupLBox->count(); ++i) {
    if(groupLBox->item(i)->isSelected()) {
      groups[i - numFixedGroups]->deselectRefs();
    }
  }

  doc->emitDocumentChanged();
  ((QWidget *)parent())->topLevelWidget()->setActiveWindow();
}

void KSegSelectionGroupDialog::newGroup()
{
  if(doc->getSelected().size() == 0) return; //shouldn't be here anyway

  //set up the name query dialog
  QDialog nameDlg(0, 0, true);
  QPushButton *ok, *cancel;
  QLabel *query;
  QLineEdit *edit;
  
  QVBoxLayout vlayout(&nameDlg);

  vlayout.addSpacing(8);

  QHBoxLayout hlayout0;
  vlayout.addLayout(&hlayout0);

  hlayout0.addSpacing(8);
  
  query = new QLabel(tr("Enter new group name:"), &nameDlg);
  hlayout0.addWidget(query);

  hlayout0.addSpacing(8);

  edit = new QLineEdit(&nameDlg);
  hlayout0.addWidget(edit);
  
  hlayout0.addSpacing(8);
  vlayout.addSpacing(8);

  QHBoxLayout hlayout;
  vlayout.addLayout(&hlayout);

  hlayout.addSpacing(8);
  ok = new QPushButton(qApp->translate("KSegDocument", "OK", ""), &nameDlg );
  hlayout.addWidget(ok);
  QObject::connect( ok, SIGNAL(clicked()), &nameDlg, SLOT(accept()) );
  ok->setDefault(true);
  hlayout.addSpacing(8);
  cancel = new QPushButton(qApp->translate("KSegDocument", "Cancel", ""), &nameDlg );
  hlayout.addWidget(cancel);
  QObject::connect( cancel, SIGNAL(clicked()), &nameDlg, SLOT(reject()) );
  hlayout.addSpacing(8);

  vlayout.addSpacing(8);

  //run the dialog
  nameDlg.exec();
  if(nameDlg.result() == QDialog::Rejected) return;

  //create the group
  groups.push_back(new KSegSelectionGroup(doc));
  groups.back()->setName(edit->text());
  
  for(int i = 0; i < (int)doc->getSelected().size(); ++i)
    groups.back()->addRef(doc->getSelected()[i]);

  groupLBox->addItem(" " + edit->text());

  doc->emitDocumentModified();
}

void KSegSelectionGroupDialog::deleteGroup()
{
  int res = QMessageBox::information(this, "kseg", tr("Are you sure you want to delete the selected group(s)?"),
				     QMessageBox::Yes, QMessageBox::No);

  if(res == QMessageBox::No) return;

  int i;
  for(i = 0; i < (int)groupLBox->count(); ++i) {
    if(!groupLBox->item(i)->isSelected()) continue;

    if(i < numFixedGroups) continue; //this really shouldn't happen, but...

    groups.erase(groups.begin() + (i - numFixedGroups));
    //groupLBox->removeItemWidget(groupLBox->item(i));
    groupLBox->model()->removeRow(i);
    i--;
  }

  doc->emitDocumentModified();
}


