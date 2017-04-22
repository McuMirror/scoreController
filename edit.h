#ifndef EDIT_H
#define EDIT_H

#include <QLineEdit>

class Edit : public QLineEdit
{
    Q_OBJECT

public:
    explicit Edit(int iTeam=0, QWidget *parent = Q_NULLPTR);
    explicit Edit(const QString &, int iTeam=0, QWidget *parent = Q_NULLPTR);

public:
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void onTextChanged(QString sText);

signals:
    void textChanged(QString sText, int iTeam);

protected:
    int myTeam;
};

#endif // EDIT_H
