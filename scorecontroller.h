#ifndef SCORECONTROLLER_H
#define SCORECONTROLLER_H

#include <QWidget>

class ScoreController : public QWidget
{
    Q_OBJECT

public:
    ScoreController(QWidget *parent = 0);
    ~ScoreController();

protected:
    void WaitForNetworkReady();
};

#endif // SCORECONTROLLER_H
