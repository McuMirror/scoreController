#ifndef VOLLEYCONTROLLER_H
#define VOLLEYCONTROLLER_H

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

class VolleyController : public ScoreController
{
    Q_OBJECT

public:
    VolleyController();
    void closeEvent(QCloseEvent *event);

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
    QGroupBox    *CreateTeamBox(int iTeam);
    QGroupBox    *CreateGameButtonBox();
    QString       FormatStatusMsg();
    void          SaveStatus();

private:
    QSettings    *pSettings;

    QString       sTeam[2];
    int           iTimeout[2];
    int           iSet[2];
    int           iScore[2];
    int           iServizio;
    int           lastService;

    Edit*         teamName[2];
    Edit*         timeoutEdit[2];
    Edit*         setsEdit[2];
    Edit*         scoreEdit[2];
    Button*       timeoutIncrement[2];
    Button*       timeoutDecrement[2];
    Button*       setsIncrement[2];
    Button*       setsDecrement[2];
    Button*       scoreIncrement[2];
    Button*       scoreDecrement[2];
    QRadioButton *service[2];

    QPushButton  *newSetButton;
    QPushButton  *newGameButton;
    QPushButton  *changeFieldButton;
};

#endif // VOLLEYCONTROLLER_H
