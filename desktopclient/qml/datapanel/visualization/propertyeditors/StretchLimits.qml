import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../Global.js" as Global
import "../../../controls" as Controls

Row {
    width : Math.min(300,parent ? Math.min(parent.width,500) : 300)
    height: parent ? parent.height - 10 : 0
    clip : true
    property var editor
    property int offset : 30
    Column {
        id : minihist
        width : parent.width - 60
        height: parent ? parent.height - 10 : 0
        x : 10

        //spacing : 5

        Loader {
            id : chartArea
            width : parent.width + offset
            height : parent.height - slider.height - 10
            source : models.mainPanelUrl("minimalchart")
            x : -offset

            onLoaded : {
			    var extraParams = {name : 'histogram',chartType : 'line', color : 'black'}
                chartArea.item.chart.addDataTable(editor.coverageId,"max","counts",extraParams)
                chartArea.item.margins.left = 0
                chartArea.item.margins.right = 0
                chartArea.item.margins.top = 0
                chartArea.item.margins.bottom = 0
				slider.minValue = editor.min //initStretchMin;
                slider.maxValue = editor.max //initStretchMax;

                 if (editor.zoomOnPreset) {
                    chartArea.item.xmin = editor.min;
                    chartArea.item.xmax = editor.max;
                    slider.minValue = editor.min;
                    slider.maxValue = editor.max;
                }
            }
        }
        Connections {
            target: slider
            onMarkerPositions : {
                editor.setMarkers(positions);
            }  
        }
        Connections {
            target: slider
            onMarkerReleased :{
                editor.markersConfirmed()
            }
        }
            
        Controls.MultiPointSlider {
            id : slider
            model : editor.markers
            minValue : editor.min
            maxValue : editor.max
            width : parent.width - 20
            resolution : editor.resolution
            x : 12
        }
    }
    Column {
        width : 70
        height :110
        anchors.leftMargin : 5
        anchors.rightMargin : 5

        Text {
            width : parent.width
            height : 24
            text : "Presets"
        }
        Button {
            width : parent.width - 20
            height : 20
            text : "0%"

            onClicked : {
                updateMarkerPositions(0)
            }
        }

        Button {
            width : parent.width - 20
            height : 20
            text : "1%"

            onClicked : {
                var limits = []
                updateMarkerPositions(0.01)
            }
        }

        Button {
            width : parent.width - 20
            height : 20
            text : "2%"

            onClicked : {
                updateMarkerPositions(0.02)
            }
        }

        Button {
            width : parent.width - 20
            height : 20
            text : "5%"

            onClicked : {
                updateMarkerPositions(0.05)
            }
        }
        Text {
            width : parent.width
            height : 24
            text : "Zoom on preset"
        }
        Switch {
            id : zoomonpreset
            checked : editor.zoomOnPreset

            onClicked : {
                editor.zoomOnPreset = checked

                if (!zoomonpreset.checked) {
                    var oldzoom = editor.zoomlevel
                    updateMarkerPositions(0.0);     // first reset to full scale
                    slider.minValue = editor.min;
                    slider.maxValue = editor.max;
                    chartArea.item.xmin = editor.min;
                    chartArea.item.xmax = editor.max;
//                    updateMarkerPositions(oldzoom);    // put slider in position of selected %
                    slider.paint();
                }

            }

        }
    }
    function updateMarkerPositions(fraction){
        editor.setStretchLimit(fraction); // calls editor.setMarkers(positions)
        editor.markersConfirmed();

        if (zoomonpreset.checked) {
            chartArea.item.xmin = editor.min;
            chartArea.item.xmax = editor.max;
            slider.minValue = editor.min;
            slider.maxValue = editor.max;
        }

        slider.paint();
    }
}

