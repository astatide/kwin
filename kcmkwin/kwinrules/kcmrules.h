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

#pragma once

#include <rules.h>
#include <rulesmodel.h>

#include <QStringListModel>

#include <KConfig>
#include <KQuickAddons/ConfigModule>

class QAbstractItemModel;
class QQuickItem;

namespace KWin
{

class KCMKWinRules : public KQuickAddons::ConfigModule
{
    Q_OBJECT

    Q_PROPERTY(QStringList rulesListModel READ rulesListModel NOTIFY rulesListModelChanged)
    Q_PROPERTY(RulesModel *rulesModel MEMBER m_rulesModel CONSTANT)

public:
    explicit KCMKWinRules(QObject *parent, const QVariantList &arguments);
    ~KCMKWinRules();


public slots:
    void load() override;
    void save() override;

    void newRule();
    void editRule(int index);
    void move(int sourceRow, int destRow);

signals:
    void rulesListModelChanged();

private:
    QStringList rulesListModel() const;
    void pushRulesEditor();

private:
    KConfig *m_rulesConfig;
    QStringList m_rulesListModel;
    RulesModel* m_rulesModel;

    //QList<KWin::Rules *> m_rulesList;

};

} // namespace
