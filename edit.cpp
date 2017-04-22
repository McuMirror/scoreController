#include <QtWidgets>
#include "edit.h"

Edit::Edit(int iTeam, QWidget *parent)
    : QLineEdit(parent)
    , myTeam(iTeam)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}


Edit::Edit(const QString &sString, int iTeam, QWidget *parent)
    : QLineEdit(sString, parent)
    , myTeam(iTeam)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setText(sString);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}


QSize
Edit::sizeHint() const
{
    int length = maxLength();
    QFont myFont = this->font();
    QSize size = QLineEdit::sizeHint();
    QMargins margins = this->contentsMargins();
    size.rheight() = myFont.pointSize() + margins.bottom() + margins.top();
    size.rwidth() = length*myFont.pointSize() + margins.left() + margins.right();
    return size;
}


void
Edit::onTextChanged(QString sText) {
    emit textChanged(sText, myTeam);
}

