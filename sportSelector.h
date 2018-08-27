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
#ifndef SPORTSELECTOR_H
#define SPORTSELECTOR_H

#include <QObject>
#include <QDialog>
#include <QTranslator>
#include "utility.h"
#include <QSettings>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QGridLayout>

class SportSelector : public QDialog
{
    Q_OBJECT
public:
    SportSelector(QWidget *parent);

    int getDiscipline();
    QString getLanguage();

private slots:
    void on_volleyRadioButton_clicked();
    void on_handballRadioButton_clicked();
    void on_basketRadioButton_clicked();
    void on_goPushButton_clicked();
    void on_exitPushButton_clicked();
    void on_LanguageComboBox_currentIndexChanged(const QString &arg1);
    void changeEvent(QEvent *event);

protected:
    QGridLayout* createLayout();
    void         setEventHandlers();

private:
    QSettings    *pSettings;

    QRadioButton volleyRadioButton;
    QRadioButton handballRadioButton;
    QRadioButton basketRadioButton;
    QComboBox    languageComboBox;
    QPushButton  goPushButton;
    QPushButton  exitPushButton;

    QString      sCurrentLanguage;
    QTranslator  Translator;

    int          discipline;
};

#endif // SPORTSELECTOR_H
