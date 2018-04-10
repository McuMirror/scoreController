#ifndef CHOOSEDISCIPLINE_H
#define CHOOSEDISCLPLINE_H

#include <QDialog>
#include <QTranslator>
#include "utility.h"
#include <QSettings>


QT_FORWARD_DECLARE_CLASS(ScoreController)



namespace Ui {
class ChooseDiscipline;
}


class ChooseDiscipline : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseDiscipline(QWidget *parent = 0);
    ~ChooseDiscipline();
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

    void on_LanguageComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::ChooseDiscipline *ui;
    int discipline;
    ScoreController* pController;
    QTranslator Translator;
    QSettings    *pSettings;
};

#endif // CHOOSEDISCIPLINE_H
