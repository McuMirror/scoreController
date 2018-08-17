#ifndef BASKETTAB_H
#define BASKETTAB_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QSettings)


class BasketTab : public QWidget
{
    Q_OBJECT
public:
    explicit BasketTab(QWidget *parent = nullptr);
    void GetSettings();
    void StoreSettings();

signals:

public slots:

private:
    QSettings *pSettings;
};

#endif // BASKETTAB_H
