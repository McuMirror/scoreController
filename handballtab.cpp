#include "handballtab.h"

#include <QLabel>
#include <QGridLayout>
#include <QSettings>


HandBallTab::HandBallTab(QWidget *parent)
    : QWidget(parent)
{
    QLabel *numTimeoutLabel  = new QLabel(tr("Number of Timeouts:"));
    QLabel *numPeriodLabel   = new QLabel(tr("Number of Periods:"));
    QLabel *regularTimeLabel = new QLabel(tr("Regular Time [min]:"));

    numTimeoutEdit.setMaxLength(1);
    numPeriodEdit.setMaxLength(1);
    regularTimeEdit.setMaxLength(2);

    auto *mainLayout = new QGridLayout;
    mainLayout->addWidget(numTimeoutLabel,  0, 0, 1, 3);
    mainLayout->addWidget(&numTimeoutEdit,  0, 3, 1, 1);

    mainLayout->addWidget(numPeriodLabel,   1, 0, 1, 3);
    mainLayout->addWidget(&numPeriodEdit,   1, 3, 1, 1);

    mainLayout->addWidget(regularTimeLabel, 2, 0, 1, 3);
    mainLayout->addWidget(&regularTimeEdit, 2, 3, 1, 1);

    setLayout(mainLayout);
}



void
HandBallTab::GetSettings() {
    pSettings = new QSettings("Gabriele Salvato", "Handball Parameters");
    numTimeoutEdit.setText(pSettings->value("volley/maxTimeout", 2).toString());
    numPeriodEdit.setText(pSettings->value("volley/maxPeriod", 2).toString());
    regularTimeEdit.setText(pSettings->value("volley/RegularTime", 30).toString());
}


void
HandBallTab::StoreSettings() {
    if(pSettings) {
        pSettings->setValue("volley/maxTimeout", numTimeoutEdit.text().toInt());
        pSettings->setValue("volley/maxPeriod", numPeriodEdit.text().toInt());
        pSettings->setValue("volley/RegularTime", regularTimeEdit.text().toInt());
    }
}


int
HandBallTab::getNumTimeout() {
    return numTimeoutEdit.text().toInt();
}


int
HandBallTab::getNumPeriod() {
    return numPeriodEdit.text().toInt();
}


int
HandBallTab::getRegularTime() {
    return regularTimeEdit.text().toInt();
}
