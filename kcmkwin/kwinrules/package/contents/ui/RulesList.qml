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
import org.kde.kcm 1.2
import org.kde.kirigami 2.5 as Kirigami

ScrollViewKCM {

    ConfigModule.buttons: ConfigModule.Help | ConfigModule.Apply

    view: ListView {
        id: ruleListView
        model: kcm.rulesListModel
        delegate: Kirigami.AbstractListItem {
            id: ruleListItem

            width: parent.width
            //height: 2 * Kirigami.Units.gridUnit
            focus: true

            highlighted: ListView.isCurrentItem
            onClicked: ruleListView.currentIndex = index

            RowLayout {
                QQC2.Label {
                    text: modelData
                    Layout.fillWidth: true
                }

                Kirigami.Action {
                    iconName: "edit"
                    onTriggered: {
                        kcm.editRule(index)
                    }
                }
            }
        }
    }
}
