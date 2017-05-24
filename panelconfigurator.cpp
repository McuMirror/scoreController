#include "panelconfigurator.h"
#include "ui_panelconfigurator.h"

PanelConfigurator::PanelConfigurator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PanelConfigurator)
    , panMin(-30)
    , panMax(30)
    , tiltMin(-30)
    , tiltMax(30)
{
    ui->setupUi(this);
    SetupButtons();
    ui->orientationCombo->addItem(QString("Normale"));
    ui->orientationCombo->addItem(QString("Riflesso"));
    ui->orientationCombo->addItem(QString("Rot. Dx"));
    ui->orientationCombo->addItem(QString("Rot. Sx"));
    ui->orientationCombo->setDisabled(true);
}


PanelConfigurator::~PanelConfigurator() {
    delete ui;
}


int
PanelConfigurator::exec() {
    ui->leftButton->setDisabled(true);
    ui->rightButton->setDisabled(true);
    ui->upButton->setDisabled(true);
    ui->downButton->setDisabled(true);
    ui->orientationCombo->setDisabled(true);
    if(!isVisible()) QDialog::show();
    return 0;
}


void
PanelConfigurator::show() {
    ui->leftButton->setDisabled(true);
    ui->rightButton->setDisabled(true);
    ui->upButton->setDisabled(true);
    ui->downButton->setDisabled(true);
    ui->orientationCombo->setDisabled(true);
    if(!isVisible()) QDialog::show();
}


void
PanelConfigurator::SetCurrentPanTilt(int newPan, int newTilt) {
    iPan  = newPan;
    iTilt = newTilt;
    ui->leftButton->setDisabled(iPan <= panMin);
    ui->rightButton->setDisabled(iPan >= panMax);
    ui->upButton->setDisabled(iTilt >= tiltMax);
    ui->downButton->setDisabled(iTilt <= tiltMin);
}


void
PanelConfigurator::SetCurrrentOrientaton(int index) {
    ui->orientationCombo->setCurrentIndex(index);
    ui->orientationCombo->setEnabled(true);
}


void
PanelConfigurator::SetupButtons() {
    int iDelay  = 300;
    int iRepeat = 100;

    ui->upButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    ui->upButton->setAutoRepeat(true);
    ui->upButton->setAutoRepeatDelay(iDelay);
    ui->upButton->setAutoRepeatInterval(iRepeat);

    ui->downButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    ui->downButton->setAutoRepeat(true);
    ui->downButton->setAutoRepeatDelay(iDelay);
    ui->downButton->setAutoRepeatInterval(iRepeat);

    ui->leftButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->leftButton->setAutoRepeat(true);
    ui->leftButton->setAutoRepeatDelay(iDelay);
    ui->leftButton->setAutoRepeatInterval(iRepeat);

    ui->rightButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->rightButton->setAutoRepeat(true);
    ui->rightButton->setAutoRepeatDelay(iDelay);
    ui->rightButton->setAutoRepeatInterval(iRepeat);
}


void
PanelConfigurator::on_upButton_pressed() {
    iTilt++;
    ui->downButton->setDisabled(false);
    if(iTilt <= tiltMax)
        emit newTiltValue(iTilt);
    else {
        iTilt = tiltMax;
        ui->upButton->setDisabled(true);
    }
}


void
PanelConfigurator::on_downButton_pressed() {
    iTilt--;
    ui->upButton->setDisabled(false);
    if(iTilt >= tiltMin)
        emit newTiltValue(iTilt);
    else {
        iTilt = tiltMin;
        ui->downButton->setDisabled(true);
    }
}


void
PanelConfigurator::on_leftButton_pressed() {
    iPan--;
    ui->rightButton->setDisabled(false);
    if(iPan >= panMin)
        emit newPanValue(iPan);
    else {
        iPan = panMin;
        ui->leftButton->setDisabled(true);
    }
}


void
PanelConfigurator::on_rightButton_pressed() {
    iPan++;
    ui->leftButton->setDisabled(false);
    if(iPan <= panMax)
        emit newPanValue(iPan);
    else {
        iPan = panMax;
        ui->rightButton->setDisabled(true);
    }
}


void
PanelConfigurator::on_comboBox_currentIndexChanged(int index) {
    emit changeOrientation(static_cast<PanelOrientation>(index));
}
