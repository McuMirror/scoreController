#ifndef CHOOSEDISCIPLINE_H
#define CHOOSEDISCLPLINE_H

#include <QDialog>
#include <QTranslator>
#include "utility.h"
#include <QSettings>



namespace Ui {
class ChooseDiscipline;
}


class ChooseDiscipline : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseDiscipline(QWidget *parent = Q_NULLPTR);
    ~ChooseDiscipline();
    int getDiscipline();

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
    QTranslator Translator;
    QSettings    *pSettings;
};

#endif // CHOOSEDISCIPLINE_H
