//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#include "simple3d.h"
#include "partsModel.h"
#include "openGLWidget.h"
#include "dimEditDialog.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>


// Constructor.
Simple3D::Simple3D(PartsModel * partsModel) : m_partsModel(partsModel)
{
    // Set up the GUI as designed in the Qt Designer.
    setupUi(this);

    // Create an empty recent files menu.
    m_recentFilesMenu = new RecentFilesQMenu(this, true, 20);
    // Read in and apply the settings.
    readSettings();

    // Create an openGL widget and add it to the main window.
    m_openGLWidget = new OpenGLWidget(this, m_partsModel);
    viewerVertLayout->addWidget(m_openGLWidget);

    // Show the initial volume and number of triangles (both should be 0);
    updateGui();
    // Disable the "unload" button.
    unloadPushButton->setEnabled(false);

    // Launch browsing for a part file.
    connect(loadPushButton, SIGNAL(clicked()), actionLoad, SIGNAL(triggered()));
    // Add the selected part file.
    connect(this, SIGNAL(partFileSelected(const QString &)), m_partsModel, SLOT(addPart(const QString &)));
    // Remove the currently selected parts.
    connect(unloadPushButton, SIGNAL(clicked()), actionUnload, SIGNAL(triggered()));
    // Restore the focus on the drawings.
    connect(loadPushButton, SIGNAL(clicked()), m_openGLWidget, SLOT(restoreFocus()));
    connect(unloadPushButton, SIGNAL(clicked()), m_openGLWidget, SLOT(restoreFocus()));

    // Update the GUI once a part is added or removed.
    connect(m_partsModel, SIGNAL(partAdded()), this, SLOT(updateGui()));
    connect(m_partsModel, SIGNAL(partRemoved(int)), this, SLOT(updateGui()));
    // Update the list of buffers once a part is added or removed.
    connect(m_partsModel, SIGNAL(partAdded()), m_openGLWidget, SLOT(addBuffer()));
    connect(m_partsModel, SIGNAL(partRemoved(int)), m_openGLWidget, SLOT(removeBuffer(int)));
    // Invoke a message box when a new part can not be fit.
    connect(m_partsModel, SIGNAL(addingPartFailed()), this, SLOT(informOfPartFailure()));

    // Enable the "Unload" button when a part is selected.
    connect(m_openGLWidget, SIGNAL(selectedPartsChanged()), this, SLOT(enableOrDisableUnloadButton()));
    connect(m_openGLWidget, SIGNAL(selectedPartsChanged()), this, SLOT(updateGui()));

    // Try to repack with the new minimal gap.
    connect(gapDoubleSpinBox, SIGNAL(valueChanged(double)), m_partsModel, SLOT(repack(double)));
    // Restore the previous minimal gap value in the double spin box in case
    // packing with the new value failed.
    connect(m_partsModel, SIGNAL(resettingGapNeeded(double)), gapDoubleSpinBox, SLOT(setValue(double)));

    // Resize the workspace (aka master box).
    connect(this, SIGNAL(workspaceResized(BoxSize)), m_partsModel, SLOT(resizeMasterBox(BoxSize)));
    connect(m_partsModel, SIGNAL(masterBoxResized()), m_openGLWidget, SLOT(updateWhenMasterBoxChange()));
    // Invoke a message box if repacking loaded parts fails.
    connect(m_partsModel, SIGNAL(repackingFailed()), this, SLOT(informOfFailureToRepackAll()));

    // Connect menu actions to the corresponding slots that actually do the work.
    connectMenuActions();
}


// Destructor.
Simple3D::~Simple3D()
{
    delete m_openGLWidget;
    delete m_recentFilesMenu;
}


//=============================================================================
// The function "browseForPartFile" opens a file dialog allowing the user to
// select a part file.
//=============================================================================
void Simple3D::browseForPartFile()
{
    // Open a dialog allowing the user to select a source file.
    QString partFileName = QFileDialog::getOpenFileName(
                this, tr("Open File"), m_lastSourceDir, tr("STL (*.stl)"));

    // If no source was selected, then there is nothing to do.
    if (partFileName.isNull() == true)
        return;

    // Store the last source directory.
    m_lastSourceDir = QFileInfo(partFileName).dir().absolutePath();

    // Add the source to the list of recent files.
    m_recentFilesMenu->addFile(partFileName);

    // Emit signal that a part file is selected.
    emit partFileSelected(partFileName);
}


//=============================================================================
// The function "updateGui" updates the labels showing volume and number of
// triangles.
//=============================================================================
void Simple3D::updateGui()
{
    QLocale locale;
    volumeValueLabel->setText(locale.toString(m_partsModel->volume(m_openGLWidget->selectedParts()), 'f', 1) +
                              QStringLiteral("/") +
                              locale.toString(m_partsModel->totalVolume(), 'f', 1) +
                              QStringLiteral(" mL"));
    facesValueLabel->setText(QStringLiteral("%L1").arg(m_partsModel->numVertices(m_openGLWidget->selectedParts()) / 3) +
                             QStringLiteral("/") +
                             QStringLiteral("%L1").arg(m_partsModel->totalNumVertices() / 3));
    gapDoubleSpinBox->setValue(m_partsModel->minGapBetweenParts());
}


//=============================================================================
// The function "removeSelectedParts" removes the currently selected parts.
//=============================================================================
void Simple3D::removeSelectedParts()
{
    // Keep a copy of the indices of the selected parts.
    QSet<int> selectedParts = m_openGLWidget->selectedParts();
    // Clear the list of selected parts indices.
    m_openGLWidget->clearSelectedParts();
    // Remove the selected parts from the model.
    m_partsModel->removeParts(selectedParts);
    // Disable the "unload" button.
    unloadPushButton->setEnabled(false);
}


//=============================================================================
// The function "enableOrDisableUnloadButton" enables or disables the "Unload"
// button.
//=============================================================================
void Simple3D::enableOrDisableUnloadButton()
{
    if (m_openGLWidget->selectedParts().empty() == false)
        unloadPushButton->setEnabled(true);
    else
        unloadPushButton->setEnabled(false);
}


//=============================================================================
// The function "informOfPartFailure" opens a message box to inform the user
// that the application failed to accommodate the last specified part in the
// printing area along with the previously loaded parts.
//=============================================================================
void Simple3D::informOfPartFailure() const
{
    QString text = "Unfortunately, the application is unable "
                   "to accommodate the last specified part in the "
                   "printing area along with the previously loaded parts.\n\n"
                   "You still might be able to load smaller parts.";
    QMessageBox messageBox(QMessageBox::Warning, QStringLiteral("Failure to Accommodate"), text, QMessageBox::Ok);
    messageBox.exec();
}


//=============================================================================
// The function "informOfFailureToRepackAll" opens a message box to inform the
// user that the application failed to repack all loaded part (in a new,
// smaller printing area).
//=============================================================================
void Simple3D::informOfFailureToRepackAll() const
{
    QString text = "Unfortunately, the application is unable to "
                   "repack all loaded models in the printing area.\n\n"
                   "You can start loading models anew.";
    QMessageBox messageBox(QMessageBox::Warning, QStringLiteral("Failure to Repack"), text, QMessageBox::Ok);
    messageBox.exec();
}


//=============================================================================
// The function "connectMenuActions" connects menu actions to the corresponding
// slots that actually do the work.
//=============================================================================
void Simple3D::connectMenuActions()
{
    // Load an STL file.
    actionLoad->setStatusTip(tr("Load an STL file"));
    actionLoad->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogStart));
    connect(actionLoad, SIGNAL(triggered()), this, SLOT(browseForPartFile()));

    // Unload selected models.
    actionUnload->setStatusTip(tr("Unload selected models"));
    connect(actionUnload, SIGNAL(triggered()), this, SLOT(removeSelectedParts()));

    // Access recent files menu.
    actionRecentFiles->setMenu(m_recentFilesMenu);
    connect(m_recentFilesMenu, SIGNAL(sourceSelected(const QString &)), m_partsModel, SLOT(addPart(const QString &)));

    // Close the application.
    actionExit->setStatusTip(tr("Close the application"));
    actionExit->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

    // Resize the workspace.
    actionResizeWorkspace->setStatusTip(tr("Resize the workspace"));
    connect(actionResizeWorkspace, SIGNAL(triggered()), this, SLOT(resizeWorkspace()));

    // Open the About message box.
    actionAbout->setStatusTip(tr("About this application"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(openAbout()));
}


//=============================================================================
// The function "resizeWorkspace" opens a dialog allowing the user to modify
// the dimensions of the workspace (aka master box).
//=============================================================================
void Simple3D::resizeWorkspace()
{
    DimEditDialog dimDialog(m_partsModel->masterBox(), this);
    int isAccepted = dimDialog.exec();
    if (isAccepted == QDialog::Accepted)
        emit workspaceResized(BoxSize(dimDialog.xDim(), dimDialog.yDim(), dimDialog.zDim()));
}


//=============================================================================
// The function "openAbout" opens the Simple3D's About message box.
//=============================================================================
void Simple3D::openAbout()
{
    QString textStr = "<big><b> Simple3D 0.1 beta </b></big><br>"
            "<br>"
            "Copyright 2014-2015 Borislav Karaivanov. All rights reserved. <br>"
            "<br>"
            "The program is provided AS IS with NO WARRANTY OF ANY KIND, <br>"
            "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND <BR>"
            "FITNESS FOR A PARTICULAR PURPOSE.";

    m_aboutBox.about(this, "About Simple3D", textStr);
}


//=============================================================================
// The function "writeSettings" saves the size and position of the application
// window so that they can be recovered next time the application is launched.
//=============================================================================
void Simple3D::writeSettings()
{
    QSettings settings("Simple3D", "Simple3D");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("isFullScreen", windowState() == Qt::WindowFullScreen);
    settings.setValue("recentFilesList", m_recentFilesMenu->files());
    settings.setValue("masterBox", m_partsModel->masterBox());
    settings.setValue("minGapBetweenParts", m_partsModel->minGapBetweenParts());
    settings.endGroup();
}


//=============================================================================
// The function "readSettings" reads the size and position of the application
// window properly stored at the end of the previous session.
//=============================================================================
void Simple3D::readSettings()
{
    QSettings settings("Simple3D", "Simple3D");

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    if (settings.value("isFullScreen", false).toBool() == true)
        setWindowState(Qt::WindowFullScreen);
    else
        setWindowState(Qt::WindowNoState);
    // Set the read recent files in the recent files menu.
    m_recentFilesMenu->setFileList(settings.value("recentFilesList").toStringList());
    // Set the dimensions of the workspace (aka printing area).
    QVariant v = settings.value("masterBox", QVector3D(200, 200, 150));
    m_partsModel->resizeMasterBox(BoxSize(v.value<QVector3D>()));
    // Set the munimal gap between parts.
    m_partsModel->setMinGapBetweenParts(settings.value("minGapBetweenParts", 1.0f).toFloat());
    settings.endGroup();
}


//=============================================================================
// The function "closeEvent" finishes everything that needs to be done before
// the application is closed (e.g., updates and saves preferences, saves the
// project, etc.).
// INPUT: "QCloseEvent *event" is the close event that may be accepted (to
// close the application) or ignored (to keep it alive).
//=============================================================================
void Simple3D::closeEvent(QCloseEvent *event)
{
    // Save the current settings.
    writeSettings();
    // Accept the close event, i.e., the application is going to close.
    event->accept();
}
