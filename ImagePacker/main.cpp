#include "imagepacker.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ImagePacker w;
	w.show();
	return a.exec();
}
