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


/*!
 * \brief ChooseDiscipline::ChooseDiscipline
 * A Dialog to select the Panel to show
 * \param parent
 */
ChooseDiscipline::ChooseDiscipline(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChooseDiscipline)
    , pSettings(Q_NULLPTR)
{
    pSettings = new QSettings("Gabriele Salvato", "Choose Discipline");
    sCurrentLanguage = pSettings->value("language", QString(tr("Italiano"))).toString();
    ui->setupUi(this);
    discipline = VOLLEY_PANEL;
    ui->volleyRadioButton->setChecked(true);
    ui->LanguageComboBox->addItem("Italiano");
    ui->LanguageComboBox->addItem("English");
    ui->LanguageComboBox->setCurrentText(sCurrentLanguage);
}


/*!
 * \brief ChooseDiscipline::~ChooseDiscipline
 */
ChooseDiscipline::~ChooseDiscipline() {
    delete pSettings;
    pSettings = Q_NULLPTR;
    delete ui;
}


/*!
 * \brief ChooseDiscipline::on_basketRadioButton_clicked
 */
void
ChooseDiscipline::on_basketRadioButton_clicked() {
    discipline = BASKET_PANEL;
}


/*!
 * \brief ChooseDiscipline::on_volleyRadioButton_clicked
 */
void
ChooseDiscipline::on_volleyRadioButton_clicked() {
    discipline = VOLLEY_PANEL;
}


/*!
 * \brief ChooseDiscipline::on_handballRadioButton_clicked
 */
void
ChooseDiscipline::on_handballRadioButton_clicked() {
    discipline = HANDBALL_PANEL;
}


/*!
 * \brief ChooseDiscipline::on_goPushButton_clicked
 */
void
ChooseDiscipline::on_goPushButton_clicked() {
    done(QDialog::Accepted);
}


/*!
 * \brief ChooseDiscipline::getDiscipline
 * \return
 */
int
ChooseDiscipline::getDiscipline() {
    return discipline;
}


/*!
 * \brief ChooseDiscipline::on_closePushButton_clicked
 */
void
ChooseDiscipline::on_closePushButton_clicked() {
    done(QDialog::Rejected);
}


/*!
 * \brief ChooseDiscipline::getLanguage
 * \return A QString representing the selected langage;
 */
QString
ChooseDiscipline::getLanguage() {
    return sCurrentLanguage;
}


/*!
 * \brief ChooseDiscipline::on_LanguageComboBox_currentIndexChanged
 * To select the language of the Controller Panel
 * \param arg1
 */
void
ChooseDiscipline::on_LanguageComboBox_currentIndexChanged(const QString &arg1) {
    sCurrentLanguage = arg1;
    pSettings->setValue("language", sCurrentLanguage);
    if(sCurrentLanguage == QString("Italiano")) {
        QCoreApplication::removeTranslator(&Translator);
    }
    else if(sCurrentLanguage == QString("English")) {
        Translator.load(":/scoreController_en");
        QCoreApplication::installTranslator(&Translator);
    }
    ui->retranslateUi(this);
}
