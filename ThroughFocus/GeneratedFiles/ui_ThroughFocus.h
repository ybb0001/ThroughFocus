/********************************************************************************
** Form generated from reading UI file 'ThroughFocus.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THROUGHFOCUS_H
#define UI_THROUGHFOCUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mQWidget
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QPushButton *pushButton_Save;
    QLabel *label_7;
    QTextEdit *data_path;
    QPushButton *pushButton_Manual;
    QPushButton *pushButton_Start;
    QTextBrowser *log;
    QTextBrowser *result;
    QCheckBox *F3;
    QCheckBox *F5;
    QCheckBox *F6;
    QCheckBox *F7;
    QCheckBox *F8;
    QGroupBox *groupBox;
    QPushButton *pushButton_open;
    QPushButton *pushButton_Test_All;
    QPushButton *pushButton_Diagram;
    QCheckBox *curve_fit;
    QCheckBox *MTF;
    QCheckBox *move_Center;
    QPushButton *pushButton_MTF_csv;
    QCheckBox *AA_SFR;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QLabel *label_9;

    void setupUi(QWidget *mQWidget)
    {
        if (mQWidget->objectName().isEmpty())
            mQWidget->setObjectName(QStringLiteral("mQWidget"));
        mQWidget->resize(580, 400);
        mQWidget->setMinimumSize(QSize(0, 0));
        mQWidget->setMaximumSize(QSize(720, 400));
        mQWidget->setProperty("iconSize", QVariant(QSize(24, 24)));
        mQWidget->setProperty("animated", QVariant(true));
        centralWidget = new QWidget(mQWidget);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setGeometry(QRect(0, 0, 581, 401));
        centralWidget->setMinimumSize(QSize(0, 0));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 5, 231, 16));
        pushButton_Save = new QPushButton(centralWidget);
        pushButton_Save->setObjectName(QStringLiteral("pushButton_Save"));
        pushButton_Save->setGeometry(QRect(500, 270, 75, 25));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 320, 231, 16));
        data_path = new QTextEdit(centralWidget);
        data_path->setObjectName(QStringLiteral("data_path"));
        data_path->setGeometry(QRect(10, 340, 561, 40));
        pushButton_Manual = new QPushButton(centralWidget);
        pushButton_Manual->setObjectName(QStringLiteral("pushButton_Manual"));
        pushButton_Manual->setGeometry(QRect(500, 140, 75, 40));
        pushButton_Start = new QPushButton(centralWidget);
        pushButton_Start->setObjectName(QStringLiteral("pushButton_Start"));
        pushButton_Start->setEnabled(false);
        pushButton_Start->setGeometry(QRect(500, 30, 75, 30));
        log = new QTextBrowser(centralWidget);
        log->setObjectName(QStringLiteral("log"));
        log->setGeometry(QRect(350, 20, 141, 121));
        result = new QTextBrowser(centralWidget);
        result->setObjectName(QStringLiteral("result"));
        result->setGeometry(QRect(10, 20, 331, 301));
        F3 = new QCheckBox(centralWidget);
        F3->setObjectName(QStringLiteral("F3"));
        F3->setGeometry(QRect(360, 220, 51, 16));
        F5 = new QCheckBox(centralWidget);
        F5->setObjectName(QStringLiteral("F5"));
        F5->setGeometry(QRect(360, 240, 51, 16));
        F5->setChecked(true);
        F6 = new QCheckBox(centralWidget);
        F6->setObjectName(QStringLiteral("F6"));
        F6->setGeometry(QRect(360, 260, 51, 16));
        F7 = new QCheckBox(centralWidget);
        F7->setObjectName(QStringLiteral("F7"));
        F7->setGeometry(QRect(360, 280, 51, 16));
        F8 = new QCheckBox(centralWidget);
        F8->setObjectName(QStringLiteral("F8"));
        F8->setGeometry(QRect(360, 300, 51, 16));
        F8->setChecked(true);
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(350, 200, 70, 120));
        pushButton_open = new QPushButton(centralWidget);
        pushButton_open->setObjectName(QStringLiteral("pushButton_open"));
        pushButton_open->setGeometry(QRect(500, 310, 75, 25));
        pushButton_Test_All = new QPushButton(centralWidget);
        pushButton_Test_All->setObjectName(QStringLiteral("pushButton_Test_All"));
        pushButton_Test_All->setEnabled(true);
        pushButton_Test_All->setGeometry(QRect(500, 100, 75, 40));
        pushButton_Test_All->setCheckable(false);
        pushButton_Diagram = new QPushButton(centralWidget);
        pushButton_Diagram->setObjectName(QStringLiteral("pushButton_Diagram"));
        pushButton_Diagram->setGeometry(QRect(500, 245, 75, 25));
        curve_fit = new QCheckBox(centralWidget);
        curve_fit->setObjectName(QStringLiteral("curve_fit"));
        curve_fit->setGeometry(QRect(430, 300, 71, 16));
        MTF = new QCheckBox(centralWidget);
        MTF->setObjectName(QStringLiteral("MTF"));
        MTF->setGeometry(QRect(430, 280, 51, 16));
        move_Center = new QCheckBox(centralWidget);
        move_Center->setObjectName(QStringLiteral("move_Center"));
        move_Center->setGeometry(QRect(430, 260, 71, 16));
        pushButton_MTF_csv = new QPushButton(centralWidget);
        pushButton_MTF_csv->setObjectName(QStringLiteral("pushButton_MTF_csv"));
        pushButton_MTF_csv->setGeometry(QRect(500, 190, 75, 25));
        AA_SFR = new QCheckBox(centralWidget);
        AA_SFR->setObjectName(QStringLiteral("AA_SFR"));
        AA_SFR->setGeometry(QRect(430, 240, 71, 16));
        AA_SFR->setChecked(true);
        groupBox->raise();
        label->raise();
        pushButton_Save->raise();
        label_7->raise();
        data_path->raise();
        pushButton_Manual->raise();
        pushButton_Start->raise();
        log->raise();
        result->raise();
        F3->raise();
        F5->raise();
        F6->raise();
        F7->raise();
        F8->raise();
        pushButton_open->raise();
        pushButton_Test_All->raise();
        pushButton_Diagram->raise();
        curve_fit->raise();
        MTF->raise();
        move_Center->raise();
        pushButton_MTF_csv->raise();
        AA_SFR->raise();
        mainToolBar = new QToolBar(mQWidget);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setGeometry(QRect(0, 0, 4, 12));
        statusBar = new QStatusBar(mQWidget);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        statusBar->setGeometry(QRect(0, 0, 3, 18));
        label_9 = new QLabel(mQWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(310, 380, 221, 20));

        retranslateUi(mQWidget);

        QMetaObject::connectSlotsByName(mQWidget);
    } // setupUi

    void retranslateUi(QWidget *mQWidget)
    {
        mQWidget->setWindowTitle(QApplication::translate("mQWidget", "TF_Simulation_230220", Q_NULLPTR));
        label->setText(QApplication::translate("mQWidget", "SW Last TF Data Check Result ", Q_NULLPTR));
        pushButton_Save->setText(QApplication::translate("mQWidget", "Save", Q_NULLPTR));
        label_7->setText(QApplication::translate("mQWidget", "TF_Data_Path", Q_NULLPTR));
        data_path->setHtml(QApplication::translate("mQWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", Q_NULLPTR));
        pushButton_Manual->setText(QApplication::translate("mQWidget", "Last Data\n"
"ReadTest", Q_NULLPTR));
        pushButton_Start->setText(QApplication::translate("mQWidget", "Auto Stop", Q_NULLPTR));
        log->setHtml(QApplication::translate("mQWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:36pt;\">N/A</span></p></body></html>", Q_NULLPTR));
        F3->setText(QApplication::translate("mQWidget", "0.3F", Q_NULLPTR));
        F5->setText(QApplication::translate("mQWidget", "0.5F", Q_NULLPTR));
        F6->setText(QApplication::translate("mQWidget", "0.6F", Q_NULLPTR));
        F7->setText(QApplication::translate("mQWidget", "0.7F", Q_NULLPTR));
        F8->setText(QApplication::translate("mQWidget", "0.8F", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("mQWidget", "PS_PD Test", Q_NULLPTR));
        pushButton_open->setText(QApplication::translate("mQWidget", "Open Path", Q_NULLPTR));
        pushButton_Test_All->setText(QApplication::translate("mQWidget", "All Data\n"
"ScanTest", Q_NULLPTR));
        pushButton_Diagram->setText(QApplication::translate("mQWidget", "Diagram", Q_NULLPTR));
        curve_fit->setText(QApplication::translate("mQWidget", "CurveFit", Q_NULLPTR));
        MTF->setText(QApplication::translate("mQWidget", "SFR>1", Q_NULLPTR));
        move_Center->setText(QApplication::translate("mQWidget", "M_Center", Q_NULLPTR));
        pushButton_MTF_csv->setText(QApplication::translate("mQWidget", "MTF csv", Q_NULLPTR));
        AA_SFR->setText(QApplication::translate("mQWidget", "AA SFR", Q_NULLPTR));
        label_9->setText(QApplication::translate("mQWidget", "supported by songye.guo@samsung.com", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class mQWidget: public Ui_mQWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THROUGHFOCUS_H
