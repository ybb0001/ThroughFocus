#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QtGui> 
#include <QtCore> 

class MyThread : public QThread
{
	Q_OBJECT

private:
	int num;
	QString str;

public:
	MyThread(QObject *parent = 0);
	~MyThread();

protected:
	void run();

public:
	void inputSet(QString s);
	void strSet(QString str);
	void logSet(QString s);
	void setNum(int num);

signals:
	void changeText(QString str);
	void changeNum(int i);

public slots:
	void ResetSlot();

};

#endif // MYTHREAD_H