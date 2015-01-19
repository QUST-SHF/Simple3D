/********************************************************************************
** Form generated from reading UI file 'dimEditDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIMEDITDIALOG_H
#define UI_DIMEDITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_DimEditDialog
{
public:
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *xLabel;
    QSpinBox *xSpinBox;
    QLabel *yLabel;
    QSpinBox *ySpinBox;
    QLabel *yLabel_2;
    QSpinBox *zSpinBox;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DimEditDialog)
    {
        if (DimEditDialog->objectName().isEmpty())
            DimEditDialog->setObjectName(QStringLiteral("DimEditDialog"));
        DimEditDialog->resize(178, 166);
        gridLayout_3 = new QGridLayout(DimEditDialog);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        xLabel = new QLabel(DimEditDialog);
        xLabel->setObjectName(QStringLiteral("xLabel"));
        xLabel->setLayoutDirection(Qt::LeftToRight);
        xLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(xLabel, 0, 0, 1, 1);

        xSpinBox = new QSpinBox(DimEditDialog);
        xSpinBox->setObjectName(QStringLiteral("xSpinBox"));
        xSpinBox->setMaximum(9999);
        xSpinBox->setValue(200);

        gridLayout->addWidget(xSpinBox, 0, 1, 1, 1);

        yLabel = new QLabel(DimEditDialog);
        yLabel->setObjectName(QStringLiteral("yLabel"));
        yLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(yLabel, 1, 0, 1, 1);

        ySpinBox = new QSpinBox(DimEditDialog);
        ySpinBox->setObjectName(QStringLiteral("ySpinBox"));
        ySpinBox->setMaximum(9999);
        ySpinBox->setValue(200);

        gridLayout->addWidget(ySpinBox, 1, 1, 1, 1);

        yLabel_2 = new QLabel(DimEditDialog);
        yLabel_2->setObjectName(QStringLiteral("yLabel_2"));
        yLabel_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(yLabel_2, 2, 0, 1, 1);

        zSpinBox = new QSpinBox(DimEditDialog);
        zSpinBox->setObjectName(QStringLiteral("zSpinBox"));
        zSpinBox->setMaximum(9999);
        zSpinBox->setValue(200);

        gridLayout->addWidget(zSpinBox, 2, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 1, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DimEditDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_2->addWidget(buttonBox, 2, 0, 1, 2);


        gridLayout_3->addLayout(gridLayout_2, 0, 0, 1, 1);


        retranslateUi(DimEditDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DimEditDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DimEditDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DimEditDialog);
    } // setupUi

    void retranslateUi(QDialog *DimEditDialog)
    {
        DimEditDialog->setWindowTitle(QApplication::translate("DimEditDialog", "Workspace", 0));
        xLabel->setText(QApplication::translate("DimEditDialog", "X", 0));
        xSpinBox->setSuffix(QApplication::translate("DimEditDialog", " mm", 0));
        yLabel->setText(QApplication::translate("DimEditDialog", "Y", 0));
        ySpinBox->setSuffix(QApplication::translate("DimEditDialog", " mm", 0));
        yLabel_2->setText(QApplication::translate("DimEditDialog", "Z", 0));
        zSpinBox->setSuffix(QApplication::translate("DimEditDialog", " mm", 0));
    } // retranslateUi

};

namespace Ui {
    class DimEditDialog: public Ui_DimEditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIMEDITDIALOG_H
