import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "workbench" as WorkBench
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import QtGraphicalEffects 1.0
import "Global.js" as Global
import "controls" as Controls


Item {

    id : topRoot
    Rectangle {
        id : topItem
        z: topRoot.z + 1
        width: parent.width
        height: 3
		color : uicontext.darkestColor
    }
    DropShadow {
        id: cmlShadow
        z: topRoot.z + 1
        anchors.fill: source
        cached: true
        verticalOffset: 3
        radius: 3
        samples: 12
        color: "#80000000"
        smooth: true
        source: topItem
    }

}
