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


#include <qapplication.h>
#include <QFileDialog>
#include <qmessagebox.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qsplitter.h>
#include <qimage.h>
#include <qcursor.h>

#include <QTranslator>
#include <QKeyEvent>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QImageWriter>
#include <QListWidget>
#include <QTextBrowser>
#include <QSvgGenerator>

#include "KSegConstructionList.H"
#include "KSegWindow.H"
#include "KSegView.H"
#include "KSegDocument.H"
#include "KSegConstruction.H"
#include "G_ref.H"
#include "G_object.H"
#include "defs.H"
#include "KSegProperties.H"
#include "KSegPreferencesDialog.H"
#include "KSegMenuMaintainer.H"


QString KSegWindow::quickPlayDirectory;

KSegWindow::~KSegWindow()
{
  delete maintainer;
}

KSegWindow::KSegWindow(KSegDocument *doc)
  : QMainWindow(0, 0, Qt::WDestructiveClose | Qt::WType_TopLevel), openInNewWindow(true)
{
  quickPlayDirectory = KSegProperties::getProperty("QuickPlayDirectory");

  if(doc == 0) {
    doc = new KSegDocument();
  }
  else {
    if(doc->getFilename().isEmpty()) setCaption(tr("kseg: Untitled"));
    else setCaption(QString("kseg: ") + doc->getFilename());
  }

  if(doc->isConstruction() == false) {
    view = new KSegView(doc, this);

    setCentralWidget(view);

    resize(800, 600);
  }
  else {
    QSplitter *sp = new QSplitter(this);

    view = new KSegView(doc, sp);

    KSegConstructionList *lb = new KSegConstructionList(doc, sp);

    sp->setOrientation(Qt::Horizontal);
    sp->setOpaqueResize(true);

    setCentralWidget(sp);

    resize(800, 600);
    lb->resize(60, 600);
  }

  connect(view, SIGNAL(statusBarMessage(const QString &)), this, SLOT(statusBarMessage(const QString &)));
  connect(view, SIGNAL(updateMenus()), this, SLOT(updateMenus()));

  maintainer = new KSegMainMenuMaintainer(this);

  //setUsesBigPixmaps(true);

  statusBar()->addWidget(new QLabel(statusBar(), "zoomLabel"), 0, true);
  statusBar()->show();
  statusBarMessage("");
}

void KSegWindow::changeDocument(KSegDocument *doc)
{
  KSegMenuMaintainer *oldM = maintainer;
  maintainer = NULL;

  menuBar()->clear();

  delete centralWidget();

  if(doc->getFilename().isEmpty()) setCaption(tr("kseg: Untitled"));
  else setCaption(QString("kseg: ") + doc->getFilename());

  if(doc->isConstruction() == false) {
    view = new KSegView(doc, this);

    setCentralWidget(view);
    
    resize(800, 600);
  }
  else {
    QSplitter *sp = new QSplitter(this);

    view = new KSegView(doc, sp);
    KSegConstructionList *lb = new KSegConstructionList(doc, sp);

    sp->setOrientation(Qt::Horizontal);
    sp->setOpaqueResize(true);

    setCentralWidget(sp);

    resize(800, 600);
    lb->resize(60, 600);
  }

  connect(view, SIGNAL(statusBarMessage(const QString &)), this, SLOT(statusBarMessage(const QString &)));
  connect(view, SIGNAL(updateMenus()), this, SLOT(updateMenus()));

  centralWidget()->show();
  maintainer = new KSegMainMenuMaintainer(this);
  delete oldM; //TODO-- segfaults!
}


void KSegWindow::statusBarMessage(const QString &s)
{
  statusBar()->message(s);
  QLabel *l = ((QLabel *)statusBar()->child("zoomLabel"));
  QString newText = tr("Zoom:") + " " + QString::number((int)(view->getZoom() * 100 + 0.5)) + QString("%");
  if(l->text() != newText) {
    l->setText(newText);
    l->repaint();
  }
}


void KSegWindow::updateMenus()
{
  if(maintainer != NULL) maintainer->update();
}


void KSegWindow::compressRecentList()
{
  int count = KSegProperties::getProperty("RecentListSize").toInt();

  hash_set<QString> recentItems;
  int skip = 0;

  int i;
  for(i = 0; i < count; ++i) {
    if(i + skip >= count) {
      KSegProperties::setProperty("RecentFile" + QString::number(i + 1), QString());
      continue;
    }

    QString cur = KSegProperties::getProperty("RecentFile" + QString::number(i + skip + 1));
    if(cur.isNull() || recentItems.count(cur)) {
      skip++;
      i--;
      continue;
    }

    if(skip)
      KSegProperties::setProperty("RecentFile" + QString::number(i + 1),
				 KSegProperties::getProperty("RecentFile" +
							     QString::number(i + skip + 1)));

    recentItems.insert(KSegProperties::getProperty("RecentFile" + QString::number(i + 1)));
  }
}


void KSegWindow::quickPlay(int which)
{
  vector<QString> files = getValidConstructionFiles();

  //just in case:
  QFile fi(files[which]);
  ScanType s = KSegConstruction::scanConstructionFile(fi, view->getDocument()->getSelected());

  if(s != MATCHED) {
    Q_ASSERT(s == MATCHED);
    return;
  }

  QFile f(files[which]);

  KSegConstruction temp;

  if(f.open(QIODevice::ReadOnly)) {
    QDataStream stream(&f);
    stream.setVersion(3);
    
    stream >> temp;
    f.close();
  }

  vector<KSegConstruction *> cs = KSegConstruction::getAllConstructions();

  int c = (int)cs.size();

  while(c--) {
    if(cs[c] == &temp) break;
  }
  
  Q_ASSERT(c != -1);
  
  view->playConstruction(c);
}

void KSegWindow::quickPlaySetDirectory()
{
  QFileDialog fd(0,"",quickPlayDirectory, "*.sec");
  
  fd.setMode(QFileDialog::Directory);
  
  if(fd.exec() != QDialog::Accepted) return;

  QString newDir = fd.selectedFile();
  if(newDir.isNull()) return;

  quickPlayDirectory = newDir;
  KSegProperties::setProperty("QuickPlayDirectory", newDir);
}

//returns a vector of full paths for potential
//filenames in the current directory
vector<QString> KSegWindow::getValidConstructionFiles()
{
  vector<QString> toBeReturned;

  QDir myDir = QDir(quickPlayDirectory);
  myDir.setNameFilter("*.sec");
  myDir.setSorting(QDir::Name);
  myDir.setFilter(QDir::Files | QDir::Readable);

  QStringList entries = myDir.entryList();

  int i;
  for(i = 0; i < (int)entries.count(); ++i) {
    QString entry = entries[i];
    
    toBeReturned.push_back(myDir.absPath() + "/" + entry);
  }

  return toBeReturned;
}


void KSegWindow::keyPressEvent(QKeyEvent *ev)
{
  QMainWindow::keyPressEvent(ev);

  view->updateStatusBar(ev->stateAfter());
}

//this is a workaround for a seeming bug...
void KSegWindow::keyReleaseEvent(QKeyEvent *ev)
{
  QMainWindow::keyReleaseEvent(ev);

  view->updateStatusBar(ev->stateAfter());
}


bool KSegWindow::closingDocument()
{
  if(view->getDocument()->isModified() == false || view->getDocument()->getNumViews() > 1) {
    view->delDocumentRef();
    return true;
  }

  int ans = QMessageBox::warning(this, tr("Save?"),
				 tr("Would you like to save your changes?"),
				 QMessageBox::Yes, QMessageBox::No,
				 QMessageBox::Cancel);

  if(ans == QMessageBox::Cancel) {
    return false;
  }
  if(ans == QMessageBox::Yes) {
    fileSave();
    if(view->getDocument()->isModified() == true) { //check if the guy hit cancel
      return false;
    }
  }

  view->delDocumentRef();
  return true;
}


void KSegWindow::closeEvent(QCloseEvent *ev)
{
  if(closingDocument())
    ev->accept();
  else
    ev->ignore();
}


void KSegWindow::newSketch()
{
  (new KSegWindow())->show();
}

void KSegWindow::newConstruction()
{
  KSegConstruction *c = new KSegConstruction();

  (new KSegWindow(c))->show();
}


void KSegWindow::newView()
{
  (new KSegWindow(view->getDocument()))->show();
}

void KSegWindow::fileSaveAs()
{
  QString fileName;

  if(view->getDocument()->isConstruction()) {
    fileName = QFileDialog::
      getSaveFileName(view->getDocument()->getFilename(), "*.sec");

    if(fileName.isNull()) return;
    
    if(!QFileInfo(fileName).fileName().contains('.'))
      fileName = fileName + ".sec";
  }
  else {
    fileName = QFileDialog::
      getSaveFileName(view->getDocument()->getFilename(), "*.seg");

    if(fileName.isNull()) return;

    if(!QFileInfo(fileName).fileName().contains('.'))
      fileName = fileName + ".seg";
  }

  QFile f(fileName);

  if(f.open(QIODevice::WriteOnly)) {
    QDataStream stream(&f);
    stream.setVersion(3);

    stream << *(view->getDocument());
    f.close();
  }

  //add it to the Recent File list
  int count = KSegProperties::getProperty("RecentListSize").toInt();
  int i;

  for(i = count - 1; i > 0; --i) {
    KSegProperties::setProperty("RecentFile" + QString::number(i + 1),
				KSegProperties::getProperty("RecentFile" + QString::number(i)));
  }
  
  KSegProperties::setProperty("RecentFile1", fileName);

  view->getDocument()->emitDocumentSaved(fileName);
}

void KSegWindow::fileSave()
{
  if(view->getDocument()->getFilename().isEmpty()) {
    fileSaveAs();
    return;
  }

  QString fileName = view->getDocument()->getFilename();

  QFile f(fileName);

  if(f.open(QIODevice::WriteOnly)) {
    QDataStream stream(&f);
    stream.setVersion(3);

    stream << *(view->getDocument());
    f.close();
  }

  view->getDocument()->emitDocumentSaved(fileName);
}

void KSegWindow::fileOpenRecent(int num)
{
  num = num - ID_FILE_RECENTLIST_START;

  QString fileName = KSegProperties::getProperty(QString("RecentFile") + QString::number(num + 1));

  if(fileName.isNull()) return;

  if(openFile(fileName, openInNewWindow ? NULL : this)) {
    int i;

    for(i = num; i > 0; --i) {
      KSegProperties::setProperty("RecentFile" + QString::number(i + 1),
				  KSegProperties::getProperty("RecentFile" + QString::number(i)));
    }

    KSegProperties::setProperty("RecentFile1", fileName);
  }
  else {
    KSegProperties::setProperty("RecentFile" + QString::number(num + 1), QString());
    compressRecentList();
  }
}


void KSegWindow::fileOpen()
{
  openInNewWindow = false;
  fileOpenInNewWindow();
  openInNewWindow = true;
}


void KSegWindow::fileOpenInNewWindow()
{

  QString fileName = QFileDialog::getOpenFileName(QString::null, "*.seg *.sec");

  if(fileName.isNull()) return;

  if(openInNewWindow == false && !closingDocument()) return;

  //if the file is one of the recent ones, call the function
  //because it knows what to do with the recent list
  compressRecentList();

  int count = KSegProperties::getProperty("RecentListSize").toInt();

  int i;
  for(i = 0; i < count; ++i) {
    QString cur = KSegProperties::getProperty("RecentFile" + QString::number(i + 1));
    if(cur == fileName) {
      fileOpenRecent(i + ID_FILE_RECENTLIST_START);
      return;
    }
  }

  if(openFile(fileName, openInNewWindow ? NULL : this)) {
    //add the file name to the recently used list:
    for(i = count - 1; i > 0; --i) {
      KSegProperties::setProperty("RecentFile" + QString::number(i + 1),
				  KSegProperties::getProperty("RecentFile" + QString::number(i)));
    }

    KSegProperties::setProperty("RecentFile1", fileName);
  }
}


bool KSegWindow::openFile(QString fileName, KSegWindow *win)
{
  if(QFileInfo(fileName).exists() == false) {
      QMessageBox::warning(0, tr("File Not Found"),
			   tr("Sorry, I am afraid that file %1 does not exist.").arg(fileName),
			   QMessageBox::Ok, 0);
      return false;
  }

  QFile f(fileName);

  bool isConstruction;

  //check to make sure the file won't crash us.
  if(f.open(QIODevice::ReadOnly)) {
    char block[4];
 
    f.readBlock(block, 4);

    if(block[0] || block[1] || block[2] || block[3] + 5 > (int)f.size()) {
      QMessageBox::warning(0, tr("Invalid file"),
			   tr("The file %1 does not look like a valid KSEG file.").arg(fileName),
			   QMessageBox::Ok, 0);
      f.close();
      return false;
    }

    
    f.ungetch(block[3]);
    f.ungetch(block[2]);
    f.ungetch(block[1]);
    f.ungetch(block[0]);
    

    QDataStream stream(&f);
    stream.setVersion(3);

    QString tmp;
    
    stream >> tmp;

    if(!KSegDocument::parseVersionString(tmp, &isConstruction)) {
      QMessageBox::warning(0, tr("Invalid file"),
			   tr("The file %1 does not look like a valid KSEG file.").arg(fileName),
			   QMessageBox::Ok, 0);
      f.close();
      return false;
    }


    f.close();
  }

  if(f.open(QIODevice::ReadOnly)) {
    QDataStream stream(&f);
    stream.setVersion(3);
    
    if(isConstruction) {
      if(win == NULL) win = new KSegWindow(new KSegConstruction());
      else win->changeDocument(new KSegConstruction());
    }
    else {
      if(win == NULL) win = new KSegWindow();
      else win->changeDocument(new KSegDocument());
    }

    stream >> *(win->view->getDocument());
    f.close();
  }

  win->view->getDocument()->emitDocumentSaved(fileName);
  
  win->show();

  win->view->redrawBuffer();

  return true;
}

void KSegWindow::fileCopyAsConstruction()
{
  KSegConstruction *c = new KSegConstruction();
  KSegWindow *w = new KSegWindow(c);

  QByteArray arr;
  QDataStream s(&arr, QIODevice::WriteOnly);
  s.setVersion(3);

  s << *(view->getDocument());

  QDataStream l(&arr, QIODevice::ReadOnly);
  l.setVersion(3);

  l >> (*c);

  w->show();
}

void KSegWindow::fileToSVG()
{
	QString fileName = QFileDialog::
            getSaveFileName(view->getDocument()->getFilename().replace(QRegExp("\\.se[cg]$"), ".svg"), "*.svg");
	if(fileName.isNull()) return;
	if(!QFileInfo(fileName).fileName().contains('.'))
		fileName = fileName + ".svg";

	QSvgGenerator gen;
	gen.setFileName(fileName);
	QRect docSize(0, 0, view->width() - KSegView::ScrollBarThickness,
								view->height() - KSegView::ScrollBarThickness);
	QRect vp = docSize;

	gen.setViewBox(vp);
	QPainter p(&gen);
	p.setViewport(vp);
	p.setWindow(docSize);

	view->getDocument()->print(p, ViewTransform(view->getOffsetX(), view->getOffsetY(), view->getZoom()));
	p.end();
	
}

void KSegWindow::filePrint()
{
  QPrinter *pr = view->getDocument()->getPrinter();

  if(pr->setup(this)) {
    pr->setFullPage(false);

    QPainter p(pr);

#if 1 //to go back to original fitting algorithm, set this 1 to a 0
    QRect viewPort = p.viewport();

    QRect docSize(0, 0, view->width() - KSegView::ScrollBarThickness,
		  view->height() - KSegView::ScrollBarThickness);

    QRect vp = docSize;

    vp.setWidth((vp.width() * pr->resolution()) / 96); //96 DPI looks good in my tests
    vp.setHeight((vp.height() * pr->resolution()) / 96);

    QPoint offs = viewPort.center() - vp.center();

    vp.moveBy(offs.x(), offs.y());

    p.setViewport(vp);

    p.setWindow(docSize);

    p.setClipRect(docSize);//, QPainter::CoordPainter);

    view->getDocument()->print(p, ViewTransform(view->getOffsetX(), view->getOffsetY(), view->getZoom()));
#else
    QRect docSize = view->getDocument()->getSize(ViewTransform(0, 0, 1));

    //make sure the document is at least 400 by 400
    QPoint oldCenter = docSize.center();
    if(docSize.width() < 400) docSize.setWidth(400);
    if(docSize.height() < 400) docSize.setHeight(400);
    docSize.moveCenter(oldCenter);

    QRect viewPort = p.viewport();

    if(docSize.width() * viewPort.height() >
       docSize.height() * viewPort.width()) {
      //the docSize needs to be made taller
      int oldHeight = docSize.height();
      int h = (docSize.width() * viewPort.height()) / viewPort.width();
      docSize.setHeight(h);
      docSize.moveBy(0, -(h - oldHeight) / 2);
    }
    else {
      //the docSize needs to be made wider
      int oldWidth = docSize.width();
      int w = (docSize.height() * viewPort.width()) / viewPort.height();
      docSize.setWidth(w);
      docSize.moveBy(-(w - oldWidth) / 2, 0);

    }

    p.setWindow(docSize);
    view->getDocument()->print(p, ViewTransform(0, 0, 1));
#endif
  }
}


void KSegWindow::fileExportToImage()
{
  QDialog dial(this, 0, true);
  QVBoxLayout layout(&dial);

  dial.setCaption(tr("E&xport To Image...").replace(QRegExp("&"), ""));
  QLabel lab(tr("Choose the File Format:"), &dial);
  QListWidget format(&dial);
//  format.insertStringList(QImage::outputFormatList());
  QList<QByteArray> fmtbytes = QImageWriter::supportedImageFormats();
  QList<QString> fmtlist;
  for(int i=0;i<fmtbytes.size();++i){fmtlist.append(fmtbytes[i]);}
  format.insertItems(0,QStringList(fmtlist));
  QCheckBox hiq(tr("High Quality Image"), &dial);
  QPushButton okBut(tr("OK"), &dial);
  connect(&okBut, SIGNAL(clicked()), &dial, SLOT(accept()));
  okBut.setDefault(true);

  layout.setMargin(10);
  layout.setSpacing(5);
  layout.addWidget(&lab);
  layout.addWidget(&format);
  layout.addSpacing(15);
  layout.addWidget(&hiq);
  layout.addSpacing(15);
  layout.addWidget(&okBut, 0, Qt::AlignHCenter);
  //dial.setLayout(layout);
  QList<QListWidgetItem *> pngItem = format.findItems("png",Qt::MatchFixedString);
  if(!pngItem.empty()) {
    //format.setSelected(pngItem, true);
    format.setCurrentItem(pngItem[0]);
  }

  dial.exec();

  if(dial.result() == QDialog::Rejected) return;

  QString fmt = format.currentItem()->text();
  QString ext = fmt.lower();
  if(ext == "jpeg") ext = "jpg";

  QString fileName = QFileDialog::
    getSaveFileName(QString::null, "*." + ext, 0, 0, tr("E&xport To Image...").replace(QRegExp("&"), ""));
  
  if(fileName.isNull()) return;

  QRect area = view->rect();
  area.setWidth(area.width() - KSegView::ScrollBarThickness);
  area.setHeight(area.height() - KSegView::ScrollBarThickness);

  if(hiq.isChecked()) {
    setCursor(QCursor(Qt::WaitCursor));
    repaint();
    statusBar()->message(tr("Exporting, Please wait..."));

    int qX = (area.width() + 3) / 4, qY = (area.height() + 3) / 4;
    QPixmap pmap(4 * (qX + 3), 4 * (qY + 3));
    
    QImage outImg(QPixmap(4 * qX, 4 * qY).convertToImage());
    
    for(int x = 0; x < 4; ++x) for(int y = 0; y < 4; ++y) {
      pmap.fill(QColor(KSegProperties::getProperty("BackgroundColor")));
      
      QPainter p(&pmap);
      
      int offsX = x * qX;
      int offsY = y * qY;
      
      p.setWindow(QRect(offsX - 1, offsY - 1, qX + 3, qY + 3));
      
      view->getDocument()->print(p, ViewTransform(view->getOffsetX(), view->getOffsetY(), view->getZoom()));
      
      QImage img(pmap.convertToImage());
    
      img = img.smoothScale(qX + 3, qY + 3);
      
      for(int x2 = 0; x2 < qX; ++x2) for(int y2 = 0; y2 < qY; ++y2) {
	outImg.setPixel(offsX + x2, offsY + y2, img.pixel(x2 + 1, y2 + 1));
      }
    }

    outImg.save(fileName, fmt);
    setCursor(QCursor(Qt::ArrowCursor));
    view->updateStatusBar(Qt::NoButton);
  }
  else {
    QPixmap pmap(area.width(), area.height());

    pmap.fill(QColor(KSegProperties::getProperty("BackgroundColor")));
      
    QPainter p(&pmap);
    
    view->getDocument()->print(p, ViewTransform(view->getOffsetX(), view->getOffsetY(), view->getZoom()));

    pmap.save(fileName, fmt);
  }

}


void KSegWindow::fileChooseLanguage()
{
  QString fileName = QFileDialog::getOpenFileName(KSegProperties::getProperty("LanguageDir") +
						  "/" + KSegProperties::getProperty("Language"),
						  "*.qm");

  if(fileName.isNull()) return;

  QString dir = QFileInfo(fileName).dirPath(true);
  QString file = QFileInfo(fileName).fileName();

  QTranslator *t = new QTranslator(0);
  if(t->load(file, dir)) {
    KSegProperties::setProperty("LanguageDir", dir);
    KSegProperties::setProperty("Language", file);
    
    qApp->installTranslator(t);

    QMessageBox::information(0, tr("Language Change"),
			     tr("You will need to restart KSEG for the language "
				"change to fully take effect."),
			     QMessageBox::Ok, 0);

    return;
  }

  delete t;
}


void KSegWindow::editPreferences()
{
  KSegPreferencesDialog p(this, 0, true);

  p.exec();
}


void KSegWindow::editColorBlack()
{
  view->getDocument()->setCurrentColor(Qt::black);
  updateMenus();
}

void KSegWindow::editColorGray()
{
  view->getDocument()->setCurrentColor(Qt::gray);
  updateMenus();
}

void KSegWindow::editColorRed()
{
  view->getDocument()->setCurrentColor(Qt::red);
  updateMenus();
}

void KSegWindow::editColorGreen()
{
  view->getDocument()->setCurrentColor(Qt::green);
  updateMenus();
}

void KSegWindow::editColorBlue()
{
  view->getDocument()->setCurrentColor(Qt::blue);
  updateMenus();
}

void KSegWindow::editColorYellow()
{
  view->getDocument()->setCurrentColor(Qt::yellow);
  updateMenus();
}

void KSegWindow::editColorMagenta()
{
  view->getDocument()->setCurrentColor(Qt::magenta);
  updateMenus();
}

void KSegWindow::editColorCyan()
{
  view->getDocument()->setCurrentColor(Qt::cyan);
  updateMenus();
}

void KSegWindow::editColorOther()
{
  QColor newColor;
  
  newColor = QColorDialog::getColor(view->getDocument()->getCurrentColor());

  if(newColor.isValid()) {
    view->getDocument()->setCurrentColor(newColor);
  }

  updateMenus();
}

//----------

void KSegWindow::editPointstyleLargecircle()
{
  view->getDocument()->setCurrentPointstyle(LARGE_CIRCLE);
  updateMenus();
}


void KSegWindow::editPointstyleMediumcircle()
{
  view->getDocument()->setCurrentPointstyle(MEDIUM_CIRCLE);
  updateMenus();
}


void KSegWindow::editPointstyleSmallcircle()
{
  view->getDocument()->setCurrentPointstyle(SMALL_CIRCLE);
  updateMenus();
}

//---------

void KSegWindow::editLinestyleSolid()
{
  view->getDocument()->setCurrentPenstyle(Qt::SolidLine);
  updateMenus();
}


void KSegWindow::editLinestyleDashed()
{
  view->getDocument()->setCurrentPenstyle(Qt::DashLine);
  updateMenus();
}


void KSegWindow::editLinestyleDotted()
{
  view->getDocument()->setCurrentPenstyle(Qt::DotLine);
  updateMenus();
}


void KSegWindow::editLinestyleThin()
{
  view->getDocument()->setCurrentPenWidth(0);
  updateMenus();
}


void KSegWindow::editLinestyleNormal()
{
  view->getDocument()->setCurrentPenWidth(2);
  updateMenus();
}


void KSegWindow::editLinestyleThick()
{
  view->getDocument()->setCurrentPenWidth(3);
  updateMenus();
}

//---------

void KSegWindow::editFont(int size)
{
  view->getDocument()->setCurrentFontsize(size);
  updateMenus();
}

void KSegWindow::editFontFont()
{
  bool ok;
  QFont f = QFontDialog::getFont(&ok, view->getDocument()->getCurrentFont());
  if(ok) {
    view->getDocument()->setCurrentFont(f);
  }
  updateMenus();
}

void KSegWindow::help()
{
  QString langChars = KSegProperties::getProperty("Language").mid(5, 2);
  QString helpSource = KSegProperties::getProperty("LanguageDir") + "/kseg_help_";
  helpSource = helpSource + langChars + ".html";
  //the above assumes the language is kseg_??.qm and the helpfile is in the language directory
  
  if(QFileInfo(helpSource).exists() == false)
    helpSource = KSegProperties::getProperty("LanguageDir") + "/kseg_help_en.html";

  if(QFileInfo(helpSource).exists() == false) {
    QMessageBox::warning(0, tr("File Not Found"),
			 tr("Sorry, I am afraid that file %1 does not exist.").arg(helpSource),
			 QMessageBox::Ok, 0);    

    return;
  }

  QTextBrowser *b = new QTextBrowser();
  b->setSource(helpSource);
  b->setCaption(b->documentTitle());
  b->resize(640, 480);
  b->show();
}

void KSegWindow::helpAbout()
{
  QMessageBox::about(this, tr("About KSEG"),
		     tr("KSEG v%1\n"
			"Copyright (C) %2 Yury P. Fedorchenko\n"
			"http://fedorchenko.net\n"
			"Copyright (C) %3 Ilya Baran\n"
			"http://www.mit.edu/~ibaran").arg("1.0_pre20140603").arg("2014").arg("1999-2006"));
}




