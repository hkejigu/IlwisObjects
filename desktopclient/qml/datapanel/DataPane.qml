import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MessageModel 1.0
import ResourceModel 1.0
import UIContextModel 1.0
import MasterCatalogModel 1.0
import DataPaneModel 1.0
import TabModel 1.0
import ".." as Base
import "catalog" as Catalog
import "modeller" as Modeller
import "../workbench" as Workbench
import "../Global.js" as Global
import "../controls" as Controls

Rectangle {

    id: centerItem
    color: uicontext.lightColor
    objectName : "datapane_container_mainui"
    height : parent.height
    property int activeSplit : 2
    property var activeItem

    /*
     * Signal, thrown if a tab is closed
     */
    signal closedTab(string title)


    SplitView {
        id : datapanesplit
        objectName : "datapane_mainui"
        orientation: Qt.Horizontal
        anchors.fill: parent
        anchors.leftMargin: 3

        DataTabView2 {
            id : lefttab
            property bool fillWidth : true
            side : 1
            objectName: "datapane_lefttab_mainui"
            Layout.fillWidth: fillWidth

            Component.onCompleted: {
                datapane.leftSide.setTabview(lefttab.objectName)
            }
        }

        DataTabView2 {
            id : righttab
            side : 2
            objectName: "datapane_righttab_mainui"
            Component.onCompleted: {
                datapane.rightSide.setTabview(righttab.objectName)
            }
        }

        function closeTab(isleft, tabindex){
            var ok = datapane.removeTab(isleft, tabindex)
            if ( ok){
                var tabview = isleft ? lefttab : righttab
                tabview.removeTab(tabindex)
                tabview.currentIndex = Math.max(0,tabindex - 1)

                if ( datapane.leftSide.tabCount === 0){
                    lefttab.state = "zerosize"
                    righttab.state = "fullsize"
                }else if(datapane.rightSide.tabCount === 0){
                    lefttab.state = "fullsize"
                    righttab.state = "zerosize"
                }
            }
        }

        function changePanel(filter, outputtype, url){
            var sidePanel = datapane.activeSide
            var tabview = sidePanel.tabview
            var insertetTab
            if ( tabview ){
                var oldType = tabview.getTab(tabview.currentIndex).item.panelType
                if ( oldType === "catalog" || oldType === "operationcatalog" || oldType === "scriptpanel"){
                    tabview.removeTab(tabview.currentIndex)
                    var newPanel = sidePanel.createPanel(tabview.currentIndex,filter,outputtype, url)
                    if ( newPanel){
                        var component = Qt.createComponent(newPanel.componentUrl)
                        if (component.status === Component.Ready){
                            var data= newPanel.displayName
                            insertetTab = tabview.insertTab(tabview.currentIndex, data, component)
                            insertetTab.active = true
                            if ( insertetTab && insertetTab.item){
                                var ind = tabview.currentIndex
                                tabview.currentIndex = -1;
                                tabview.currentIndex = ind; // forces a reset of the index and thus a redraw
                                insertetTab.item.addDataSource(filter, url, outputtype)
                                insertetTab.item.tabmodel = newPanel
                                datapane.select(sidePanel.side === "left", tabview.currentIndex, true)
                                activeItem = insertetTab.item
                            }
                        }
                    }
                }else {
                    datapanesplit.newPanel(filter,outputtype,url,"other")
                }
            }

            return insertetTab
        }

        function newPanel(filter,outputtype, url,side, options) {
            var id = -1
            var insertetTab
            var allNew = datapane.leftSide.tabCount === 0 && datapane.rightSide.tabCount === 0
            var newPanel = datapane.createPanel(filter,outputtype, url, side)
            if ( newPanel){
                var component = Qt.createComponent(newPanel.componentUrl)
                if (component.status === Component.Error)
                    console.log("Error loading component:", component.errorString());
                var sidePanel = datapane.activeSide
                var tabview = sidePanel.tabview
                if ( tabview){
                    var data= newPanel.displayName
                    insertetTab = tabview.addTab(data, component)
                    insertetTab.active = true
                    insertetTab.item.tabmodel = newPanel
                    id = insertetTab.item.addDataSource(filter, url, outputtype, options)
                    activeItem = insertetTab.item

                }
                if ( allNew){
                    lefttab.state = "fullsize"
                }else if ( datapane.leftSide.tabCount === 1 && datapane.rightSide.tabCount === 1){ // one of them was new, so halfsize
                    lefttab.state ="halfsize"
                    righttab.state = "halfsize"
                } //else we dont do anything, leave it as it is
                if ( sidePanel.side == "left"){
                    lefttab.currentIndex = sidePanel.tabCount - 1
                }else{
                    righttab.currentIndex = sidePanel.tabCount - 1
                }
            }
            return insertetTab
        }

        function changeWidth(pside, partside){
            if ( partside === 0){
                lefttab.fillWidth = false
                lefttab.state = "zerosize"
            }else {
                lefttab.fillWidth = true
                righttab.state = "zerosize"

                activeSplit = 2
                activeSplit = 1
            }
        }

        function totalTabCount(){
            return lefttab.count + righttab.count
        }
    }

    function iconSource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        var iconP = "../images/" + name
        return iconP

    }
    function getCurrentCatalogTab(){
        var tabview = Math.abs(activeSplit) == 1 ? lefttab : righttab
        if ( tabview && tabview.currentIndex >= 0 && tabview.count > 0) {
            var tab = tabview.getTab(tabview.currentIndex)
            if ( tab && tab.item){
                if ( "currentCatalog" in tab.item){
                    return tab.item
                }else{ // apparently the tab has no catalog so we look at the other side
                    tabview = Math.abs(activeSplit) == 2 ? lefttab : righttab
                    if ( tabview && tabview.currentIndex >= 0 && tabview.count > 0) {
                        tab = tabview.getTab(tabview.currentIndex)
                        if ( tab && tab.item){
                            if ( "currentCatalog" in tab.item){
                                activeSplit = Math.abs(activeSplit) == 2 ? 1 : 2
                                return tab.item
                            }
                        }
                    }
                }
            }
        }
    }

    function newCatalog(filter,outputtype, url,side, options){
        return datapanesplit.newPanel(filter, outputtype, url,side, options)
    }

    function setCatalogByIndex(currentTab, tabindex){
        currentTab.currentIndex = tabindex
        var catalogtab = getCurrentCatalogTab()
        if ( catalogtab && catalogtab.currentCatalog){
            mastercatalog.currentUrl = catalogtab.currentCatalog.url
            mastercatalog.currentCatalog = catalogtab.currentCatalog
        }
    }

    function changeCatalog(filter, outputtype, url){
        return datapanesplit.changePanel(filter,outputtype, url)
    }

}
