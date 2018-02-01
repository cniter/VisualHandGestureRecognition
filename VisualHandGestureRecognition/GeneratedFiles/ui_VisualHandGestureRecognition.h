/********************************************************************************
** Form generated from reading UI file 'VisualHandGestureRecognition.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VISUALHANDGESTURERECOGNITION_H
#define UI_VISUALHANDGESTURERECOGNITION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VisualHandGestureRecognitionClass
{
public:
    QWidget *centralWidget;
    QLabel *label_show_color;
    QLabel *label_show_depth;
    QLabel *label_show_hand_gesture;
    QLabel *label_hand_gesture_num;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *VisualHandGestureRecognitionClass)
    {
        if (VisualHandGestureRecognitionClass->objectName().isEmpty())
            VisualHandGestureRecognitionClass->setObjectName(QStringLiteral("VisualHandGestureRecognitionClass"));
        VisualHandGestureRecognitionClass->resize(1470, 490);
        centralWidget = new QWidget(VisualHandGestureRecognitionClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label_show_color = new QLabel(centralWidget);
        label_show_color->setObjectName(QStringLiteral("label_show_color"));
        label_show_color->setGeometry(QRect(10, 10, 640, 480));
        label_show_depth = new QLabel(centralWidget);
        label_show_depth->setObjectName(QStringLiteral("label_show_depth"));
        label_show_depth->setGeometry(QRect(820, 10, 640, 480));
        label_show_hand_gesture = new QLabel(centralWidget);
        label_show_hand_gesture->setObjectName(QStringLiteral("label_show_hand_gesture"));
        label_show_hand_gesture->setGeometry(QRect(660, 120, 151, 191));
        label_hand_gesture_num = new QLabel(centralWidget);
        label_hand_gesture_num->setObjectName(QStringLiteral("label_hand_gesture_num"));
        label_hand_gesture_num->setGeometry(QRect(660, 330, 151, 41));
        label_hand_gesture_num->setAlignment(Qt::AlignCenter);
        label_hand_gesture_num->setWordWrap(false);
        VisualHandGestureRecognitionClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(VisualHandGestureRecognitionClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1470, 21));
        VisualHandGestureRecognitionClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(VisualHandGestureRecognitionClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        VisualHandGestureRecognitionClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(VisualHandGestureRecognitionClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        VisualHandGestureRecognitionClass->setStatusBar(statusBar);

        retranslateUi(VisualHandGestureRecognitionClass);

        QMetaObject::connectSlotsByName(VisualHandGestureRecognitionClass);
    } // setupUi

    void retranslateUi(QMainWindow *VisualHandGestureRecognitionClass)
    {
        VisualHandGestureRecognitionClass->setWindowTitle(QApplication::translate("VisualHandGestureRecognitionClass", "VisualHandGestureRecognition", 0));
        label_show_color->setText(QString());
        label_show_depth->setText(QString());
        label_show_hand_gesture->setText(QString());
        label_hand_gesture_num->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class VisualHandGestureRecognitionClass: public Ui_VisualHandGestureRecognitionClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VISUALHANDGESTURERECOGNITION_H
