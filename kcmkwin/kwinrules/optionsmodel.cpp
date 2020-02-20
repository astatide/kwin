/*
 * Copyright (c) 2020 Ismael Asensio <isma.af@gmail.com>
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

#include "optionsmodel.h"

#include <KLocalizedString>


namespace KWin
{

QHash<int, QByteArray> OptionsModel::roleNames() const
{
    return {
        {Qt::DisplayRole,    QByteArrayLiteral("text")},
        {Qt::UserRole,       QByteArrayLiteral("value")},
        {Qt::DecorationRole, QByteArrayLiteral("icon")},
        {Qt::ToolTipRole,    QByteArrayLiteral("description")},
        {Qt::UserRole + 1,   QByteArrayLiteral("iconName")},
    };
}

int OptionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_data.size();
}

QVariant OptionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() != 0 || index.row() < 0 || index.row() >= int(m_data.size())) {
        return QVariant();
    }

    const Data data = m_data.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return data.text;
        case Qt::UserRole:
            return data.value;
        case Qt::DecorationRole:
            return data.icon;
        case Qt::UserRole + 1:
            return data.icon.name();
        case Qt::ToolTipRole:
            return data.description;
    }
    return QVariant();
}

int OptionsModel::selectedIndex() const
{
    return m_index;
}

QVariant OptionsModel::value() const
{
    if (m_data.isEmpty()) {
        return QVariant();
    }
    return m_data.at(m_index).value;
}

void OptionsModel::setValue(QVariant value)
{
    if (this->value() == value) {
        return;
    }
    for (int index = 0; index < m_data.count(); index++) {
        if (m_data.at(index).value != value) {
            continue;
        }
        if (m_index != index) {
            m_index = index;
            emit valueChanged(value);
        }
    }
}

void OptionsModel::updateModelData(const QList<Data> &data) {
    beginResetModel();
    m_data = data;
    endResetModel();
}


RulePolicy::Type RulePolicy::type() const
{
    return m_type;
}

int RulePolicy::value() const
{
    if (m_type == RulePolicy::NoPolicy) {
        return Rules::Apply;   // To simplify external checks when rule has no policy
    }
    return OptionsModel::value().toInt();
}

QString RulePolicy::policyKey(const QString &key) const
{
    switch (m_type) {
        case NoPolicy:
            return QString();
        case StringMatch:
            return QStringLiteral("%1match").arg(key);
        case SetRule:
        case ForceRule:
            return QStringLiteral("%1rule").arg(key);
    }

    return QString();
}

QList<RulePolicy::Data> RulePolicy::policyOptions(RulePolicy::Type type)
{
    switch (type) {
    case NoPolicy:
        return {};
    case StringMatch:
        return {
            {Rules::UnimportantMatch, i18n("Unimportant")},
            {Rules::ExactMatch,       i18n("Exact Match")},
            {Rules::SubstringMatch,   i18n("Substring Match")},
            {Rules::RegExpMatch,      i18n("Regular Expression")}
        };
    case SetRule:
        return {
            {Rules::DontAffect,       i18n("Do Not Affect")},
            {Rules::Apply,            i18n("Apply Initially")},
            {Rules::Remember,         i18n("Remember")},
            {Rules::Force,            i18n("Force")},
            {Rules::ApplyNow,         i18n("Apply Now")},
            {Rules::ForceTemporarily, i18n("Force Temporarily")},
        };
    case ForceRule:
        return {
            {Rules::DontAffect,       i18n("Do Not Affect")},
            {Rules::Force,            i18n("Force")},
            {Rules::ForceTemporarily, i18n("Force Temporarily")},
        };
    }
    return {};
}

}   //namespace

