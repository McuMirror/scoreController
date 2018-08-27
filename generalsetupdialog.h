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
