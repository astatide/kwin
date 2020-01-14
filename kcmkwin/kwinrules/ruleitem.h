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

#include "ruleitem_p.h"

#include <QIcon>


namespace KWin
{

class RuleItem : public QObject
{
public:
    RuleItem(const QString &key,
             const RulePolicy::Type policyType,
             const RuleType type,
             const QString &name,
             const QString &section,
             const QString &iconName=QStringLiteral("window"),
             const QString &description=QString());
    ~RuleItem();

    QString key() const;
    QString name() const;
    QString section() const;
    QString iconName() const;
    QIcon icon() const;
    QString description() const;

    bool isEnabled() const;

    RuleType type() const;
    QVariant value() const;

    RulePolicy::Type policyType() const;
    int policy() const;          // int belongs to anonymous enum in Rules::
    int policyIndex() const;
    QStringList policyModel() const;
    QString policyKey() const;

    void setEnabled(bool enabled);
    void setValue(QVariant value);
    void setPolicy(int policy);  // int belongs to anonymous enum in Rules::
    void setPolicyIndex(int policyIndex);

    void reset();

private:
    static QVariant typedValue(const QVariant &value, const RuleType type);

private:
    RuleItemPrivate *d;
    RulePolicy *p;
};

}   //namespace

#endif  //KWIN_RULEITEM_H
