import QtQuick 2.15
import QtQuick.Controls 2.15
import GraphicsLayer 1.0
import "qrc:/"

Rectangle {
	id: root
	width: 640
	height: 480
	color: "red"

	Keys.onPressed:
	{
		graphicsLayer.onKeyPressed(event.key, event.modifiers)
	}

	Keys.onReleased:
	{
		graphicsLayer.onKeyReleased(event.key, event.modifiers)
	}

	GraphicsLayer {
		id: graphicsLayer
		anchors.fill: parent
		mirrorVertically: true
	} // GraphicsLayer

	MouseArea {
		anchors.fill: graphicsLayer
		hoverEnabled: true
		acceptedButtons: Qt.LeftButton | Qt.RightButton
		onPressed: graphicsLayer.onMousePressed(mouse.x, mouse.y, mouse.button)
		onReleased: graphicsLayer.onMouseReleased(mouse.x, mouse.x, mouse.button)
		onMouseXChanged: graphicsLayer.onMousePositionChanged(mouse.x, mouse.y)
		onMouseYChanged: graphicsLayer.onMousePositionChanged(mouse.x, mouse.y)

		onClicked: graphicsLayer.forceActiveFocus()

		onWheel: function(wheelEvent){
			graphicsLayer.onWheeled(wheelEvent)
		}
	} // MouseArea

	Text {
		text: "FPS: " + graphicsLayer.fpsCounter.fps
		color: "white"
		font.pixelSize: 24
	}
}
