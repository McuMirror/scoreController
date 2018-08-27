#ifndef GENERALSETUPDIALOG_H
#define GENERALSETUPDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

#include "directorytab.h"
#include "volleytab.h"
#include "baskettab.h"
#include "handballtab.h"
#include <QLabel>


QT_FORWARD_DECLARE_CLASS(QDialogButtonBox)
QT_FORWARD_DECLARE_CLASS(QTabWidget)


class GeneralSetupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GeneralSetupDialog(int PanelType, QWidget *parent = nullptr);

    void setSlideDir(const QString& sDir);
    void setSpotDir(const QString& sDir);
    QString getSlideDir();
    QString getSpotDir();

    int getNumTimeoutVY();
    int getNumSetVY();
    int getTimeoutDurationVY();

    int getNumTimeoutHB();
    int getNumPeriodHB();
    int getRegularTimeHB();

    int getNumTimeout1BB();
    int getNumTimeout2BB();
    int getNumTimeout3BB();
    int getGamePeriodsBB();
    int getBonusTargetBB();
    int getRegularTimeBB();
    int getOverTimeBB();

public slots:
    void onFinished(int iResult);

private:
    QTabWidget       *tabWidget;
    QDialogButtonBox *buttonBox;
    QLabel           *pStatusBar;
    DirectoryTab     *pDirectoryTab;
    VolleyTab        *pVolleyTab;
    BasketTab        *pBasketTab;
    HandBallTab      *pHandballTab;
};

#endif // GENERALSETUPDIALOG_H
