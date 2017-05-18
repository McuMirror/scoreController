#include "choosediscilpline.h"
#include "ui_choosediscilpline.h"
#include "volleycontroller.h"
#include "basketcontroller.h"


ChooseDiscilpline::ChooseDiscilpline(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseDiscilpline)
{
    ui->setupUi(this);
    ui->VolleyRadioButton->setChecked(true);
}



ChooseDiscilpline::~ChooseDiscilpline() {
    delete ui;
}


void
ChooseDiscilpline::on_basketRadioButton_clicked() {
    discipline = BASKET_PANEL;
}


void
ChooseDiscilpline::on_VolleyRadioButton_clicked() {
    discipline = VOLLEY_PANEL;
}


void
ChooseDiscilpline::on_goPushButton_clicked() {
    hide();
    if(getDiscipline() == BASKET_PANEL)
        pController = new BasketController();
    else
        pController = new VolleyController();
    connect(pController, SIGNAL(panelDone()),
            this, SLOT(onPanelDone()));
#ifdef Q_OS_ANDROID
    pController->showFullScreen();
#else
    pController->showMaximized();
//        pController->show();
#endif
}


int
ChooseDiscilpline::getDiscipline() {
    return discipline;
}


void
ChooseDiscilpline::on_closePushButton_clicked() {
    done(QDialog::Rejected);
}

void
ChooseDiscilpline::onPanelDone() {
    pController->deleteLater();
    show();
}
