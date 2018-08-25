#ifndef SPORTSELECTOR_H
#define SPORTSELECTOR_H

#include <QObject>
#include <QDialog>
#include <QTranslator>
#include "utility.h"
#include <QSettings>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QGridLayout>

class SportSelector : public QDialog
{
    Q_OBJECT
public:
    SportSelector(QWidget *parent);

    int getDiscipline();
    QString getLanguage();

private slots:
    void on_volleyRadioButton_clicked();
    void on_handballRadioButton_clicked();
    void on_basketRadioButton_clicked();
    void on_goPushButton_clicked();
    void on_exitPushButton_clicked();
    void on_LanguageComboBox_currentIndexChanged(const QString &arg1);
    void changeEvent(QEvent *event);

protected:
    QGridLayout* createLayout();
    void         setEventHandlers();

private:
    QSettings    *pSettings;

    QRadioButton volleyRadioButton;
    QRadioButton handballRadioButton;
    QRadioButton basketRadioButton;
    QComboBox    languageComboBox;
    QPushButton  goPushButton;
    QPushButton  exitPushButton;

    QString      sCurrentLanguage;
    QTranslator  Translator;

    int          discipline;
};

#endif // SPORTSELECTOR_H
