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
import org.kde.kcm 1.1 as KCM
import org.kde.kirigami 2.10 as Kirigami

Kirigami.Page {
    KCM.ConfigModule.quickHelp: i18n("This module lets you configure the window rules.")
    KCM.ConfigModule.buttons: KCM.ConfigModule.Help | KCM.ConfigModule.Default | KCM.ConfigModule.Apply
    title: kcm.name

    // To match SimpleKCM's borders of Page + headerParent/footerParent (also specified in raw pixels)
    leftPadding: Kirigami.Settings.isMobile ? 0 : 8
    topPadding: leftPadding
    rightPadding: leftPadding
    bottomPadding: leftPadding

    implicitWidth: Kirigami.Units.gridUnit * 48
    implicitHeight: Kirigami.Units.gridUnit * 33



    Kirigami.InlineMessage {
        Layout.fillWidth: true
        id: infoLabel
        type: Kirigami.MessageType.Information
        text: i18nc("popup tip", "The KCM Module is showing fine!!!")
        visible: true
    }

}
