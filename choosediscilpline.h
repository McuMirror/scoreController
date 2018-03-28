#ifndef CHOOSEDISCILPLINE_H
#define CHOOSEDISCILPLINE_H

#include <QDialog>
#include "utility.h"


QT_FORWARD_DECLARE_CLASS(ScoreController)



namespace Ui {
class ChooseDiscilpline;
}


class ChooseDiscilpline : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseDiscilpline(QWidget *parent = 0);
    ~ChooseDiscilpline();
    int getDiscipline();

public slots:
    void onPanelDone();

signals:
    void finished();

private slots:
    void on_volleyRadioButton_clicked();
    void on_basketRadioButton_clicked();
    void on_goPushButton_clicked();
    void on_closePushButton_clicked();
    void on_handballRadioButton_clicked();

private:
    Ui::ChooseDiscilpline *ui;
    int discipline;
    ScoreController* pController;
};

#endif // CHOOSEDISCILPLINE_H
