#include "panelconfigurator.h"
#include "ui_panelconfigurator.h"


/*!
 * \brief PanelConfigurator::PanelConfigurator
 * A Dialog to configure some Panel behaviours
 * \param parent
 */
PanelConfigurator::PanelConfigurator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PanelConfigurator)
    , panMin(-30)
    , panMax(30)
    , tiltMin(-30)
    , tiltMax(30)
{
    ui->setupUi(this);
    SetupButtons();
    ui->orientationCombo->addItem(QString(tr("Normale")));
    ui->orientationCombo->addItem(QString(tr("Riflesso")));
    ui->orientationCombo->setDisabled(true);
}


/*!
 * \brief PanelConfigurator::~PanelConfigurator
 */
PanelConfigurator::~PanelConfigurator() {
    delete ui;
}


/*!
 * \brief PanelConfigurator::exec
 * \return
 */
int
PanelConfigurator::exec() {
    ui->leftButton->setDisabled(true);
    ui->rightButton->setDisabled(true);
    ui->upButton->setDisabled(true);
    ui->downButton->setDisabled(true);
    ui->orientationCombo->setDisabled(true);
    ui->scoreOnlyCheckBox->setDisabled(true);
    ui->tabWidget->setCurrentIndex(0);
    emit stopCamera();
    if(!isVisible()) QDialog::show();
    return 0;
}


/*!
 * \brief PanelConfigurator::show
 */
void
PanelConfigurator::show() {
    ui->leftButton->setDisabled(true);
    ui->rightButton->setDisabled(true);
    ui->upButton->setDisabled(true);
    ui->downButton->setDisabled(true);
    ui->orientationCombo->setDisabled(true);
    ui->scoreOnlyCheckBox->setDisabled(true);
    ui->tabWidget->setCurrentIndex(0);
    emit stopCamera();
    if(!isVisible()) QDialog::show();
}


/*!
 * \brief PanelConfigurator::SetCurrentPanTilt
 * \param newPan
 * \param newTilt
 */
void
PanelConfigurator::SetCurrentPanTilt(int newPan, int newTilt) {
    iPan  = newPan;
    iTilt = newTilt;
    ui->leftButton->setDisabled(iPan <= panMin);
    ui->rightButton->setDisabled(iPan >= panMax);
    ui->upButton->setDisabled(iTilt >= tiltMax);
    ui->downButton->setDisabled(iTilt <= tiltMin);
}


/*!
 * \brief PanelConfigurator::SetCurrrentOrientaton
 * \param index
 */
void
PanelConfigurator::SetCurrrentOrientaton(int index) {
    ui->orientationCombo->setCurrentIndex(index);
    ui->orientationCombo->setEnabled(true);
}


/*!
 * \brief PanelConfigurator::SetupButtons
 */
void
PanelConfigurator::SetupButtons() {
    int iDelay  = 300;
    int iRepeat = 100;

    ui->upButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    ui->upButton->setAutoRepeat(true);
    ui->upButton->setAutoRepeatDelay(iDelay);
    ui->upButton->setAutoRepeatInterval(iRepeat);

    ui->downButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
    ui->downButton->setAutoRepeat(true);
    ui->downButton->setAutoRepeatDelay(iDelay);
    ui->downButton->setAutoRepeatInterval(iRepeat);

    ui->leftButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->leftButton->setAutoRepeat(true);
    ui->leftButton->setAutoRepeatDelay(iDelay);
    ui->leftButton->setAutoRepeatInterval(iRepeat);

    ui->rightButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->rightButton->setAutoRepeat(true);
    ui->rightButton->setAutoRepeatDelay(iDelay);
    ui->rightButton->setAutoRepeatInterval(iRepeat);
}


/*!
 * \brief PanelConfigurator::on_upButton_pressed
 */
void
PanelConfigurator::on_upButton_pressed() {
    iTilt++;
    ui->downButton->setDisabled(false);
    if(iTilt <= tiltMax)
        emit newTiltValue(iTilt);
    else {
        iTilt = tiltMax;
        ui->upButton->setDisabled(true);
    }
}


/*!
 * \brief PanelConfigurator::on_downButton_pressed
 */
void
PanelConfigurator::on_downButton_pressed() {
    iTilt--;
    ui->upButton->setDisabled(false);
    if(iTilt >= tiltMin)
        emit newTiltValue(iTilt);
    else {
        iTilt = tiltMin;
        ui->downButton->setDisabled(true);
    }
}


/*!
 * \brief PanelConfigurator::on_leftButton_pressed
 */
void
PanelConfigurator::on_leftButton_pressed() {
    iPan--;
    ui->rightButton->setDisabled(false);
    if(iPan >= panMin)
        emit newPanValue(iPan);
    else {
        iPan = panMin;
        ui->leftButton->setDisabled(true);
    }
}


/*!
 * \brief PanelConfigurator::on_rightButton_pressed
 */
void
PanelConfigurator::on_rightButton_pressed() {
    iPan++;
    ui->leftButton->setDisabled(false);
    if(iPan <= panMax)
        emit newPanValue(iPan);
    else {
        iPan = panMax;
        ui->rightButton->setDisabled(true);
    }
}


/*!
 * \brief PanelConfigurator::on_orientationCombo_currentIndexChanged
 * \param index
 */
void
PanelConfigurator::on_orientationCombo_currentIndexChanged(int index) {
    emit changeOrientation(static_cast<PanelOrientation>(index));
}


/*!
 * \brief PanelConfigurator::on_tabWidget_tabBarClicked
 * \param index
 */
void
PanelConfigurator::on_tabWidget_tabBarClicked(int index) {
    if(index == 0) {
        emit stopCamera();
    }
    else {
        emit startCamera();
    }
}


/*!
 * \brief PanelConfigurator::on_closeButton_clicked
 */
void
PanelConfigurator::on_closeButton_clicked() {
    emit closingDialog();
    close();
}


/*!
 * \brief PanelConfigurator::on_scoreOnlyCheckBox_clicked
 * \param checked
 */
void
PanelConfigurator::on_scoreOnlyCheckBox_clicked(bool checked) {
    emit scoreOnly(checked);
}


/*!
 * \brief PanelConfigurator::SetIsScoreOnly
 * \param bScoreOnly
 */
void
PanelConfigurator::SetIsScoreOnly(bool bScoreOnly) {
    ui->scoreOnlyCheckBox->setChecked(bScoreOnly);
    ui->scoreOnlyCheckBox->setEnabled(true);
}

