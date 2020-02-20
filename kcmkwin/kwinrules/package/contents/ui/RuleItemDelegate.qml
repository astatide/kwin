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

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as QQC2
import org.kde.kirigami 2.10 as Kirigami

//Kirigami.AbstractListItem {
// !! crashes on load due to segmentation fault
// Traceback is not much helpful (Crashing... crashRecursionCounter = 2)

QQC2.ItemDelegate {
    id: ruleDelegate

    property bool showAll: false
    property bool itemIsEnabled: model.enabled

    enabled: itemIsEnabled || showAll
    height: enabled ? 2.1 * Kirigami.Units.gridUnit : 0
    opacity: (enabled) ? 1 : 0
    focus: true

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Behavior on height {
        PropertyAnimation { duration: 2 * Kirigami.Units.longDuration }
    }
    Behavior on opacity {
        PropertyAnimation { duration: 2 * Kirigami.Units.longDuration }
    }

    anchors {
        left: parent.left
        right: parent.right
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing
        anchors {
            left: parent.left
            right: parent.right
            margins: Kirigami.Units.smallSpacing
        }

        QQC2.CheckBox {
            opacity: model.selectable ? 1 : 0
            checked: model.enabled
            onToggled: { model.enabled = checked; }
        }
/*
        Kirigami.Icon {
            source: model.icon
            Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            Layout.preferredWidth: Kirigami.Units.iconSizes.medium
*/
        QQC2.ToolButton {
            icon.name: model.iconName
            QQC2.ToolTip.text: model.description
            QQC2.ToolTip.visible: (down || hovered) && (model.description != "")
        }

        QQC2.Label {
            text: model.name
            Layout.fillWidth: true
        }

        OptionsComboBox {
            id: policyCombo
            Layout.preferredWidth: 9 * Kirigami.Units.gridUnit
            flat: true

            visible: count > 0
            enabled: itemIsEnabled

            model: policyModel
            onActivated: {
                print ("Policy changed for rule " + key + ": policyIndex = " + index);
                policy = currentValue;
            }
        }

        ValueEditor {
            id: valueEditor
            Layout.preferredWidth: policyCombo.visible ? 11 * Kirigami.Units.gridUnit
                                                       : 20 * Kirigami.Units.gridUnit + Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignRight
            enabled: itemIsEnabled // && model.policy > 0 (or 1)

            ruleValue: model.value
            ruleOptions: model.options
            controlType: model.type

            onValueEdited: (value) => {
                print ("Rule changed: " + model.key + " = " + value)
                model.value = value
            }
        }
    }
}
