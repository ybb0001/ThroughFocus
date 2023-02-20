#include "mythread.h"

MyThread::MyThread(QObject *parent)
{
	str = "0";
}

MyThread::~MyThread()
{

}

void MyThread::inputSet(QString s) {

	str = s;
	emit changeText(s);
}

void MyThread::strSet(QString s) {

	str = s;
}

void MyThread::setNum(int n)
{
	num = n;
	emit changeNum(num);
}

void MyThread::logSet(QString s) {

	emit changeText(s);

}

void MyThread::run() {

	emit changeNum(num);
}

void MyThread::ResetSlot() {



}