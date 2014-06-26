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


#ifndef KSEGCALCULATEEDITOR_H
#define KSEGCALCULATEEDITOR_H

#include <qstring.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <QPixmap>
#include <QEvent>
#include <QHBoxLayout>

#include "formula/kformulaedit.H"
#include "G_refs.H"
#include "KSegView.H"
#include "G_refSearcher.H"


class KSegCalculateEditor : public QDialog
{
Q_OBJECT

public:
  KSegCalculateEditor(KSegView *view, QString initialString = QString(),
		      G_refs initialParents = G_refs(), G_ref *inCurRef = 0);

  virtual ~KSegCalculateEditor();

  QString getOutputString() { return outputString; }
  const G_refs &getOutputParents() { return outputParents; } 

  void run();

protected slots:
  void OKClicked();
  void reject() { finished = true; QDialog::reject(); }
  void accept() { finished = true; QDialog::accept(); }
  void formulaChanged(const QString &newText);

protected:
  void removeParent(int i);
  void updateValue();
  void insertReference(G_ref *r);

  bool eventFilter(QObject *, QEvent *e);

  G_refs outputParents;
  QString outputString;
  KFormulaEdit *editor;
  QPushButton *ok;
  QLabel *valueDisplay;
  G_ref *curRef;
  
  DescendantSearcher descendantSearcher;

  bool finished;
};


class EditorButton : public QToolButton
{
  Q_OBJECT
public:
  EditorButton(QPixmap &p, QWidget *parent=0L, int charValue = 0)
    : QToolButton(parent), value(charValue)
  {
    setPixmap(p);
    connect(this, SIGNAL(clicked()), this, SLOT(gotClick()));
  }
  	
signals:
  void valueClicked(int);

private slots:
  void gotClick() { emit valueClicked(value); }

private:
  int value;
};


class EditorToolBar : public QWidget
{
  Q_OBJECT
public:
  EditorToolBar(QWidget *parent=0L, const char *name=0L);
  
  void connectToFormula(KFormulaEdit *inFormula);

private:
  QHBoxLayout *layout;
  KFormulaEdit *formula;
  void insertButton(QPixmap &icon, int key, QString toolTip);
};


#endif // KSEGCALCULATEEDITOR_H
