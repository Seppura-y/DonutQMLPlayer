#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickWindow>

#include "renderer.h"
#include "renderer_2d.h"


int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", QByteArray("opengl"));
    QGuiApplication app(argc, argv);


    auto format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
#ifdef DEBUG_GL    
    format.setOption(QSurfaceFormat::DebugContext);
#endif    
    QSurfaceFormat::setDefaultFormat(format);

    Donut::Renderer::init();
    Donut::Renderer2D::init();

    QQmlEngine engine;

    QQmlComponent component(&engine);

    qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", QByteArrayLiteral("Dense"));
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("Material"));

    component.loadUrl(QUrl(QStringLiteral("qrc:/main.qml")));
    if(component.isError())
    {
        qDebug() << "QQmlComponent error : " << component.errorString();
    }

    std::unique_ptr<QQuickWindow> winObject(static_cast<QQuickWindow*>(component.create()));
    if(component.isError())
    {
        qDebug() << "QQmlComponent error : " << component.errorString();
    }




    winObject->show();

    return app.exec();
}
