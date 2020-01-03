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

#ifndef KWIN_RULEITEM_H
#define KWIN_RULEITEM_H

#include <QIcon>
#include <QObject>
#include <QVariant>

namespace KWin
{

class RuleItemPrivate;

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

enum RulePolicyType {
    NoPolicy,
    StringMatch,
    SetRule,
    ForceRule
};

class RuleItem : public QObject
{
public:
    RuleItem(RuleItemPrivate *d);
    ~RuleItem();

    RuleItem(const QString &key,
             const RulePolicyType policyType,
             const RuleType type,
             const QString &name,
             const QString &section,
             const QString &iconName=QStringLiteral("window"),
             const QString &description=QString());

    QString key() const;
    QString name() const;
    QString section() const;
    bool isEnabled() const;
    QIcon icon() const;
    QString description() const;

    QVariant value() const;
    RuleType type() const;

    int policy() const;
    RulePolicyType policyType() const;
    QStringList policyModel() const;
    QString policyKey() const;

    void setEnabled(bool enabled);
    void setValue(QVariant value);
    void setPolicy(int policy);
    void reset();

protected:
    RuleItemPrivate *d;

private:
    static QVariant typedValue(const QVariant &value, const RuleType type);
    static QVector<int> policyOptions(RulePolicyType policyType);
};

class RuleItemPrivate
{
public:
    QString m_key;
    QString m_name;
    QString m_section;
    QIcon m_icon;
    QString m_description;
    bool m_enabled = false;
    QVariant m_value;
    RuleType m_type;
    RulePolicyType m_policyType;
    int m_policyValue = 0;
};

}

#endif  //KWIN_RULEITEM_H
