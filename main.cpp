#include <QDebug>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>

//#include <QApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QFontDatabase>
#include "graphicslayer.h"

#include "enums.h"


int main(int argc, char** argv)
{
    //QSurfaceFormat format;
    //format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setVersion(4, 3);
    //format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //format.setSamples(4);
    //QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);

	QFontDatabase::addApplicationFont(":/HelveticaNeueCyr-Roman.ttf");
	QFontDatabase::addApplicationFont(":/HelveticaNeueCyr-Bold.ttf");

	QFont qtFont = app.font();
	qtFont.setFamily("HelveticaNeueCyr");
	qtFont.setPixelSize(14);
	app.setFont(qtFont);

	qmlRegisterType<GraphicsLayer>("GraphicsLayer", 1, 0, "GraphicsLayer");
	//qmlRegisterType<MyRow>("MyRow", 1, 0, "MyRow");
	//qmlRegisterType<MyColumn>("MyColumn", 1, 0, "MyColumn");
	//qmlRegisterUncreatableType<GraphicsLayer>("StringArt.Enums", 1, 0, "SelectedImageType", "Error: enums only");
	//qmlRegisterUncreatableMetaObject(Enums::staticMetaObject, "Enums", 1, 0, "SelectedImageType", "Error: enums only");
	//qmlRegisterUncreatableMetaObject(Enums::staticMetaObject, "Enums", 1, 0, "DummyEnumType", "Error: enums only");

	QQuickView view;
	view.setMinimumSize(QSize(1024, 800));
	view.setResizeMode(QQuickView::SizeRootObjectToView);

	view.rootContext()->setContextProperty("view", &view);

	view.setSource(QStringLiteral("qrc:/qml/main.qml"));

	//Qt::WindowFlags flags = view.flags();
	//flags |= Qt::FramelessWindowHint;
	//flags |= Qt::Window;
	//view.setFlags(flags);

	view.show();

	return app.exec();
}
