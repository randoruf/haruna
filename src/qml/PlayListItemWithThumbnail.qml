/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

Kirigami.BasicListItem {
    id: root

    property bool isPlaying: model.isPlaying
    property bool isLocal: model.isLocal
    property string rowNumber: (index + 1).toString()
    property var alpha: PlaylistSettings.overlayVideo ? 0.6 : 1

    implicitHeight: (Kirigami.Units.gridUnit - 6) * 8
    padding: 0
    backgroundColor: {
        let color = model.isPlaying ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor
        Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, alpha)
    }

    onDoubleClicked: {
        mpv.playlistProxyModel.setPlayingItem(index)
        mpv.loadFile(path)
        mpv.pause = false
    }

    contentItem: Rectangle {
        anchors.fill: parent
        color: "transparent"
        RowLayout {
            anchors.fill: parent
            anchors.rightMargin: Kirigami.Units.largeSpacing
            spacing: Kirigami.Units.largeSpacing

            Label {
                text: pad(root.rowNumber, playlistView.count.toString().length)
                visible: PlaylistSettings.showRowNumber
                font.pointSize: (window.isFullScreen() && playList.bigFont)
                                ? Kirigami.Units.gridUnit
                                : Kirigami.Units.gridUnit - 6
                horizontalAlignment: Qt.AlignCenter
                Layout.leftMargin: Kirigami.Units.largeSpacing

                function pad(number, length) {
                    while (number.length < length)
                        number = "0" + number;
                    return number;
                }
            }

            Rectangle {
                width: 1
                color: Kirigami.Theme.alternateBackgroundColor
                visible: PlaylistSettings.showRowNumber
                Layout.fillHeight: true
            }

            Item {
                width: (root.height - 20) * 1.33333
                height: root.height - 20

                Image {
                    anchors.fill: parent
                    source: "image://thumbnail/" + model.path
                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                    asynchronous: true
                    fillMode: Image.PreserveAspectFit

                    Rectangle {
                        visible: model.duration.length > 0
                        height: 25
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        color: {
                            let color = Kirigami.Theme.alternateBackgroundColor
                            Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.8)
                        }

                        Label {
                            anchors.centerIn: parent
                            color: Kirigami.Theme.textColor
                            horizontalAlignment: Qt.AlignCenter
                            text: model.duration
                            font.pointSize: (window.isFullScreen() && playList.bigFont)
                                            ? Kirigami.Units.gridUnit
                                            : Kirigami.Units.gridUnit - 5

                            Layout.margins: Kirigami.Units.largeSpacing
                        }
                    }
                }
            }

            Kirigami.Icon {
                source: "media-playback-start"
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
                visible: isPlaying

                Layout.leftMargin: PlaylistSettings.showRowNumber ? 0 : Kirigami.Units.largeSpacing
            }

            LabelWithTooltip {
                text: PlaylistSettings.showMediaTitle ? model.title : model.name
                color: Kirigami.Theme.textColor
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                elide: Text.ElideRight
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pointSize: (window.isFullScreen() && playList.bigFont)
                                ? Kirigami.Units.gridUnit
                                : Kirigami.Units.gridUnit - 5
                font.weight: isPlaying ? Font.ExtraBold : Font.Normal
                layer.enabled: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: PlaylistSettings.showRowNumber || isPlaying ? 0 : Kirigami.Units.largeSpacing
            }
        }
    }
}
