/*
 * Copyright (c) 2004 Lubos Lunak <l.lunak@kde.org>
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

#include <QtDBus>

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>


namespace
{
    const QString s_configFile { QLatin1String("kwinrulesrc") };
}

namespace KWin
{

KCMKWinRules::KCMKWinRules(QObject *parent, const QVariantList &arguments)
    : KQuickAddons::ConfigModule(parent, arguments)
    , m_rulesConfig(new KConfig(s_configFile, KConfig::NoGlobals))
    , m_rulesModel(new RulesModel(this))
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

    connect(m_rulesModel, &RulesModel::descriptionChanged, this, [this]{
        if (m_editIndex >=0 && m_editIndex < m_rulesListModel.count()) {
            m_rulesListModel.replace(m_editIndex, m_rulesModel->description());
            emit rulesListModelChanged();
        }
    } );
    connect(m_rulesModel, &RulesModel::dataChanged, this, &KCMKWinRules::updateNeedsSave);
}

KCMKWinRules::~KCMKWinRules() {
    m_rulesConfig->markAsClean();
    delete m_rulesConfig;
}


QStringList KCMKWinRules::rulesListModel() const
{
    return m_rulesListModel;
}


void KCMKWinRules::load()
{
    m_rulesConfig->markAsClean();
    m_rulesConfig->reparseConfiguration();

    KConfigGroup cfg(m_rulesConfig, QLatin1String("General"));
    int rulesCount = cfg.readEntry("count", 0);

    m_rulesListModel.clear();
    m_rulesListModel.reserve(rulesCount);

    for (int index = 0; index < rulesCount; index++) {
        cfg = rulesConfigGroup(index);
        const QString ruleDescription = cfg.readEntry("Description");
        m_rulesListModel.append(ruleDescription);
    }

    setNeedsSave(false);
    emit rulesListModelChanged();

    // Reset current rule editor
    if (m_editIndex > 0) {
        KConfigGroup cfgGroup = rulesConfigGroup(m_editIndex);
        m_rulesModel->readFromConfig(&cfgGroup);
    }
}

void KCMKWinRules::save()
{
    saveCurrentRule();
    m_rulesConfig->sync();

    QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}

void KCMKWinRules::updateState()
{
    KConfigGroup cfg(m_rulesConfig, QLatin1String("General"));
    cfg.writeEntry("count", m_rulesListModel.count());

    emit editIndexChanged();
    emit rulesListModelChanged();

    updateNeedsSave();
}

void KCMKWinRules::updateNeedsSave()
{
    setNeedsSave(true);
    emit needsSaveChanged();
}

void KCMKWinRules::pushRulesEditor()
{
    if (depth() < 2) {
        push(QStringLiteral("RulesEditor.qml"));
    }

    setCurrentIndex(1);
}

void KCMKWinRules::saveCurrentRule()
{
    if (m_editIndex < 0) {
        return;
    }
    if (needsSave()) {
        KConfigGroup cfgGroup = rulesConfigGroup(m_editIndex);
        m_rulesModel->writeToConfig(&cfgGroup);
    }
}


void KCMKWinRules::newRule()
{
    saveCurrentRule();

    m_editIndex = m_rulesListModel.count();
    blockSignals(true);
    m_rulesModel->initRules();
    saveCurrentRule();
    blockSignals(false);

    m_rulesListModel.append(m_rulesModel->description());
    updateState();

    pushRulesEditor();
}

int KCMKWinRules::editIndex() const
{
    return m_editIndex;
}

void KCMKWinRules::editRule(int index)
{
    Q_ASSERT(index >= 0 && index < m_rulesListModel.count());

    saveCurrentRule();

    m_editIndex = index;
    blockSignals(true);
    KConfigGroup cfgGroup = rulesConfigGroup(index);
    m_rulesModel->readFromConfig(&cfgGroup);
    blockSignals(false);

    emit editIndexChanged();

    pushRulesEditor();
}

void KCMKWinRules::removeRule(int index)
{
    Q_ASSERT(index >= 0 && index < m_rulesListModel.count());
    saveCurrentRule();

    if (m_editIndex == index) {
        m_editIndex = -1;
        pop();
    }

    // First move the deleted group to the end, so the other rules get rearranged
    const int lastIndex = m_rulesListModel.count() - 1;
    moveConfigGroup(index, lastIndex);

    rulesConfigGroup(lastIndex).deleteGroup();
    m_rulesListModel.removeAt(index);

    updateState();
}

void KCMKWinRules::moveRule(int sourceIndex, int destIndex)
{
    const int lastIndex = m_rulesListModel.count() - 1;
    if (sourceIndex == destIndex
            || (sourceIndex < 0 || sourceIndex > lastIndex)
            || (destIndex < 0 || destIndex > lastIndex)) {
        return;
    }

    saveCurrentRule();

    moveConfigGroup(sourceIndex, destIndex);
    m_rulesListModel.move(sourceIndex, destIndex);

    updateState();
}


void KCMKWinRules::moveConfigGroup(int sourceIndex, int destIndex)
{
    if (sourceIndex == destIndex) {
        return;
    }

    KConfigGroup auxGroup = KConfigGroup(m_rulesConfig, QStringLiteral("auxiliar"));
    KConfigGroup fromGroup;
    KConfigGroup toGroup = rulesConfigGroup(sourceIndex);

    // Save source into auxiliar group
    toGroup.copyTo(&auxGroup);

    // Slide all of the config groups between sourceIndex and destIndex
    const int moveDir = (sourceIndex < destIndex) ? 1 : -1;
    for (int index = sourceIndex; index != destIndex; index += moveDir)
    {
        fromGroup = rulesConfigGroup(index + moveDir);
        toGroup.deleteGroup();  // delete group before copying to avoid old properties
        fromGroup.copyTo(&toGroup);
        toGroup = fromGroup;
    }

    // Save auxliar into destination group
    toGroup.deleteGroup();
    auxGroup.copyTo(&toGroup);
    auxGroup.deleteGroup();

    // Update editIndex
    if (m_editIndex == sourceIndex) {
        m_editIndex = destIndex;
    } else if (sourceIndex < destIndex) {
        if (m_editIndex > sourceIndex && m_editIndex <= destIndex) {
            m_editIndex -= 1;
        }
    } else if (sourceIndex > destIndex) {
        if (m_editIndex < sourceIndex && m_editIndex >= destIndex) {
            m_editIndex += 1;
        }
    }
}

KConfigGroup KCMKWinRules::rulesConfigGroup(int index) const
{
    const QString groupName = QString::number(index + 1);
    if (!m_rulesConfig->hasGroup(groupName)) {
        KConfigGroup newGroup = KConfigGroup(m_rulesConfig, groupName);
        return newGroup;
    }
    return m_rulesConfig->group(groupName);
}


K_PLUGIN_CLASS_WITH_JSON(KCMKWinRules, "kcm_kwinrules_qml.json");

} // namespace

#include "kcmrules.moc"
