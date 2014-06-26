TEMPLATE	= app
QT		+= qt3support svg
CONFIG		= qt warn_on release
LIBS           += -lz
CFLAGS		+= -std=c++11

HEADERS = defs.H \
	  formula/box.H \
	  formula/kformulaedit.H \
	  formula/kformula.H \
	  formula/matrixbox.H \
	  formula/MatrixDialog.H \
	  G_arc.H \
	  G_arcSector.H \
	  G_arcSegment.H \
	  G_circle.H \
	  G_circleInterior.H \
	  G_drawstyle.H \
	  G_geometry.H \
	  G_label.H \
	  G_line.H \
	  G_locus.H \
	  G_matrix.H \
	  G_object.H \
	  G_point.H \
	  G_polygon.H \
	  G_ray.H \
	  G_ref.H \
	  G_refSearcher.H \
	  G_refs.H \
	  G_segment.H \
	  G_undo.H \
	  G_undoStack.H \
	  KSegCalculateEditor.H \
	  KSegConstruction.H \
	  KSegConstructionList.H \
	  KSegDocument.H \
	  KSegMenuMaintainer.H \
	  KSegPreferencesDialog.H \
	  KSegProperties.H \
	  KSegRecursionDialog.H \
	  KSegSelectionGroupDialog.H \
	  KSegSelectionGroup.H \
	  KSegView.H \
	  KSegWindow.H \
	  my_hash_map.H \
	  my_hash_set.H

SOURCES	= BuildReverse.cpp \
          cans.cpp \
	  formula/box.cpp \
	  formula/kformula.cpp \
	  formula/kformulaedit.cpp \
	  formula/matrixbox.cpp \
	  formula/MatrixDialog.cpp \
	  G_arc.cpp \
	  G_arcObject.cpp \
	  G_arcSector.cpp \
	  G_arcSectorObject.cpp \
	  G_arcSegment.cpp \
	  G_arcSegmentObject.cpp \
	  G_calculateObject.cpp \
	  G_circle.cpp \
	  G_circleInterior.cpp \
	  G_circleInteriorObject.cpp \
	  G_circleObject.cpp \
	  G_drawstyle.cpp \
	  G_label.cpp \
	  G_line.cpp \
	  G_lineObject.cpp \
	  G_locus.cpp \
	  G_locusObject.cpp \
	  G_measureObject.cpp \
	  G_object.cpp \
	  G_point.cpp \
	  G_pointLocus.cpp \
	  G_pointObject.cpp \
	  G_polygon.cpp \
	  G_polygonObject.cpp \
	  G_ray.cpp \
	  G_rayObject.cpp \
	  G_ref.cpp \
	  G_refs.cpp \
	  G_refSearcher.cpp \
	  G_segment.cpp \
	  G_segmentObject.cpp \
	  G_undo.cpp \
	  G_undoStack.cpp \
	  KSegCalculateEditor.cpp \
	  KSegConstruction.cpp \
	  KSegConstructionList.cpp \
	  KSegConstructionListNamer.cpp \
	  KSegDocument.cpp \
	  KSegMenuMaintainer.cpp \
	  KSegMenus.cpp \
	  KSegPreferencesDialog.cpp \
	  KSegProperties.cpp \
	  KSegRecursionDialog.cpp \
	  KSegSelectionGroupDialog.cpp \
	  KSegView.cpp \
	  KSegWindow.cpp \
	  main.cpp \
	  menu.cpp 

TRANSLATIONS    = kseg_en.ts \
		  kseg_xx.ts \
		  kseg_pt.ts \
		  kseg_fr.ts \
		  kseg_de.ts \
		  kseg_nb.ts \
		  kseg_hu.ts \
		  kseg_es.ts \
		  kseg_nl.ts \
		  kseg_it.ts \
		  kseg_ja.ts \
		  kseg_cy.ts \
		  kseg_tr.ts \
		  kseg_ru.ts \
		  kseg_tc.ts \
		  kseg_zh.ts

DISTFILES += README VERSION AUTHORS COPYING kseg.lsm \
             README.translators seg.plan INSTALL INSTALL_PT *.qm \
             *.html pics/*.xpm examples/*.seg examples/*.sec

trans.target =
trans.commands = lupdate kseg.pro; lrelease kseg.pro

QMAKE_EXTRA_TARGETS += trans
