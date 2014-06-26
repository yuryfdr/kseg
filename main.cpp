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
#include <qapplication.h>
#include <QTranslator>
#include <time.h>
#include "KSegWindow.H"
#include "G_ref.H"
#include "KSegProperties.H"

int main(int argc, char **argv)
{
  srand((long int)time(NULL));

  QApplication a(argc, argv, "kseg");
  QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

  //set some default properties
  KSegProperties::setPropertyDefault("Language", "");
  KSegProperties::setPropertyDefault("LanguageDir", ".");
  KSegProperties::setPropertyDefault("SelectType", "BorderSelect");
  KSegProperties::setPropertyDefault("QuickPlayDirectory", ".");
  KSegProperties::setPropertyDefault("RecentListSize", "4");
  KSegProperties::setPropertyDefault("BackgroundColor", QColor(Qt::white).name());
  KSegProperties::setPropertyDefault("ForegroundColor", QColor(Qt::black).name());
  KSegProperties::setPropertyDefault("ViewToolbarVisible", "true");
  KSegProperties::setPropertyDefault("NewToolbarVisible", "true");
  KSegProperties::setPropertyDefault("MeasureToolbarVisible", "true");
  KSegProperties::setPropertyDefault("TransformToolbarVisible", "true");

  //load the default language
  QTranslator translator(0);
  translator.load(KSegProperties::getProperty("Language"),
		  KSegProperties::getProperty("LanguageDir"));
  a.installTranslator(&translator);

  //open the files, if any were specified
  bool anyOpenSuccessful = false;
  if(argc > 1) {
    int i;

    for(i = 1; i < argc; ++i) {
      anyOpenSuccessful |= KSegWindow::openFile(QString(argv[i]));
    }
  }
  if(!anyOpenSuccessful) {
    KSegWindow *w = new KSegWindow();
    
    w->show();
  }

  return a.exec();
}
