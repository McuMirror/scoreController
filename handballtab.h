#ifndef HANDBALLTAB_H
#define HANDBALLTAB_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QSettings)


class HandBallTab : public QWidget
{
    Q_OBJECT
public:
    explicit HandBallTab(QWidget *parent = nullptr);
    void GetSettings();
    void StoreSettings();

signals:

public slots:

private:
    QSettings *pSettings;
};

#endif // HANDBALLTAB_H
