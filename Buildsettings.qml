import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Controls 2.0
import Qt.labs.platform 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Dialogs 1.2

Item {
    id: build_settings

    Connections{
        target: build_manager
        onJson_build_success:{
            build_settings.visible = false
        }
    }

    Rectangle {
        id: rectangle
        color: "#ffffff"
        anchors.fill: parent
        radius: 3
        border.width: 3

        Text {
            id: text1
            x: 319
            text: qsTr("Drone's CSR Attributes")
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 16
        }

        Text {
            id: text2
            height: 20
            text: qsTr("Common Name")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.horizontalCenter
            anchors.rightMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: text1.bottom
            anchors.topMargin: 5
            font.pixelSize: 14
        }

        TextInput {
            id: comman_name
            y: 29
            height: 20
            text: qsTr("A name for your drone")
            selectionColor: "#020080"
            renderType: Text.NativeRendering
            anchors.verticalCenter: text2.verticalCenter
            anchors.left: parent.horizontalCenter
            anchors.leftMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 5
            font.pixelSize: 14
        }

        Text {
            id: text3
            x: 3
            y: 48
            height: 20
            text: qsTr("Orgainization/Business name")
            anchors.top: text2.bottom
            verticalAlignment: Text.AlignVCenter
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            font.pixelSize: 14
            anchors.topMargin: 2
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            horizontalAlignment: Text.AlignRight
        }

        Text {
            id: text4
            x: 4
            y: 69
            height: 20
            text: qsTr("City")
            anchors.top: text5.bottom
            verticalAlignment: Text.AlignVCenter
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            anchors.topMargin: 2
            font.pixelSize: 14
            anchors.rightMargin: 5
            anchors.leftMargin: 5
            horizontalAlignment: Text.AlignRight
        }

        Text {
            id: text5
            x: 0
            y: 69
            height: 20
            text: qsTr("Department Name/Organizational unit")
            anchors.top: text3.bottom
            verticalAlignment: Text.AlignVCenter
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            font.pixelSize: 14
            anchors.topMargin: 2
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            horizontalAlignment: Text.AlignRight
        }

        Text {
            id: text6
            x: 10
            height: 20
            text: qsTr("State")
            anchors.top: text4.bottom
            verticalAlignment: Text.AlignVCenter
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            font.pixelSize: 14
            anchors.topMargin: 2
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            horizontalAlignment: Text.AlignRight
        }

        Text {
            id: text7
            x: 4
            height: 20
            text: qsTr("Country")
            anchors.top: text6.bottom
            verticalAlignment: Text.AlignVCenter
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            anchors.topMargin: 2
            font.pixelSize: 14
            anchors.rightMargin: 5
            anchors.leftMargin: 5
            horizontalAlignment: Text.AlignRight
        }

        TextInput {
            id: state_name
            x: -4
            y: 29
            height: 20
            text: qsTr("Delhi")
            anchors.verticalCenter: text6.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.horizontalCenter
            font.pixelSize: 14
            anchors.rightMargin: 5
            anchors.leftMargin: 5
        }

        TextInput {
            id: city_name
            x: 0
            y: 29
            height: 20
            text: qsTr("New Delhi")
            anchors.verticalCenter: text4.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.horizontalCenter
            font.pixelSize: 14
            anchors.leftMargin: 5
            anchors.rightMargin: 5
        }

        TextInput {
            id: country_name
            x: -3
            y: 29
            height: 20
            text: qsTr("India")
            anchors.verticalCenter: text7.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.horizontalCenter
            font.pixelSize: 14
            anchors.leftMargin: 5
            anchors.rightMargin: 5
        }

        TextInput {
            id: man_name
            x: 1
            y: 29
            height: 20
            text: qsTr("XYZ ltd.")
            anchors.verticalCenterOffset: 1
            anchors.verticalCenter: text3.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.horizontalCenter
            font.pixelSize: 14
            anchors.rightMargin: 5
            anchors.leftMargin: 5
        }

        TextInput {
            id: unit_name
            x: 9
            y: 29
            height: 20
            text: qsTr("software section")
            anchors.verticalCenter: text5.verticalCenter
            anchors.right: parent.right
            anchors.left: parent.horizontalCenter
            font.pixelSize: 14
            anchors.leftMargin: 5
            anchors.rightMargin: 5
        }

        Text {
            id: text8
            x: 316
            y: -9
            text: qsTr("Drone's CSR Attributes")
            anchors.horizontalCenterOffset: -7
            anchors.top: text9.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 16
            anchors.topMargin: 5
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            id: text9
            x: 9
            height: 20
            text: qsTr("Email ID")
            anchors.top: text7.bottom
            verticalAlignment: Text.AlignVCenter
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            font.pixelSize: 14
            anchors.topMargin: 2
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            horizontalAlignment: Text.AlignRight
        }

        TextInput {
            id: email_id
            x: -7
            y: 160
            height: 20
            text: qsTr("Optional")
            anchors.right: parent.right
            anchors.verticalCenter: text9.verticalCenter
            anchors.left: parent.horizontalCenter
            font.pixelSize: 14
            anchors.rightMargin: 5
            anchors.leftMargin: 5
        }

        FileDialog{
            id: file_dialog_dgca_pub_key
            title: "Please select DGCA public key"
            nameFilters: ["DER files(*.der)"]
            folder: shortcuts.home
        }

        Button {
            id: dgca_pub_key
            height: 30
            text: "Select DGCA Public key"
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.right: parent.horizontalCenter
            anchors.rightMargin: 5
            anchors.top: text8.bottom
            anchors.topMargin: 2

            onClicked: file_dialog_dgca_pub_key.open()
        }

        FileDialog{
            id: file_dialog_firmware_build_key
            title: "Please select firmware build key"
            nameFilters: ["PEM files(*.pem)"]
            folder: shortcuts.home
        }

        Button {
            id: firmware_key_button
            x: -5
            y: 205
            height: 30
            text: "Select your firmware build keys"
            anchors.top: text8.bottom
            anchors.right: parent.right
            anchors.left: parent.horizontalCenter
            anchors.topMargin: 2
            anchors.rightMargin: 5
            anchors.leftMargin: 5

            onClicked: file_dialog_firmware_build_key.open()
        }

        FolderDialog{
            id: folder_dialog_json
            folder: StandardPaths.standardLocations(StandardPaths)[0]
            onAccepted: {
                build_manager.create_json(currentFolder)
            }
        }

        Button {
            id: ok_button
            x: 0
            y: 241
            height: 30
            text: "Ok"
            anchors.top: firmware_key_button.bottom
            anchors.right: parent.right
            anchors.left: parent.horizontalCenter
            anchors.topMargin: 5
            anchors.rightMargin: 5
            anchors.leftMargin: 5

            onClicked:{
                build_manager.send_info(comman_name.text, man_name.text, unit_name.text, city_name.text, state_name.text, country_name.text, email_id.text, file_dialog_dgca_pub_key.fileUrl, file_dialog_firmware_build_key.fileUrl)
                folder_dialog_json.open()
            }
        }

        Button {
            id: cancel_button
            x: 0
            height: 30
            text: "Cancel"
            anchors.top: dgca_pub_key.bottom
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 5
            anchors.rightMargin: 5

            onClicked: build_settings.visible = false
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
