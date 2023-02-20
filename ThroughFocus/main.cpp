#include "ThroughFocus.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ThroughFocus w;
	w.show();
	return a.exec();
}
