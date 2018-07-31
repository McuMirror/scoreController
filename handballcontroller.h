#ifndef HANDBALLCONTROLLER_H
#define HANDBALLCONTROLLER_H

#include <QObject>
#include <QWidget>
#include <QLabel>

#include "scorecontroller.h"

QT_FORWARD_DECLARE_CLASS(Edit)
QT_FORWARD_DECLARE_CLASS(Button)

class HandballController : public ScoreController
{
    Q_OBJECT

public:
    HandballController();
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

public:
    void GetSettings();

private slots:
    void onTimeOutIncrement(int iTeam);
    void onTimeOutDecrement(int iTeam);
    void onScoreIncrement(int iTeam);
    void onScoreDecrement(int iTeam);
    void onTeamTextChanged(QString sText, int iTeam);
    void onPeriodIncrement();
    void onPeriodDecrement();

    void onButtonChangeFieldClicked();
    void onButtonNewPeriodClicked();
    void onButtonNewGameClicked();

private:
    void          setEventHandlers();
    void          buildControls();
    void          buildFontSizes();
    QGridLayout  *CreateGamePanel();
    QHBoxLayout  *CreateGameButtons();
    QString       FormatStatusMsg();
    void          SaveStatus() ;

private:
    bool          bFontBuilt;
    QString       sTeam[2];
    int           iTimeout[2]{};
    int           iScore[2]{};
    int           iPeriod{};

    Edit         *teamName[2]{};
    Edit         *timeoutEdit[2]{};
    Edit         *scoreEdit[2]{};
    Edit         *periodEdit{};
    Button       *timeoutIncrement[2]{};
    Button       *timeoutDecrement[2]{};
    Button       *scoreIncrement[2]{};
    Button       *scoreDecrement[2]{};
    QPushButton  *periodIncrement{};
    QPushButton  *periodDecrement{};
    QLabel       *timeoutLabel{};
    QLabel       *scoreLabel{};
    QLabel       *periodLabel{};

    int           maxTimeouts;
    int           maxPeriods;
    int           periodTime;

    QPushButton  *newPeriodButton{};
    QPushButton  *newGameButton{};
    QPushButton  *changeFieldButton{};
};

#endif // HANDBALLCONTROLLER_H
