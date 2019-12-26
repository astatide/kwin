/*
 * Copyright (c) 2019 Ismael Asensio <isma.af@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include <KQuickAddons/ConfigModule>

class QAbstractItemModel;
class QSortFilterProxyModel;
class QQuickItem;

class KCMKWinRules2 : public KQuickAddons::ConfigModule
{
    Q_OBJECT
  /*  Q_PROPERTY(QSortFilterProxyModel *themesModel READ themesModel CONSTANT)
    Q_PROPERTY(QStringList borderSizesModel READ borderSizesModel CONSTANT)
    Q_PROPERTY(int theme READ theme WRITE setTheme NOTIFY themeChanged)
 */

public:
    KCMKWinRules2(QObject *parent, const QVariantList &arguments);

public Q_SLOTS:
    void load() override;
    void save() override;
    void defaults() override;

private Q_SLOTS:
    void updateNeedsSave();
    void reloadKWinSettings();

private:
    struct Settings
    {
//         int themeIndex;
//         bool closeOnDoubleClickOnMenu;
//         bool showToolTips;
    };

    Settings m_savedSettings;
    Settings m_currentSettings;
};
