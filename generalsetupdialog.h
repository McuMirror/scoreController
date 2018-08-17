#ifndef GENERALSETUPDIALOG_H
#define GENERALSETUPDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

#include "directorytab.h"
#include "volleytab.h"
#include "baskettab.h"
#include "handballtab.h"



QT_FORWARD_DECLARE_CLASS(QDialogButtonBox)
QT_FORWARD_DECLARE_CLASS(QTabWidget)


class GeneralSetupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GeneralSetupDialog(QWidget *parent = nullptr);

    void setSlideDir(const QString& sDir);
    void setSpotDir(const QString& sDir);
    QString getSlideDir();
    QString getSpotDir();

    int getNumTimeout();
    int getNumSet();
    int getTimeoutDuration();

public slots:
    void onFinished(int iResult);

private:
    QTabWidget       *tabWidget;
    QDialogButtonBox *buttonBox;
    DirectoryTab     *pDirectoryTab;
    VolleyTab        *pVolleyTab;
    BasketTab        *pBasketTab;
    HandBallTab      *pHandballTab;
};

#endif // GENERALSETUPDIALOG_H
