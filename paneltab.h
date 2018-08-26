#ifndef PANELTAB_H
#define PANELTAB_H

#include "paneldirection.h"

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>

class PanelTab : public QWidget
{
    Q_OBJECT
public:
    explicit PanelTab(QWidget *parent = nullptr);
    void setDirection(PanelDirection newDirection);
    void setScoreOnly(bool bScoreOnly);

signals:
    void getDirection(QString sIpAdress);
    void getScoreOnly(QString sIpAdress);
    void changeDirection(PanelDirection newDirection);
    void changeScoreOnly(bool bScoreOnly);

public slots:
    void onChangePanelDirection(QString sDirection);
    void onChangeScoreOnly(int newState);

private:
    QLabel labelDirection;
    QComboBox directionCombo;
    QCheckBox scoreOnlyCheckBox;
};

#endif // PANELTAB_H
