#include "VisualHandGestureRecognition.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VisualHandGestureRecognition w;
	w.show();
	return a.exec();
}
