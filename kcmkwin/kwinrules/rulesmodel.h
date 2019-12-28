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
#ifndef KWIN_RULES_MODEL_H
#define KWIN_RULES_MODEL_H

#include <QAbstractListModel>
#include <QObject>

namespace KWin
{

class RuleItem;

class RulesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString ruleName READ ruleName WRITE setRuleName)

public:
    enum RulesRole {
        NameRole = Qt::DisplayRole,
        IconRole = Qt::UserRole + 1,
        DescriptionRole = Qt::ToolTipRole,
        KeyRole = Qt::UserRole + 10,
        ValueRole,
        ValueTypeRole,
        SectionRole,
        EnabledRole,
    };

public:
    explicit RulesModel(QObject *parent = nullptr);
    ~RulesModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash< int, QByteArray > roleNames() const override;

    QModelIndex findRule(const QString &ruleKey) const;

    const QString ruleName();
    void setRuleName(const QString& ruleName);

public slots:
    void init();

private:
    QList<RuleItem *> m_ruleList;
    QString m_ruleName;

};


class RuleItem : public QObject
{
public:
    enum RuleType {
        Boolean,
        Integer,
        String,
        Position,
        Size,
    };

    enum RulePolicy{
        NoPolicy,
        StringMatch,
        SetRule,
        ForceRule
    };

public:
    RuleItem(const QString& key,
             const QString& name,
             const QString& iconName=QStringLiteral("window"));

    QString key() const;
    QString name() const;
    QIcon icon() const;

private:
    QString m_key;
    QString m_name;
    QString m_iconName;
    QString m_description;
    QVariant m_value;
    RuleType m_type;
    RulePolicy m_policy;
    QString m_section;
    bool m_enabled = false;
};

}

#endif
