/*
 *  KSeg/formula
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


#ifndef MATRIXDIALOG_H_INCLUDED
#define MATRIXDIALOG_H_INCLUDED

#include <qwidget.h>
#include <qdialog.h>
#include <qvalidator.h>

/**
 * Dialog for entering matrix sizes.
 * @internal
 * @author Ilya Baran (ibaran@mit.edu)
 * @version $Id: MatrixDialog.H,v 1.2 2000/12/30 07:07:37 ibaran Exp $
 */
class MatrixDialog : public QDialog
{
  Q_OBJECT

public:

  int w;
  int h;
  bool zeroFill;

  MatrixDialog(QWidget *parent);

  ~MatrixDialog() {}

protected slots:
  void setWidth(int value);
  void setHeight(int value);
  void setFill(bool value);

};

#endif // MATRIXDIALOG_H_INCLUDED
