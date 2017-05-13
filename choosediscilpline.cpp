#include "choosediscilpline.h"
#include "ui_choosediscilpline.h"



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
    done(QDialog::Accepted);
}


int
ChooseDiscilpline::getDiscipline() {
    return discipline;
}


void
ChooseDiscilpline::on_closePushButton_clicked() {
    done(QDialog::Rejected);
}
