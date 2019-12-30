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

#include <QDebug>
#include <QIcon>
#include <KLocalizedString>

using namespace KWin;



RuleItem::RuleItem(RuleItemPrivate *d)
    : d(d)
{
}

RuleItem::RuleItem(const QString &key,
                   const RulePolicy policy,
                   const RuleType type,
                   const QString &name,
                   const QString &section,
                   const QString &iconName,
                   const QString &description)
{
    d = new RuleItemPrivate();

    d->m_key = key;
    d->m_name = name;
    d->m_section = section;
    d->m_icon = QIcon::fromTheme(iconName);
    d->m_description = description;

    d->m_policy = policy;

    d->m_type = type;
    d->m_value = initialValue(type);
}

RuleItem::~RuleItem()
{
    delete d;
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

QIcon RuleItem::icon() const
{
    return d->m_icon;
}

QString RuleItem::description() const
{
    return d->m_description;
}

QVariant RuleItem::value() const
{
    return d->m_value;
}

void RuleItem::setValue(QVariant value)
{
    d->m_value = value;
}

bool RuleItem::isEnabled() const
{
    return d->m_enabled;
}

void RuleItem::setEnabled(bool enabled)
{
    d->m_enabled = enabled;
}

RulePolicy RuleItem::policy() const
{
    return d->m_policy;
}

RuleType RuleItem::type() const
{
    return d->m_type;
}

QString RuleItem::policyKey() const
{
    switch (d->m_policy) {
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

int RuleItem::policyValue() const {
    return d->m_policyValue;
}

void RuleItem::setPolicyValue(int policyValue)
{
    d->m_policyValue = policyValue;
}

QVector<int> RuleItem::policyOptions(RulePolicy policy)
{
    switch (policy) {
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

QVariant RuleItem::initialValue(RuleType type)
{
    switch (type) {
        case Undefined:
            return QVariant();
        case Boolean:
            return bool(true);
        case String:
            return QString("test");
        case Integer:
            return int(5);
        case Option:
            return QString();
        case Flags:
            return uint();
        case Percentage:
            return uint(50);
        case Shortcut:
            return QString();
    }

    return QVariant();
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
