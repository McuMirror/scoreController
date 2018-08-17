#include "baskettab.h"

#include <QLabel>
#include <QGridLayout>
#include <QSettings>

//#define MAX_TIMEOUTS_1  2 // Numero massimo di sospensioni nella prima metà di gara
//#define MAX_TIMEOUTS_2  3 // Numero massimo di sospensioni nella seconda metà di gara
//#define MAX_TIMEOUTS_3  1 // Numero massimo di sospensioni negli OVERTIMES
//#define MAX_FAULS      99 // Da definire il comportamento dopo il numero max di falli !
//#define MAX_PERIODS    99 // Da definire il comportamento dopo il numero max di periodi !
//#define BONUS_TARGET    4 // Dopo questo valore il Bonus per il team è finito !
//#define GAME_PERIODS    4 // Oltre questo valore ci sono gli OVERTIMES
//#define REGULAR_TIME   10 // 10 Minuti è la durata di un tempo regolare
//#define OVER_TIME       5 // 5 Minuti è a durata di ciascun overtime


BasketTab::BasketTab(QWidget *parent)
    : QWidget(parent)
{

}


void
BasketTab::GetSettings() {
    pSettings = new QSettings("Gabriele Salvato", "Basket Parameters");
//    maxTimeout       = pSettings->value("volley/maxTimeout", 2).toInt();
//    maxSet           = pSettings->value("volley/maxSet", 3).toInt();
//    iTimeoutDuration = pSettings->value("volley/TimeoutDuration", 30).toInt();;

//    numTimeoutEdit.setText(QString("%1").arg(maxTimeout));
//    maxSetEdit.setText(QString("%1").arg(maxSet));
//    timeoutDurationEdit.setText(QString("%1").arg(iTimeoutDuration));
}


void
BasketTab::StoreSettings() {
    if(pSettings) {
//        maxTimeout       = numTimeoutEdit.text().toInt();
//        maxSet           = maxSetEdit.text().toInt();
//        iTimeoutDuration = timeoutDurationEdit.text().toInt();
//        pSettings->setValue("volley/maxTimeout",      maxTimeout);
//        pSettings->setValue("volley/maxSet",          maxSet);
//        pSettings->setValue("volley/TimeoutDuration", iTimeoutDuration);
    }
}

