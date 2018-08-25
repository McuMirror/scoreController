#include "sportSelector.h"
#include "utility.h"

#include <QCoreApplication>


/*!
 * \brief SportSelector::SportSelector
 * \param parent
 */
SportSelector::SportSelector(QWidget *parent)
    : QDialog(parent)
    , pSettings(Q_NULLPTR)
{
    QIcon myIcon(WINDOW_ICON);
    setWindowIcon(myIcon);
    discipline = VOLLEY_PANEL;
    createLayout();

    pSettings =  new QSettings("Gabriele Salvato", "Choose Discipline");
    sCurrentLanguage = pSettings->value("language/current", QString(tr("Italiano"))).toString();
    delete pSettings;
    pSettings = Q_NULLPTR;

    volleyRadioButton.setText("Volley");
    handballRadioButton.setText("Handball");
    basketRadioButton.setText("Basket");
    volleyRadioButton.setChecked(true);

    languageComboBox.addItem(QString("Italiano"));
    languageComboBox.addItem(QString("English"));
    languageComboBox.setCurrentText(sCurrentLanguage);

    QCoreApplication::removeTranslator(&Translator);
    if(sCurrentLanguage == QString("English")) {
        Translator.load(":/scoreController_en");
        QCoreApplication::installTranslator(&Translator);
    }

    goPushButton.setText(tr("Avvia"));
    exitPushButton.setText(tr("Esci"));

    setEventHandlers();
    setLayout(createLayout());
}



/*!
 * \brief SportSelector::getDiscipline
 * \return
 */
int
SportSelector::getDiscipline() {
    return discipline;
}


/*!
 * \brief SportSelector::getLanguage
 * \return
 */
QString
SportSelector::getLanguage() {
    return sCurrentLanguage;
}


/*!
 * \brief SportSelector::setEventHandlers
 */
void
SportSelector::setEventHandlers() {
    connect(&volleyRadioButton, SIGNAL(clicked()),
            this, SLOT(on_volleyRadioButton_clicked()));
    connect(&handballRadioButton, SIGNAL(clicked()),
            this, SLOT(on_handballRadioButton_clicked()));
    connect(&basketRadioButton, SIGNAL(clicked()),
            this, SLOT(on_basketRadioButton_clicked()));

    connect(&languageComboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(on_LanguageComboBox_currentIndexChanged(QString)));

    connect(&goPushButton, SIGNAL(clicked()),
            this, SLOT(on_goPushButton_clicked()));
    connect(&exitPushButton, SIGNAL(clicked()),
            this, SLOT(on_exitPushButton_clicked()));
}


/*!
 * \brief SportSelector::createLayout
 * \return
 */
QGridLayout*
SportSelector::createLayout() {
    auto*  mainLayout = new QGridLayout();
    mainLayout->addWidget(&volleyRadioButton,   0, 0, 1, 3);
    mainLayout->addWidget(&handballRadioButton, 1, 0, 1, 3);
    mainLayout->addWidget(&basketRadioButton,   2, 0, 1, 3);

    mainLayout->addWidget(&languageComboBox,    1, 3, 1, 2);

    mainLayout->addWidget(&goPushButton,        3, 0, 1, 2);
    mainLayout->addWidget(&exitPushButton,      3, 4, 1, 2);

    return mainLayout;
}


/*!
 * \brief SportSelector::on_basketRadioButton_clicked
 */
void
SportSelector::on_basketRadioButton_clicked() {
    discipline = BASKET_PANEL;
}


/*!
 * \brief SportSelector::on_volleyRadioButton_clicked
 */
void
SportSelector::on_volleyRadioButton_clicked() {
    discipline = VOLLEY_PANEL;
}


/*!
 * \brief SportSelector::on_handballRadioButton_clicked
 */
void
SportSelector::on_handballRadioButton_clicked() {
    discipline = HANDBALL_PANEL;
}


/*!
 * \brief SportSelector::on_goPushButton_clicked
 */
void
SportSelector::on_goPushButton_clicked() {
    pSettings =  new QSettings("Gabriele Salvato", "Choose Discipline");
    pSettings->setValue("language/current", sCurrentLanguage);
    delete pSettings;
    pSettings = Q_NULLPTR;
    done(QDialog::Accepted);
}

/*!
 * \brief SportSelector::on_exitPushButton_clicked
 */
void
SportSelector::on_exitPushButton_clicked() {
    done(QDialog::Rejected);
}


/*!
 * \brief SportSelector::on_LanguageComboBox_currentIndexChanged
 * \param arg1
 */
void
SportSelector::on_LanguageComboBox_currentIndexChanged(const QString &arg1) {
    sCurrentLanguage = arg1;
    QCoreApplication::removeTranslator(&Translator);
    if(sCurrentLanguage == QString("English")) {
        Translator.load(":/scoreController_en");
        QCoreApplication::installTranslator(&Translator);
    }
}


/*!
 * \brief SportSelector::changeEvent
 * \param event
 */
void
SportSelector::changeEvent(QEvent *event) {
     if (event->type() == QEvent::LanguageChange) {
         #ifdef LOG_VERBOSE
             logMessage(Q_NULLPTR,
                        Q_FUNC_INFO,
                        QString("%1  %2")
                        .arg(goPushButton.text())
                        .arg(exitPushButton.text()));
         #endif
         goPushButton.setText(tr("Avvia"));
         exitPushButton.setText(tr("Esci"));
     } else
         QWidget::changeEvent(event);
}

