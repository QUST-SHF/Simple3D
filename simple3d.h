//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

#ifndef SIMPLE_3D_HEADER
#define SIMPLE_3D_HEADER

#include "ui_simple3d.h"
#include "boxSize.h"
#include "recentFilesQMenu.h"
#include <QMainWindow>
#include <QMessageBox>

namespace Ui { class Simple3D; }

class OpenGLWidget;
class PartsModel;

class Simple3D : public QMainWindow, private Ui::Simple3D
{
    Q_OBJECT

public:
    explicit Simple3D(PartsModel * partsModel);
    ~Simple3D();

signals:
    void partFileSelected(const QString & partFileName);
    void workspaceResized(BoxSize newSize);

public slots:
    void browseForPartFile();
    void updateGui();
    void removeSelectedParts();
    void enableOrDisableUnloadButton();
    void informOfPartFailure() const;
    void informOfFailureToRepackAll() const;
    void resizeWorkspace();

private slots:
    void openAbout();
    void writeSettings();
    void readSettings();

private: // member functions
    void connectMenuActions();
    virtual void closeEvent(QCloseEvent *event) override;

private: // member variables
    OpenGLWidget * m_openGLWidget;
    PartsModel * m_partsModel;
    QString m_lastSourceDir;
    RecentFilesQMenu * m_recentFilesMenu;     // recent files menu
    QMessageBox m_aboutBox;
};

#endif // SIMPLE_3D_HEADER
