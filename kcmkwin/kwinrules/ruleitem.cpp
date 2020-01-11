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

#include "ruleitem.h"
#include <rules.h>

#include <KLocalizedString>

using namespace KWin;

RuleItem::RuleItem(const QString &key,
                   const RulePolicyType policyType,
                   const RuleType type,
                   const QString &name,
                   const QString &section,
                   const QString &iconName,
                   const QString &description)
    : d(new RuleItemPrivate(key, name, section, iconName, description))
{
    d->m_policyType = policyType;

    d->m_type = type;
    setValue(QVariant());
}

RuleItem::~RuleItem()
{
    delete d;
}

void RuleItem::reset()
{
    setValue(QVariant());
    setPolicy(0);
    setEnabled(false);
}

QString RuleItem::key() const
{
    return d->m_key;
}

QString RuleItem::name() const
{
    return d->m_name;
}

QString RuleItem::section() const
{
    return d->m_section;
}

QString RuleItem::iconName() const
{
    return d->m_iconName;
}

QIcon RuleItem::icon() const
{
    return QIcon::fromTheme(d->m_iconName);
}

QString RuleItem::description() const
{
    return d->m_description;
}

bool RuleItem::isEnabled() const
{
    return d->m_enabled;
}

void RuleItem::setEnabled(bool enabled)
{
    d->m_enabled = enabled;
}

RuleType RuleItem::type() const
{
    return d->m_type;
}

QVariant RuleItem::value() const
{
    return d->m_value;
}

void RuleItem::setValue(QVariant value)
{
    d->m_value = typedValue(value, d->m_type);
}

int RuleItem::policy() const
{
    if (d->m_policyType == RulePolicyType::NoPolicy) {
        return Rules::Apply;
    }
    return d->m_policyValue;
}

void RuleItem::setPolicy(int policy)
{
    d->m_policyValue = policy;
}

RulePolicyType RuleItem::policyType() const
{
    return d->m_policyType;
}

QStringList RuleItem::policyModel() const
{
    QStringList policyList;
/*
    const auto intOptions = policyOptions(d->m_policyType);
    for (const int option : intOptions)
    {
        if (d->m_policyType == RulePolicyType::StringMatch) {
            switch (option):
                case Rules::UnimportantMatch:
                    policyList << i18n("");
                case     Rules::ExactMatch,
                    return i18n("");
                case     Rules::SubstringMatch,
                    return i18n("");
                case     Rules::RegExpMatch,
                    return i18n("");
                };
            case SetRule:
            case ForceRule:
                return QVector<int> {
                    Rules::DontAffect,
                    Rules::Apply,
                    Rules::Remember,
                    Rules::Force,
                    Rules::ApplyNow,
                    Rules::ForceTemporarily
                };
    }
    */
    return QStringList();
}

QString RuleItem::policyKey() const
{
    switch (d->m_policyType) {
        case NoPolicy:
            return QString();
        case StringMatch:
            return QStringLiteral("%1match").arg(d->m_key);
        case SetRule:
        case ForceRule:
            return QStringLiteral("%1rule").arg(d->m_key);
    }

    return QString();
}

QVariant RuleItem::typedValue(const QVariant &value, const RuleType type)
{
    switch (type) {
        case Undefined:
            return value;
        case Boolean:
            return value.toBool();
        case String:
            return value.toString();
        case Integer:
            return value.toInt();
        case Option:
            return value.toString();
        case Flags:
            return value.toUInt();
        case Percentage:
            return value.toUInt();
        case Coordinate:
            return value.toString();
        case Shortcut:
            return value.toString();
    }

    return value;
}

QVector<int> RuleItem::policyOptions(RulePolicyType policyType)
{
    switch (policyType) {
    case NoPolicy:
        return QVector<int> {};
    case StringMatch:
        return QVector<int> {
            Rules::UnimportantMatch,
            Rules::ExactMatch,
            Rules::SubstringMatch,
            Rules::RegExpMatch,
        };
    case SetRule:
        return QVector<int> {
            Rules::DontAffect,
            Rules::Apply,
            Rules::Remember,
            Rules::Force,
            Rules::ApplyNow,
            Rules::ForceTemporarily
        };
    case ForceRule:
        return QVector<int> {
            Rules::DontAffect,
            Rules::Force,
            Rules::ForceTemporarily
        };
    }

    return QVector<int> {};
}

/*
static const int set_rule_to_combo[] = {
    0, // Unused
    0, // Don't Affect
    3, // Force
    1, // Apply
    2, // Remember
    4, // ApplyNow
    5  // ForceTemporarily
};

static const Rules::SetRule combo_to_set_rule[] = {
    (Rules::SetRule)Rules::DontAffect,
    (Rules::SetRule)Rules::Apply,
    (Rules::SetRule)Rules::Remember,
    (Rules::SetRule)Rules::Force,
    (Rules::SetRule)Rules::ApplyNow,
    (Rules::SetRule)Rules::ForceTemporarily
};

static const int force_rule_to_combo[] = {
    0, // Unused
    0, // Don't Affect
    1, // Force
    0, // Apply
    0, // Remember
    0, // ApplyNow
    2  // ForceTemporarily
};

static const Rules::ForceRule combo_to_force_rule[] = {
    (Rules::ForceRule)Rules::DontAffect,
    (Rules::ForceRule)Rules::Force,
    (Rules::ForceRule)Rules::ForceTemporarily
};
*/
