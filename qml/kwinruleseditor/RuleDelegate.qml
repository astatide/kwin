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
    //visible: enabled
    property bool editMode: false

    enabled: model.enabled || editMode
    height: enabled ? 2.1 * Kirigami.Units.gridUnit : 0
    opacity: (enabled) ? 1 : 0
    focus: true

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    //alternatingBackground: true

    Behavior on height {
        PropertyAnimation { duration: 2 * Kirigami.Units.longDuration }
    }
    Behavior on opacity {
        PropertyAnimation {}
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
        }

        QQC2.CheckBox {
            visible: editMode
            checked: model.enabled
            onToggled: { model.enabled = checked; }
        }

        QQC2.ToolButton {
            icon.name: model.iconName
            QQC2.ToolTip.text: model.description
            QQC2.ToolTip.visible: (down || hovered) && (model.description != "")
        }

        QQC2.Label {
            text: model.name
            font.bold: model.enabled
            Layout.fillWidth: true
        }

        QQC2.ComboBox {
            id: policyCombo
            Layout.preferredWidth: 8 * Kirigami.Units.gridUnit
            flat: true

            visible: count > 0
            enabled: valueEditor.enabled

            model: policyModel
            currentIndex: policy
            onActivated: {
                print ("Policy changed for: " + key + " policyId: " + index);
                policy = index;
            }
        }

        ValueEditor {
            id: valueEditor
            Layout.preferredWidth: 12 * Kirigami.Units.gridUnit
            Layout.alignment: Qt.AlignRight
            enabled: model.enabled //&& !editMode //&& model.policy > 0
            //visible: !editMode

            modelValue: model.value
            controlType: model.type

            onValueEdited: (value) => {
                print ("Rule changed: " + model.key + " = " + value)
                model.value = value
            }
        }

        // FIXME: to reserve space behind the scrollbar.
        // This will probably be solved in Kirigami's scrollbar
        Item {
            id: scrollBarPadder
            visible: true
            width: Kirigami.Units.gridUnit
        }

        /* Old attemps on controls for the delegate

         QQC2.ToolButton {
         icon.name: !(editMode && hovered) ? model.iconName :
                             model.enabled ? 'list-remove'
                                           : 'list-add-symbolic'
            checkable: editMode

            QQC2.ToolTip.text: model.description
            QQC2.ToolTip.visible: (down || hovered) && (model.description != "")
        }

         QQC2.ToolButton {
            id: descriptionButton
            icon.name: "documentinfo"
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            width: height

            visible: (model.description != "")
            QQC2.ToolTip.text: model.description
            QQC2.ToolTip.visible: down || hovered && (model.description != "")
        }
        */
    }
}
