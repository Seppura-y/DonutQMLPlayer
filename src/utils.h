#ifndef UTILS_H
#define UTILS_H

#include <QObject>
//#include <QUrl>
//#include <QtQml/qqmlregistration.h>
#include <QQmlEngine>

class Utils : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    // Update video parsers
    Q_INVOKABLE static void updateParser(void);

    // Get environment variable
    Q_INVOKABLE static QString environmentVariable(const QString& env);

    // Paths
    Q_INVOKABLE static QUrl homeLocation(void);
    Q_INVOKABLE static QUrl movieLocation(void);
    Q_INVOKABLE static QUrl musicLocation(void);
    Q_INVOKABLE static QUrl desktopLocation(void);
    Q_INVOKABLE static QUrl downloadLocation(void);
};

#endif  // UTILS_H