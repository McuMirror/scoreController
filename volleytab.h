#ifndef VOLLEYTAB_H
#define VOLLEYTAB_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>


class VolleyTab : public QWidget
{
    Q_OBJECT
public:
    explicit VolleyTab(QWidget *parent = nullptr);
    void setNumTimeout(int nTimeout);
    void setNumSet(int nSet);
    void setTimeoutDuration(int iDuration);
    int getNumTimeout();
    int getNumSet();
    int getTimeoutDuration();

signals:

public slots:

private:
    QLineEdit numTimeoutEdit;
    QLineEdit maxSetEdit;
    QLineEdit timeoutDurationEdit;

};

#endif // VOLLEYTAB_H
