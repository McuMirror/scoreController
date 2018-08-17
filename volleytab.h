#ifndef VOLLEYTAB_H
#define VOLLEYTAB_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

QT_FORWARD_DECLARE_CLASS(QSettings)


class VolleyTab : public QWidget
{
    Q_OBJECT
public:
    explicit VolleyTab(QWidget *parent = nullptr);
    void GetSettings();
    void StoreSettings();
    int getNumTimeout();
    int getNumSet();
    int getTimeoutDuration();

signals:

public slots:

private:
    QSettings *pSettings;
    QLineEdit  numTimeoutEdit;
    QLineEdit  maxSetEdit;
    QLineEdit  timeoutDurationEdit;

    int        maxTimeout;
    int        maxSet;
    int        iTimeoutDuration;

};

#endif // VOLLEYTAB_H
