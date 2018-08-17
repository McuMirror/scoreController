#include "handballtab.h"

#include <QLabel>
#include <QGridLayout>
#include <QSettings>

#define MAX_TIMEOUTS   2 // Numero massimo di sospensioni
#define MAX_PERIODS    2 //
#define MAX_SCORE    999 //
#define REGULAR_TIME  30 // 30 Minuti è la durata di un tempo regolare

HandBallTab::HandBallTab(QWidget *parent)
    : QWidget(parent)
{

}



void
HandBallTab::GetSettings() {
    pSettings = new QSettings("Gabriele Salvato", "Hanball Parameters");
//    maxTimeout       = pSettings->value("volley/maxTimeout", 2).toInt();
//    maxSet           = pSettings->value("volley/maxSet", 3).toInt();
//    iTimeoutDuration = pSettings->value("volley/TimeoutDuration", 30).toInt();;

//    numTimeoutEdit.setText(QString("%1").arg(maxTimeout));
//    maxSetEdit.setText(QString("%1").arg(maxSet));
//    timeoutDurationEdit.setText(QString("%1").arg(iTimeoutDuration));
}


void
HandBallTab::StoreSettings() {
    if(pSettings) {
//        maxTimeout       = numTimeoutEdit.text().toInt();
//        maxSet           = maxSetEdit.text().toInt();
//        iTimeoutDuration = timeoutDurationEdit.text().toInt();
//        pSettings->setValue("volley/maxTimeout",      maxTimeout);
//        pSettings->setValue("volley/maxSet",          maxSet);
//        pSettings->setValue("volley/TimeoutDuration", iTimeoutDuration);
    }
}

