/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2018 Konsulko Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import AGL.Demo.Controls 1.0
import PhoneNumber 1.0

Item {
    id: root

    function display_type(type) {
        if (type == PhoneNumber.CELL) {
            return "M"
        } else if (type == PhoneNumber.WORK) {
            return "W"
        } else if (type == PhoneNumber.HOME) {
            return "H"
        } else if (type == PhoneNumber.FAX) {
            return "F"
        } else if (type == PhoneNumber.VOICE) {
            return "V"
        } else {
            return "O"
        }
    }

    signal call(var cname, var cnumber)

    ListView {
        anchors.fill: parent
        model: ContactsModel
        //cacheBuffer: 2880
        delegate: MouseArea {
            width: ListView.view.width
            height: width / 3
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 200
                spacing: 100
                Image {
                    source: './images/HMI_ContactScreen_ImageHolder-01.svg'
                }
                ColumnLayout {
                    Label {
                        Layout.fillWidth: true
                        color: '#59FF7F'
                        font.pixelSize: 50
                        text: model.modelData.name
                    }
                    Repeater {
                        model: numbers
                        delegate: Label {
                            Layout.fillWidth: true
                            font.pixelSize: 50
                            text: display_type(type) + ": " + number
                        }
                    }
                }
            }
            onClicked: {
                root.call(model.name, model.numbers[0].number)
            }
        }
        section.property: 'name'
        section.criteria: ViewSection.FirstCharacter
        section.delegate: Item {
            Label {
                width: root.width / 5
                height: width
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: '#59FF7F'
                text: section
            }
        }
    }
}
