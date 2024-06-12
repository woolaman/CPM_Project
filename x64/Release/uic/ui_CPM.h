/********************************************************************************
** Form generated from reading UI file 'CPM.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CPM_H
#define UI_CPM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CPMClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CPMClass)
    {
        if (CPMClass->objectName().isEmpty())
            CPMClass->setObjectName(QString::fromUtf8("CPMClass"));
        CPMClass->resize(600, 400);
        menuBar = new QMenuBar(CPMClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        CPMClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CPMClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        CPMClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(CPMClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        CPMClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(CPMClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        CPMClass->setStatusBar(statusBar);

        retranslateUi(CPMClass);

        QMetaObject::connectSlotsByName(CPMClass);
    } // setupUi

    void retranslateUi(QMainWindow *CPMClass)
    {
        CPMClass->setWindowTitle(QCoreApplication::translate("CPMClass", "CPM", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CPMClass: public Ui_CPMClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CPM_H
