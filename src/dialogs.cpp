#include "dialogs.h"
#include <QTimer>

Dialogs* Dialogs::s_instance = nullptr;

Dialogs::Dialogs(QObject* parent) : QObject(parent)
{
    Q_ASSERT(s_instance == nullptr);
    s_instance = this;

    // Init QProcess for the console dialog
    m_process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Dialogs::consoleFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &Dialogs::consoleFinished);
    connect(&m_process, &QProcess::readyReadStandardOutput, [this]() {
        while (m_process.canReadLine())
        {
            m_consoleOutputs << QString::fromLocal8Bit(m_process.readLine());
        }
        emit consoleOutputsChanged();
        });
}

Dialogs::~Dialogs()
{
    s_instance = nullptr;
}


// Console dialog
void Dialogs::consoleDialog(const QString& title, const QString& program, const QStringList& args)
{
    if (m_process.state() == QProcess::Running)
    {
        return;
    }
    m_consoleOutputs.clear();
    emit consoleOutputsChanged();
    m_process.start(program, args, QIODevice::ReadOnly);
    emit consoleStarted(title);
}


// Selection dialog
void Dialogs::selectionDialog(const QString& title, const QStringList& items, std::function<void(int, bool)> callback,
    const QString& checkboxText)
{
    m_selectionCb = std::move(callback);
    // Avoid showing dialog during previous dialog is closing
    QTimer::singleShot(500, [=] {
        emit selectionStarted(title, items, checkboxText);
        });
}

void Dialogs::selectionCallback(int index, bool checked)
{
    m_selectionCb(index, checked);
}

// Text input dialog
void Dialogs::textInputDialog(const QString& title, std::function<void(const QString&)> callback, const QString& defaultValue)
{
    m_textInputCb = std::move(callback);
    emit textInputStarted(title, defaultValue);
}

void Dialogs::textInputCallback(const QString& text)
{
    m_textInputCb(text);
}
