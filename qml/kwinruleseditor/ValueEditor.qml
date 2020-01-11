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

Loader {
    id: valueEditor
    focus: true

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
    }

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
            property var coords: modelValue ? modelValue.split(',') : [0, 0]

            QQC2.SpinBox {
                id: coord_x
                editable: true
                Layout.fillWidth: true
                from: 0
                to: 4098
                value: coords[0]
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
                value: coords[1]
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
