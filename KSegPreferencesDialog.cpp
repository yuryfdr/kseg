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


#include <qvariant.h>   // first for gcc 2.7.2
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qcolordialog.h>
#include <QGroupBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "KSegPreferencesDialog.H"
#include "KSegProperties.H"
#include "KSegView.H"

KSegPreferencesDialog::KSegPreferencesDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KSegPreferencesDialog" );
    resize( 298, 200 ); 
    setCaption( tr( "KSEG Preferences" ) );
    setSizeGripEnabled( TRUE );
    KSegPreferencesDialogLayout = new QVBoxLayout( this ); 
    KSegPreferencesDialogLayout->setSpacing( 6 );
    KSegPreferencesDialogLayout->setMargin( 11 );

    Layout1 = new QHBoxLayout(); 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    BackgroundLabel = new QLabel( this, "BackgroundLabel" );
    BackgroundLabel->setText( tr( "Background Color" ) );
    Layout1->addWidget( BackgroundLabel );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    BackgroundColorButton = new QPushButton( this, "BackgroundColorButton" );
    BackgroundColorButton->setText("");
    Layout1->addWidget( BackgroundColorButton );
    KSegPreferencesDialogLayout->addLayout( Layout1 );

    Layout2 = new QHBoxLayout(); 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    ForegroundLabel = new QLabel( this, "ForegroundLabel" );
    ForegroundLabel->setText( tr( "Default Foreground Color" ) );
    Layout2->addWidget( ForegroundLabel );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_2 );

    ForegroundColorButton = new QPushButton( this, "ForegroundColorButton" );
    ForegroundColorButton->setText("");
    Layout2->addWidget( ForegroundColorButton );
    KSegPreferencesDialogLayout->addLayout( Layout2 );

    Layout3 = new QHBoxLayout(); 
    Layout3->setSpacing( 6 );
    Layout3->setMargin( 0 );

    SelectionButtonGroup = new QButtonGroup( this);//, "SelectionButtonGroup" );
    //SelectionButtonGroup->setTitle( tr( "Selection Display" ) );
    //SelectionButtonGroup->setColumnLayout(0, Qt::Vertical );
    //SelectionButtonGroup->layout()->setSpacing( 0 );
    //SelectionButtonGroup->layout()->setMargin( 0 );
    QGroupBox *bx = new QGroupBox(tr( "Selection Display" ),this);
    SelectionButtonGroupLayout = new QHBoxLayout( bx );
    SelectionButtonGroupLayout->setAlignment( Qt::AlignTop );
    SelectionButtonGroupLayout->setSpacing( 6 );
    SelectionButtonGroupLayout->setMargin( 11 );

    BorderSelectButton = new QRadioButton( "BorderSelectButton" );
    BorderSelectButton->setText( tr( "Border Selection" ) );
    SelectionButtonGroup->addButton(BorderSelectButton);
    SelectionButtonGroupLayout->addWidget( BorderSelectButton );

    BlinkingSelectButton = new QRadioButton( "BlinkingSelectButton" );
    BlinkingSelectButton->setText( tr( "Blinking Selection" ) );
    SelectionButtonGroup->addButton(BlinkingSelectButton);
    SelectionButtonGroupLayout->addWidget( BlinkingSelectButton );
    bx->setLayout(SelectionButtonGroupLayout);
    Layout3->addWidget( bx );

    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer_3 );
    KSegPreferencesDialogLayout->addLayout( Layout3 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    KSegPreferencesDialogLayout->addItem( spacer_4 );

    Layout4 = new QHBoxLayout(); 
    Layout4->setSpacing( 6 );
    Layout4->setMargin( 0 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( tr( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout4->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( tr( "Cancel" ) );
    buttonCancel->setAccel( 0 );
    Layout4->addWidget( buttonCancel );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout4->addItem( spacer_5 );
    KSegPreferencesDialogLayout->addLayout( Layout4 );





    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    connect(ForegroundColorButton, SIGNAL(clicked()), this, SLOT(foregroundButtonClicked()));
    connect(BackgroundColorButton, SIGNAL(clicked()), this, SLOT(backgroundButtonClicked()));
    connect(BlinkingSelectButton, SIGNAL(clicked()), this, SLOT(selectButtonClicked()));
    connect(BorderSelectButton, SIGNAL(clicked()), this, SLOT(selectButtonClicked()));

    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KSegPreferencesDialog::~KSegPreferencesDialog()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

void KSegPreferencesDialog::init()
{
  buttonPM = QPixmap(ForegroundColorButton->size() - QSize(10, 10));

  foregroundColorChosen = QColor(KSegProperties::getProperty("ForegroundColor"));
  backgroundColorChosen = QColor(KSegProperties::getProperty("BackgroundColor"));
  blinkingSelectChosen = KSegProperties::getProperty("SelectType") == "BlinkingSelect";

  updateButtons();

}

void KSegPreferencesDialog::destroy()
{
}


void KSegPreferencesDialog::accept()
{
  KSegProperties::setProperty("ForegroundColor", foregroundColorChosen.name());
  KSegProperties::setProperty("BackgroundColor", backgroundColorChosen.name());
  KSegProperties::setProperty("SelectType", blinkingSelectChosen ? "BlinkingSelect" : "BorderSelect");

  KSegView::updateAllViews();

  QDialog::accept();
}


void KSegPreferencesDialog::updateButtons()
{

  buttonPM.fill(foregroundColorChosen);
  ForegroundColorButton->setPixmap(buttonPM);

  buttonPM.fill(backgroundColorChosen);
  BackgroundColorButton->setPixmap(buttonPM);

  if(blinkingSelectChosen) {
    BlinkingSelectButton->setChecked(true);
  }
  else {
    BorderSelectButton->setChecked(true);
  }
}


void KSegPreferencesDialog::foregroundButtonClicked()
{
  QColor c = QColorDialog::getColor(foregroundColorChosen, this);
  if(c.isValid()) {
    foregroundColorChosen = c;
    updateButtons();
  }
}

void KSegPreferencesDialog::backgroundButtonClicked()
{
  QColor c = QColorDialog::getColor(backgroundColorChosen, this);
  if(c.isValid()) {
    backgroundColorChosen = c;
    updateButtons();
  }
}

void KSegPreferencesDialog::selectButtonClicked()
{
  blinkingSelectChosen = BlinkingSelectButton->isChecked();
}



