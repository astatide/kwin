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

    Kirigami.InlineMessage {
        text: i18n("You have specified the window class as unimportant.\n" +
        "This means the settings will possibly apply to windows from all applications. " +
        "If you really want to create a generic setting, it is recommended you at least " +
        "limit the window types to avoid special window types.")
        visible: rulesModel.showWarning
        Layout.fillWidth: true
    }

    QQC2.Frame {
        Layout.fillWidth: true
        Layout.fillHeight: true

        ColumnLayout {
            anchors.fill: parent

            ListView {
                id: enabledRulesView

                Layout.fillWidth: true
                Layout.fillHeight: true
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                focus: true

                QQC2.ScrollBar.vertical: QQC2.ScrollBar { }

                model: rulesModel
                delegate: ruleDelegate

                section.delegate: Kirigami.ListSectionHeader { label: section }
                section.property: "section"
            }

            QQC2.TextField {
                id: searchBar
                Layout.fillWidth: true

                placeholderText: i18n("Filter")
            }
        }
    }

    Component {
        id: ruleDelegate

        Kirigami.BasicListItem {
            id: ruleItem
            icon: model.icon
            label: '<b>' + name + '</b> <i>(' + key + ')</i>'

            height: 2.5 * Kirigami.Units.gridUnit
            anchors {
                left: parent.left
                right: parent.right
            }

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            backgroundColor: Kirigami.Theme.backgroundColor

            RowLayout {

                spacing: Kirigami.Units.smallSpacing

                QQC2.ToolButton {
                    id: descriptionButton
                    icon.name: "documentinfo"
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter
                    width: height

                    visible: (model.description != "")
                    QQC2.ToolTip.text: model.description
                    QQC2.ToolTip.visible: down || hovered
                }

                Loader {
                    id: fieldEditor
                    focus: true

                    property var value: model.value

                    sourceComponent: {
                        switch (model.type) {
                            case 1: //RuleType.Boolean
                                return booleanEditor
                            case 2: //RuleType.String
                                return textEditor
                            case 3: //RuleType.Integer
                                return integerEditor
                            case 4: //RuleType.Option
                                return optionEditor
                            case 6: //RuleType.Percentage
                                return percentageEditor
                            default:
                                return notImplementedEditor
                        }
                    }

                    onLoaded: {
                        item.width = 8 * Kirigami.Units.gridUnit;
                        item.Layout.alignment = Qt.AlignVCenter + Qt.AlignRight;
                    }
                }
            }
        }
    }

    Component {
        id: notImplementedEditor
        QQC2.Label {
            text: "[ :D ]"
        }
    }

    Component {
        id: booleanEditor
        QQC2.Switch {
            checked: value
        }
    }

    Component {
        id: textEditor
        QQC2.TextField {
            text: value
        }
    }

    Component {
        id: integerEditor
        QQC2.SpinBox {
            value: value
            editable: true
        }
    }

    Component {
        id: optionEditor
        QQC2.ComboBox {
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
                value: value
            }
            QQC2.Label {
                text: i18n("%1 %", Math.round(slider.value))
            }
        }
    }

}
