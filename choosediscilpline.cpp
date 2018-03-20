/*
 *
Copyright (C) 2016  Gabriele Salvato

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
#include "choosediscilpline.h"
#include "ui_choosediscilpline.h"
#include "volleycontroller.h"
#include "basketcontroller.h"
#include "handballcontroller.h"


ChooseDiscilpline::ChooseDiscilpline(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChooseDiscilpline)
    , pController(Q_NULLPTR)
{
    ui->setupUi(this);
    discipline = VOLLEY_PANEL;
    ui->volleyRadioButton->setChecked(true);
}



ChooseDiscilpline::~ChooseDiscilpline() {
    if(pController != Q_NULLPTR) {
        disconnect(pController, 0, 0, 0);
        pController->deleteLater();
    }
    pController = Q_NULLPTR;
    delete ui;
}


void
ChooseDiscilpline::on_basketRadioButton_clicked() {
    discipline = BASKET_PANEL;
}


void
ChooseDiscilpline::on_volleyRadioButton_clicked() {
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
    done(QDialog::Accepted);
#else
    hide();
//  pController->showMaximized();
    pController->show();
#endif
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
    if(pController != Q_NULLPTR) {
        disconnect(pController, 0, 0, 0);
        pController->deleteLater();
    }
    pController = Q_NULLPTR;
    show();
}


