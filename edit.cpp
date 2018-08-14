#include <QtWidgets>
#include <QDesktopWidget>
#include <utility>
#include "edit.h"


/*!
 * \brief Edit::Edit
 * \param iTeam
 * \param parent
 */
Edit::Edit(int iTeam, QWidget *parent)
    : QLineEdit(parent)
    , myTeam(iTeam)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}


/*!
 * \brief Edit::Edit
 * \param sString
 * \param iTeam
 * \param parent
 */
Edit::Edit(const QString &sString, int iTeam, QWidget *parent)
    : QLineEdit(sString, parent)
    , myTeam(iTeam)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setText(sString);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}


/*!
 * \brief Edit::sizeHint
 * \return
 */
QSize
Edit::sizeHint() const
{
    int length = maxLength();
    QFontMetrics f(font());
    QSize size = QLineEdit::sizeHint();
    QMargins margins = contentsMargins();
    size.setHeight(f.height() + f.descent() + f.ascent() + margins.bottom() + margins.top());
    size.setWidth(length*f.maxWidth() + margins.left() + margins.right());
    return size;
}


/*!
 * \brief Edit::onTextChanged
 * \param sText
 */
void
Edit::onTextChanged(QString sText) {
    emit textChanged(std::move(sText), myTeam);
}

