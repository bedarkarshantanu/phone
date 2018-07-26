/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2017-2018 Konsulko Group
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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtMultimedia 5.5
import AGL.Demo.Controls 1.0

Item {
    id: root

    Connections {
        target: phone

        onElapsedTimeChanged: {
            callStateLabel.text = phone.elapsedTime
        }
    }

    Connections {
        target: telephony

        onConnectedChanged: {
            // FIXME: keep dialpad inactive until connected
        }

        onCallStateChanged: {
            if (telephony.callState == "incoming") {
                pbap.search(telephony.callClip)
                rejectButton.active = true
                callStateLabel.text = "Incoming call from " + telephony.callClip
            } else if (telephony.callState == "dialing") {
                callStateLabel.text = "Dialing " + telephony.callColp
            } else if (telephony.callState == "active") {
                rejectButton.active = false
            } else if (telephony.callState == "disconnected") {
                pbap.refreshCalls(100);
                rejectButton.active = false
                callButton.checked = false
                callStateLabel.text = ""
            }
        }
    }

    Connections {
        target: pbap

        onSearchResults: {
            if (name != "Not Found") {
                if (telephony.callState == "incoming") {
                    callStateLabel.text = "Incoming call from " + name
                } else if (telephony.callState == "dialing") {
                    callStateLabel.text = "Dialing " + name
                }
            }
        }
    }

    function call(cname, cnumber) {
        name.text = cname
        var rawNumber = cnumber
        number.text = rawNumber.replace(/-/g, '')
        callButton.checked = true
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 50
        anchors.bottomMargin: 50
        spacing: 20
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            Label {
                id: name
                font.pixelSize: 40
                color: '#59FF7F'
            }
            TextField {
                id: number
                readOnly: true

                ImageButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    offImage: './images/HMI_Phone_Back_Icon.svg'
                    onClicked: {
                        if (number.text.length > 0)
                            number.text = number.text.substring(0, number.text.length - 1)
                    }
                }
            }
        }

        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 3
            columnSpacing: 30
            rowSpacing: 30
            Repeater {
                model: ListModel {
                    ListElement { value: '1'; image: '1' }
                    ListElement { value: '2'; image: '2' }
                    ListElement { value: '3'; image: '3' }
                    ListElement { value: '4'; image: '4' }
                    ListElement { value: '5'; image: '5' }
                    ListElement { value: '6'; image: '6' }
                    ListElement { value: '7'; image: '7' }
                    ListElement { value: '8'; image: '8' }
                    ListElement { value: '9'; image: '9' }
                    ListElement { value: '*'; image: 'asterisk' }
                    ListElement { value: '0'; image: '0' }
                    ListElement { value: '#'; image: 'NumberSign' }
                }
                ImageButton {
                    onImage: './images/HMI_Phone_Button_%1_Active-01.svg'.arg(model.image)
                    offImage: './images/HMI_Phone_Button_%1_Inactive-01.svg'.arg(model.image)
                    onClicked: {
                        number.text += model.value
                    }
                }
            }
        }

        Label {
            id: callStateLabel
            Layout.alignment: Qt.AlignHCenter
            text: ""
        }

        ToggleButton {
            id: callButton
            Layout.alignment: Qt.AlignHCenter
            onImage: './images/HMI_Phone_Hangup.svg'
            offImage: './images/HMI_Phone_Call.svg'
            property var active: (number.text.length > 0) || (telephony.callState == "incoming") || (telephony.callState == "active")
            opacity: active ? 1 : 0.25

            onCheckedChanged: {
                if (checked) {
                    if (!active) {
                        callButton.checked = false
                        return
                    }

                    var contact = {'name': name.text, 'number': number.text}
                    if (contact.name === '')
                        contact.name = 'Unknown'
                    if (telephony.callState == "incoming") {
                        telephony.answer()
                    } else {
                        pbap.search(number.text)
                        telephony.dial(number.text)
                    }
                } else {
                    name.text = ''
                    number.text = ''
                    telephony.hangup()
                }
            }
        }

        Loader {
            id: rejectButton
            Layout.alignment: Qt.AlignHCenter
            active: false
            sourceComponent: ImageButton {
                offImage: './images/HMI_Phone_Hangup.svg'
                onClicked: {
                    telephony.hangup()
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.preferredHeight: 130
            orientation: Qt.Horizontal
            clip: true
            model: RecentCallModel

            delegate: MouseArea {
                width: root.width / 2.5
                height: ListView.view.height
                RowLayout {
                    anchors.fill: parent
                    spacing: 20
                    Image {
                        source: './images/HMI_Phone_Contact_BlankPhoto.svg'
                    }
                    ColumnLayout {
                        Label {
                            Layout.fillWidth: true
                            color: '#59FF7F'
                            text: model.modelData.name
                        }

                        Label {
                            Layout.fillWidth: true
                            font.pixelSize: 30
                            text: model.modelData.number
                        }
                    }
                }
                onClicked: root.call(model.modelData.name, model.modelData.number)
            }
        }
    }
}
