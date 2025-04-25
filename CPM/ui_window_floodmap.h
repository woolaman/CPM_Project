/********************************************************************************
** Form generated from reading UI file 'window_floodmap.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW_FLOODMAP_H
#define UI_WINDOW_FLOODMAP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_window_floodmap
{
public:
    QLabel *label_floodmap;

    void setupUi(QWidget *window_floodmap)
    {
        if (window_floodmap->objectName().isEmpty())
            window_floodmap->setObjectName(QString::fromUtf8("window_floodmap"));
        window_floodmap->resize(1024, 1024);
        label_floodmap = new QLabel(window_floodmap);
        label_floodmap->setObjectName(QString::fromUtf8("label_floodmap"));
        label_floodmap->setGeometry(QRect(0, 0, 1024, 1024));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(10);
        label_floodmap->setFont(font);
        label_floodmap->setAlignment(Qt::AlignCenter);

        retranslateUi(window_floodmap);

        QMetaObject::connectSlotsByName(window_floodmap);
    } // setupUi

    void retranslateUi(QWidget *window_floodmap)
    {
        window_floodmap->setWindowTitle(QCoreApplication::translate("window_floodmap", "Floodmap", nullptr));
        label_floodmap->setText(QCoreApplication::translate("window_floodmap", "iamge_area", nullptr));
    } // retranslateUi

};

namespace Ui {
    class window_floodmap: public Ui_window_floodmap {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_FLOODMAP_H
