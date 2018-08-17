#ifndef BASKETTAB_H
#define BASKETTAB_H

#include <QWidget>
#include <QLineEdit>

QT_FORWARD_DECLARE_CLASS(QSettings)


class BasketTab : public QWidget
{
    Q_OBJECT
public:
    explicit BasketTab(QWidget *parent = nullptr);
    void GetSettings();
    void StoreSettings();
    int getNumTimeout1();
    int getNumTimeout2();
    int getNumTimeout3();
    int getGamePeriods();
    int getBonusTarget();
    int getRegularTime();
    int getOverTime();

signals:

public slots:

private:
    QSettings *pSettings;
    QLineEdit numTimeout1Edit;
    QLineEdit numTimeout2Edit;
    QLineEdit numTimeout3Edit;
    QLineEdit BonusTargetEdit;
    QLineEdit GamePeriodsEdit;
    QLineEdit regularTimeEdit;
    QLineEdit overTimeEdit;
};

#endif // BASKETTAB_H
