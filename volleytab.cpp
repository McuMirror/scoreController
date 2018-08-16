#include "volleytab.h"
#include <QLabel>
#include <QGridLayout>


// MAX_TIMEOUTS      2
// MAX_SETS          3
// TIMEOUT_DURATION 30


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


void
VolleyTab::setNumTimeout(int nTimeout) {
    numTimeoutEdit.setText(QString(nTimeout));
}


void
VolleyTab::setNumSet(int nSet) {
    maxSetEdit.setText(QString(nSet));
}


void
VolleyTab::setTimeoutDuration(int iDuration) {
    timeoutDurationEdit.setText(QString(iDuration));
}


int
VolleyTab::getNumTimeout() {
    return timeoutDurationEdit.text().toInt();
}


int
VolleyTab::getNumSet(){
    return maxSetEdit.text().toInt();
}


int
VolleyTab::getTimeoutDuration() {
    return timeoutDurationEdit.text().toInt();
}
