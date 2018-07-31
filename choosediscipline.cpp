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


ChooseDiscipline::ChooseDiscipline(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChooseDiscipline)
    , pSettings(Q_NULLPTR)
{
    pSettings = new QSettings("Gabriele Salvato", "Choose Discipline");
    QString sCurrentLanguage = pSettings->value("language", QString(tr("Italiano"))).toString();
    ui->setupUi(this);
    discipline = VOLLEY_PANEL;
    ui->volleyRadioButton->setChecked(true);
    ui->LanguageComboBox->addItem("Italiano");
    ui->LanguageComboBox->addItem("English");
    ui->LanguageComboBox->setCurrentText(sCurrentLanguage);
}


ChooseDiscipline::~ChooseDiscipline() {
    delete pSettings;
    pSettings = Q_NULLPTR;
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
    done(QDialog::Accepted);
}


int
ChooseDiscipline::getDiscipline() {
    return discipline;
}


void
ChooseDiscipline::on_closePushButton_clicked() {
    done(QDialog::Rejected);
}


void
ChooseDiscipline::on_LanguageComboBox_currentIndexChanged(const QString &arg1) {
    pSettings->setValue("language", arg1);
    if(arg1 == QString("Italiano")) {
        QCoreApplication::removeTranslator(&Translator);
    }
    else if(arg1 == QString("English")) {
        Translator.load(":/scoreController_en");
        QCoreApplication::installTranslator(&Translator);
    }
    ui->retranslateUi(this);
}
