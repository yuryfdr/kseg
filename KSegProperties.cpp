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

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

#include "KSegProperties.H"


KSegProperties *KSegProperties::properties = NULL;
const QString KSegProperties::configFile = QDir::homeDirPath() + "/.kseg";


QString KSegProperties::getProperty(QString name)
{
  if(properties == NULL) properties = new KSegProperties();
  return properties->getProp(name);
}


void KSegProperties::setProperty(QString name, QString value)
{
  if(properties == NULL) properties = new KSegProperties();
  properties->setProp(name, value);
}


void KSegProperties::setPropertyDefault(QString name, QString value)
{
  if(getProperty(name).isNull())
    properties->setProp(name, value); //properties must already be created at this point
}


//private member functions


KSegProperties::KSegProperties()
{
  QFile f(configFile);

  if(!f.open(QIODevice::ReadOnly)) return;
  
  QTextStream ts(&f);
  
  QString curLine;


  while(true) {
    curLine = ts.readLine();
    if(curLine.isNull()) break;

    if(curLine.length() < 2) continue; //skip (almost) empty lines
    if(curLine[0] == '#') continue; //comment line

    int ind = curLine.find('=');
    if(ind == -1) continue;

    QString key = curLine.left(ind).simplifyWhiteSpace();
    QString value = curLine.right(curLine.length() - ind - 1).simplifyWhiteSpace();

    myMap[key] = value;
  }
}


QString KSegProperties::getProp(QString name)
{
  if(myMap.count(name) == 0) return QString();
  return myMap[name];
}


void KSegProperties::setProp(QString name, QString value)
{
  if(myMap.count(name) != 0 && myMap[name] == value) return;
  if(value.isNull()) myMap.erase(name);
  else myMap[name] = value;

  save();
}


void KSegProperties::save()
{
  QFile f(configFile);

  if(!f.open(QIODevice::WriteOnly)) return;
  
  QTextStream ts(&f);

  hash_map<QString, QString>::iterator it;

  ts << "# KSeg configuration file--automatically generated in random order :)\n";

  for(it = myMap.begin(); it != myMap.end(); ++it) {
    ts << it->first << " = " << it->second << "\n";
  }

  f.close();
}
