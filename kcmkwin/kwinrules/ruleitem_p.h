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

#ifndef KWIN_RULEITEM_P_H
#define KWIN_RULEITEM_P_H

#include <QVariant>

namespace KWin {

enum RuleType {
    Undefined,
    Boolean,
    String,
    Integer,
    Option,
    Flags,
    Percentage,
    Coordinate,
    Shortcut
};


class RuleItemPrivate
{
public:
    RuleItemPrivate();
    RuleItemPrivate(const QString &key,
                    const QString &name,
                    const QString &section,
                    const QString &iconName,
                    const QString &description)
        : m_key(key)
        , m_name(name)
        , m_section(section)
        , m_iconName(iconName)
        , m_description(description)
        , m_enabled(false)
        {};

public:
    QString m_key;
    QString m_name;
    QString m_section;
    QString m_iconName;
    QString m_description;

    bool m_enabled;

    RuleType m_type;
    QVariant m_value;
};


class RulePolicy
{
public:
    enum Type {
        NoPolicy,
        StringMatch,
        SetRule,
        ForceRule
    };

private:
    struct Data {
        int value;
        QString text;
    };

public:
    RulePolicy(Type type)
        : m_type(type)
        , m_data(policyOptions(type))
        , m_index(0)
        {};

    Type type() const;
    QString policyKey(const QString &key) const;

    QStringList descriptionList() const;

    int value() const;
    void setValue(int value);

    int index() const;
    void setIndex(int index);

private:
    static QList<Data> policyOptions(RulePolicy::Type type);

private:
    Type m_type;
    QList<Data> m_data;
    int m_index = 0;
};

}   //namespace

#endif  //KWIN_RULEITEM_P_H
