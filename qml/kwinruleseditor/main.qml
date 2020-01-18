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

        QQC2.ScrollView {
            anchors.fill: parent
            anchors.margins: 0

            ListView {
                id: enabledRulesView
                model: rules
                delegate: RuleDelegate {
                    editMode: editModeButton.checked
                }
                section {
                    delegate: Kirigami.ListSectionHeader { label: section }
                    property: "section"
                }
            }
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
                id: editModeButton
                icon.name: checked ? 'view-visible' : 'view-hidden'
                checkable: true
                text: i18n("Show all rules")
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
}
