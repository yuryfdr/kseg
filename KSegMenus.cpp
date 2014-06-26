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

#include "defs.H"
#include <qapplication.h>
#include <qfileinfo.h>
#include <QPixmap>
#include <QMenu>

#include "KSegMenuMaintainer.H"
#include "KSegProperties.H"
#include "KSegWindow.H"
#include "KSegView.H"
#include "KSegDocument.H"
#include "KSegConstruction.H"

//View menu pics
#include "pics/pan.xpm"
#include "pics/zoom.xpm"
#include "pics/zoomOrig.xpm"
#include "pics/zoomFit.xpm"

//New menu pics
#include "pics/intersection.xpm"
#include "pics/line.xpm"
#include "pics/ray.xpm"
#include "pics/segment.xpm"
#include "pics/midpoint.xpm"
#include "pics/perpendicular.xpm"
#include "pics/parallel.xpm"
#include "pics/circle.xpm"
#include "pics/arc.xpm"
#include "pics/bisector.xpm"
#include "pics/locus.xpm"
#include "pics/polygon.xpm"
#include "pics/sector.xpm"
#include "pics/arcsegment.xpm"
#include "pics/circleinterior.xpm"

//Measure menu pics
#include "pics/distance.xpm"
#include "pics/length.xpm"
#include "pics/radius.xpm"
#include "pics/angle.xpm"
#include "pics/ratio.xpm"
#include "pics/slope.xpm"
#include "pics/area.xpm"
#include "pics/calculate.xpm"

//Transform menu pics
#include "pics/vector.xpm"
#include "pics/mirror.xpm"
#include "pics/center.xpm"
#include "pics/chooseangle.xpm"
#include "pics/chooseratio.xpm"
#include "pics/translate.xpm"
#include "pics/reflect.xpm"
#include "pics/scale.xpm"
#include "pics/rotate.xpm"

/*------------------------- some generic ones ---------------------*/
class PopupInfo : public KSegCommandInfo
{
public:
  PopupInfo(QString text, bool (KSegDocument::*function)() = NULL) : KSegCommandInfo(), f(function), txt(text) {}
  
  QString menuText(KSegWindow *) { return txt; }
  
protected:
  bool privateIsEnabled(KSegWindow *win)
  {
    if(!f) return true;
    return ((win->getView()->getDocument())->*f)();
  }

  bool (KSegDocument::*f)();
  QString txt;
};


class SCInfo : public KSegCommandInfo //StaticCommandInfo
{
public:
  enum ReceiverType { Window, View, Document, App };

  SCInfo(QString text, ReceiverType receiver, const char *member, int shortCutKey = 0, QIcon is = QIcon())
    : KSegCommandInfo(), txt(text), rec(receiver), mem(member), key(shortCutKey), iconset(is) {}

  QString menuText(KSegWindow *) { return txt; }
  QIcon iconSet(KSegWindow *) { return iconset; }
  QObject *receiver(KSegWindow *win)
  {
    if(rec == Window) return win;
    if(rec == View) return win->getView();
    if(rec == Document) return win->getView()->getDocument();
    if(rec == App) return qApp;
    return NULL; //should not be here
  }
  const char *member() { return mem; }
  int shortCutKey() { return key; }

protected:
  QString txt;
  ReceiverType rec;
  const char *mem;
  int key;
  QIcon iconset;
};


class PCInfo : public SCInfo //PossibleCommandInfo--sometimes disabled
{
public:
  PCInfo(QString text, ReceiverType receiver, const char *member,
	 bool (KSegDocument::*function)(), int shortCutKey = 0, QIcon is = QIcon())
    : SCInfo(text, receiver, member, shortCutKey, is), f(function) {}

protected:
  bool privateIsEnabled(KSegWindow *win) { return ((win->getView()->getDocument())->*f)(); }
  bool (KSegDocument::*f)();
};


class CPCInfo : public SCInfo //ConstructionPossibleCommandInfo--sometimes disabled
{
public:
  CPCInfo(QString text, ReceiverType receiver, const char *member,
	 bool (KSegConstruction::*function)(), int shortCutKey = 0)
    : SCInfo(text, receiver, member, shortCutKey), f(function) {}

protected:
  bool privateIsEnabled(KSegWindow *win) {
    //the following for some reason works around a bug in some gcc compiler...
    KSegConstruction *c = (KSegConstruction *)(win->getView()->getDocument());
    if(f == &KSegConstruction::canConstructionRecurse) {
      return c->canConstructionRecurse();
    }
    return (c->*f)();
  }
  bool (KSegConstruction::*f)();
};


class MCInfo : public PCInfo //MultipleCommandInfo--plural or singular text
{
public:
  MCInfo(QString text, QString plural, ReceiverType receiver, const char *member,
	 bool (KSegDocument::*function)(), int shortCutKey = 0, int pluralCount = 2, QIcon is = QIcon())
    : PCInfo(text, receiver, member, function, shortCutKey, is), plur(plural), plurCount(pluralCount) {}

  QString menuText(KSegWindow *win)
  {
    if(privateIsEnabled(win) == false) return txt;
    return (win->getView()->getDocument()->selectedCount() < plurCount) ? txt : plur;
  }

protected:
  QString plur; //plural string for the menu
  int plurCount; //least number of selected objects for plural string to be displayed
};



/*------------------------File Menu-----------------------*/
class KSegFileMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegFileMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&File"));
    
    add(new SCInfo(qApp->translate("KSegWindow", "&New Sketch"), SCInfo::Window, SLOT(newSketch()), Qt::CTRL+Qt::Key_N));
    add(new SCInfo(qApp->translate("KSegWindow", "Ne&w Construction"), SCInfo::Window, SLOT(newConstruction())));
    add(new SCInfo(qApp->translate("KSegWindow", "&Open..."), SCInfo::Window,
		   SLOT(fileOpenInNewWindow()), Qt::CTRL+Qt::Key_O));
    add(new SCInfo(qApp->translate("KSegWindow", "Open In Current Window..."), SCInfo::Window, SLOT(fileOpen())));
    add(new SCInfo(qApp->translate("KSegWindow", "&Save"), SCInfo::Window, SLOT(fileSave()), Qt::CTRL+Qt::Key_S));
    add(new SCInfo(qApp->translate("KSegWindow", "Save &As..."), SCInfo::Window, SLOT(fileSaveAs())));

    if(!win->getView()->getDocument()->isConstruction()) {
      addSep();

      add(new SCInfo(qApp->translate("KSegWindow", "Copy As Construction"),
		     SCInfo::Window, SLOT(fileCopyAsConstruction())));
    }
    
    addSep();
    
    add(new SCInfo(qApp->translate("KSegWindow", "&Print..."), SCInfo::Window, SLOT(filePrint()), Qt::CTRL+Qt::Key_P));
    
    add(new SCInfo(qApp->translate("KSegWindow", "E&xport To Image..."), SCInfo::Window,
		   SLOT(fileExportToImage())));  

    add(new SCInfo(qApp->translate("KSegWindow", "Export To S&VG..."), SCInfo::Window,
		   SLOT(fileToSVG())));  

    addSep();
    
    add(new SCInfo(qApp->translate("KSegWindow", "Choose &Language..."), SCInfo::Window, SLOT(fileChooseLanguage())));
    
    addSep();
    
    add(new SCInfo(qApp->translate("KSegWindow", "&Close"), SCInfo::Window, SLOT(close()), Qt::CTRL+Qt::Key_W));
    add(new SCInfo(qApp->translate("KSegWindow", "&Quit"), SCInfo::App, SLOT(closeAllWindows()), Qt::CTRL+Qt::Key_Q));

    addSep();

    itemCount = menu->count();
  }

  void update()
  {
    KSegMenuMaintainer::update(); //parent first

    // now the recent file list
   
    bool ok;
    int count = KSegProperties::getProperty("RecentListSize").toInt(&ok);
    if(!ok) {
      count = 4;
      KSegProperties::setProperty("RecentListSize", "4");
    }
    
    //clear it first:
    int i;

    while((int)menu->count() > itemCount) {
      menu->removeItemAt(itemCount);
    }
    
    KSegWindow::compressRecentList();
    
    //now recreate it
    for(i = 0; i < count; ++i) {
      QString name = KSegProperties::getProperty(QString("RecentFile") + QString::number(i + 1));
      if(name.isNull()) continue;
      
      menu->insertItem(QString("&") + QString::number(i + 1) + ". " + name, window,
		       SLOT(fileOpenRecent(int)), 0, ID_FILE_RECENTLIST_START + i);
    } 
  }

private:
  int itemCount;
};


/*------------------------------EditMenu------------------------------------*/
/*------------------------------ColorMenu------------------------------------*/
class ColorInfo : public KSegCommandInfo
{
public:
  ColorInfo(QColor color, const char *member) : KSegCommandInfo(), c(color), mem(member)
  {
  }

  QString menuText(KSegWindow *)
  {
    if(!c.isValid()) return qApp->translate("KSegWindow", "&Other...");
    return QString();
  }
  QObject *receiver(KSegWindow *win) { return win; }
  const char *member() { return mem; }

  int isChecked(KSegWindow *win)
  {
    if(privateIsEnabled(win) == false) return 0;

    QColor tmp = win->getView()->getDocument()->getCurrentColor();

    if(!c.isValid()) return tmp.isValid() && tmp != Qt::black && tmp != Qt::gray &&
		     tmp != Qt::red && tmp != Qt::green && tmp != Qt::blue &&
		     tmp != Qt::yellow && tmp != Qt::magenta && tmp != Qt::cyan ? 1 : 0;

    else return tmp == c ? 1 : 0;
  }

  void addTo(KSegWindow *win, QMenu *menu)
  {
    if(!c.isValid()) { KSegCommandInfo::addTo(win, menu); return; }

    QPixmap p(100, 20);
    p.fill(c);
    
    menu->insertItem(p, receiver(win), member(), shortCutKey(), getID());
  }

protected:
  bool privateIsEnabled(KSegWindow *win) { return win->getView()->getDocument()->canEditChangeColor(); }

  QColor c;
  const char *mem;
};


class KSegEditColorMenuMaintainer : public KSegMenuMaintainer
{
 public:
  KSegEditColorMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&Color"),
			   &KSegDocument::canEditChangeColor);

    ((QMenu *)menu)->insertTearOffHandle();
   
    add(new ColorInfo(Qt::black, SLOT(editColorBlack())));
    add(new ColorInfo(Qt::gray, SLOT(editColorGray())));
    add(new ColorInfo(Qt::red, SLOT(editColorRed())));
    add(new ColorInfo(Qt::green, SLOT(editColorGreen())));
    add(new ColorInfo(Qt::blue, SLOT(editColorBlue())));
    add(new ColorInfo(Qt::yellow, SLOT(editColorYellow())));
    add(new ColorInfo(Qt::magenta, SLOT(editColorMagenta())));
    add(new ColorInfo(Qt::cyan, SLOT(editColorCyan())));

    addSep();
    
    add(new ColorInfo(QColor(), SLOT(editColorOther())));
  }

};


/*------------------------------LinestyleMenu-------------------------------------*/
class LinestyleInfo : public KSegCommandInfo
{
public:
  LinestyleInfo(bool isWidth, QPen style, const char *member)
    : KSegCommandInfo(), isw(isWidth), p(style), mem(member)
  {
  }

  QObject *receiver(KSegWindow *win) { return win; }
  const char *member() { return mem; }

  int isChecked(KSegWindow *win)
  {
    if(privateIsEnabled(win) == false) return 0;
    if(isw) return win->getView()->getDocument()->getCurrentPenWidth() == (int)p.width() ? 1 : 0;
    return win->getView()->getDocument()->getCurrentPenstyle() == p.style() ? 1 : 0;
  }

  void addTo(KSegWindow *win, QMenu *menu)
  {
    QPixmap pm(100, 20);
    pm.fill(Qt::lightGray);
    
    QPainter pa(&pm);
    pa.setPen(p);

    pa.drawLine(0, 10, 100, 10);
    
    menu->insertItem(pm, receiver(win), member(), shortCutKey(), getID());
  }

protected:
  bool privateIsEnabled(KSegWindow *win) { return win->getView()->getDocument()->canEditChangeLinestyle(); }

  bool isw; //true if this is a width rather than a style option
  QPen p;
  const char *mem;
};


class KSegEditLinestyleMenuMaintainer : public KSegMenuMaintainer
{
 public:
  KSegEditLinestyleMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "Line &Style"),
			   &KSegDocument::canEditChangeLinestyle);

    ((QMenu *)menu)->insertTearOffHandle();
   
    add(new LinestyleInfo(false, QPen(Qt::SolidLine), SLOT(editLinestyleSolid())));
    add(new LinestyleInfo(false, QPen(Qt::DashLine), SLOT(editLinestyleDashed())));
    add(new LinestyleInfo(false, QPen(Qt::DotLine), SLOT(editLinestyleDotted())));

    addSep();

    add(new LinestyleInfo(true, QPen(Qt::black, 0), SLOT(editLinestyleThin())));
    add(new LinestyleInfo(true, QPen(Qt::black, 2), SLOT(editLinestyleNormal())));
    add(new LinestyleInfo(true, QPen(Qt::black, 3), SLOT(editLinestyleThick())));
  }
};



/*------------------------------PointstyleMenu------------------------------------*/
class PointstyleInfo : public KSegCommandInfo
{
public:
  PointstyleInfo(PointStyle style, const char *member) : KSegCommandInfo(), p(style), mem(member)
  {
  }

  QObject *receiver(KSegWindow *win) { return win; }
  const char *member() { return mem; }

  int isChecked(KSegWindow *win)
  {
    if(privateIsEnabled(win) == false) return 0;
    return win->getView()->getDocument()->getCurrentPointstyle() == p ? 1 : 0;
  }

  void addTo(KSegWindow *win, QMenu *menu)
  {
    QPixmap pm(100, 20);
    pm.fill(Qt::lightGray);
    
    QPainter pa(&pm);

    QBrush b(Qt::black);

    G_drawstyle *d = G_drawstyle::match(p, 0, 0, &b);

    G_point(50, 10).draw(pa, *d);
    
    d->deleteReference();
    
    menu->insertItem(pm, receiver(win), member(), shortCutKey(), getID());
  }

protected:
  bool privateIsEnabled(KSegWindow *win) { return win->getView()->getDocument()->canEditChangePointstyle(); }

  PointStyle p;
  const char *mem;
};


class KSegEditPointstyleMenuMaintainer : public KSegMenuMaintainer
{
 public:
  KSegEditPointstyleMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&Point Style"),
			   &KSegDocument::canEditChangePointstyle);

    ((QMenu *)menu)->insertTearOffHandle();
   
    add(new PointstyleInfo(LARGE_CIRCLE, SLOT(editPointstyleLargecircle())));
    add(new PointstyleInfo(MEDIUM_CIRCLE, SLOT(editPointstyleMediumcircle())));
    add(new PointstyleInfo(SMALL_CIRCLE, SLOT(editPointstyleSmallcircle())));
  }
};

/*-----------------------font menu--------------------------*/
class FontInfo : public PCInfo
{
public:
  FontInfo(QString text, int size, const char *member) 
    : PCInfo(text, Window, member, &KSegDocument::canEditChangeFont), sz(size)
  { }

  int isChecked(KSegWindow *win)
  {
    if(privateIsEnabled(win) == false) return 0;
    return sz == win->getView()->getDocument()->getCurrentFontsize() ? 1 : 0;
  }

  void addTo(KSegWindow *win, QMenu *menu)
  {
    PCInfo::addTo(win, menu);
    menu->setItemParameter(getID(), sz);
  }

protected:
  int sz;
};


class KSegEditFontMenuMaintainer : public KSegMenuMaintainer
{
 public:
  KSegEditFontMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&Font"),
			   &KSegDocument::canEditChangeFont);

    ((QMenu *)menu)->insertTearOffHandle();
   
    add(new FontInfo("10", 10, SLOT(editFont(int))));
    add(new FontInfo("12", 12, SLOT(editFont(int))));
    add(new FontInfo("14", 14, SLOT(editFont(int))));
    add(new FontInfo("20", 20, SLOT(editFont(int))));
    add(new FontInfo("30", 30, SLOT(editFont(int))));

    addSep();

    add(new FontInfo(qApp->translate("KSegWindow", "&Font..."), -1, SLOT(editFontFont())));
  }
};


/*-----------------edit, finally----------------*/

class KSegEditMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegEditMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&Edit"));
    
    add(new PCInfo(qApp->translate("KSegWindow", "&Undo"), SCInfo::Document, SLOT(editUndo()),
		   &KSegDocument::canEditUndo, Qt::CTRL+Qt::Key_Z));
    add(new PCInfo(qApp->translate("KSegWindow", "&Redo"), SCInfo::Document, SLOT(editRedo()),
		   &KSegDocument::canEditRedo, Qt::CTRL+Qt::Key_R));

    addSep();

    add(new MCInfo(qApp->translate("KSegWindow", "&Delete Object"),
		   qApp->translate("KSegWindow", "&Delete Objects"), SCInfo::Document, SLOT(editDelete()),
		   &KSegDocument::canEditDelete, Qt::CTRL+Qt::Key_Delete));
    

    addSep();

    add(new MCInfo(qApp->translate("KSegWindow", "Toggle &Label"),
		   qApp->translate("KSegWindow", "Toggle &Labels"), SCInfo::Document, SLOT(editToggleLabels()),
		   &KSegDocument::canEditToggleLabels, Qt::CTRL+Qt::Key_L));

    add(new MCInfo(qApp->translate("KSegWindow", "Hide Label"),
		   qApp->translate("KSegWindow", "Hide Labels"), SCInfo::Document, SLOT(editHideLabels()),
		   &KSegDocument::canEditToggleLabels));

    add(new MCInfo(qApp->translate("KSegWindow", "Show Label"),
		   qApp->translate("KSegWindow", "Show Labels"), SCInfo::Document, SLOT(editShowLabels()),
		   &KSegDocument::canEditToggleLabels));

    add(new PCInfo(qApp->translate("KSegWindow", "Change La&bel..."), SCInfo::Document, SLOT(editChangeLabel()),
		   &KSegDocument::canEditChangeLabel));

    addSep();

    add(new MCInfo(qApp->translate("KSegWindow", "&Hide Object"),
		   qApp->translate("KSegWindow", "&Hide Objects"), SCInfo::Document, SLOT(editHide()),
		   &KSegDocument::canEditHide, Qt::CTRL+Qt::Key_H));

    add(new PCInfo(qApp->translate("KSegWindow", "&Unhide All"), SCInfo::Document, SLOT(editShowHidden()),
		   &KSegDocument::canEditShowHidden, Qt::CTRL+Qt::Key_U));

    addSep();

    add(new KSegEditColorMenuMaintainer(window));
    add(new KSegEditLinestyleMenuMaintainer(window));
    add(new KSegEditPointstyleMenuMaintainer(window));
    add(new KSegEditFontMenuMaintainer(window));

    addSep();

    add(new PCInfo(qApp->translate("KSegWindow", "Change &Number of Samples..."),
		   SCInfo::Document, SLOT(editChangeNumberOfSamples()),
		   &KSegDocument::canEditChangeNumberOfSamples));

    addSep();
    
    add(new SCInfo(qApp->translate("KSegWindow", "Pr&eferences..."),
		   SCInfo::Window, SLOT(editPreferences())));

    addSep();
    
    add(new SCInfo(qApp->translate("KSegWindow", "Selection &Groups..."),
		   SCInfo::Document, SLOT(editShowSelectionGroupDialog())));

  }
};


/*--------------------------------View menu-------------------------------*/

class ZoomInfo : public SCInfo
{
public:
  ZoomInfo() :
    SCInfo(qApp->translate("KSegWindow", "&Zoom"), SCInfo::View, SLOT(viewZoom()), Qt::ALT + Qt::Key_Z) {
    iconset = QIcon(QPixmap((const char **)zoom_xpm));
  }

  int isChecked(KSegWindow *win)
  {
    if(privateIsEnabled(win) == false) return 0;
    return win->getView()->getNextZoom() ? 1 : 0;
  }
};

class PanInfo : public SCInfo
{
public:
  PanInfo() :
    SCInfo(qApp->translate("KSegWindow", "&Pan"), SCInfo::View, SLOT(viewPan())) {
    iconset = QIcon(QPixmap((const char **)pan_xpm));
  }

  int isChecked(KSegWindow *win)
  {
    if(privateIsEnabled(win) == false) return 0;
    return win->getView()->getNextPan() ? 1 : 0;
  }
};

class KSegViewMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegViewMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    toolBar = new QToolBar(win);
    win->addToolBar(toolBar);
    toolBar->setLabel(qApp->translate("KSegWindow", "&View").replace(QRegExp("&"), ""));
    if(KSegProperties::getProperty("ViewToolbarVisible").lower() == "false")
      toolBar->hide();
    else toolBar->show();
    connect(toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(toolbarVisibilityChanged(bool)));

    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&View"));

    add(new SCInfo(qApp->translate("KSegWindow", "New &View"), SCInfo::Window, SLOT(newView())));

    addNonToolbarSep();

    add(new PanInfo());
    add(new ZoomInfo());

    QIcon isZoomFit(QPixmap((const char **)zoomFit_xpm));

    add(new SCInfo(qApp->translate("KSegWindow", "Zoom To &Fit"), SCInfo::View,
		   SLOT(viewZoomFit()), 0, isZoomFit));

    QIcon isZoomOrig(QPixmap((const char **)zoomOrig_xpm));

    add(new SCInfo(qApp->translate("KSegWindow", "&Original Zoom (100%)"),
		   SCInfo::View, SLOT(viewOrigZoom()), 0, isZoomOrig));
  }

protected:
  virtual void privateToolbarVisibilityChanged(bool visible)
  {
    if(visible) KSegProperties::setProperty("ViewToolbarVisible", "true");
    else  KSegProperties::setProperty("ViewToolbarVisible", "false");
  }

};


/*--------------------------------New menu-------------------------------*/
class NewCInfo : public PCInfo
{
public:
  NewCInfo(QString usual, QString (*textFunction)(KSegDocument *), const char *member,
	   bool (KSegDocument::*function)(), int shortCutKey = 0, QIcon is = QIcon())
    : PCInfo(usual, Document, member, function, shortCutKey, is),
      textFunc(textFunction), iconFunc(NULL) {}

  NewCInfo(QString usual, QString (*textFunction)(KSegDocument *), const char *member,
	   bool (KSegDocument::*function)(), int shortCutKey, QIcon defaultIs,
	   QIcon (*iconFunction)(KSegDocument *))
    : PCInfo(usual, Document, member, function, shortCutKey, defaultIs),
      textFunc(textFunction), iconFunc(iconFunction) {}
  

  QString menuText(KSegWindow *win)
  {
    if(!privateIsEnabled(win)) return txt;
    return (*textFunc)(win->getView()->getDocument());
  }

  QIcon iconSet(KSegWindow *win) {
    if(iconFunc == NULL || !privateIsEnabled(win)) return PCInfo::iconSet(win);
    return (*iconFunc)(win->getView()->getDocument());
  }

protected:
  QString (*textFunc)(KSegDocument *);
  QIcon (*iconFunc)(KSegDocument *);
};


class KSegNewMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegNewMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    toolBar = new QToolBar(win);
    win->addToolBar(toolBar);
    toolBar->setLabel(qApp->translate("KSegWindow", "&New").replace(QRegExp("&"), ""));
    if(KSegProperties::getProperty("NewToolbarVisible").lower() == "false")
      toolBar->hide();
    else toolBar->show();
    connect(toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(toolbarVisibilityChanged(bool)));

    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&New"));
    
    QIcon isIntersect(QPixmap((const char **)intersection_xpm));
    add(new NewCInfo(qApp->translate("KSegWindow", "&Intersection Point"), intersectionFunc,
		     SLOT(newIntersection()), &KSegDocument::canNewIntersection, Qt::ALT+Qt::Key_I, isIntersect));

    QIcon isSegment(QPixmap((const char **)segment_xpm));
    add(new MCInfo(qApp->translate("KSegWindow", "&Segment"),
		   qApp->translate("KSegWindow", "&Segments"), SCInfo::Document, SLOT(newSegment()),
		   &KSegDocument::canNewSegment, Qt::ALT+Qt::Key_S, 3, isSegment));

    QIcon isMidpoint(QPixmap((const char **)midpoint_xpm));
    add(new MCInfo(qApp->translate("KSegWindow", "&Midpoint"),
		   qApp->translate("KSegWindow", "&Midpoints"), SCInfo::Document, SLOT(newMidpoint()),
		   &KSegDocument::canNewMidpoint, Qt::ALT+Qt::Key_M, 2, isMidpoint));

    QIcon isRay(QPixmap((const char **)ray_xpm));
    add(new MCInfo(qApp->translate("KSegWindow", "&Ray"),
		   qApp->translate("KSegWindow", "&Rays"), SCInfo::Document, SLOT(newRay()),
		   &KSegDocument::canNewRay, Qt::ALT+Qt::Key_R, 3, isRay));

    QIcon isLine(QPixmap((const char **)line_xpm));
    add(new NewCInfo(qApp->translate("KSegWindow", "&Line"), lineFunc,
		     SLOT(newLine()), &KSegDocument::canNewLine, Qt::ALT+Qt::Key_L, isLine, lineIconFunc));

    QIcon isPerpendicular(QPixmap((const char **)perpendicular_xpm));
    add(new MCInfo(qApp->translate("KSegWindow", "&Perpendicular Line"),
		   qApp->translate("KSegWindow", "&Perpendicular Lines"), SCInfo::Document,
		   SLOT(newPerpendicular()), &KSegDocument::canNewPerpendicular, Qt::ALT+Qt::Key_P, 3,
		   isPerpendicular));

    QIcon isCircle(QPixmap((const char **)circle_xpm));
    add(new NewCInfo(qApp->translate("KSegWindow", "&Circle By Center And Point"), circleFunc,
		     SLOT(newCircle()), &KSegDocument::canNewCircle, Qt::ALT+Qt::Key_C, isCircle));

    QIcon isArc(QPixmap((const char **)arc_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Arc By Three Points"), SCInfo::Document,
		   SLOT(newArc()), &KSegDocument::canNewArc, Qt::ALT+Qt::Key_A, isArc));

    QIcon isBisector(QPixmap((const char **)bisector_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Angle &Bisector"), SCInfo::Document,
		   SLOT(newBisector()), &KSegDocument::canNewBisector, Qt::ALT+Qt::Key_B, isBisector));

    QIcon isLocus(QPixmap((const char **)locus_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "L&ocus"), SCInfo::Document,
		   SLOT(newLocus()), &KSegDocument::canNewLocus, Qt::ALT+Qt::Key_O, isLocus));

    add(new NewCInfo(qApp->translate("KSegWindow", "Endpoints"), endPtFunc,
		     SLOT(newEndpoints()), &KSegDocument::canNewEndpoints));

    add(new PCInfo(qApp->translate("KSegWindow", "Center Point"), SCInfo::Document,
		   SLOT(newCenterpoint()), &KSegDocument::canNewCenterpoint));

    addSep();

    QIcon isPolygon(QPixmap((const char **)polygon_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Poly&gon"), SCInfo::Document,
		   SLOT(newPolygon()), &KSegDocument::canNewPolygon, Qt::ALT+Qt::Key_G, isPolygon));

    QIcon isArcSector(QPixmap((const char **)sector_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Arc S&ector"), SCInfo::Document,
		   SLOT(newArcSector()), &KSegDocument::canNewArcSector, 0, isArcSector));

    QIcon isArcSegment(QPixmap((const char **)arcsegment_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Arc Se&gment"), SCInfo::Document,
		   SLOT(newArcSegment()), &KSegDocument::canNewArcSector, 0, isArcSegment));

    QIcon isCircleInterior(QPixmap((const char **)circleinterior_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Circle I&nterior"), SCInfo::Document,
		   SLOT(newCircleInterior()), &KSegDocument::canNewCircleInterior, 0, isCircleInterior));
  }

  static QString endPtFunc(KSegDocument *d)
  {
    return (d->getSelected()[0]->getType() == G_RAY) ?
      qApp->translate("KSegWindow", "Endpoint") :
      qApp->translate("KSegWindow", "Endpoints");
  }

  static QString intersectionFunc(KSegDocument *d)
  {
    return !((d->getSelected()[0]->getType() | d->getSelected()[1]->getType()) & (G_CURVE - G_STRAIGHT)) ?
      qApp->translate("KSegWindow", "&Intersection Point") :
      qApp->translate("KSegWindow", "&Intersection Points");
  }

  static QString lineFunc(KSegDocument *d)
  {
    int i;

    const G_refs &x = d->getSelected();

    for(i = 0; i < (int)x.count(); i++) if(x[i]->getType() & G_STRAIGHT) break;

    if(i == (int)x.count()) {
      if(x.count() > 2) return qApp->translate("KSegWindow", "&Lines");
      else return qApp->translate("KSegWindow", "&Line");
    }
    else {
      if(x.count() > 2) return qApp->translate("KSegWindow", "Parallel &Lines");
      else return qApp->translate("KSegWindow", "Parallel &Line");
    }
  }

  static QIcon lineIconFunc(KSegDocument *d)
  {
    int i;

    const G_refs &x = d->getSelected();

    for(i = 0; i < (int)x.count(); i++) if(x[i]->getType() & G_STRAIGHT) break;

    if(i == (int)x.count()) return QIcon(QPixmap((const char **)line_xpm));

    return QIcon(QPixmap((const char **)parallel_xpm));
  }

  static QString circleFunc(KSegDocument *d)
  {
    return (d->getSelected()[0]->getType() == d->getSelected()[1]->getType()) ?
      qApp->translate("KSegWindow", "&Circle By Center And Point") :
      qApp->translate("KSegWindow", "&Circle By Center And Radius");
  }

protected:
  virtual void privateToolbarVisibilityChanged(bool visible)
  {
    if(visible) KSegProperties::setProperty("NewToolbarVisible", "true");
    else  KSegProperties::setProperty("NewToolbarVisible", "false");
  }

};


/*--------------------------------Measure Menu-----------------------------*/
class KSegMeasureMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegMeasureMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    toolBar = new QToolBar(win);
    win->addToolBar(Qt::LeftToolBarArea,toolBar);
    toolBar->setLabel(qApp->translate("KSegWindow", "Meas&ure").replace(QRegExp("&"), ""));
    if(KSegProperties::getProperty("MeasureToolbarVisible").lower() == "false")
      toolBar->hide();
    else toolBar->show();
    connect(toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(toolbarVisibilityChanged(bool)));

    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "Meas&ure"));
    
    QIcon isDistance(QPixmap((const char **)distance_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Distance"), SCInfo::Document,
		   SLOT(measureDistance()), &KSegDocument::canMeasureDistance, 0, isDistance));

    QIcon isLength(QPixmap((const char **)length_xpm));
    add(new NewCInfo(qApp->translate("KSegWindow", "&Length"), lengthFunc,
		     SLOT(measureLength()), &KSegDocument::canMeasureLength, 0, isLength));

    QIcon isRadius(QPixmap((const char **)radius_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Radius"), SCInfo::Document,
		   SLOT(measureRadius()), &KSegDocument::canMeasureRadius, 0, isRadius));

    QIcon isAngle(QPixmap((const char **)angle_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Angle"), SCInfo::Document,
		   SLOT(measureAngle()), &KSegDocument::canMeasureAngle, 0, isAngle));

    QIcon isRatio(QPixmap((const char **)ratio_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Ra&tio"), SCInfo::Document,
		   SLOT(measureRatio()), &KSegDocument::canMeasureRatio, 0, isRatio));

    QIcon isSlope(QPixmap((const char **)slope_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Slope"), SCInfo::Document,
		   SLOT(measureSlope()), &KSegDocument::canMeasureSlope, 0, isSlope));

    QIcon isArea(QPixmap((const char **)area_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Ar&ea"), SCInfo::Document,
		   SLOT(measureArea()), &KSegDocument::canMeasureArea, 0, isArea));

    addSep();

    QIcon isCalculate(QPixmap((const char **)calculate_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Calculate..."), SCInfo::Document,
		   SLOT(measureCalculate()), &KSegDocument::canMeasureCalculate, 0, isCalculate));

  }

  static QString lengthFunc(KSegDocument *d)
  {
    if(d->getSelected()[0]->getType() == G_SEGMENT) {
      return qApp->translate("KSegWindow", "&Length");
    }
    else if(d->getSelected()[0]->getType() == G_CIRCLE) {
      return qApp->translate("KSegWindow", "&Circumference");
    }
    else { //the object is an arc
      return qApp->translate("KSegWindow", "Arc &Length");
    }
  }

protected:
  virtual void privateToolbarVisibilityChanged(bool visible)
  {
    if(visible) KSegProperties::setProperty("MeasureToolbarVisible", "true");
    else  KSegProperties::setProperty("MeasureToolbarVisible", "false");
  }

};


/*--------------------------------Transform Menu-----------------------------*/
class KSegTransformMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegTransformMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    toolBar = new QToolBar(win);
    win->addToolBar(Qt::RightToolBarArea,toolBar);
    toolBar->setLabel(qApp->translate("KSegWindow", "&Transform").replace(QRegExp("&"), ""));
    if(KSegProperties::getProperty("TransformToolbarVisible").lower() == "false")
      toolBar->hide();
    else toolBar->show();
    connect(toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(toolbarVisibilityChanged(bool)));

    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&Transform"));
    
    QIcon isVector(QPixmap((const char **)vector_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Choose &Vector"), SCInfo::Document,
		   SLOT(transformChooseVector()), &KSegDocument::canTransformChooseVector,
		   0, isVector));

    QIcon isMirror(QPixmap((const char **)mirror_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Choose &Mirror"), SCInfo::Document,
		   SLOT(transformChooseMirror()), &KSegDocument::canTransformChooseMirror,
		   0, isMirror));

    QIcon isCenter(QPixmap((const char **)center_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Choose &Center"), SCInfo::Document,
		   SLOT(transformChooseCenter()), &KSegDocument::canTransformChooseCenter,
		   0, isCenter));

    QIcon isChooseAngle(QPixmap((const char **)chooseangle_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Choose &Angle"), SCInfo::Document,
		   SLOT(transformChooseAngle()), &KSegDocument::canTransformChooseAngle,
		   0, isChooseAngle));

    QIcon isChooseRatio(QPixmap((const char **)chooseratio_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "Choose Rati&o"), SCInfo::Document,
		   SLOT(transformChooseRatio()), &KSegDocument::canTransformChooseRatio,
		   0, isChooseRatio));

    add(new PCInfo(qApp->translate("KSegWindow", "C&lear Chosen"), SCInfo::Document,
		   SLOT(transformClearChosen()), &KSegDocument::canTransformClearChosen));

    addSep();
    
    QIcon isTranslate(QPixmap((const char **)translate_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Translate"), SCInfo::Document,
		   SLOT(transformTranslate()), &KSegDocument::canTransformTranslate,
		   0, isTranslate));

    QIcon isReflect(QPixmap((const char **)reflect_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "R&eflect"), SCInfo::Document,
		   SLOT(transformReflect()), &KSegDocument::canTransformReflect,
		   0, isReflect));

    QIcon isScale(QPixmap((const char **)scale_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Scale"), SCInfo::Document,
		   SLOT(transformScale()), &KSegDocument::canTransformScale, 0,
		   isScale));

    QIcon isRotate(QPixmap((const char **)rotate_xpm));
    add(new PCInfo(qApp->translate("KSegWindow", "&Rotate"), SCInfo::Document,
		   SLOT(transformRotate()), &KSegDocument::canTransformRotate,
		   0, isRotate));

  }

protected:
  virtual void privateToolbarVisibilityChanged(bool visible)
  {
    if(visible) KSegProperties::setProperty("TransformToolbarVisible", "true");
    else  KSegProperties::setProperty("TransformToolbarVisible", "false");
  }

};

/*--------------------------------Play Menu----------------------------------*/

class KSegQuickPlayMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegQuickPlayMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&Quick Play"));
    
    add(new SCInfo(qApp->translate("KSegWindow", "&Change Directory..."),
		   SCInfo::Window, SLOT(quickPlaySetDirectory())));
  }
  
  void update()
  {
    KSegMenuMaintainer::update();

    while(menu->count() > 1) {
      menu->removeItemAt(1);
    }

    addSep();

    vector<QString> files = window->getValidConstructionFiles();
    
    int i;
    for(i = 0; i < (int)files.size(); ++i) {
      QFile f(files[i]);
      ScanType s = KSegConstruction::scanConstructionFile(f, window->getView()->getDocument()->getSelected());
      if(s == INVALID) continue;

      menu->insertItem(QFileInfo(files[i]).fileName(),
		       window, SLOT(quickPlay(int)));
      
      int id = menu->idAt(menu->count() - 1);
      
      menu->setItemParameter(id, i);
      menu->setItemEnabled(id, s == MATCHED);
    }
    
    if(menu->count() == 2) { //add dummy item
      menu->insertItem(qApp->translate("KSegWindow", "No Constructions"),
		       window, SLOT(fileOpen())); //dummy slot
      
      int id = menu->idAt(menu->count() - 1);
      menu->setItemEnabled(id, false);
    }
  }

protected:
  virtual bool updateOften() { return false; }
};
  

class KSegPlayMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegPlayMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "Pla&y"));
    
    add(new KSegQuickPlayMenuMaintainer(win));
  }

  void update()
  {
    KSegMenuMaintainer::update();

    while(menu->count() > 1) {
      menu->removeItemAt(0);
    }
    
    std::vector<KSegConstruction *> c = KSegConstruction::getAllConstructions();
    
    int i;
    for(i = (int)c.size() - 1; i >= 0; --i) {
      if(c[i] == window->getView()->getDocument()) continue;
      
      QString f = c[i]->getFilename();
      if(f.isEmpty()) f = QString(qApp->translate("KSegWindow", "Untitled"));
      else {
	f = QFileInfo(f).fileName();
      }
      
      menu->insertItem(f, window->getView(), SLOT(playConstruction(int)), 0, -1, 0);
      
      int id = menu->idAt(0);
      
      menu->setItemParameter(id, i);
      menu->setItemEnabled(id, window->getView()->getDocument()->canPlay(i));
    }
    
    if(menu->count() > 1) {
      menu->insertSeparator(menu->count() - 1);
    }
  }  

protected:
  virtual bool updateOften() { return false; }
};



/*--------------------------------Construction Menu-----------------------------*/
class KSegConstructionMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegConstructionMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "Const&ruction"));
    
    add(new CPCInfo(qApp->translate("KSegWindow", "Make &Normal"), SCInfo::Document,
		   SLOT(constructionMakeNormal()), &KSegConstruction::canConstructionMakeNormal));
    
    add(new CPCInfo(qApp->translate("KSegWindow", "Make &Given"), SCInfo::Document,
		   SLOT(constructionMakeGiven()), &KSegConstruction::canConstructionMakeGiven));
    
    add(new CPCInfo(qApp->translate("KSegWindow", "Make &Final"), SCInfo::Document,
		   SLOT(constructionMakeFinal()), &KSegConstruction::canConstructionMakeFinal));
    
    add(new CPCInfo(qApp->translate("KSegWindow", "Make &Initial"), SCInfo::Document,
		   SLOT(constructionMakeInitial()), &KSegConstruction::canConstructionMakeInitial));

    addSep();

    add(new CPCInfo(qApp->translate("KSegWindow", "&Recurse"), SCInfo::Document,
		   SLOT(constructionRecurse()), &KSegConstruction::canConstructionRecurse));
  }
};


/*--------------------------------Help Menu----------------------------------*/
class KSegHelpMenuMaintainer : public KSegMenuMaintainer
{
public:
  KSegHelpMenuMaintainer(KSegWindow *win) : KSegMenuMaintainer(win)
  {
    menu = new QMenu();
    myInfo = new PopupInfo(qApp->translate("KSegWindow", "&Help"));

    add(new SCInfo(qApp->translate("KSegWindow", "&Help"), SCInfo::Window, SLOT(help()), Qt::Key_F1));

    add(new SCInfo(qApp->translate("KSegWindow", "&About..."), SCInfo::Window, SLOT(helpAbout())));
  }
};

/*--------------------------------Main Menu----------------------------------*/

KSegMainMenuMaintainer::KSegMainMenuMaintainer(KSegWindow *win)
  : KSegMenuMaintainer(win)
{
  KSegCommandInfo::resetID();

  myInfo = NULL;
  
  menuBar = win->menuBar();//->addMenu("Menu");
  menuBar->clear(); //just in case

  add(new KSegFileMenuMaintainer(window));
  add(new KSegEditMenuMaintainer(window));
  add(new KSegViewMenuMaintainer(window));
  add(new KSegNewMenuMaintainer(window));
  add(new KSegMeasureMenuMaintainer(window));
  add(new KSegTransformMenuMaintainer(window));
  add(new KSegPlayMenuMaintainer(window));

  if(win->getView()->getDocument()->isConstruction())
    add(new KSegConstructionMenuMaintainer(window));

  addSep();

  add(new KSegHelpMenuMaintainer(window));
}
