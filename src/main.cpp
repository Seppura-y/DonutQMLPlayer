#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickWindow>


int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", QByteArray("opengl"));
    QGuiApplication app(argc, argv);

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
