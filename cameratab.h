#ifndef CAMERATAB_H
#define CAMERATAB_H

#include <QWidget>
#include <QPushButton>

class CameraTab : public QWidget
{
    Q_OBJECT
public:
    explicit CameraTab(QWidget *parent = nullptr);
    void SetCurrentPanTilt(int newPan, int newTilt);

protected:
    void setupButtons();
    void setupButtonEvents();

signals:
    void newTiltValue(int);
    void newPanValue(int);

public slots:
    void on_upButton_pressed();
    void on_downButton_pressed();
    void on_leftButton_pressed();
    void on_rightButton_pressed();

private:
    QPushButton downButton;
    QPushButton upButton;
    QPushButton leftButton;
    QPushButton rightButton;

    int iTilt;
    int iPan;
    int panMin;
    int panMax;
    int tiltMin;
    int tiltMax;
};

#endif // CAMERATAB_H
