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
#include <QTemporaryFile>

#include <KLocalizedString>

using namespace KWin;

RulesModel::RulesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    initRuleList();
    initPropertyMap();
}

QHash< int, QByteArray > RulesModel::roleNames() const
{
    QHash<int, QByteArray> roles({
        {KeyRole,           QByteArrayLiteral("key")},
        {NameRole,          QByteArrayLiteral("name")},
        {IconRole,          QByteArrayLiteral("icon")},
        {IconNameRole,      QByteArrayLiteral("iconName")},
        {SectionRole,       QByteArrayLiteral("section")},
        {DescriptionRole,   QByteArrayLiteral("description")},
        {EnabledRole,       QByteArrayLiteral("enabled")},
        {SelectableRole,    QByteArrayLiteral("selectable")},
        {ValueRole,         QByteArrayLiteral("value")},
        {TypeRole,          QByteArrayLiteral("type")},
        {PolicyIdRole,      QByteArrayLiteral("policy")},
        {PolicyModelRole,   QByteArrayLiteral("policyModel")},
        {OptionsModelRole,  QByteArrayLiteral("options")},
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
    case KeyRole:
        return rule->key();
    case NameRole:
        return rule->name();
    case IconRole:
        return rule->icon();
    case IconNameRole:
        return rule->iconName();
    case DescriptionRole:
        return rule->description();
    case SectionRole:
        return rule->section();
    case EnabledRole:
        return rule->isEnabled();
    case SelectableRole:
        return !rule->hasFlag(RuleItem::AlwaysEnabled);
    case ValueRole:
        return rule->value();
    case TypeRole:
        return rule->type();
    case PolicyIdRole:
        return rule->policyIndex();
    case PolicyModelRole:
        return rule->policyModel();
    }
    return QVariant();
}

bool RulesModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid() || index.column() != 0 || index.row() < 0 || index.row() >= int(m_ruleList.size())) {
        return false;
    }

    RuleItem *rule = m_ruleList.at(index.row());

    switch (role) {
    case EnabledRole:
        rule->setEnabled(value.toBool());
        break;
    case ValueRole:
        rule->setValue(value);
        break;
    case PolicyIdRole:
        rule->setPolicyIndex(value.toInt());
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, QVector<int>{role});

    if (rule->hasFlag(RuleItem::AffectsWarning)) {
        emit showWarningChanged();
    }
    if (rule->hasFlag(RuleItem::AffectsDescription)) {
        emit defaultDescriptionChanged();
    }

    return true;
}

RuleItem *RulesModel::addRule(RuleItem *rule)
{
    m_ruleList << rule;
    m_rules.insert(rule->key(), rule);

    return rule;
}

RuleItem *RulesModel::operator[](const QString& key) const
{
    return m_rules[key];
}

void RulesModel::init()
{
    setDescription(QString());

    beginResetModel();
    for (RuleItem *rule : qAsConst(m_ruleList)) {
        rule->reset();
    }
    endResetModel();
}

void RulesModel::initRuleList()
{
    m_ruleList.clear();

    // Window matching
    addRule(new RuleItem(QLatin1String("wmclass"),
                         RulePolicy::StringMatch, RuleType::String,
                         i18n("Window class (application)"), i18n("Window matching"),
                         QStringLiteral("window")));
    m_rules["wmclass"]->setFlags(RuleItem::AlwaysEnabled | RuleItem::AffectsWarning | RuleItem::AffectsDescription);

    addRule(new RuleItem(QLatin1String("wmclasscomplete"),
                         RulePolicy::NoPolicy, RuleType::Boolean,
                         i18n("Match whole window class"), i18n("Window matching"),
                         QStringLiteral("window")));
    m_rules["wmclasscomplete"]->setFlags(RuleItem::AlwaysEnabled);

    addRule(new RuleItem(QLatin1String("windowrole"),
                         RulePolicy::NoPolicy, RuleType::String,
                         i18n("Window role"), i18n("Window matching"),
                         QStringLiteral("dialog-object-properties")));

    addRule(new RuleItem(QLatin1String("types"),
                         RulePolicy::NoPolicy, RuleType::FlagsOption,
                         i18n("Window types"), i18n("Window matching"),
                         QStringLiteral("window-duplicate")));
    m_rules["types"]->setFlags(RuleItem::StartEnabled | RuleItem::AffectsWarning );

    addRule(new RuleItem(QLatin1String("title"),
                         RulePolicy::StringMatch, RuleType::String,
                         i18n("Window title"), i18n("Window matching"),
                         QStringLiteral("edit-comment")));
    m_rules["title"]->setFlags(RuleItem::AffectsDescription);

    addRule(new RuleItem(QLatin1String("clientmachine"), RulePolicy::StringMatch, RuleType::String,
                         i18n("Machine (hostname)"), i18n("Window matching"),
                         QStringLiteral("computer")));

    // Size & Position
    addRule(new RuleItem(QLatin1String("position"),
                         RulePolicy::SetRule, RuleType::Coordinate,
                         i18n("Position"), i18n("Size & Position"),
                         QStringLiteral("transform-move")));

    addRule(new RuleItem(QLatin1String("size"),
                         RulePolicy::SetRule, RuleType::Coordinate,
                         i18n("Size"), i18n("Size & Position"),
                         QStringLiteral("image-resize-symbolic")));

    addRule(new RuleItem(QLatin1String("maximizehoriz"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Maximized horizontally"), i18n("Size & Position"),
                         QStringLiteral("resizecol")));

    addRule(new RuleItem(QLatin1String("maximizevert"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Maximized vertically"), i18n("Size & Position"),
                         QStringLiteral("resizerow")));

    addRule(new RuleItem(QLatin1String("desktop"),
                         RulePolicy::SetRule, RuleType::Option,
                         i18n("Virtual Desktop"), i18n("Size & Position"),
                         QStringLiteral("virtual-desktops")));
#ifdef KWIN_BUILD_ACTIVITIES
    addRule(new RuleItem(QLatin1String("activity"),
                         RulePolicy::SetRule, RuleType::Option,
                         i18n("Activity"), i18n("Size & Position"),
                         QStringLiteral("activities")));
#endif
    addRule(new RuleItem(QLatin1String("screen"),
                         RulePolicy::SetRule, RuleType::Integer,
                         i18n("Screen"), i18n("Size & Position"),
                         QStringLiteral("osd-shutd-screen")));

    addRule(new RuleItem(QLatin1String("fullscreen"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Fullscreen"), i18n("Size & Position"),
                         QStringLiteral("view-fullscreen")));

    addRule(new RuleItem(QLatin1String("minimize"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Minimized"), i18n("Size & Position"),
                         QStringLiteral("window-minimize")));

    addRule(new RuleItem(QLatin1String("shade"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Shaded"), i18n("Size & Position"),
                         QStringLiteral("window-shade")));

    addRule(new RuleItem(QLatin1String("placement"),
                         RulePolicy::ForceRule, RuleType::Option,
                         i18n("Initial placement"), i18n("Size & Position"),
                         QStringLiteral("preferences-system-windows-effect-presentwindows")));

    addRule(new RuleItem(QLatin1String("ignoregeometry"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Ignore requested geometry"), i18n("Size & Position"),
                         QStringLiteral("view-time-schedule-baselined-remove"),
                         i18n("Windows can ask to appear in a certain position.\n"
                              "By default this overrides the placement strategy\n"
                              "what might be nasty if the client abuses the feature\n"
                              "to unconditionally popup in the middle of your screen.")));

    addRule(new RuleItem(QLatin1String("minsize"),
                         RulePolicy::ForceRule, RuleType::Coordinate,
                         i18n("Minimum Size"), i18n("Size & Position"),
                         QStringLiteral("image-resize-symbolic")));

    addRule(new RuleItem(QLatin1String("maxsize"),
                         RulePolicy::ForceRule, RuleType::Coordinate,
                         i18n("Maximum Size"), i18n("Size & Position"),
                         QStringLiteral("image-resize-symbolic")));

    addRule(new RuleItem(QLatin1String("strictgeometry"),
                         RulePolicy::ForceRule, RuleType::Boolean,
                         i18n("Obey geometry restrictions"), i18n("Size & Position"),
                         QStringLiteral("transform-crop-and-resize"),
                         i18n("Eg. terminals or video players can ask to keep a certain aspect ratio\n"
                              "or only grow by values larger than one\n"
                              "(eg. by the dimensions of one character).\n"
                              "This may be pointless and the restriction prevents arbitrary dimensions\n"
                              "like your complete screen area.")));

    // Arrangement & Access
    addRule(new RuleItem(QLatin1String("above"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Keep above"), i18n("Arrangement & Access"),
                         QStringLiteral("window-keep-above")));

    addRule(new RuleItem(QLatin1String("below"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Keep below"), i18n("Arrangement & Access"),
                         QStringLiteral("window-keep-below")));

    addRule(new RuleItem(QLatin1String("skiptaskbar"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Skip taskbar"), i18n("Arrangement & Access"),
                         QStringLiteral("kt-show-statusbar"),
                         i18n("Window shall (not) appear in the taskbar.")));

    addRule(new RuleItem(QLatin1String("skippager"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Skip pager"), i18n("Arrangement & Access"),
                         QStringLiteral("org.kde.plasma.pager"),
                         i18n("Window shall (not) appear in the manager for virtual desktops")));

    addRule(new RuleItem(QLatin1String("skipswitcher"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("Skip switcher"), i18n("Arrangement & Access"),
                         QStringLiteral("preferences-system-windows-effect-flipswitch"),
                         i18n("Window shall (not) appear in the Alt+Tab list")));

    addRule(new RuleItem(QLatin1String("shortcut"),
                         RulePolicy::SetRule, RuleType::Shortcut,
                         i18n("Shortcut"), i18n("Arrangement & Access"),
                         QStringLiteral("configure-shortcuts")));

    // Appearance & Fixes
    addRule(new RuleItem(QLatin1String("noborder"),
                         RulePolicy::SetRule, RuleType::Boolean,
                         i18n("No titlebar and frame"), i18n("Appearance & Fixes"),
                         QStringLiteral("dialog-cancel")));

    addRule(new RuleItem(QLatin1String("decocolor"),
                         RulePolicy::ForceRule, RuleType::Option,
                         i18n("Titlebar color scheme"), i18n("Appearance & Fixes"),
                         QStringLiteral("preferences-desktop-theme")));

    addRule(new RuleItem(QLatin1String("opacityactive"),
                         RulePolicy::ForceRule, RuleType::Percentage,
                         i18n("Active opacity"), i18n("Appearance & Fixes"),
                         QStringLiteral("edit-opacity")));

    addRule(new RuleItem(QLatin1String("opacityinactive"),
                         RulePolicy::ForceRule, RuleType::Percentage,
                         i18n("Inactive opacity"), i18n("Appearance & Fixes"),
                         QStringLiteral("edit-opacity")));

    addRule(new RuleItem(QLatin1String("fsplevel"),
                         RulePolicy::ForceRule, RuleType::Option,
                         i18n("Focus stealing prevention"), i18n("Appearance & Fixes"),
                         QStringLiteral("preferences-system-windows-effect-glide"),
                         i18n("KWin tries to prevent windows from taking the focus\n"
                              "(\"activate\") while you're working in another window,\n"
                              "but this may sometimes fail or superact.\n"
                              "\"None\" will unconditionally allow this window to get the focus while\n"
                              "\"Extreme\" will completely prevent it from taking the focus.")));

    addRule(new RuleItem(QLatin1String("fpplevel"),
                         RulePolicy::ForceRule, RuleType::Option,
                         i18n("Focus protection"), i18n("Appearance & Fixes"),
                         QStringLiteral("preferences-system-windows-effect-minimize"),
                         i18n("This controls the focus protection of the currently active window.\n"
                              "None will always give the focus away,\n"
                              "Extreme will keep it.\n"
                              "Otherwise it's interleaved with the stealing prevention\n"
                              "assigned to the window that wants the focus.")));

    addRule(new RuleItem(QLatin1String("acceptfocus"),
                         RulePolicy::ForceRule, RuleType::Boolean,
                         i18n("Accept focus"), i18n("Appearance & Fixes"),
                         QStringLiteral("preferences-desktop-cursors"),
                         i18n("Windows may prevent to get the focus (activate) when being clicked.\n"
                              "On the other hand you might wish to prevent a window\n"
                              "from getting focused on a mouse click.")));

    addRule(new RuleItem(QLatin1String("disableglobalshortcuts"),
                         RulePolicy::ForceRule, RuleType::Boolean,
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

    addRule(new RuleItem(QLatin1String("closeable"),
                         RulePolicy::ForceRule, RuleType::Boolean,
                         i18n("Closeable"), i18n("Appearance & Fixes"),
                         QStringLiteral("dialog-close")));

    addRule(new RuleItem(QLatin1String("type"),
                         RulePolicy::ForceRule, RuleType::Option,
                         i18n("Set window type"), i18n("Appearance & Fixes"),
                         QStringLiteral("window-duplicate")));

    addRule(new RuleItem(QLatin1String("desktopfile"),
                         RulePolicy::SetRule, RuleType::String,
                         i18n("Desktop file name"), i18n("Appearance & Fixes"),
                         QStringLiteral("application-x-desktop")));

    addRule(new RuleItem(QLatin1String("blockcompositing"),
                         RulePolicy::ForceRule, RuleType::Boolean,
                         i18n("Block compositing"), i18n("Appearance & Fixes"),
                         QStringLiteral("composite-track-on")));

    init();
}

void RulesModel::initPropertyMap()
{
    m_ruleForProperty.clear();

    // Map window properties to rule keys
    m_ruleForProperty.insert(QStringLiteral("x11DesktopNumber"), QStringLiteral("desktop"));
    m_ruleForProperty.insert(QStringLiteral("maximizeHorizontal"), QStringLiteral("maximizehoriz"));
    m_ruleForProperty.insert(QStringLiteral("maximizeVertical"), QStringLiteral("maximizevert"));
    m_ruleForProperty.insert(QStringLiteral("minimized"), QStringLiteral("minimize"));
    m_ruleForProperty.insert(QStringLiteral("shaded"), QStringLiteral("shade"));
    m_ruleForProperty.insert(QStringLiteral("fullscreen"), QStringLiteral("fullscreen"));
    m_ruleForProperty.insert(QStringLiteral("keepAbove"), QStringLiteral("above"));
    m_ruleForProperty.insert(QStringLiteral("keepBelow"), QStringLiteral("below"));
    m_ruleForProperty.insert(QStringLiteral("noBorder"), QStringLiteral("noborder"));
    m_ruleForProperty.insert(QStringLiteral("skipTaskbar"), QStringLiteral("skiptaskbar"));
    m_ruleForProperty.insert(QStringLiteral("skipPager"), QStringLiteral("skippager"));
    m_ruleForProperty.insert(QStringLiteral("skipSwitcher"), QStringLiteral("skipswitcher"));
    m_ruleForProperty.insert(QStringLiteral("type"), QStringLiteral("type"));
    m_ruleForProperty.insert(QStringLiteral("desktopFile"), QStringLiteral("desktopfile"));
}


const QString RulesModel::description() const
{
    return m_description;
}

void RulesModel::setDescription(const QString& name)
{
    if (m_description != name) {
        m_description = name;
        emit descriptionChanged();
    }
}

const QString RulesModel::defaultDescription() const
{
    const QString wmclass = m_rules["wmclass"]->value().toString();
    const QString title = m_rules["title"]->isEnabled() ? m_rules["title"]->value().toString() : QString();

    if (!title.isEmpty()) {
        return i18n("Window settings for %1", title);
    }
    if (!wmclass.isEmpty()) {
        return i18n("Settings for %1", wmclass);
    }

    return i18n("New specific window settings");
}

bool RulesModel::isWarningShown()
{
    const bool no_wmclass = !m_rules["wmclass"]->isEnabled()
                                || m_rules["wmclass"]->policy() == Rules::UnimportantMatch;
    const bool alltypes = !m_rules["types"]->isEnabled()
                              || m_rules["types"]->value() == 0
                              || m_rules["types"]->value() == 0x3FF;

    return (no_wmclass && alltypes);
}

void RulesModel::readFromConfig(KConfigGroup *config)
{
    const QString desc = config->readEntry(QLatin1String("Description"));
    setDescription(desc);

    beginResetModel();
    for (RuleItem *rule : qAsConst(m_ruleList)) {
        if (!config->hasKey(rule->key())) {
            rule->reset();
            continue;
        }

        rule->setEnabled(true);

        const QVariant value = config->readEntry(rule->key());
        rule->setValue(value);

        if (rule->policyType() != RulePolicy::NoPolicy) {
            const int policy = config->readEntry(rule->policyKey(), int());
            rule->setPolicy(policy);
        }
    }
    endResetModel();

    emit descriptionChanged();
    emit defaultDescriptionChanged();
    emit showWarningChanged();
}

void RulesModel::writeToConfig(KConfigGroup *config) const
{
    if (!m_description.isEmpty()) {
        config->writeEntry(QLatin1String("Description"), m_description);
    } else {
        config->writeEntry(QLatin1String("Description"), defaultDescription());
    }

    for (const RuleItem *rule : qAsConst(m_ruleList)) {
        const bool ruleHasPolicy = rule->policyType() != RulePolicy::NoPolicy;

        //TODO: Add condition `&& rule->policy() > 0` to match the classic RuleWidget behavior
        //      after implementing policy management in the UI
        if (rule->isEnabled()) {
            config->writeEntry(rule->key(), rule->value(), KConfig::Persistent);
            if (ruleHasPolicy) {
                config->writeEntry(rule->policyKey(), rule->policy(), KConfig::Persistent);
            }
        } else {
            config->deleteEntry(rule->key(), KConfig::Persistent);
            if (ruleHasPolicy) {
                config->deleteEntry(rule->policyKey(), KConfig::Persistent);
            }
        }
    }
}

void RulesModel::prefillProperties(const QVariantMap &info)
{
    beginResetModel();

    const QString position = QStringLiteral("%1,%2").arg(info.value("x").toInt())
                                                    .arg(info.value("y").toInt());
    const QString size = QStringLiteral("%1,%2").arg(info.value("width").toInt())
                                                .arg(info.value("height").toInt());

    if (!m_rules["position"]->isEnabled()) {
        m_rules["position"]->setValue(position);
    }
    if (!m_rules["size"]->isEnabled()) {
        m_rules["size"]->setValue(size);
    }
    if (!m_rules["minsize"]->isEnabled()) {
        m_rules["minsize"]->setValue(size);
    }
    if (!m_rules["maxsize"]->isEnabled()) {
        m_rules["maxsize"]->setValue(size);
    }

    for (QString &property : info.keys()) {
        const QString ruleKey = m_ruleForProperty.value(property, QString());
        if (ruleKey.isEmpty() || m_rules[ruleKey]->isEnabled()) {
            continue;
        }

        const QVariant value = info.value(property);
        m_rules[ruleKey]->setValue(value);
    }

    endResetModel();

    emit showWarningChanged();
}

void RulesModel::importFromRules(Rules* rules)
{
    if (rules == nullptr) {
        init();
        return;
    }

    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        return;
    }
    const QString tempPath = tempFile.fileName();
    KConfig config(tempPath, KConfig::SimpleConfig);
    KConfigGroup cfg(&config, QLatin1String("KWinRule_temp"));

    rules->write(cfg);
    readFromConfig(&cfg);
}

Rules *RulesModel::exportToRules() const
{
    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        return nullptr;
    }
    const QString tempPath = tempFile.fileName();
    KConfig config(tempPath, KConfig::SimpleConfig);
    KConfigGroup cfg(&config, QLatin1String("KWinRule_temp"));

    writeToConfig(&cfg);

    Rules *rules = new Rules(cfg);
    return rules;
}
