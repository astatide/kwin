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

#include "kcm_qml.h"
#include <config-kwin.h>

#include <KAboutData>
// #include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSortFilterProxyModel>


K_PLUGIN_FACTORY_WITH_JSON(KCMKWinRules2Factory, "kwinrules.json", registerPlugin<KCMKWinRules2>(););


namespace
{
const QString s_configFile { QStringLiteral("kwinrc") };
const QString s_configGroup { QStringLiteral("org.kde.kwinrules_qml") };
const QString s_configPlugin { QStringLiteral("library") };
}

KCMKWinRules2::KCMKWinRules2(QObject *parent, const QVariantList &arguments)
    : KQuickAddons::ConfigModule(parent, arguments)
{
    auto about = new KAboutData(QStringLiteral("kcm_kwinrules_qml"),
                                i18n("Window Rules"),
                                QStringLiteral("1.0"),
                                QString(),
                                KAboutLicense::GPL);
    about->addAuthor(i18n("Ismael Asensio"),
                     i18n("Author"),
                     QStringLiteral("isma.af@gmail.com"));
    setAboutData(about);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    qmlRegisterType<QAbstractListModel>();
    qmlRegisterType<QSortFilterProxyModel>();
#else
    qmlRegisterAnonymousType<QAbstractListModel>("org.kde.kwin.KCMKWinRules", 1);
    qmlRegisterAnonymousType<QSortFilterProxyModel>("org.kde.kwin.KCMKWinRules", 1);
#endif
//     QMetaObject::invokeMethod(m_themesModel, "init", Qt::QueuedConnection);
}

void KCMKWinRules2::reloadKWinSettings()
{
//     QMetaObject::invokeMethod(m_themesModel, "init", Qt::QueuedConnection);
}

void KCMKWinRules2::load()
{
//     const KConfigGroup config = KSharedConfig::openConfig(s_configFile)->group(s_configGroup);
//
//     const QString plugin = config.readEntry(s_configPlugin, s_defaultPlugin);
//     const QString theme = config.readEntry(s_configTheme, s_defaultTheme);
//     int themeIndex = m_proxyThemesModel->mapFromSource(m_themesModel->findDecoration(plugin, theme)).row();
//     if (themeIndex < 0) {
//         qWarning() << "Plugin" << plugin << "and theme" << theme << "not found";
//     } else {
//         qDebug() << "Current theme: plugin" << plugin << "and theme" << theme;
//     }
//     setTheme(themeIndex);
//
//     setCloseOnDoubleClickOnMenu(config.readEntry(s_configCloseOnDoubleClickOnMenu, s_defaultCloseOnDoubleClickOnMenu));
//     setShowToolTips(config.readEntry(s_configShowToolTips, s_defaultShowToolTips));
//
//     const QString defaultSizeName = Utils::borderSizeToString(s_defaultBorderSize);
//     setBorderSize(Utils::stringToBorderSize(config.readEntry(s_configBorderSize, defaultSizeName)));
//     setBorderSizeAuto(config.readEntry(s_configBorderSizeAuto, s_defaultBorderSizeAuto));
//
//     m_leftButtonsModel->replace(Utils::readDecorationButtons(config, s_configDecoButtonsOnLeft, s_defaultDecoButtonsOnLeft));
//     m_rightButtonsModel->replace(Utils::readDecorationButtons(config, s_configDecoButtonsOnRight, s_defaultDecoButtonsOnRight));
//     m_currentSettings.buttonsOnLeft = m_leftButtonsModel->buttons();
//     m_currentSettings.buttonsOnRight = m_rightButtonsModel->buttons();
//
//     m_savedSettings = m_currentSettings;

    updateNeedsSave();
}

void KCMKWinRules2::save()
{
//     KConfigGroup config = KSharedConfig::openConfig(s_configFile)->group(s_configGroup);
//
//     if (m_currentSettings.themeIndex >= 0) {
//         const QModelIndex index = m_proxyThemesModel->index(m_currentSettings.themeIndex, 0);
//         if (index.isValid()) {
//             const QString plugin = index.data(KDecoration2::Configuration::DecorationsModel::PluginNameRole).toString();
//             const QString theme = index.data(KDecoration2::Configuration::DecorationsModel::ThemeNameRole).toString();
//             config.writeEntry(s_configPlugin, plugin);
//             config.writeEntry(s_configTheme, theme);
//             qDebug() << "Saved theme: plugin" << plugin << "and theme" << theme;
//         } else {
//             qWarning() << "Cannot match theme index" << m_currentSettings.themeIndex << "in model";
//         }
//     }
//
//     config.writeEntry(s_configCloseOnDoubleClickOnMenu, m_currentSettings.closeOnDoubleClickOnMenu);
//     config.writeEntry(s_configShowToolTips, m_currentSettings.showToolTips);
//     config.writeEntry(s_configBorderSize, Utils::borderSizeToString(m_currentSettings.borderSize));
//     config.writeEntry(s_configBorderSizeAuto, m_currentSettings.borderSizeAuto);
//     config.writeEntry(s_configDecoButtonsOnLeft, Utils::buttonsToString(m_currentSettings.buttonsOnLeft), KConfigGroup::Notify);
//     config.writeEntry(s_configDecoButtonsOnRight, Utils::buttonsToString(m_currentSettings.buttonsOnRight), KConfigGroup::Notify);
//     config.sync();
//
//     m_savedSettings = m_currentSettings;
//
//     // Send a signal to all kwin instances
//     QDBusMessage message = QDBusMessage::createSignal(QStringLiteral("/KWin"),
//                                                       QStringLiteral("org.kde.KWin"),
//                                                       QStringLiteral("reloadConfig"));
//     QDBusConnection::sessionBus().send(message);

    updateNeedsSave();
}

void KCMKWinRules2::defaults()
{
    /*setTheme(themeIndex);
    setBorderSize(s_defaultBorderSize);
    setBorderSizeAuto(s_defaultBorderSizeAuto);
    setCloseOnDoubleClickOnMenu(s_defaultCloseOnDoubleClickOnMenu);
    */

    updateNeedsSave();
}

void KCMKWinRules2::updateNeedsSave()
{
}

#include "kcm_qml.moc"
