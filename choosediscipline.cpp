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
#include "choosediscipline.h"
#include "ui_choosediscipline.h"
#include "volleycontroller.h"
#include "basketcontroller.h"
#include "handballcontroller.h"


ChooseDiscipline::ChooseDiscipline(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChooseDiscipline)
    , pController(Q_NULLPTR)
{
    ui->setupUi(this);
    discipline = VOLLEY_PANEL;
    ui->volleyRadioButton->setChecked(true);
}


ChooseDiscipline::~ChooseDiscipline() {
    if(pController != Q_NULLPTR) {
        disconnect(pController, 0, 0, 0);
        delete pController;
    }
    pController = Q_NULLPTR;
    delete ui;
}


void
ChooseDiscipline::on_basketRadioButton_clicked() {
    discipline = BASKET_PANEL;
}


void
ChooseDiscipline::on_volleyRadioButton_clicked() {
    discipline = VOLLEY_PANEL;
}


void
ChooseDiscipline::on_handballRadioButton_clicked() {
    discipline = HANDBALL_PANEL;
}


void
ChooseDiscipline::on_goPushButton_clicked() {
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
    hide();
#ifdef Q_OS_ANDROID
    pController->showFullScreen();
#else
    pController->show();
#endif
}


int
ChooseDiscipline::getDiscipline() {
    return discipline;
}


void
ChooseDiscipline::on_closePushButton_clicked() {
    if(pController != Q_NULLPTR) {
        disconnect(pController, 0, 0, 0);
        delete pController;
    }
    pController = Q_NULLPTR;
    done(QDialog::Accepted);
}


void
ChooseDiscipline::onPanelDone() {
    if(pController != Q_NULLPTR) {
        disconnect(pController, 0, 0, 0);
        delete pController;
    }
    pController = Q_NULLPTR;
    show();
}


