#include "scorecontroller.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>

int
main(int argc, char *argv[]) {

#ifndef Q_OS_ANDROID
  QSharedMemory lockingMemory;
#endif

    QApplication app(argc, argv);

    QFont myFont = QApplication::font();
    myFont.setPointSize(32);
    app.setFont(myFont, "QEdit");
    myFont.setPointSize(18);
    app.setFont(myFont, "QRadioButton");
    app.setFont(myFont, "QLabel");

#ifndef Q_OS_ANDROID
    lockingMemory.setKey("ScoreController");
    if(lockingMemory.attach()) {
        QMessageBox::information(NULL, "Score Controller", "Esiste un'altra istanza attiva");
        return 0;
    }
    if(!lockingMemory.create(1)) {
        QMessageBox::information(NULL, "Score Controller", "Impossibile creare il lock");
        lockingMemory.detach();
        return 0;
    }
    #endif

    ScoreController w;

#ifdef Q_OS_ANDROID
    w.showFullScreen();
#else
    w.showMaximized();
    //w.show();
#endif
    int iresult = app.exec();

#ifndef Q_OS_ANDROID
    lockingMemory.detach();
#endif
    return iresult;
}
