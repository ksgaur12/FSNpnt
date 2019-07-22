import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Controls 2.0
import Qt.labs.platform 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Dialogs 1.2

Item {
    id:cert
//    width: 544
//    height: 446

    Connections{
        target: cert_manager
        onKey_generate_success:{
            message_box.title = "Message"
            message_box.text = "Key Generation Successful"
            message_box.visible = true
        }
        onKey_load_success:{
            message_box.title = "Message"
            message_box.text = "Key Load Successful"
            message_box.visible = true
        }
        onCert_load_success:{
            message_box.title = "Message"
            message_box.text = "Cert Load Successful"
            message_box.visible = true
        }
        onCsr_load_success:{
            message_box.title = "Message"
            message_box.text = "CSR Load Successful"
            message_box.visible = true
        }
    }

    FolderDialog{
        id: folder_dialog_keys
        folder: StandardPaths.standardLocations(StandardPaths)[0]
        onAccepted: {
            cert_manager.generate_keys(currentFolder)
        }
    }

    FolderDialog{
        id: folder_dialog_cert
        folder: StandardPaths.standardLocations(StandardPaths)[0]
        onAccepted: {
            cert_manager.generate_cert(currentFolder)
        }
    }

    FileDialog{
        id: folder_dialog_load_keys
        title: "Please select your key"
        nameFilters: ["PEM files(*.pem)"]
        folder: shortcuts.home
        onAccepted: {
            cert_manager.load_keys(fileUrl)
        }
    }

    FileDialog{
        id: folder_dialog_load_cert
        title: "Please select your certificate"
        nameFilters: ["PEM files(*.pem)"]
        folder: shortcuts.home
        onAccepted: {
            cert_manager.load_cert(fileUrl)
        }
    }

    FileDialog{
        id: folder_dialog_load_csr
        title: "Please select drone's CSR"
        nameFilters: ["PEM files(*.pem)"]
        folder: shortcuts.home
        onAccepted: {
            cert_manager.load_csr(fileUrl)
        }
    }

    Buildsettings{
        id: build_settings
        anchors.fill: parent
        z: 4
        visible: false
    }


    Rectangle {
        id: rectangle
        color: "#ffffff"
        radius: 3
        anchors.bottomMargin: 0
        border.width: 3
        anchors.fill: parent

        SplitView {
            id: splitView
            anchors.bottomMargin: 307
            anchors.fill: parent
        }

        Button {
            id: geneate_key_button
            height: 30
            text: qsTr("Gerenate Key and CSR")
            autoRepeat: false
            focusPolicy: Qt.StrongFocus
            z: 2
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.top: geneate_build_attribute.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5

            onClicked: folder_dialog_keys.open()
        }

        Button {
            id: load_key_button
            x: 2
            height: 30
            text: qsTr("Load Manufacturer's Key")
            anchors.top: geneate_key_button.bottom
            anchors.topMargin: 5
            z: 2
            anchors.leftMargin: 5
            anchors.left: parent.left
            anchors.rightMargin: 5
            anchors.right: parent.horizontalCenter
            onClicked: folder_dialog_load_keys.open()
        }

        Button {
            id: load_drone_csr_button
            x: 1
            height: 30
            text: qsTr("Load Drone CSR")
            z: 2
            anchors.leftMargin: 5
            anchors.top: load_man_cert_button.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.rightMargin: 5
            anchors.right: parent.right
            onClicked: folder_dialog_load_csr.open()
        }

        Button {
            id: geneate_drone_cert
            x: 4
            y: 0
            height: 30
            text: qsTr("Generate Drone's Certificate")
            z: 2
            anchors.leftMargin: 5
            anchors.top: load_drone_csr_button.bottom
            anchors.left: parent.left
            autoRepeat: false
            focusPolicy: Qt.StrongFocus
            anchors.rightMargin: 5
            anchors.topMargin: 5
            anchors.right: parent.right

            onClicked:folder_dialog_cert.open()
        }

        Button {
            id: load_man_cert_button
            x: 3
            height: 30
            text: qsTr("Load Manufacturer's Certificate")
            z: 2
            anchors.leftMargin: 5
            anchors.top: geneate_key_button.bottom
            anchors.left: parent.horizontalCenter
            anchors.topMargin: 5
            anchors.rightMargin: 5
            anchors.right: parent.right

            onClicked: folder_dialog_load_cert.open()
        }

        Button {
            id: geneate_build_attribute
            y: -9
            height: 30
            text: qsTr("Generate build settting")
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            focusPolicy: Qt.StrongFocus
            anchors.topMargin: 5
            anchors.top: parent.top
            z: 2
            autoRepeat: false
            onClicked: build_settings.visible = true
        }
    }
}
