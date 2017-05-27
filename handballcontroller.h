#ifndef HANDBALLCONTROLLER_H
#define HANDBALLCONTROLLER_H

#include <QObject>

#include "scorecontroller.h"

QT_FORWARD_DECLARE_CLASS(Edit)
QT_FORWARD_DECLARE_CLASS(Button)

class HandballController : public ScoreController
{
public:
    HandballController();
    void closeEvent(QCloseEvent *event);

public:
    void GetSettings();

private slots:
    void onTimeOutIncrement(int iTeam);
    void onTimeOutDecrement(int iTeam);
    void onScoreIncrement(int iTeam);
    void onScoreDecrement(int iTeam);
    void onTeamTextChanged(QString sText, int iTeam);
    void onPeriodIncrement(int iDummy);
    void onPeriodDecrement(int iDummy);

    void onButtonChangeFieldClicked();
    void onButtonNewPeriodClicked();
    void onButtonNewGameClicked();

private:
    QGridLayout  *CreateGamePanel();
    QGroupBox    *CreateTeamBox(int iTeam);
    QGroupBox    *CreateGameBox();
    QGroupBox    *CreateGameButtonBox();
    QString       FormatStatusMsg();
    void          SaveStatus() ;

private:
    QString       sTeam[2];
    int           iTimeout[2];
    int           iScore[2];
    int           iPeriod;

    Edit         *teamName[2];
    Edit         *timeoutEdit[2];
    Edit         *scoreEdit[2];
    Edit         *periodEdit;
    Button       *timeoutIncrement[2];
    Button       *timeoutDecrement[2];
    Button       *scoreIncrement[2];
    Button       *scoreDecrement[2];
    Button       *periodIncrement;
    Button       *periodDecrement;

    QPushButton  *newPeriodButton;
    QPushButton  *newGameButton;
    QPushButton  *changeFieldButton;
};

#endif // HANDBALLCONTROLLER_H
