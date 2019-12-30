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

#include "rulesmodel.h"
#include <rules.h>

#include <QDebug>
#include <QIcon>
#include <KLocalizedString>

using namespace KWin;

RulesModel::RulesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QHash< int, QByteArray > RulesModel::roleNames() const
{
    QHash<int, QByteArray> roles({
        {KeyRole,           QByteArrayLiteral("key")},
        {NameRole,          QByteArrayLiteral("name")},
        {IconRole,          QByteArrayLiteral("icon")},
        {SectionRole,       QByteArrayLiteral("section")},
        {DescriptionRole,   QByteArrayLiteral("description")},
        {EnabledRole,       QByteArrayLiteral("enabled")},
        {ValueRole,         QByteArrayLiteral("value")},
        {TypeRole,     QByteArrayLiteral("type")},
    });
    return roles;
}

int RulesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_ruleList.size();
}

QVariant RulesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() != 0 || index.row() < 0 || index.row() >= int(m_ruleList.size())) {
        return QVariant();
    }
    const RuleItem *rule = m_ruleList.at(index.row());
    switch (role) {
    case NameRole:
        return rule->name();
    case KeyRole:
        return rule->key();
    case IconRole:
        return rule->icon();
    case DescriptionRole:
        return rule->description();
    case SectionRole:
        return rule->section();
    case PolicyRole:
        return rule->policy();
    case EnabledRole:
        return rule->isEnabled();
    case ValueRole:
        return rule->value();
    case TypeRole:
        return rule->type();
    }
    return QVariant();
}

void RulesModel::init()
{
    beginResetModel();

    m_ruleList.clear();

    // Window matching
    //TODO: Implement bool wmclasscomplete within the model
    m_ruleList.append(new RuleItem("wmclass", RulePolicy::StringMatch, RuleType::String,
                                   i18n("Window class (application)"), i18n("Window matching"),
                                   QStringLiteral("window")));
    m_ruleList.at(0)->setEnabled(true);
    m_ruleList.at(0)->setPolicyValue(Rules::ExactMatch);
    m_ruleList.append(new RuleItem("windowrole", RulePolicy::StringMatch, RuleType::String,
                                   i18n("Window role"), i18n("Window matching"),
                                   QStringLiteral("dialog-object-properties")));

    m_ruleList.append(new RuleItem("types", RulePolicy::NoPolicy, RuleType::Flags,
                                   i18n("Window types"), i18n("Window matching"),
                                   QStringLiteral("window-duplicate")));
    m_ruleList.append(new RuleItem("title", RulePolicy::StringMatch, RuleType::String,
                                   i18n("Window title"), i18n("Window matching"),
                                   QStringLiteral("edit-comment")));
    m_ruleList.append(new RuleItem("clientmachine", RulePolicy::StringMatch, RuleType::String,
                                   i18n("Machine (hostname)"), i18n("Window matching"),
                                   QStringLiteral("computer")));

    // Size & Position
    m_ruleList.append(new RuleItem("position", RulePolicy::SetRule, RuleType::String,
                                   i18n("Position"), i18n("Size & Position"),
                                   QStringLiteral("transform-move")));
    m_ruleList.append(new RuleItem("size", RulePolicy::SetRule, RuleType::String,
                                   i18n("Size"), i18n("Size & Position"),
                                   QStringLiteral("image-resize-symbolic")));

    m_ruleList.append(new RuleItem("maximizehoriz", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Maximized horizontally"), i18n("Size & Position"),
                                   QStringLiteral("resizecol")));
    m_ruleList.append(new RuleItem("maximizevert", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Maximized vertically"), i18n("Size & Position"),
                                   QStringLiteral("resizerow")));

    m_ruleList.append(new RuleItem("desktop", RulePolicy::SetRule, RuleType::Option,
                                   i18n("Virtual Desktop"), i18n("Size & Position"),
                                   QStringLiteral("virtual-desktops")));
    m_ruleList.append(new RuleItem("activity", RulePolicy::SetRule, RuleType::Option,
                                   i18n("Activity"), i18n("Size & Position"),
                                   QStringLiteral("activities")));
    m_ruleList.append(new RuleItem("screen", RulePolicy::SetRule, RuleType::Integer,
                                   i18n("Screen"), i18n("Size & Position"),
                                   QStringLiteral("osd-shutd-screen")));

    m_ruleList.append(new RuleItem("fullscreen", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Fullscreen"), i18n("Size & Position"),
                                   QStringLiteral("view-fullscreen")));
    m_ruleList.append(new RuleItem("minimize", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Minimized"), i18n("Size & Position"),
                                   QStringLiteral("window-minimize")));
    m_ruleList.append(new RuleItem("shade", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Shaded"), i18n("Size & Position"),
                                   QStringLiteral("window-shade")));

    m_ruleList.append(new RuleItem("placement", RulePolicy::ForceRule, RuleType::Option,
                                   i18n("Initial placement"), i18n("Size & Position"),
                                   QStringLiteral("preferences-system-windows-effect-presentwindows")));
    m_ruleList.append(new RuleItem("ignoregeometry", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Ignore requested geometry"), i18n("Size & Position"),
                                   QStringLiteral("view-time-schedule-baselined-remove"),
                                   i18n("Windows can ask to appear in a certain position.\n"
                                        "By default this overrides the placement strategy\n"
                                        "what might be nasty if the client abuses the feature\n"
                                        "to unconditionally popup in the middle of your screen.")));

    m_ruleList.append(new RuleItem("minsize", RulePolicy::ForceRule, RuleType::String,
                                   i18n("Minimum Size"), i18n("Size & Position"),
                                   QStringLiteral("image-resize-symbolic")));
    m_ruleList.append(new RuleItem("maxsize", RulePolicy::ForceRule, RuleType::String,
                                   i18n("Maximum Size"), i18n("Size & Position"),
                                   QStringLiteral("image-resize-symbolic")));
    m_ruleList.append(new RuleItem("strictgeometry", RulePolicy::ForceRule, RuleType::Boolean,
                                   i18n("Obey geometry restrictions"), i18n("Size & Position"),
                                   QStringLiteral("transform-crop-and-resize"),
                                   i18n("Eg. terminals or video players can ask to keep a certain aspect ratio\n"
                                        "or only grow by values larger than one\n"
                                        "(eg. by the dimensions of one character).\n"
                                        "This may be pointless and the restriction prevents arbitrary dimensions\n"
                                        "like your complete screen area.")));

    // Arrangement & Access
    m_ruleList.append(new RuleItem("above", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Keep above"), i18n("Arrangement & Access"),
                                   QStringLiteral("window-keep-above")));
    m_ruleList.append(new RuleItem("below", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Keep below"), i18n("Arrangement & Access"),
                                   QStringLiteral("window-keep-below")));

    m_ruleList.append(new RuleItem("skiptaskbar", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Skip taskbar"), i18n("Arrangement & Access"),
                                   QStringLiteral("kt-show-statusbar"),
                                   i18n("Window shall (not) appear in the taskbar.")));
    m_ruleList.append(new RuleItem("skippager", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Skip pager"), i18n("Arrangement & Access"),
                                   QStringLiteral("org.kde.plasma.pager"),
                                   i18n("Window shall (not) appear in the manager for virtual desktops")));
    m_ruleList.append(new RuleItem("skipswitcher", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("Skip switcher"), i18n("Arrangement & Access"),
                                   QStringLiteral("preferences-system-windows-effect-flipswitch"),
                                   i18n("Window shall (not) appear in the Alt+Tab list")));

    m_ruleList.append(new RuleItem("shortcut", RulePolicy::SetRule, RuleType::Shortcut,
                                   i18n("Shortcut"), i18n("Arrangement & Access"),
                                   QStringLiteral("configure-shortcuts")));

    //Appearance & Fixes
    m_ruleList.append(new RuleItem("noborder", RulePolicy::SetRule, RuleType::Boolean,
                                   i18n("No titlebar and frame"), i18n("Appearance & Fixes"),
                                   QStringLiteral("dialog-cancel")));
    m_ruleList.append(new RuleItem("decocolor", RulePolicy::ForceRule, RuleType::Option,
                                   i18n("Titlebar color scheme"), i18n("Appearance & Fixes"),
                                   QStringLiteral("preferences-desktop-theme")));

    m_ruleList.append(new RuleItem("opacityactive", RulePolicy::ForceRule, RuleType::Percentage,
                                   i18n("Active opacity"), i18n("Appearance & Fixes"),
                                   QStringLiteral("edit-opacity")));
    m_ruleList.append(new RuleItem("opacityinactive", RulePolicy::ForceRule, RuleType::Percentage,
                                   i18n("Inactive opacity"), i18n("Appearance & Fixes"),
                                   QStringLiteral("edit-opacity")));

    m_ruleList.append(new RuleItem("fsplevel", RulePolicy::ForceRule, RuleType::Option,
                                   i18n("Focus stealing prevention"), i18n("Appearance & Fixes"),
                                   QStringLiteral("preferences-system-windows-effect-glide"),
                                   i18n("KWin tries to prevent windows from taking the focus\n"
                                        "(\"activate\") while you're working in another window,\n"
                                        "but this may sometimes fail or superact.\n"
                                        "\"None\" will unconditionally allow this window to get the focus while\n"
                                        "\"Extreme\" will completely prevent it from taking the focus.")));
    m_ruleList.append(new RuleItem("fpplevel", RulePolicy::ForceRule, RuleType::Option,
                                   i18n("Focus protection"), i18n("Appearance & Fixes"),
                                   QStringLiteral("preferences-system-windows-effect-minimize"),
                                   i18n("This controls the focus protection of the currently active window.\n"
                                        "None will always give the focus away,\n"
                                        "Extreme will keep it.\n"
                                        "Otherwise it's interleaved with the stealing prevention\n"
                                        "assigned to the window that wants the focus.")));
    m_ruleList.append(new RuleItem("acceptfocus", RulePolicy::ForceRule, RuleType::Boolean,
                                   i18n("Accept focus"), i18n("Appearance & Fixes"),
                                   QStringLiteral("preferences-desktop-cursors"),
                                   i18n("Windows may prevent to get the focus (activate) when being clicked.\n"
                                        "On the other hand you might wish to prevent a window\n"
                                        "from getting focused on a mouse click.")));
    m_ruleList.append(new RuleItem("disableglobalshortcuts", RulePolicy::ForceRule, RuleType::Boolean,
                                   i18n("Ignore global shortcuts"), i18n("Appearance & Fixes"),
                                   QStringLiteral("input-keyboard-virtual-off"),
                                   i18n("When used, a window will receive\n"
                                        "all keyboard inputs while it is active, including Alt+Tab etc.\n"
                                        "This is especially interesting for emulators or virtual machines.\n"
                                        "\n"
                                        "Be warned:\n"
                                        "you won't be able to Alt+Tab out of the window\n"
                                        "nor use any other global shortcut (such as Alt+F2 to show KRunner)\n"
                                        "while it's active!")));

    m_ruleList.append(new RuleItem("closeable", RulePolicy::ForceRule, RuleType::Boolean,
                                   i18n("Closeable"), i18n("Appearance & Fixes"),
                                   QStringLiteral("dialog-close")));
    m_ruleList.append(new RuleItem("type", RulePolicy::ForceRule, RuleType::Option,
                                   i18n("Set window type"), i18n("Appearance & Fixes"),
                                   QStringLiteral("window-duplicate")));
    m_ruleList.append(new RuleItem("desktopfile", RulePolicy::SetRule, RuleType::String,
                                   i18n("Desktop file name"), i18n("Appearance & Fixes"),
                                   QStringLiteral("application-x-desktop")));

    m_ruleList.append(new RuleItem("blockcompositing", RulePolicy::ForceRule, RuleType::Boolean,
                                   i18n("Block compositing"), i18n("Appearance & Fixes"),
                                   QStringLiteral("composite-track-on")));

    endResetModel();
}

const QString RulesModel::ruleName() {
    return m_ruleName;
}

void RulesModel::setRuleName(const QString& ruleName) {
    m_ruleName = ruleName;
}

bool RulesModel::isWarningShown() {
    const RuleItem *wmclass_rule = m_ruleList.at(0);

    //TODO: add condition that no window types are selected
    // const RuleItem *wmtypes_rule = m_ruleList.at(2);
    //  && wmtypes_rule->value() ~== NET.Types::None
    return (wmclass_rule->policyValue() == Rules::UnimportantMatch);
}


int RulesModel::indexForKey(const QString& key) const
{
    for (int i = 0; i < m_ruleList.count(); i++)
    {
        const RuleItem *rule = m_ruleList.at(i);
        if (rule->key() == key) {
            return i;
        }
    }
    return -1;
}
