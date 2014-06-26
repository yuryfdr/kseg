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


#ifndef KSEGPREFERENCESDIALOG_H
#define KSEGPREFERENCESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpixmap.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>

class KSegPreferencesDialog : public QDialog
{ 
  Q_OBJECT
  
public:
  KSegPreferencesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
  ~KSegPreferencesDialog();
  
  QLabel* BackgroundLabel;
  QPushButton* BackgroundColorButton;
  QLabel* ForegroundLabel;
  QPushButton* ForegroundColorButton;
  QButtonGroup* SelectionButtonGroup;
  QRadioButton* BorderSelectButton;
  QRadioButton* BlinkingSelectButton;
  QPushButton* buttonOk;
  QPushButton* buttonCancel;
  
  
protected slots:
  virtual void init();
  virtual void destroy();

  virtual void accept();

  virtual void foregroundButtonClicked();
  virtual void backgroundButtonClicked();
  virtual void selectButtonClicked();

protected:
  QVBoxLayout* KSegPreferencesDialogLayout;
  QHBoxLayout* Layout1;
  QHBoxLayout* Layout2;
  QHBoxLayout* Layout3;
  QHBoxLayout* SelectionButtonGroupLayout;
  QHBoxLayout* Layout4;

  QPixmap buttonPM;
  QColor foregroundColorChosen;
  QColor backgroundColorChosen;
  bool blinkingSelectChosen;

  void updateButtons();
};


#endif // KSEGPREFERENCESDIALOG_H
