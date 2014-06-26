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

#ifndef KSEGMENUMAINTAINER_H
#define KSEGMENUMAINTAINER_H

#include "defs.H"
#include <vector>
#include <iostream>

#include <qobject.h>
#include <QMenu>
#include <QToolBar>
#include <qtoolbutton.h>
#include <qregexp.h>
#include <qmenubar.h>

class KSegWindow;

//-------------------CommandInfo class

class KSegCommandInfo //: public Qt
{
public:
  KSegCommandInfo() { id = ++curId; toolButton = NULL; }
  virtual ~KSegCommandInfo() {}

  virtual int getID() { return id; }

  bool isEnabled(KSegWindow *win);
  virtual int isChecked(KSegWindow *win) { return -1; } //uncheckable by default
  virtual QString menuText(KSegWindow *win) { return QString(); }
  virtual QIcon iconSet(KSegWindow *win) { return QIcon(); }
  virtual QObject *receiver(KSegWindow *win) { return NULL; }
  virtual const char *member() { return NULL; }
  virtual int shortCutKey() { return 0; } // no shortcut by default
  
  virtual void addTo(KSegWindow *win, QMenu *menu)
  { menu->insertItem(menuText(win), receiver(win), member(), shortCutKey(), getID()); }

  virtual void addTo(KSegWindow *win, QToolBar *toolBar)
  { 
    QIcon is = iconSet(win);
    if(is.pixmap().isNull()) { toolButton = NULL; return; }
    toolButton = new QToolButton(is, menuText(win).replace(QRegExp("&"), ""), "", receiver(win), member(), toolBar);
    toolBar->addWidget(toolButton);
  }

  virtual QToolButton *getToolButton() { return toolButton; }

  static void resetID() { curId = 1; }

protected:
  virtual bool privateIsEnabled(KSegWindow *win) { return true; }

  int id;
  QToolButton *toolButton;

private:
  static int curId;
};


//--------------------MenuMaintainer class


class KSegMenuMaintainer : public QObject
{
  Q_OBJECT
public:
  KSegMenuMaintainer(KSegWindow *win) : window(win), toolBar(NULL) { }
  virtual ~KSegMenuMaintainer();

public slots:
  virtual void update();

protected:
  virtual void add(KSegMenuMaintainer *m);
  virtual void add(KSegCommandInfo *i);

public:
  virtual bool updateOften() { return true; }
protected:
  virtual void addSep() { menu->insertSeparator(); if(toolBar != NULL) toolBar->addSeparator(); }
  void addNonToolbarSep() { menu->insertSeparator(); }

public:
  QMenu *menu;
  KSegCommandInfo *myInfo;
  KSegWindow *window;
  QToolBar *toolBar;
  vector<KSegMenuMaintainer *> subMaintainers;
  vector<KSegCommandInfo *> commands;

  virtual void privateToolbarVisibilityChanged(bool visible) {}

protected slots:
  virtual void toolbarVisibilityChanged(bool visible) { privateToolbarVisibilityChanged(visible); }

};


//---------------------MainMenuMaintainer

class KSegMainMenuMaintainer : public KSegMenuMaintainer
{
  Q_OBJECT
public:
  KSegMainMenuMaintainer(KSegWindow *win);
public slots:
  virtual void update();
protected:
  virtual void addSep() { menuBar->insertSeparator(); if(toolBar != NULL) toolBar->addSeparator(); }
  QMenuBar* menuBar;
  virtual void add(KSegMenuMaintainer *m);
};


#endif //KSEGMENUMAINTAINER_H

