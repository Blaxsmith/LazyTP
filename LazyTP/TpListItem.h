//
// Created by TKK on 28.06.2022.
//

#ifndef LAZYTP_TPLISTITEM_H
#define LAZYTP_TPLISTITEM_H

#include <QListWidgetItem>

class TpListItem : public QListWidgetItem
{
public:
    TpListItem(const QString &text, const QString &name);

    [[nodiscard]] QString getName() const;

private:
    QString m_name;

};


#endif //LAZYTP_TPLISTITEM_H
