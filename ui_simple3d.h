/********************************************************************************
** Form generated from reading UI file 'simple3d.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMPLE3D_H
#define UI_SIMPLE3D_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Simple3D
{
public:
    QAction *actionLoad;
    QAction *actionRecentFiles;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionUnload;
    QAction *actionResizeWorkspace;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *viewerVertLayout;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QLabel *volumeTextLabel;
    QLabel *volumeValueLabel;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout;
    QLabel *gapLabel;
    QDoubleSpinBox *gapDoubleSpinBox;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *facesTextLabel;
    QLabel *facesValueLabel;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *loadPushButton;
    QPushButton *unloadPushButton;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuSettings;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Simple3D)
    {
        if (Simple3D->objectName().isEmpty())
            Simple3D->setObjectName(QStringLiteral("Simple3D"));
        Simple3D->resize(813, 600);
        actionLoad = new QAction(Simple3D);
        actionLoad->setObjectName(QStringLiteral("actionLoad"));
        actionRecentFiles = new QAction(Simple3D);
        actionRecentFiles->setObjectName(QStringLiteral("actionRecentFiles"));
        actionExit = new QAction(Simple3D);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionAbout = new QAction(Simple3D);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionUnload = new QAction(Simple3D);
        actionUnload->setObjectName(QStringLiteral("actionUnload"));
        actionResizeWorkspace = new QAction(Simple3D);
        actionResizeWorkspace->setObjectName(QStringLiteral("actionResizeWorkspace"));
        centralwidget = new QWidget(Simple3D);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        viewerVertLayout = new QHBoxLayout();
        viewerVertLayout->setObjectName(QStringLiteral("viewerVertLayout"));

        verticalLayout->addLayout(viewerVertLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        volumeTextLabel = new QLabel(centralwidget);
        volumeTextLabel->setObjectName(QStringLiteral("volumeTextLabel"));

        horizontalLayout_3->addWidget(volumeTextLabel);

        volumeValueLabel = new QLabel(centralwidget);
        volumeValueLabel->setObjectName(QStringLiteral("volumeValueLabel"));

        horizontalLayout_3->addWidget(volumeValueLabel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        horizontalLayout_5->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        gapLabel = new QLabel(centralwidget);
        gapLabel->setObjectName(QStringLiteral("gapLabel"));

        horizontalLayout->addWidget(gapLabel);

        gapDoubleSpinBox = new QDoubleSpinBox(centralwidget);
        gapDoubleSpinBox->setObjectName(QStringLiteral("gapDoubleSpinBox"));
        gapDoubleSpinBox->setDecimals(1);
        gapDoubleSpinBox->setMinimum(0);
        gapDoubleSpinBox->setSingleStep(0.5);
        gapDoubleSpinBox->setValue(1);

        horizontalLayout->addWidget(gapDoubleSpinBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        horizontalLayout_5->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        facesTextLabel = new QLabel(centralwidget);
        facesTextLabel->setObjectName(QStringLiteral("facesTextLabel"));

        horizontalLayout_4->addWidget(facesTextLabel);

        facesValueLabel = new QLabel(centralwidget);
        facesValueLabel->setObjectName(QStringLiteral("facesValueLabel"));

        horizontalLayout_4->addWidget(facesValueLabel);


        horizontalLayout_5->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        loadPushButton = new QPushButton(centralwidget);
        loadPushButton->setObjectName(QStringLiteral("loadPushButton"));

        horizontalLayout_2->addWidget(loadPushButton);

        unloadPushButton = new QPushButton(centralwidget);
        unloadPushButton->setObjectName(QStringLiteral("unloadPushButton"));

        horizontalLayout_2->addWidget(unloadPushButton);


        horizontalLayout_5->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(horizontalLayout_5);


        verticalLayout_2->addLayout(verticalLayout);

        Simple3D->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Simple3D);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 813, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName(QStringLiteral("menuSettings"));
        Simple3D->setMenuBar(menubar);
        statusbar = new QStatusBar(Simple3D);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        Simple3D->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuSettings->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionLoad);
        menuFile->addAction(actionUnload);
        menuFile->addAction(actionRecentFiles);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);
        menuSettings->addAction(actionResizeWorkspace);

        retranslateUi(Simple3D);

        QMetaObject::connectSlotsByName(Simple3D);
    } // setupUi

    void retranslateUi(QMainWindow *Simple3D)
    {
        Simple3D->setWindowTitle(QApplication::translate("Simple3D", "Simple3D", 0));
        actionLoad->setText(QApplication::translate("Simple3D", "Load", 0));
        actionLoad->setShortcut(QApplication::translate("Simple3D", "Ctrl+L", 0));
        actionRecentFiles->setText(QApplication::translate("Simple3D", "Recent Files", 0));
        actionExit->setText(QApplication::translate("Simple3D", "Exit", 0));
        actionExit->setShortcut(QApplication::translate("Simple3D", "Ctrl+X", 0));
        actionAbout->setText(QApplication::translate("Simple3D", "About", 0));
        actionUnload->setText(QApplication::translate("Simple3D", "Unload", 0));
        actionUnload->setShortcut(QApplication::translate("Simple3D", "Ctrl+U", 0));
        actionResizeWorkspace->setText(QApplication::translate("Simple3D", "Resize Workspace", 0));
        actionResizeWorkspace->setShortcut(QApplication::translate("Simple3D", "Ctrl+W", 0));
        volumeTextLabel->setText(QApplication::translate("Simple3D", "Volume (selected/all): ", 0));
        volumeValueLabel->setText(QApplication::translate("Simple3D", "0/0", 0));
#ifndef QT_NO_TOOLTIP
        gapLabel->setToolTip(QApplication::translate("Simple3D", "Minimal gap between adjacent models.", 0));
#endif // QT_NO_TOOLTIP
        gapLabel->setText(QApplication::translate("Simple3D", "Gap", 0));
#ifndef QT_NO_TOOLTIP
        gapDoubleSpinBox->setToolTip(QApplication::translate("Simple3D", "Minimal gap to be maintained between adjacent models.", 0));
#endif // QT_NO_TOOLTIP
        gapDoubleSpinBox->setSuffix(QApplication::translate("Simple3D", " mm", 0));
        facesTextLabel->setText(QApplication::translate("Simple3D", "Number of faces (selected/all):", 0));
        facesValueLabel->setText(QApplication::translate("Simple3D", "0/0", 0));
#ifndef QT_NO_TOOLTIP
        loadPushButton->setToolTip(QApplication::translate("Simple3D", "Load an STL file.", 0));
#endif // QT_NO_TOOLTIP
        loadPushButton->setText(QApplication::translate("Simple3D", "Load", 0));
#ifndef QT_NO_TOOLTIP
        unloadPushButton->setToolTip(QApplication::translate("Simple3D", "Delete all selected models.", 0));
#endif // QT_NO_TOOLTIP
        unloadPushButton->setText(QApplication::translate("Simple3D", "Unload", 0));
        menuFile->setTitle(QApplication::translate("Simple3D", "File", 0));
        menuHelp->setTitle(QApplication::translate("Simple3D", "Help", 0));
        menuSettings->setTitle(QApplication::translate("Simple3D", "Settings", 0));
    } // retranslateUi

};

namespace Ui {
    class Simple3D: public Ui_Simple3D {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMPLE3D_H
