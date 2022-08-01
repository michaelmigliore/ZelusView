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

class ZelusViewMainWindow::Internals : public Ui::ZelusViewMainWindow
{
};

//-----------------------------------------------------------------------------
ZelusViewMainWindow::ZelusViewMainWindow()
{
    // Setup default GUI layout.
    mInternals = new Internals();
    mInternals->setupUi(this);

    // Setup the dock window corners to give the vertical docks more room.
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // Setup color editor
    // Provide access to the color-editor panel for the application and hide it
    pqApplicationCore::instance()->registerManager(
        "COLOR_EDITOR_PANEL", mInternals->colorMapEditorDock);
    mInternals->colorMapEditorDock->hide();

    // Build the sources/filters menu
    pqParaViewMenuBuilders::buildSourcesMenu(*mInternals->menuSources, this);
    pqParaViewMenuBuilders::buildFiltersMenu(*mInternals->menuFilters, this);

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
