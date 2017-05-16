#ifndef CHOOSEDISCILPLINE_H
#define CHOOSEDISCILPLINE_H

#include <QDialog>

QT_FORWARD_DECLARE_CLASS(ScoreController)


#define VOLLEY_PANEL 0
#define FIRST_PANEL  VOLLEY_PANEL
#define BASKET_PANEL 1
#define LAST_PANEL   BASKET_PANEL


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

private slots:
    void on_basketRadioButton_clicked();
    void on_VolleyRadioButton_clicked();
    void on_goPushButton_clicked();

    void on_closePushButton_clicked();

private:
    Ui::ChooseDiscilpline *ui;
    int discipline;
    ScoreController* pController;
};

#endif // CHOOSEDISCILPLINE_H
