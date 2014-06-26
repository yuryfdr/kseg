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


#ifndef KSEG_WINDOW_H
#define KSEG_WINDOW_H

#include "defs.H"
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QFile>
#include <QKeyEvent>
#include <QCloseEvent>
#include <vector>

class KSegView;
class KSegDocument;
class KSegMainMenuMaintainer;

class KSegWindow : public QMainWindow
{
  Q_OBJECT
public:
  KSegWindow(KSegDocument *doc = 0); //new view for the document
  virtual ~KSegWindow();

  KSegView *getView() const { return view; }

  static bool openFile(QString fileName, KSegWindow *win = NULL); //returns true if successful

  static void compressRecentList();
  static vector<QString> getValidConstructionFiles();

public slots:
  void newSketch();
  void newConstruction();
  void newView();
  void statusBarMessage(const QString &);

  void updateMenus();

protected slots:

  void fileSaveAs();
  void fileSave();
  void fileOpen();
  void fileOpenInNewWindow();
  void fileCopyAsConstruction();
  void fileToSVG();
  void filePrint();
  void fileExportToImage();
  void fileChooseLanguage();
  
  void fileOpenRecent(int);

  void editColorBlack();
  void editColorGray();
  void editColorRed();
  void editColorGreen();
  void editColorBlue();
  void editColorYellow();
  void editColorMagenta();
  void editColorCyan();
  void editColorOther();

  void editPointstyleLargecircle();
  void editPointstyleMediumcircle();
  void editPointstyleSmallcircle();

  void editLinestyleSolid();
  void editLinestyleDashed();
  void editLinestyleDotted();
  void editLinestyleThin();
  void editLinestyleNormal();
  void editLinestyleThick();

  void editFont(int size);
  void editFontFont();

  void editPreferences();

  void quickPlay(int);
  void quickPlaySetDirectory();

  void help();
  void helpAbout();

protected:
  void keyPressEvent(QKeyEvent *ev);
  void keyReleaseEvent(QKeyEvent *ev);
  void closeEvent(QCloseEvent *ev);

private:
  void changeDocument(KSegDocument *doc);

  bool closingDocument();

  static QString quickPlayDirectory;

  bool openInNewWindow;

  KSegView *view;

  KSegMainMenuMaintainer *maintainer;
};

#endif //KSEG_WINDOW_H
