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
#ifndef VOLLEYCONTROLLER_H
#define VOLLEYCONTROLLER_H

#include <QObject>
#include <QWidget>

#include "scorecontroller.h"

QT_FORWARD_DECLARE_CLASS(QSettings)
QT_FORWARD_DECLARE_CLASS(QGridLayout)
QT_FORWARD_DECLARE_CLASS(QHBoxLayout)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(Edit)
QT_FORWARD_DECLARE_CLASS(Button)
QT_FORWARD_DECLARE_CLASS(QRadioButton)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QLabel)


class VolleyController : public ScoreController
{
    Q_OBJECT

public:
    VolleyController(QString sMyLanguage);
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

public:
    void GetSettings();

private slots:
    void onTimeOutIncrement(int iTeam);
    void onTimeOutDecrement(int iTeam);
    void onSetIncrement(int iTeam);
    void onSetDecrement(int iTeam);
    void onScoreIncrement(int iTeam);
    void onScoreDecrement(int iTeam);
    void onServiceClicked(int iTeam, bool bChecked);
    void onTeamTextChanged(QString sText, int iTeam);

    void onButtonChangeFieldClicked();
    void onButtonNewSetClicked();
    void onButtonNewGameClicked();

private:
    QGridLayout  *CreateGamePanel();
    QHBoxLayout  *CreateGameButtons();
    void          setEventHandlers();
    void          buildControls();
    void          buildFontSizes();
    QString       FormatStatusMsg();
    void          SaveStatus();

private:
    bool          bFontBuilt;
    QString       sLanguage;
    QString       sTeam[2];
    int           iTimeout[2]{};
    int           iSet[2]{};
    int           iScore[2]{};
    int           iServizio{};
    int           lastService{};

    Edit         *teamName[2]{};
    Edit         *timeoutEdit[2]{};
    Edit         *setsEdit[2]{};
    Edit         *scoreEdit[2]{};
    Button       *timeoutIncrement[2]{};
    Button       *timeoutDecrement[2]{};
    Button       *setsIncrement[2]{};
    Button       *setsDecrement[2]{};
    Button       *scoreIncrement[2]{};
    Button       *scoreDecrement[2]{};
    QRadioButton *service[2]{};
    QLabel       *timeoutLabel{};
    QLabel       *setsLabel{};
    QLabel       *serviceLabel{};
    QLabel       *scoreLabel{};

    QPushButton  *newSetButton{};
    QPushButton  *newGameButton{};
    QPushButton  *changeFieldButton{};
};

#endif // VOLLEYCONTROLLER_H
