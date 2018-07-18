#ifndef BASKETCONTROLLER_H
#define BASKETCONTROLLER_H

#include <QObject>
#include <QWidget>

#include "scorecontroller.h"

QT_FORWARD_DECLARE_CLASS(QSettings)
QT_FORWARD_DECLARE_CLASS(QGridLayout)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(Edit)
QT_FORWARD_DECLARE_CLASS(Button)
QT_FORWARD_DECLARE_CLASS(QRadioButton)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QLabel)

class BasketController : public ScoreController
{
    Q_OBJECT

public:
    BasketController();
    void closeEvent(QCloseEvent *event);

public:
    void GetSettings();

private slots:
    void onTimeOutIncrement(int iTeam);
    void onTimeOutDecrement(int iTeam);
    void onScoreIncrement(int iTeam);
    void onScoreDecrement(int iTeam);
    void onFaulsIncrement(int iTeam);
    void onFaulsDecrement(int iTeam);
    void onTeamTextChanged(QString sText, int iTeam);
    void onPeriodIncrement(int iDummy);
    void onPeriodDecrement(int iDummy);
    void onPossessClicked(int iTeam, bool bChecked);

    void onButtonChangeFieldClicked();
    void onButtonNewPeriodClicked();
    void onButtonNewGameClicked();

private:
    void          buildControls();
    void          buildFontSizes();
    void          setEventHandlers();
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
    int           iFauls[2];
    int           iBonus[2];
    int           iPeriod;
    int           iPossess;

    Edit         *teamName[2];
    Edit         *timeoutEdit[2];
    Edit         *scoreEdit[2];
    Edit         *faulsEdit[2];
    Edit         *periodEdit;
    Edit         *bonusEdit[2];
    Button       *timeoutIncrement[2];
    Button       *timeoutDecrement[2];
    Button       *scoreIncrement[2];
    Button       *scoreDecrement[2];
    Button       *faulsIncrement[2];
    Button       *faulsDecrement[2];
    QPushButton  *periodIncrement;
    QPushButton  *periodDecrement;
    QRadioButton *possess[2];

    QLabel       *timeoutLabel;
    QLabel       *faulsLabel;
    QLabel       *scoreLabel;
    QLabel       *periodLabel;
    QLabel       *possessLabel;

    int           iTeamFontSize;
    int           iTimeoutFontSize;
    int           iFaulsFontSize;
    int           iScoreFontSize;
    int           iLabelFontSize;
    int           iPeriodLabelFontSize;

    QPushButton  *newPeriodButton;
    QPushButton  *newGameButton;
    QPushButton  *changeFieldButton;
};

#endif // BASKETCONTROLLER_H
