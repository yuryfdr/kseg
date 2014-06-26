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

#ifndef KSEGPROPERTIES_H
#define KSEGPROPERTIES_H

#include "defs.H"
#include "my_hash_map.H"
#include <qstring.h>

namespace HASH_NAMESPACE {
template <>
struct hash<QString>
{
  size_t operator ()(QString s) const
  {
    unsigned long l = 0;
    unsigned int i;
    for(i = 0; i < s.length(); ++i) {
      l = 5 * l + s[i].unicode();
    }

    return (size_t)l;
  }
};
}

//singleton class
class KSegProperties
{
public:
  static QString getProperty(QString name);
  static void setProperty(QString name, QString value);
  static void setPropertyDefault(QString name, QString value);

private:
  KSegProperties();

  void save();

  QString getProp(QString name);
  void setProp(QString name, QString value);

  hash_map<QString,QString> myMap;

  static KSegProperties *properties;
  static const QString configFile;
};


#endif //KSEGPROPERTIES_H
