#ifndef DIRECTORYTAB_H
#define DIRECTORYTAB_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class DirectoryTab : public QWidget
{
    Q_OBJECT
public:
    explicit DirectoryTab(QWidget *parent = nullptr);
    void setSlideDir(const QString& sDir);
    void setSpotDir(const QString& sDir);
    QString getSlideDir();
    QString getSpotDir();

signals:

public slots:
    void onSelectSlideDir();
    void onSelectSpotDir();

private:
    QLineEdit slidesDirEdit;
    QLineEdit spotsDirEdit;
    QPushButton buttonSelectSlidesDir;
    QPushButton buttonSelectSpotsDir;
};

#endif // DIRECTORYTAB_H
