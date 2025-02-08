#include <QDebug>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>

//#include <QApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QFontDatabase>
#include <QScreen>
#include "graphicslayer.h"
#include "ui/widthmanager.h"
#include "ui/craftcolumn.h"
#include <QQuickStyle>

#include "enums.h"
#include "application.h"

int main(int argc, char** argv)
{
    //QSurfaceFormat format;
    //format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setVersion(4, 3);
    //format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //format.setSamples(4);
    //QSurfaceFormat::setDefaultFormat(format);

	QQuickStyle::setStyle("FluentWinUI3"); // Qt6 shit
	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    QGuiApplication app(argc, argv);

	QFontDatabase::addApplicationFont(":/HelveticaNeueCyr-Roman.ttf");
	QFontDatabase::addApplicationFont(":/HelveticaNeueCyr-Bold.ttf");

	QFont qtFont = app.font();
	qtFont.setFamily("HelveticaNeueCyr");
	qtFont.setPixelSize(14);
	app.setFont(qtFont);

	qmlRegisterType<GraphicsLayer>("GraphicsLayer", 1, 0, "GraphicsLayer");
	qmlRegisterType<WidthManager>("WidthManager", 1, 0, "WidthManager");
	qmlRegisterType<CraftColumn>("CraftColumn", 1, 0, "CraftColumn");
	//qmlRegisterType<MyRow>("MyRow", 1, 0, "MyRow");
	//qmlRegisterType<MyColumn>("MyColumn", 1, 0, "MyColumn");
	//qmlRegisterUncreatableType<GraphicsLayer>("StringArt.Enums", 1, 0, "SelectedImageType", "Error: enums only");
	//qmlRegisterUncreatableMetaObject(Enums::staticMetaObject, "Enums", 1, 0, "SelectedImageType", "Error: enums only");
	//qmlRegisterUncreatableMetaObject(Enums::staticMetaObject, "Enums", 1, 0, "DummyEnumType", "Error: enums only");

	QQuickView view;
	view.setMinimumSize(QSize(1024, 768));
	view.setResizeMode(QQuickView::SizeRootObjectToView);

	Application myApp;
	view.rootContext()->setContextProperty("app", &myApp);

	view.rootContext()->setContextProperty("view", &view);
	//view.rootContext()->setContextProperty("comboModel", &comboModel);

	view.setSource(QStringLiteral("qrc:/qml/main.qml"));

	//Qt::WindowFlags flags = view.flags();
	//flags |= Qt::FramelessWindowHint;
	//flags |= Qt::Window;
	//view.setFlags(flags);

	QScreen* screen = view.screen();
	QRect screenGeometry = screen->geometry();
	int x = (screenGeometry.width() - view.minimumWidth()) / 2;
	int y = (screenGeometry.height() - view.minimumHeight()) / 2;

	// Set the window position to the center of the screen
	view.setPosition(x, y);

	view.show();

	return app.exec();
}
