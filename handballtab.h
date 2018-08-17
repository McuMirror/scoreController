#ifndef HANDBALLTAB_H
#define HANDBALLTAB_H

#include <QWidget>
#include <QLineEdit>

QT_FORWARD_DECLARE_CLASS(QSettings)


class HandBallTab : public QWidget
{
    Q_OBJECT
public:
    explicit HandBallTab(QWidget *parent = nullptr);
    void GetSettings();
    void StoreSettings();
    int getNumTimeout();
    int getNumPeriod();
    int getRegularTime();

signals:

public slots:

private:
    QSettings *pSettings;
    QLineEdit  numTimeoutEdit;
    QLineEdit  numPeriodEdit;
    QLineEdit  regularTimeEdit;

};

#endif // HANDBALLTAB_H
