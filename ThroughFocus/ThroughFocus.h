#ifndef ThroughFocus_H
#define ThroughFocus_H

#include <QtWidgets/QWidget>
#include "ui_ThroughFocus.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include "core.hpp"
#include "highgui.hpp"
#include <opencv2/opencv.hpp>

namespace Ui {
	class mQWidget;
}

class ThroughFocus : public QWidget
{
	Q_OBJECT

public:

	explicit ThroughFocus(QWidget *parent = Q_NULLPTR);
	~ThroughFocus();

public:
	QSystemTrayIcon *mSysTrayIcon = 0;
	QMenu *mMenu = NULL;;
	QAction *mShowMainAction = NULL;
	QAction *mUploadAction = NULL;
	QAction *mExitAppAction = NULL;

public:

	void Display_Result();
	void displayText(QString s);
	void get_Selection();

private slots :
//	void on_pushButton_Minimize_clicked();
//	void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
//	void on_showMainAction();
//	void on_exitAppAction();
	void on_pushButton_Start_clicked();
	void on_pushButton_Manual_clicked();
	void on_pushButton_Save_clicked();
	void on_pushButton_open_clicked();
	void on_pushButton_Diagram_clicked();
	void on_pushButton_Test_All_clicked();
	void on_pushButton_MTF_csv_clicked();

	
private:
	Ui::mQWidget *ui;
	cv::Mat image, img2, img3, dst;

};

#endif 