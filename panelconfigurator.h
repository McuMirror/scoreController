#ifndef PANELCONFIGURATOR_H
#define PANELCONFIGURATOR_H

#include <QDialog>

#include "paneldirection.h"
#include "paneltab.h"
#include "cameratab.h"


QT_FORWARD_DECLARE_CLASS(QDialogButtonBox)
QT_FORWARD_DECLARE_CLASS(QTabWidget)

class PanelConfigurator : public QDialog
{
    Q_OBJECT

public:
    explicit PanelConfigurator(QWidget *parent = Q_NULLPTR);
    void setClient(QString sClient);
    void SetCurrentPanTilt(int newPan, int newTilt);
    void SetCurrrentOrientaton(PanelDirection newDirection);
    void SetIsScoreOnly(bool bScoreOnly);

signals:
    void changeDirection(PanelDirection newDirection);
    void changeScoreOnly(bool bScoreOnly);
    void newPanValue(int newValue);
    void newTiltValue(int newValue);
    void startCamera();
    void stopCamera();

private:

public slots:
    // tabWidget event
    void onChangedTab(int iTabIndex);
    // PanelTab events
    void onChangeDirection(PanelDirection newDirection);
    void onChangeScoreOnly(bool bScoreOnly);
    // CameraTab events
    void onChangeTiltValue(int);
    void onChangePanValue(int);

private slots:

private:
    QTabWidget       *tabWidget;
    QDialogButtonBox *buttonBox;
    PanelTab         *pPanelTab;
    CameraTab        *pCameraTab;
    QString           sCurrentClient;
    int               iCameraTab;
};

#endif // PANELCONFIGURATOR_H
