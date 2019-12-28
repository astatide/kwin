/*
 * Copyright 2020  Ismael Asensio <isma.af@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rulesmodel.h"

#include <QDebug>
#include <QIcon>
#include <KLocalizedString>

namespace KWin
{

RulesModel::RulesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

RulesModel::~RulesModel() = default;

QHash< int, QByteArray > RulesModel::roleNames() const
{
    QHash<int, QByteArray> roles({
        {NameRole,          QByteArrayLiteral("name")},
        {IconRole,          QByteArrayLiteral("icon")},
        {DescriptionRole,   QByteArrayLiteral("description")},
        {KeyRole,           QByteArrayLiteral("key")},
        {ValueRole,         QByteArrayLiteral("value")},
        {SectionRole,       QByteArrayLiteral("section")},
        {EnabledRole,       QByteArrayLiteral("enabled")},
    });
    return roles;
}

int RulesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_ruleList.size();
}

QVariant RulesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() != 0 || index.row() < 0 || index.row() >= int(m_ruleList.size())) {
        return QVariant();
    }
    const RuleItem *d = m_ruleList.at(index.row());
    switch (role) {
    case NameRole:
        return d->name();
    case KeyRole:
        return d->key();
    case IconRole:
        return d->icon();
//     case DescriptionRole:
//         return d.m_description;
//     case SectionRole:
//         return d.m_section;
//     case EnabledRole:
//         return d->isEnabled();
//     case ValueRole:
//         return d.m_value;
    }
    return QVariant();
}

void RulesModel::init()
{
    beginResetModel();

    m_ruleList.clear();

    m_ruleList.append(new RuleItem("position",   i18n("Position")   , QStringLiteral("transform-move") ));
    m_ruleList.append(new RuleItem("size",       i18n("Size")       , QStringLiteral("image-resize-symbolic")));
    m_ruleList.append(new RuleItem("fullscreen", i18n("Fullscreen") , QStringLiteral("view-fullscreen") ));

    endResetModel();
}

const QString RulesModel::ruleName() {
    return m_ruleName;
}

void RulesModel::setRuleName(const QString& ruleName) {
    m_ruleName = ruleName;
}

RuleItem::RuleItem(const QString& key,
                   const QString& name,
                   const QString& iconName)
    : m_key(key)
    , m_name(name)
    , m_iconName(iconName)
{
}

QString RuleItem::key() const {
    return m_key;
}

QString RuleItem::name() const {
    return m_name;
}

QIcon RuleItem::icon() const {
    return QIcon::fromTheme(m_iconName);
    //return m_iconName;
}


}
