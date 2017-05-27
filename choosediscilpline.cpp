#include "choosediscilpline.h"
#include "ui_choosediscilpline.h"
#include "volleycontroller.h"
#include "basketcontroller.h"
#include "handballcontroller.h"


ChooseDiscilpline::ChooseDiscilpline(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChooseDiscilpline)
    , discipline(VOLLEY_PANEL)
    , pController(Q_NULLPTR)
{
    ui->setupUi(this);
    ui->volleyRadioButton->setChecked(true);
}



ChooseDiscilpline::~ChooseDiscilpline() {
    if(pController != Q_NULLPTR)
        delete pController;
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
ChooseDiscilpline::on_handballRadioButton_clicked() {
    discipline = HANDBALL_PANEL;
}


void
ChooseDiscilpline::on_goPushButton_clicked() {
    int iDiscipline = getDiscipline();
    if(iDiscipline == VOLLEY_PANEL)
        pController = new VolleyController();
    else if(iDiscipline == BASKET_PANEL)
        pController = new BasketController();
    else if(iDiscipline == HANDBALL_PANEL)
        pController = new HandballController();
    else
        pController = new VolleyController();

    connect(pController, SIGNAL(panelDone()),
            this, SLOT(onPanelDone()));
#ifdef Q_OS_ANDROID
    pController->showFullScreen();
#else
//    pController->showMaximized();
        pController->show();
#endif
        hide();
}


int
ChooseDiscilpline::getDiscipline() {
    return discipline;
}


void
ChooseDiscilpline::on_closePushButton_clicked() {
    done(QDialog::Accepted);
}


void
ChooseDiscilpline::onPanelDone() {
    pController->deleteLater();
    pController = Q_NULLPTR;
    setVisible(true);
}


