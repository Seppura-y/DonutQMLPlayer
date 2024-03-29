#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickWindow>


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQmlEngine engine;

    QQmlComponent component(&engine);

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
    winObject->setTitle("Component setData");

    return app.exec();
}
