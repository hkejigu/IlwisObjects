import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Controls 2.4 as QC2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ModelRegistry 1.0
import "../../../../Global.js" as Global
import "../../../../controls" as Controls

Rectangle {
    id : chartContainer
    property alias source : chartArea.source
	width : parent.width
	height : parent.height  - 10
	property int bandX : 0
	property int bandY : 0
	property var tableColumns : []
	property bool active : false
	property int fsIndex : 0

	color : active ? Global.selectedColor : "transparent"

	Loader {
		id : chartArea
		anchors.fill : parent
    
		Connections {
			target: chartArea.item
			onClick: {
				deActivateAll()
				active = true
				setLayerList(bandX, bandY)
			}
		}

		onLoaded : {
			chartArea.item.chart = models.createChartModel(chartArea) 
			var scpModel = modellerDataPane.model.analysisModel(0)
			if ( scpModel){
				setChart(scpModel)
			}
			chartArea.item.showManager = false
		}

	}
	function setBands(bx, by){
	    
		if ( bandX != bx || bandY != by) {
			bandX = bx
			bandY = by
			updateChart2()
		}
	}

	function column2raw(cname){
		var parts = cname.split("_")
		return parts[parts.length - 1]
	}

	function updateChart2() {
	    var scpModel = modellerDataPane.model.analysisModel(0)
		if ( scpModel && bandX != bandY){
			scpModel.calcFeatureSpace(bandX, bandY)
			var firstTime = tableColumns.length == 0
			tableColumns = scpModel.tableColumns(bandX, bandY)
			if ( firstTime)
				source = models.mainPanelUrl("chart")
			else
				setChart(scpModel)
		}
	}

	function setChart(scpModel) {
	    chartArea.item.chart.clearChart()
		// order is reversed as the selection is always the last column and selection and 'real' classes might cover each other; so by reversing the order we ensure that the 'real'class will always be on top
		for(var i=tableColumns.length-1; i >=0; --i) {
			var raw = column2raw(tableColumns[i].xbandname)
			var pcolor = scpModel.raw2color(raw)
			var scpModel = modellerDataPane.model.analysisModel(0)
			var stretch = scpModel.stretchLimits()
			var extraParameters = {name : 'fs1', chartType : 'points', color : pcolor,minx : stretch.min, maxx : stretch.max,miny : stretch.min, maxy : stretch.max}
			chartArea.item.chart.addDataTable(scpModel.featureSpaceTable(),tableColumns[i].xbandname,tableColumns[i].ybandname, extraParameters)
		}
	}
 }