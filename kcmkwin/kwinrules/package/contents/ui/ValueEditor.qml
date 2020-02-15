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
import org.kde.kquickcontrols 2.0 as KQC


Loader {
    id: valueEditor
    focus: true

    property var ruleValue
    property var ruleOptions
    property int controlType

    signal valueEdited(var value)

    sourceComponent: {
        //FIXME: Use enum from C++ model when exported
        switch (controlType) {
            case 1: return booleanEditor          //RuleType.Boolean
            case 2: return stringEditor           //RuleType.String
            case 3: return integerEditor          //RuleType.Integer
            case 4: return optionEditor           //RuleType.Option
            case 5: return flagsEditor            //RuleType.FlagsOption
            case 6: return percentageEditor       //RuleType.Percentage
            case 7: return coordinateEditor       //RuleType.Coordinate
            case 8: return shortcutEditor         //RuleType.Shortcut
            default: return emptyEditor
        }
    }

    Component {
        id: emptyEditor
        Item {}
    }

    Component {
        id: booleanEditor
        RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Switch {
                text: checked ? i18n("Yes") : i18n("No")
                checked: ruleValue
                onToggled: valueEditor.valueEdited(checked)
            }
        }
    }

    Component {
        id: stringEditor
        QQC2.TextField {
            property bool isTextEdited: false
            text: ruleValue
            onTextEdited: { isTextEdited = true; }
            onEditingFinished: {
                if (isTextEdited) { valueEditor.valueEdited(text); }
                isTextEdited = false;
            }
        }
    }

    Component {
        id: integerEditor
        QQC2.SpinBox {
            editable: true
            value: ruleValue
            onValueModified: valueEditor.valueEdited(value)
        }
    }

    Component {
        id: optionEditor
        QQC2.ComboBox {
            id: optionCombo
            flat: true
            textRole: "text"

            delegate: QQC2.ItemDelegate {
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                width: parent.width
                highlighted: optionCombo.highlightedIndex == index
                contentItem: RowLayout {
                    Kirigami.Icon {
                        source: model.icon
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                    }
                    QQC2.Label {
                        text: model.text
                        color: highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                        Layout.fillWidth: true
                    }
                }
            }

            model: ruleOptions
            currentIndex: model.selectedIndex
            onActivated: {
                print ("Rule changed: " + key + " INDEX := " + currentIndex);
                model.selectedIndex = currentIndex;
            }
            //FIXME: After Qt 5.14
            //valueRole: "value"
            //currentValue: ruleValue
            //onActivated: { valueEditor.valueEdited(currentValue); }
        }
    }

    Component {
        id: flagsEditor
        RowLayout {
            Layout.minimumWidth: 10 * Kirigami.Units.gridUnit
            Layout.alignment: Qt.AlignRight;
            spacing: 0
            Repeater {
                id: flagsRepeater
                model: ruleOptions

                QQC2.ToolButton {
                    property int bit: model.value
                    icon.name: model.iconName
                    checkable: true
                    checked: (ruleValue & (1 << bit)) == (1 << bit)
                    QQC2.ToolTip.text: model.text
                    QQC2.ToolTip.visible: hovered
                    onToggled: {
                        valueEditor.valueEdited((ruleValue & ~(1 << bit)) | (checked << bit));
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
                value: ruleValue
                onMoved: valueEditor.valueEdited(Math.round(slider.value))
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
            property var coords: ruleValue ? ruleValue.split(',') : [0, 0]

            QQC2.SpinBox {
                id: coord_x
                editable: true
                Layout.fillWidth: true
                from: 0
                to: 4098
                value: coords[0]
                onValueModified: valueEditor.valueEdited(coord_x.value + "," + coord_y.value)
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
                value: coords[1]
                onValueModified: valueEditor.valueEdited(coord_x.value + "," + coord_y.value)
            }
        }
    }

    Component {
        id: shortcutEditor
        RowLayout {
            Item {
                Layout.fillWidth: true
            }
            KQC.KeySequenceItem {
                //FIXME: KeySequenceItem.qml:14:5: QML KeySequenceHelper: Binding loop detected for property "keySequence"
                keySequence: ruleValue
                onKeySequenceChanged: valueEditor.valueEdited(keySequence)
            }
        }
    }
}