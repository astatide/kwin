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

#include "kcmrules.h"
#include <config-kwin.h>

#include <QDebug>
#include <QQuickItem>

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>


K_PLUGIN_FACTORY_WITH_JSON(KCMKWinRulesFactory,
                           "kcm_kwinrules_qml.json",
                           registerPlugin<KWin::KCMKWinRules>(););


namespace
{
    const QString s_configFile { QStringLiteral("kwinrulesrc") };
}

namespace KWin
{

KCMKWinRules::KCMKWinRules(QObject *parent, const QVariantList &arguments)
    : KQuickAddons::ConfigModule(parent, arguments)
    , m_rulesConfig(new KConfig(s_configFile))
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

    setQuickHelp(i18n("<p><h1>Window-specific Settings</h1> Here you can customize window settings specifically only"
                      " for some windows.</p>"
                      " <p>Please note that this configuration will not take effect if you do not use"
                      " KWin as your window manager. If you do use a different window manager, please refer to its documentation"
                      " for how to customize window behavior.</p>"));
}

KCMKWinRules::~KCMKWinRules() {
    delete m_rulesConfig;
}

QStringList KCMKWinRules::rulesListModel() const
{
    return m_rulesListModel;
}

void KCMKWinRules::load()
{
    m_rulesListModel.clear();

    KConfigGroup cfg(m_rulesConfig, "General");
    int rulesCount = cfg.readEntry("count", 0);

    for (int i = 1; i <= rulesCount; i++) {
        cfg = KConfigGroup(m_rulesConfig, QString::number(i));

        const QString ruleDescription = cfg.readEntry("Description");
        m_rulesListModel.append(ruleDescription);
        //qDebug() << ruleDescription;

        //KWin::Rules *rule = new KWin::Rules(cfg);
        //m_rulesList << rule;
    }

    emit rulesListModelChanged();
}

void KCMKWinRules::save()
{
}

} // namespace

#include "kcmrules.moc"
