#ifndef GENERALSETUPDIALOG_H
#define GENERALSETUPDIALOG_H


#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

QT_FORWARD_DECLARE_CLASS(QDialogButtonBox)
QT_FORWARD_DECLARE_CLASS(QTabWidget)

class DirectoryTab : public QWidget
{
    Q_OBJECT

public:
    explicit DirectoryTab(QWidget *parent = 0);
    void setSlideDir(QString sDir);
    void setSpotDir(QString sDir);
    QString getSlideDir();
    QString getSpotDir();

public slots:
    void onSelectSlideDir();
    void onSelectSpotDir();

private:
    QLineEdit slidesDirEdit;
    QLineEdit spotsDirEdit;
    QPushButton buttonSelectSlidesDir;
    QPushButton buttonSelectSpotsDir;
};


class GeneralSetupDialog : public QDialog
{
public:
    explicit GeneralSetupDialog(QWidget *parent = 0);
    void setSlideDir(QString sDir);
    void setSpotDir(QString sDir);
    QString getSlideDir();
    QString getSpotDir();

private:
    QTabWidget       *tabWidget;
    QDialogButtonBox *buttonBox;
    DirectoryTab     *pDirectoryTab;
};

#endif // GENERALSETUPDIALOG_H
