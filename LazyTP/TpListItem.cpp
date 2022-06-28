//
// Created by TKK on 28.06.2022.
//

#include "TpListItem.h"

TpListItem::TpListItem(const QString &text, const QString &name) : QListWidgetItem(text),
                                                                   m_name(name)
{}

QString TpListItem::getName() const
{
    return m_name;
}
