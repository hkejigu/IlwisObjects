import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../../../../Global.js" as Global
import "../../../../controls" as Controls


Column {
    id : colorcomp
    y : 4
	width : 200
	height : 340
	property var classification

	spacing : 2

	Component {
        id : spectral
        SetMultiSpectralData{}
    }

    Component {
        id : domain
        ConstructClasses{}
    }

	TabView {
	        id : data
            height: parent.height
            width : parent.width


			Component.onCompleted: {
				addTab(qsTr("Multi Spectral Data"), spectral).active = true
				addTab(qsTr("Construct Classes"), domain).active = true
				classification = modellerDataPane.model.analysisModel(0)
			}
	}

	function selectedBands(red,green,blue){
		 var options = red + ',' + green + ',' + blue
		 if ( analisysView && analisysView.view()){
			analisysView.view().setNewBands(options)
		}
	}
}

