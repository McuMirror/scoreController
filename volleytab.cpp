#include "volleytab.h"
#include <QLabel>
#include <QGridLayout>
#include <QSettings>


VolleyTab::VolleyTab(QWidget *parent)
    : QWidget(parent)
{
    QLabel *numTimeoutLabel = new QLabel(tr("Max Timeouts:"));
    QLabel *maxSetLabel = new QLabel(tr("Max Sets:"));
    QLabel *timeoutDurationLabel = new QLabel(tr("Timeout sec:"));

    numTimeoutEdit.setMaxLength(1);
    maxSetEdit.setMaxLength(1);
    timeoutDurationEdit.setMaxLength(2);

    auto *mainLayout = new QGridLayout;
    mainLayout->addWidget(numTimeoutLabel,      0, 0, 1, 3);
    mainLayout->addWidget(&numTimeoutEdit,      0, 3, 1, 1);

    mainLayout->addWidget(maxSetLabel,          1, 0, 1, 3);
    mainLayout->addWidget(&maxSetEdit,          1, 3, 1, 1);

    mainLayout->addWidget(timeoutDurationLabel, 2, 0, 1, 3);
    mainLayout->addWidget(&timeoutDurationEdit, 2, 3, 1, 1);

    setLayout(mainLayout);
}


int
VolleyTab::getNumTimeout() {
    return numTimeoutEdit.text().toInt();
}


int
VolleyTab::getNumSet(){
    return maxSetEdit.text().toInt();
}


int
VolleyTab::getTimeoutDuration() {
    return timeoutDurationEdit.text().toInt();
}


void
VolleyTab::GetSettings() {
    pSettings = new QSettings("Gabriele Salvato", "Volley Parameters");
    maxTimeout       = pSettings->value("volley/maxTimeout", 2).toInt();
    maxSet           = pSettings->value("volley/maxSet", 3).toInt();
    iTimeoutDuration = pSettings->value("volley/TimeoutDuration", 30).toInt();;

    numTimeoutEdit.setText(QString("%1").arg(maxTimeout));
    maxSetEdit.setText(QString("%1").arg(maxSet));
    timeoutDurationEdit.setText(QString("%1").arg(iTimeoutDuration));
}


void
VolleyTab::StoreSettings() {
    if(pSettings) {
        maxTimeout       = numTimeoutEdit.text().toInt();
        maxSet           = maxSetEdit.text().toInt();
        iTimeoutDuration = timeoutDurationEdit.text().toInt();
        pSettings->setValue("volley/maxTimeout",      maxTimeout);
        pSettings->setValue("volley/maxSet",          maxSet);
        pSettings->setValue("volley/TimeoutDuration", iTimeoutDuration);
    }
}

