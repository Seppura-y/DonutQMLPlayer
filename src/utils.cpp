//#include "utils.h"
//#include <QCoreApplication>
//#include <QFile>
//#include <QNetworkRequest>
//#include <QNetworkReply>
//#include <QRegularExpression>
//#include <QStandardPaths>
//
////#include "accessManager.h"
//#include "dialogs.h"
//
//
//void Utils::updateParser()
//{
//    Q_ASSERT(Dialogs::instance() != nullptr);
//    QStringList args;
//#ifdef Q_OS_WIN
//    args << QStringLiteral("-ExecutionPolicy") << QStringLiteral("RemoteSigned");
//    args << QStringLiteral("-File") << (QCoreApplication::applicationDirPath() + QStringLiteral("/update-parsers.ps1"));
//    Dialogs::instance()->consoleDialog(tr("Update plugins"), QStringLiteral("powershell"), args);
//#else
//    static QString shell;
//    if (shell.isNull())
//    {
//        QFile f(QStringLiteral(":/scripts/update-parsers.sh"));
//        f.open(QFile::ReadOnly);
//        shell = QString::fromLatin1(f.readAll());
//    }
//    args << QStringLiteral("-c") << shell;
//    Dialogs::instance()->consoleDialog(tr("Update plugins"), QStringLiteral("sh"), args);
//#endif
//}
//
//
//QString Utils::environmentVariable(const QString& env)
//{
//    return QString::fromUtf8(qgetenv(env.toUtf8().constData()));
//}
//
//
//// Locations
//
//QUrl Utils::desktopLocation()
//{
//    return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
//}
//
//QUrl Utils::downloadLocation()
//{
//    return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
//}
//
//QUrl Utils::homeLocation()
//{
//    return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
//}
//
//QUrl Utils::movieLocation()
//{
//    return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
//}
//
//QUrl Utils::musicLocation()
//{
//    return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
//}