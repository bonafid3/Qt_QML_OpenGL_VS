import QtQuick
import QtQuick.Controls
import GraphicsLayer 1.0

import "qrc:/qml"

Rectangle {
	id: root
	width: 640
	height: 480
	color: "red"

	property int fontPixelSize: 20

	GraphicsLayer {
		id: graphicsLayer
		anchors.fill: parent
	} // GraphicsLayer

	// input handling
	Keys.onPressed: function(event) { graphicsLayer.onKeyPressed(event.key) }
	Keys.onReleased: function(event) { graphicsLayer.onKeyReleased(event.key) }

	MouseArea {
		anchors.fill: graphicsLayer
		hoverEnabled: true
		acceptedButtons: Qt.LeftButton | Qt.RightButton
		onPressed: function(mouseEvent) { graphicsLayer.onMousePressed(mouseEvent.x, mouseEvent.y, mouseEvent.button) }
		onReleased: function(mouseEvent) { graphicsLayer.onMouseReleased(mouseEvent.x, mouseEvent.y, mouseEvent.button) }
		onMouseXChanged: function(mouseEvent) { graphicsLayer.onMousePositionChanged(mouseEvent.x, mouseEvent.y) }
		onMouseYChanged: function(mouseEvent) { graphicsLayer.onMousePositionChanged(mouseEvent.x, mouseEvent.y) }

		onClicked: { graphicsLayer.forceActiveFocus() }

		onWheel: function(wheelEvent) { graphicsLayer.onWheeled(wheelEvent) }
	} // MouseArea

	MyComboBox {
		anchors.top: parent.top
		anchors.topMargin: 10
		anchors.right: parent.right
		anchors.rightMargin: 10
		model: app.comboModel
		textRole: "textRole"
		focus: false
		onCurrentIndexChanged: {
			app.comboModel.setCurrentIndex(currentIndex)
			graphicsLayer.forceActiveFocus()
		}
	}

	Label {
		text: "FPS: " + graphicsLayer.fpsCounter.fps
		color: "white"
		font.pixelSize: root.fontPixelSize
	}
}
