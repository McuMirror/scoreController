#ifndef PANELCONFIGURATOR_H
#define PANELCONFIGURATOR_H

#include <QDialog>

#include "panelorientation.h"

namespace Ui {
class PanelConfigurator;
}

class PanelConfigurator : public QDialog
{
    Q_OBJECT

public:
    explicit PanelConfigurator(QWidget *parent = 0);
    ~PanelConfigurator();
    int exec();
    void show();
    void SetCurrentPanTilt(int newPan, int newTilt);
    void SetCurrrentOrientaton(int index);

signals:
    void newPanValue(int newPan);
    void newTiltValue(int newTilt);
    void changeOrientation(PanelOrientation newOrientation);
    void startCamera();
    void stopCamera();
    void scoreOnly(bool);

private:
    void SetupButtons();

private slots:
    void on_upButton_pressed();
    void on_downButton_pressed();
    void on_leftButton_pressed();
    void on_rightButton_pressed();
    void on_orientationCombo_currentIndexChanged(int index);
    void on_tabWidget_tabBarClicked(int index);
    void on_scoreOnlyCheckBox_clicked(bool checked);
    void on_closeButton_clicked();

private:
    Ui::PanelConfigurator *ui;
    int                    iPan;
    int                    iTilt;
    int                    panMin;
    int                    panMax;
    int                    tiltMin;
    int                    tiltMax;
};

#endif // PANELCONFIGURATOR_H
