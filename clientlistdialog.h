#ifndef CLIENTLISTDIALOG_H
#define CLIENTLISTDIALOG_H

#include <QObject>
#include <QDialog>

QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)
QT_FORWARD_DECLARE_CLASS(QSlider)

class ClientListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientListDialog(QWidget *parent);
    ~ClientListDialog();
    int exec();

public slots:
    void onRemotePanTiltReceived(int newPan, int newTilt);
    void onCloseCamera();

signals:
    void enableVideo(QString sIpAdress);
    void disableVideo();
    void setPanTilt(int newPan, int newTilt);
    void newPanValue(QString sClientIp, int newPan);
    void newTiltValue(QString sClientIp, int newTilt);

private slots:
    void onClientSelected(QListWidgetItem* selectedClient);
    void onLeftButtonPressed();
    void onRightButtonPressed();
    void onUpButtonPressed();
    void onDownButtonPressed();

private:
    QGroupBox* createClientListBox();
    QGroupBox* createPanTiltBox();

private:
    QWidget*      pMyParent;
    QListWidget*  clientListWidget;
    QPushButton*  closeButton;
    QPushButton*  upButton;
    QPushButton*  downButton;
    QPushButton*  leftButton;
    QPushButton*  rightButton;
    QString       sSelectedClient;
    int           iPan;
    int           iTilt;
    int           panMin;
    int           panMax;
    int           tiltMin;
    int           tiltMax;

public:
    void clear();
    void addItem(QString sAddress);
};

#endif // CLIENTLISTDIALOG_H
