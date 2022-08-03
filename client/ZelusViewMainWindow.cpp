#include "ZelusViewMainWindow.h"
#include "ui_ZelusViewMainWindow.h"

#include "pqApplicationCore.h"
#include "pqCategoryToolbarsBehavior.h"
#include "pqColorToolbar.h"
#include "pqLoadDataReaction.h"
#include "pqParaViewBehaviors.h"
#include "pqParaViewMenuBuilders.h"
#include "pqRepresentationToolbar.h"
#include "pqApplicationCore.h"
#include "pqMainWindowEventManager.h"
#include "pqVCRToolbar.h"
#include "pqAnimationTimeToolbar.h"

#include "vtkSMReaderFactory.h"

class ZelusViewMainWindow::Internals : public Ui::ZelusViewMainWindow
{
};

//-----------------------------------------------------------------------------
ZelusViewMainWindow::ZelusViewMainWindow()
{
	// Setup default GUI layout.
	mInternals = new Internals();
	mInternals->setupUi(this);

	// show output widget if we received an error message.
	this->connect(mInternals->outputWidget, SIGNAL(messageDisplayed(const QString&, int)),
		SLOT(handleMessage(const QString&, int)));
	mInternals->outputWidgetDock->hide();

	// Setup the dock window corners to give the vertical docks more room.
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	// Setup color editor
	// Provide access to the color-editor panel for the application and hide it
	pqApplicationCore::instance()->registerManager(
		"COLOR_EDITOR_PANEL", mInternals->colorMapEditorDock);

	// tabify some widgets
	this->tabifyDockWidget(mInternals->informationDock, mInternals->propertiesDock);
	this->tabifyDockWidget(mInternals->selectionDisplayDock, mInternals->colorMapEditorDock);

	// Build the menus
	pqParaViewMenuBuilders::buildSourcesMenu(*mInternals->menuSources, this);
	pqParaViewMenuBuilders::buildFiltersMenu(*mInternals->menuFilters, this);
	pqParaViewMenuBuilders::buildFileMenu(*mInternals->menu_File);
	pqParaViewMenuBuilders::buildEditMenu(*mInternals->menu_Edit, mInternals->propertiesPanel);
	pqParaViewMenuBuilders::buildViewMenu(*mInternals->menu_View, *this);

	// Setup the context menu for the pipeline browser.
	pqParaViewMenuBuilders::buildPipelineBrowserContextMenu(
		*mInternals->pipelineBrowser->contextMenu());

	// Add the ColorToolbar
	QToolBar* colorToolbar = new pqColorToolbar(this);
	colorToolbar->layout()->setSpacing(0);
	addToolBar(colorToolbar);

	// Add the Representation Toolbar
	QToolBar* reprToolbar = new pqRepresentationToolbar(this);
	reprToolbar->setObjectName("Representation");
	reprToolbar->layout()->setSpacing(0);
	addToolBar(reprToolbar);

	// white list readers
	vtkSMReaderFactory::AddReaderToWhitelist("sources", "objreader");
	vtkSMReaderFactory::AddReaderToWhitelist("sources", "stlreader");
	vtkSMReaderFactory::AddReaderToWhitelist("sources", "glTF 2.0 Reader");
	vtkSMReaderFactory::AddReaderToWhitelist("sources", "ZelusReader");

	// Final step, define application behaviors. Since we want all ParaView
	// behaviors, we use this convenience method.
	new pqParaViewBehaviors(this, this);
}

//-----------------------------------------------------------------------------
ZelusViewMainWindow::~ZelusViewMainWindow()
{
	delete mInternals;
}

//-----------------------------------------------------------------------------
void ZelusViewMainWindow::dragEnterEvent(QDragEnterEvent* evt)
{
	pqApplicationCore::instance()->getMainWindowEventManager()->dragEnterEvent(evt);
}

//-----------------------------------------------------------------------------
void ZelusViewMainWindow::dropEvent(QDropEvent* evt)
{
	pqApplicationCore::instance()->getMainWindowEventManager()->dropEvent(evt);
}

//-----------------------------------------------------------------------------
void ZelusViewMainWindow::showEvent(QShowEvent* evt)
{
	QMainWindow::showEvent(evt);

	pqApplicationCore::instance()->getMainWindowEventManager()->showEvent(evt);
}

//-----------------------------------------------------------------------------
void ZelusViewMainWindow::closeEvent(QCloseEvent* evt)
{
	pqApplicationCore::instance()->getMainWindowEventManager()->closeEvent(evt);
}

//-----------------------------------------------------------------------------
void ZelusViewMainWindow::handleMessage(const QString&, int type)
{
	QDockWidget* dock = mInternals->outputWidgetDock;
	if (!dock->isVisible() && (type == QtCriticalMsg || type == QtFatalMsg || type == QtWarningMsg))
	{
		// if dock is not visible, we always pop it up as a floating dialog. This
		// avoids causing re-renders which may cause more errors and more confusion.
		QRect rectApp = this->geometry();

		QRect rectDock(
			QPoint(0, 0), QSize(static_cast<int>(rectApp.width() * 0.4), dock->sizeHint().height()));
		rectDock.moveCenter(
			QPoint(rectApp.center().x(), rectApp.bottom() - dock->sizeHint().height() / 2));
		dock->setFloating(true);
		dock->setGeometry(rectDock);
		dock->show();
	}
	if (dock->isVisible())
	{
		dock->raise();
	}
}
