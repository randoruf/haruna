/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: false
    helpFile: ""

    GridLayout {
        id: content

        columns: 2

        SubtitlesFolders {
            id: subtitleFolders
            implicitWidth: root.width
            Layout.fillWidth: true
            Layout.columnSpan: 2
        }

        Label {
            text: i18nc("@label:textbox", "Preferred language")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            text: SubtitlesSettings.preferredLanguage
            placeholderText: i18nc("placeholder text", "eng,ger etc.")
            Layout.fillWidth: true
            onTextEdited: {
                SubtitlesSettings.preferredLanguage = text
                SubtitlesSettings.save()
                mpv.setProperty("slang", text)
            }

            ToolTip {
                text: i18nc("@info:tooltip", "Do not use spaces.")
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Preferred track")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 100
            value: SubtitlesSettings.preferredTrack
            editable: true
            onValueChanged: {
                SubtitlesSettings.preferredTrack = value
                SubtitlesSettings.save()
                if (value === 0) {
                    mpv.setProperty("sid", "auto")
                } else {
                    mpv.setProperty("sid", value)
                }
            }
        }

        Item { width: 1; height: 1 }
        CheckBox {
            checked: PlaylistSettings.allowOnBlackBorders
            text: i18nc("@option:check", "Allow subtitles in black borders")
            onCheckStateChanged: {
                PlaylistSettings.allowOnBlackBorders = checked
                PlaylistSettings.save()
                mpv.setProperty("sub-use-margins", checked ? "yes" : "no")
                mpv.setProperty("sub-ass-force-margins", checked ? "yes" : "no")
            }

            ToolTip {
                text: i18nc("@info:tooltip", "When checked the subtitles can be rendered outside the video, in the black borders. Might not work for all .ass subtitles.")
            }
        }

        SettingsHeader {
            text: i18nc("@title", "Styling")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Label {
            text: i18nc("@label:listbox", "Font family")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true

            ComboBox {
                id: subtitleFont

                property string defaultFamily: "Sans Serif"

                model: app.getFonts()
                onActivated: {
                    SubtitlesSettings.fontFamily = currentText
                    SubtitlesSettings.save()
                    mpv.setProperty("sub-font", currentText)
                }

                Component.onCompleted: currentIndex = indexOfValue(SubtitlesSettings.fontFamily)
            }

            Button {
                icon.name: "edit-clear-all"
                onClicked: {
                    const index = subtitleFont.find(subtitleFont.defaultFamily)
                    subtitleFont.currentIndex = index
                }
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Font size")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 1000
            value: 55
            onValueChanged: {
                SubtitlesSettings.fontSize = value
                SubtitlesSettings.save()
                mpv.setProperty("sub-font-size", value)
            }
        }

        Label {
            text: i18nc("@label", "Font style")
            Layout.alignment: Qt.AlignRight
        }

        CheckBox {
            text: i18nc("@option:check", "Bold")
            checked: SubtitlesSettings.isBold
            onCheckedChanged: {
                SubtitlesSettings.isBold = checked
                SubtitlesSettings.save()
                mpv.setProperty("sub-bold", checked)
            }
        }

        Item { width: 1 }

        CheckBox {
            text: i18nc("@option:check", "Italic")
            checked: SubtitlesSettings.isItalic
            onCheckedChanged: {
                SubtitlesSettings.isItalic = checked
                SubtitlesSettings.save()
                mpv.setProperty("sub-italic", checked)
            }
        }

        Label {
            text: i18nc("@label:textbox", "Font color")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true


            ColorPickerButton {
                id: subtitleColorPicker

                onColorChosen: subtitleColor.text = color
            }

            TextField {
                id: subtitleColor

                property string defaultColor: "#FFFFFFFF"

                text: SubtitlesSettings.fontColor
                onTextChanged: {
                    subtitleColorPicker.color = text
                    SubtitlesSettings.fontColor = text
                    SubtitlesSettings.save()
                    mpv.setProperty("sub-color", text)
                }
            }

            Button {
                icon.name: "edit-clear-all"
                onClicked: subtitleColor.text = subtitleColor.defaultColor

                ToolTip {
                    text: i18nc("@info:tooltip", "Set default value")
                }
            }
        }

        Label {
            text: i18nc("@label:textbox", "Shadow color")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true

            ColorPickerButton {
                id: shadowColorPicker

                onColorChosen: shadowColor.text = color
            }

            TextField {
                id: shadowColor

                property string defaultColor: "#80F0F0F0"

                text: SubtitlesSettings.shadowColor
                onTextChanged: {
                    shadowColorPicker.color = text
                    SubtitlesSettings.shadowColor = text
                    SubtitlesSettings.save()
                    mpv.setProperty("sub-shadow-color", text)
                }
            }

            Button {
                icon.name: "edit-clear-all"
                onClicked: shadowColor.text = shadowColor.defaultColor

                ToolTip {
                    text: i18nc("@info:tooltip", "Set default value")
                }
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Shadow offset")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 25
            value: SubtitlesSettings.shadowOffset
            editable: true
            onValueChanged: {
                SubtitlesSettings.shadowOffset = value
                SubtitlesSettings.save()
                mpv.setProperty("sub-shadow-offset", value)
            }

            ToolTip {
                text: i18nc("@info:tooltip", "Set to 0 (zero) to disable.")
            }
        }

        Label {
            text: i18nc("@label:textbox", "Border color")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            Layout.fillWidth: true

            ColorPickerButton {
                id: borderColorPicker

                color: SubtitlesSettings.borderColor
                onColorChosen: borderColor.text = color
            }

            TextField {
                id: borderColor

                property string defaultColor: "#FF000000"

                text: SubtitlesSettings.borderColor
                onTextChanged: {
                    borderColorPicker.color = text
                    SubtitlesSettings.borderColor = text
                    SubtitlesSettings.save()
                    mpv.setProperty("sub-border-color", text)
                }
            }

            Button {
                icon.name: "edit-clear-all"
                onClicked: borderColor.text = borderColor.defaultColor

                ToolTip {
                    text: i18nc("@info:tooltip", "Set default value")
                }
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Border width")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            from: 0
            to: 25
            value: SubtitlesSettings.borderSize
            editable: true
            onValueChanged: {
                SubtitlesSettings.borderSize = value
                SubtitlesSettings.save()
                mpv.setProperty("sub-border-size", value)
            }

            ToolTip {
                text: i18nc("@info:tooltip", "Set to 0 (zero) to disable.")
            }
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
