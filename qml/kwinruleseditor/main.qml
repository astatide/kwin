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

ColumnLayout {

    RowLayout {
        Kirigami.Heading {
            text: i18n("Rule description:")
            level: 3
        }
        QQC2.TextField {
            property bool isTextEdited: false
            text: rules.description
            placeholderText: rules.defaultDescription
            onTextEdited: { isTextEdited = true; }
            onEditingFinished: {
                if (isTextEdited) {
                    print ("Rule Name changed to: " + text)
                    rules.description = text;
                }
                isTextEdited = false;
            }
            Layout.fillWidth: true
        }
    }

    QQC2.Frame {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: 0

        ListView {
            id: enabledRulesView

            anchors.fill: parent
            anchors.margins: 0
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            focus: true

            QQC2.ScrollBar.vertical: QQC2.ScrollBar { z: 100 }

            model: rules
            delegate: ruleDelegate

            section.delegate: Kirigami.ListSectionHeader { label: section }
            section.property: "section"
/*
            header : {
                anchors.left: parent.left
                anchors.right: parent.right
                z: 200
            }
            headerPositioning: ListView.OverlayFooter
*/
        }

    }

    RowLayout {
        id: viewActionBar
            QQC2.TextField {
                id: searchBar
                Layout.fillWidth: true
                placeholderText: i18n("Filter...")
            }
            QQC2.ToolButton {
                id: editMode
                icon.name: 'list-add'
                checkable: true
                text: i18n("Add/Remove Rules")
            }
    }

    // FIXME: InlineMessage.qml:241:13: QML Label: Binding loop detected for property "verticalAlignment"
    Kirigami.InlineMessage {
        id: warningMessage
        text: i18n("You have specified the window class as unimportant.\n" +
                   "This means the settings will possibly apply to windows from all " +
                   "applications. If you really want to create a generic setting, it is " +
                   "recommended you at least limit the window types to avoid special window " +
                   "types.")
        visible: rules.showWarning
        Layout.fillWidth: true
    }

    Component {
        id: ruleDelegate

        //Kirigami.AbstractListItem {
        // !! crashes on load due to segmentation fault
        // Traceback is not much helpful (Crashing... crashRecursionCounter = 2)
        QQC2.ItemDelegate {
            id: ruleDelegateItem

            enabled: model.enabled || editMode.checked
            //visible: model.enabled || editMode.checked
            height: enabled ? 2.1 * Kirigami.Units.gridUnit : 0
            opacity: (enabled) ? 1 : 0
            focus: true

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
            /*
            background: Rectangle {
                anchors.fill: parent
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                //TODO: Use system colors
                // selected(focus) / hover / enabled + alternate/ disabled
                //color: model.enabled ? Kirigami.Theme.viewBackgroundColor : Kirigami.Theme.backgroundColor
            }*/

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing
                anchors {
                    left: parent.left
                    right: parent.right
                }

                QQC2.ToolButton {
                    icon.name: {
                        if (editMode.checked && hovered) {
                            return model.enabled ? 'list-remove' : 'list-add-symbolic'
                        }
                        return model.iconName;
                    }
                    checkable: editMode.checked
                    checked: editMode.checked && model.enabled
                    onToggled: { model.enabled = checked; }

                    QQC2.ToolTip.text: model.description
                    QQC2.ToolTip.visible: down || hovered && (model.description != "")
                }

                QQC2.Label {
                    text: model.name
                    font.bold: model.enabled
                    Layout.fillWidth: true
                }

                Loader {
                    id: valueEditor
                    focus: true
                    width: 12 * Kirigami.Units.gridUnit;
                    enabled: model.enabled && !editMode.checked //&& model.policy > 0
                    visible: !editMode.checked

                    property var modelValue: model.value
                    signal valueChanged(var value)

                    sourceComponent: {
                        switch (model.type) {
                            case 1: return booleanEditor          //RuleType.Boolean
                            case 2: return stringEditor           //RuleType.String
                            case 3: return integerEditor          //RuleType.Integer
                            case 4: return optionEditor           //RuleType.Option
                            case 5: return flagsEditor            //RuleType.Flags
                            case 6: return percentageEditor       //RuleType.Percentage
                            case 7: return coordinateEditor       //RuleType.Coordinate
                            case 8: return shortcutEditor         //RuleType.Shortcut
                            default: return notImplementedEditor
                        }
                    }

                    onLoaded: {
                        item.width = valueEditor.width;
                        item.anchors.right = valueEditor.right;
                        item.anchors.margins = 0;
                        item.height = 1.6 * Kirigami.Units.gridUnit;
                        item.Layout.alignment = Qt.AlignVCenter + Qt.AlignRight;
                        item.focus = true;
                    }

                    onValueChanged: {
                        print ("Rule changed: " + model.key + " = " + value)
                        model.value = value
                    }

                    onFocusChanged: { valueEditor.focus = focus; }

                    Component {
                        id: notImplementedEditor
                        QQC2.Label {
                            text: "<i>To be implemented</i>"
                        }
                    }

                    Component {
                        id: booleanEditor
                        QQC2.Switch {
                            text: checked ? i18n("Yes") : i18n("No")
                            checked: modelValue
                            onToggled: valueEditor.valueChanged(checked)
                        }
                    }

                    Component {
                        id: stringEditor
                        QQC2.TextField {
                            property bool isTextEdited: false
                            text: modelValue
                            onTextEdited: { isTextEdited = true; }
                            onEditingFinished: {
                                if (isTextEdited) { valueEditor.valueChanged(text); }
                                isTextEdited = false;
                            }
                        }
                    }

                    Component {
                        id: integerEditor
                        QQC2.SpinBox {
                            editable: true
                            value: modelValue
                            onValueModified: valueEditor.valueChanged(value)
                        }
                    }

                    Component {
                        id: optionEditor
                        QQC2.ComboBox {
                            flat: true
                            onCurrentTextChanged: valueEditor.valueChanged(currentText)
                        }
                    }

                    Component {
                        id: flagsEditor
                        RowLayout {
                            spacing: 0
                            Repeater {
                                model: 10
                                QQC2.ToolButton {
                                    property int bit: index
                                    icon.name: "window-duplicate"
                                    checkable: true
                                    checked: ((modelValue & (1 << bit)) >> bit) == 1
                                    onToggled: {
                                        valueEditor.valueChanged((modelValue & ~(1 << bit)) | (checked << bit));
                                    }
                                }
                            }
                        }
                    }

                    Component {
                        id: percentageEditor
                        RowLayout {
                            QQC2.Slider {
                                id: slider
                                Layout.fillWidth: true
                                from: 0
                                to: 100
                                value: modelValue
                                onMoved: valueEditor.valueChanged(Math.round(slider.value))
                            }
                            QQC2.Label {
                                text: i18n("%1 %", Math.round(slider.value))
                                width: 2 * Kirigami.Units.gridUnit
                            }
                        }
                    }

                    Component {
                        id: coordinateEditor
                        RowLayout {
                            QQC2.SpinBox {
                                id: coord_x
                                editable: true
                                Layout.fillWidth: true
                                from: 0
                                to: 4098
                                value: modelValue ? modelValue.split(",")[0] : 0
                                onValueModified: valueEditor.valueChanged(coord_x.value + "," + coord_y.value)
                            }
                            QQC2.Label {
                                text: i18nc("(x, y) coordinates separator in size/position"," x ")
                            }
                            QQC2.SpinBox {
                                id: coord_y
                                editable: true
                                from: 0
                                to: 4098
                                Layout.fillWidth: true
                                value: modelValue ? modelValue.split(",")[1] : 0
                                onValueModified: valueEditor.valueChanged(coord_x.value + "," + coord_y.value)
                            }
                        }
                    }

                    Component {
                        id: shortcutEditor
                        QQC2.Button {
                            icon.name: "key-enter"
                            text: modelValue ? modelValue : i18n("Set shortcut ...")
                        }
                    }
                }
                /* Old attemps on controls for the delegate
                 *
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

                QQC2.CheckBox {
                    id: toggleItemEnabled
                    visible: editMode.checked
                    checked: model.enabled
                    onToggled: { model.enabled = checked; }
                }

                QQC2.ToolButton {
                    icon.name: model.enabled? 'list-remove' : 'list-add-symbolic'
                    icon.color: model.enabled? 'red' : 'green'
                    onClicked: {
                        model.enabled = !model.enabled ;
                    }
                }
                */
            }

            // FIXME: to reserve space behind the scrollbar.
            // This will probably be solved in Kirigami's scrollbar
            Item {
                id: scrollBarPadder
                width: 4 * Kirigami.Units.gridUnit
            }
        }
    }
}
