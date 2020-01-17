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
                   const RulePolicy::Type policyType,
                   const RuleType type,
                   const QString &name,
                   const QString &section,
                   const QString &iconName)
    : d(new RuleItemPrivate(key, name, section, iconName))
    , p(new RulePolicy(policyType))
{
    d->m_type = type;
    setValue(QVariant());
}

RuleItem::~RuleItem()
{
    delete d;
    delete p;
}

void RuleItem::reset()
{
    setValue(QVariant());
    setPolicy(Rules::Unused);
    setEnabled(hasFlag(AlwaysEnabled) | hasFlag(StartEnabled));
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

void RuleItem::setDescription(const QString& description)
{
    d->m_description = description;
}


bool RuleItem::isEnabled() const
{
    return d->m_enabled;
}

void RuleItem::setEnabled(bool enabled)
{
    d->m_enabled = enabled | hasFlag(AlwaysEnabled);
}

bool RuleItem::hasFlag(uint flag) const
{
    return (d->m_flags & flag) == flag;
}

void RuleItem::setFlags(uint flags, bool active)
{
    if (active) {
        d->m_flags |= flags;
    } else {
        d->m_flags &= (AllFlags ^ flags);
    }
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
    return p->value();
}

void RuleItem::setPolicy(int policy)
{
    p->setValue(policy);
}

int RuleItem::policyIndex() const
{
    return p->index();
}

void RuleItem::setPolicyIndex(int policyIndex)
{
    p->setIndex(policyIndex);
}

RulePolicy::Type RuleItem::policyType() const
{
    return p->type();
}

QStringList RuleItem::policyModel() const
{
    return p->descriptionList();
}

QString RuleItem::policyKey() const
{
    return p->policyKey(d->m_key);
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
        case FlagsOption:
            return value.toUInt();
        case Percentage:
            return value.toUInt();
        case Coordinate:
            if (value.toString().isEmpty()) {
                return QStringLiteral("0,0");
            }
            return value.toString();
        case Shortcut:
            return value.toString();
    }

    return value;
}


int OptionsModel::index() const
{
    return m_index;
}

void OptionsModel::setIndex(int index)
{
    Q_ASSERT (index >= 0 && index < m_data.count());
    m_index = index;
}

QVariant OptionsModel::value() const
{
    return m_data.at(m_index).value;
}

void OptionsModel::setValue(QVariant value)
{
    for (int index = 0; index < m_data.count(); index++) {
        if (m_data.at(index).value == value) {
            m_index = index;
        }
    }
}

QStringList OptionsModel::descriptionList() const
{
    QStringList descList;
    for (const Data &item : m_data) {
        descList << item.text;
    }
    return descList;
}


RulePolicy::Type RulePolicy::type() const
{
    return m_type;
}

int RulePolicy::value() const
{
    if (m_type == RulePolicy::NoPolicy) {
        return Rules::Apply;   // To simplify external checks (!=0) when rule has no policy
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
            {Rules::RegExpMatch,      i18n("Regular Expresion")}
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
