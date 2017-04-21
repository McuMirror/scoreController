#include "scorecontroller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScoreController w;
    w.show();

    return a.exec();
}
