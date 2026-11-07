#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#pragma warning(disable: 4996)

#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}