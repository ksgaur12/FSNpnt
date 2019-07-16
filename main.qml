import QtQuick 2.0
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Controls 2.0
import Qt.labs.platform 1.0
import QtQuick.Dialogs 1.1


Window {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Firmware Server NPNT")

    Connections{
        target: Serialcom
        onSend_serial_port:{
            serial_model.append({"Text":port})
        }
        onSend_conn_status:{
            if(status){
                connectbutton.text = "Disconnect"
            }
            else{
                connectbutton.text = "Connect"
            }
        }
        onSend_data_status:{
            if(data_status){
                rectangle.color = "green"
            }
            else{
                rectangle.color = "red"
            }
        }
    }

    Connections{
        target: file_manager
        onDownloadProgress:{
            downloadProgressBar.value =  val
            text1.text = Math.round(val*10)/10 + "%"
            text1.visible = true
            downloadProgressBar.visible = true
            if(val == 100){
                message_box.text = "Download Complete"
                message_box.visible = true
                downloadProgressBar.visible = false
                text1.visible = false
            }
        }
    }

    TreeView{
        id: fc_list
        z: 2
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.right: port_box.left
        anchors.rightMargin: 2
        anchors.bottom: download_button.top
        anchors.bottomMargin: 2
        anchors.top: parent.top
        anchors.topMargin: 2
        model: fc_dir_model
        TableViewColumn{
            role: "display"
            title: "Files"
        }

        onClicked:{
            fc_dir_model.item_clicked(index)
        }
    }

    MessageDialog{
        id: message_box
        title: "Error"
        text: "Cannot connect"
        visible: false
    }

    ComboBox {
        id: port_box
        x: 363
        height: 30
        z:2
        displayText: "Port"
        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.right: baud_box.left
        anchors.rightMargin: 2

        model: ListModel{
            id: serial_model
        }

        onPressedChanged: {
            serial_model.clear()
            Serialcom.search_port()
        }

        onActivated: {
            displayText = currentText
        }
    }

    FolderDialog{
        id: folder_dialog
        folder: StandardPaths.standardLocations(StandardPaths)[0]

        onAccepted: {
            console.log(folder_dialog.currentFolder)
            fc_dir_model.download_file(folder_dialog.currentFolder)
        }
    }

    ProgressBar {
        id: downloadProgressBar
        visible:  false
        x: 249
        y: 449
        height: 23
        anchors.right: text1.left
        anchors.rightMargin: 2
        anchors.verticalCenter: download_button.verticalCenter
        anchors.left: download_button.right
        anchors.leftMargin: 15
        from: 0
        to: 100
        value: 0
    }

    Text {
        id: text1
        x: 603
        width: 30
        text: qsTr("0%")
        visible: false
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.bottom: downloadProgressBar.bottom
        anchors.bottomMargin: 0
        anchors.top: downloadProgressBar.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 8
        font.pixelSize: 12
    }

    Button {
        id: download_button
        x: 2
        y: 453
        text: qsTr("Download")
        z: 3
        anchors.right: fc_list.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2

        onClicked: {
            if(downloadProgressBar.visible){
                message_box.text = "Download already in progress"
                message_box.visible = true
            }
            else{
                folder_dialog.open()
            }
            //
        }
    }

    ComboBox {
        id: baud_box
        x: 500
        height: 30
        z: 2
        displayText: "Baudrate"
        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.right: connectbutton.left
        anchors.rightMargin: 2

        model: ListModel{
            id: port_model
            ListElement { text: "1200" }
            ListElement { text: "2400" }
            ListElement { text: "4800" }
            ListElement { text: "9600" }
            ListElement { text: "19200" }
            ListElement { text: "38400" }
            ListElement { text: "57600" }
            ListElement { text: "115200" }
        }

        onActivated:{
            displayText = currentText
        }
    }
    Button {
        id: connectbutton
        x: 550
        y: 0
        z: 2
        width: 120
        height: 30
        text: qsTr("Connect")
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0

        onClicked: {
            if(port_box.currentIndex == -1){
                message_box.text = "Please select a device"
                message_box.visible = true
            }
            else if(!baud_box.currentIndex){
                message_box.text = "Please select a baudrate"
                message_box.visible = true
            }
            else{
                Serialcom.connect_telem(port_box.currentText,  baud_box.currentIndex, connectbutton.text)
            }
        }

        Rectangle {
            id: rectangle
            width: 120
            height: 30
            color: "red"
            anchors.rightMargin: 2
            anchors.topMargin: 2
            anchors.fill: parent
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }




}

/*##^## Designer {
    D{i:24;anchors_width:383;anchors_x:249}D{i:25;anchors_y:459}D{i:21;anchors_height:388;anchors_width:230}
}
 ##^##*/
