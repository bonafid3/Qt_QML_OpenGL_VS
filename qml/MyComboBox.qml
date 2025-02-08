import QtQuick 2.15
import QtQuick.Controls 2.15

import WidthManager 1.0
import CraftColumn 1.0

ComboBox {
	id: combo

	background: Rectangle {
		color: "#373737"
		radius: 8
		border.color: "black"
		border.width: 1
	}

/*
	WidthManager {
		id: widthManager
	}
		
	implicitWidth: widthManager.maxControlWidth + indicator.width 
	implicitHeight: contentItem.implicitHeight

	onCurrentIndexChanged: {
		console.log("combo current index: " + currentIndex)
	}
		
	contentItem: Item {
		implicitWidth: widthManager.maxControlWidth + 14
		implicitHeight: Math.max(text.implicitHeight, indicator.height)
		Text {
			id: text
			anchors.centerIn: parent
			text: combo.displayText
			font.pixelSize: root.fontPixelSize
		}
	}
		
	indicator: Rectangle {
		id: indicator
		anchors.right: parent.right
		width: 30
		height: 30
		color: "transparent"
		border.width: 1
	}

	background: Rectangle {
		color: "white"
		implicitHeight: 30
	}

	delegate: Rectangle {
		width: parent.width
		height: 30
		color: combo.currentIndex == index ? "red" : "green"
		border.width: 1
		Text {
			id: comboDelegateText
			anchors.left: parent.left
			anchors.leftMargin: combo.currentIndex != index ? (mouseArea.containsMouse ? 14 : 7) : 14
			anchors.verticalCenter: parent.verticalCenter
			text: combo.model.data(combo.model.index(index, 0), 0)
			font.pixelSize: root.fontPixelSize

			Component.onCompleted: { 
				widthManager.registerControl(this)
			}
			Component.onDestruction: { widthManager.unregisterControl(this) }

			Behavior on anchors.leftMargin {
				NumberAnimation {
					duration: 100
				}
			}

		}

		MouseArea {
			id: mouseArea
			hoverEnabled: true
			anchors.fill: parent
			onClicked: {
				combo.currentIndex = index
			}
		}


	} // delegate


	popup: Popup {
		
		x: combo.width - width
		y: combo.height
		width: column.implicitWidth
		height: column.implicitHeight

		padding: 0
		
		CraftColumn {
			id: column
			Repeater {
				model: combo.model
				Rectangle {
					implicitWidth: 100
					implicitHeight: 30
					color: "red"
					border.width: 1
				}
			}
		}

	}
*/

}// ComboBox
