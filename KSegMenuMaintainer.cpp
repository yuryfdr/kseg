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

#include "KSegMenuMaintainer.H"
#include "KSegWindow.H"
#include "KSegView.H"
#include <QMenu>

int KSegCommandInfo::curId = 1;


KSegMenuMaintainer::~KSegMenuMaintainer()
{
  int i;
  
  for(i = 0; i < (int)subMaintainers.size(); ++i) {
    delete subMaintainers[i];
  }

  for(i = 0; i < (int)commands.size(); ++i) {
    delete commands[i];
  }

  if(myInfo != NULL){ delete menu; delete myInfo;}//if we created our own menu.
  if(toolBar != NULL) { delete toolBar; }
}


void KSegMenuMaintainer::add(KSegMenuMaintainer *m)
{
  menu->insertItem(m->myInfo->menuText(window), m->menu, m->myInfo->getID());
  if(!m->updateOften()) QObject::connect(m->menu, SIGNAL(aboutToShow()), m, SLOT(update()));
  subMaintainers.push_back(m);
}
void KSegMainMenuMaintainer::add(KSegMenuMaintainer *m)
{
  menuBar->insertItem(m->myInfo->menuText(window), m->menu, m->myInfo->getID());
  if(!m->updateOften()) QObject::connect(m->menu, SIGNAL(aboutToShow()), m, SLOT(update()));
  subMaintainers.push_back(m);
}


void KSegMenuMaintainer::add(KSegCommandInfo *i)
{
    i->addTo(window, menu);
    if(toolBar != NULL) i->addTo(window, toolBar);
    commands.push_back(i);
}

void KSegMenuMaintainer::update()
{
  int i;

  for(i = 0; i < (int)subMaintainers.size(); ++i) {
    if(subMaintainers[i]->updateOften()) subMaintainers[i]->update();
    KSegCommandInfo *info = subMaintainers[i]->myInfo;
    int id = info->getID();

    menu->setItemEnabled(id, info->isEnabled(window));
    if(info->isChecked(window)!=-1)menu->setItemChecked(id, info->isChecked(window) == 1);

    QString txt = info->menuText(window);
    if(!txt.isNull() && txt != menu->text(id)) menu->changeItem(id, txt);
  }

  for(i = 0; i < (int)commands.size(); ++i) {
    int id = commands[i]->getID();

    menu->setItemEnabled(id, commands[i]->isEnabled(window));
    if(commands[i]->isChecked(window)!=-1)menu->setItemChecked(id, commands[i]->isChecked(window) == 1);

    QString txt = commands[i]->menuText(window);
    if(txt != menu->text(id)) menu->changeItem(id, txt);

    //tool button updates here
    if(toolBar == NULL) continue;
    QToolButton *tb = commands[i]->getToolButton();
    if(tb == NULL) continue;
    tb->setEnabled(commands[i]->isEnabled(window));
    tb->setToggleButton(commands[i]->isChecked(window) != -1);
    tb->setOn(commands[i]->isChecked(window) == 1);
    tb->setTextLabel(commands[i]->menuText(window).replace(QRegExp("&"), ""));
    tb->setIconSet(commands[i]->iconSet(window));
  }
}
void KSegMainMenuMaintainer::update()
{
  int i;

  for(i = 0; i < (int)subMaintainers.size(); ++i) {
    if(subMaintainers[i]->updateOften()) subMaintainers[i]->update();
    KSegCommandInfo *info = subMaintainers[i]->myInfo;
    int id = info->getID();

    menuBar->setItemEnabled(id, info->isEnabled(window));
    if(info->isChecked(window)!=-1)menuBar->setItemChecked(id, info->isChecked(window) == 1);

    QString txt = info->menuText(window);
    if(!txt.isNull() && txt != menuBar->text(id)) menuBar->changeItem(id, txt);
  }

  for(i = 0; i < (int)commands.size(); ++i) {
    int id = commands[i]->getID();

    menuBar->setItemEnabled(id, commands[i]->isEnabled(window));
    if(commands[i]->isChecked(window)!=-1)menuBar->setItemChecked(id, commands[i]->isChecked(window) == 1);

    QString txt = commands[i]->menuText(window);
    if(txt != menuBar->text(id)) menuBar->changeItem(id, txt);

    //tool button updates here
    if(toolBar == NULL) continue;
    QToolButton *tb = commands[i]->getToolButton();
    if(tb == NULL) continue;
    tb->setEnabled(commands[i]->isEnabled(window));
    tb->setToggleButton(commands[i]->isChecked(window) != -1);
    tb->setOn(commands[i]->isChecked(window) == 1);
    tb->setTextLabel(commands[i]->menuText(window).replace(QRegExp("&"), ""));
    tb->setIconSet(commands[i]->iconSet(window));
  }
}

bool KSegCommandInfo::isEnabled(KSegWindow *win) {
  if(win->getView()->getDrag() != KSegView::NO_DRAG || win->getView()->getMenusEnabled() == false)
    return false;
  
  return privateIsEnabled(win);
}


