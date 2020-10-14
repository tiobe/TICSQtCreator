#ifndef TICSQTCREATOR_H
#define TICSQTCREATOR_H

#include <extensionsystem/iplugin.h>
#include "ticsqtcreator_global.h"
#include "ticsqtcreatorconstants.h"
#include "ticsoutputpane.h"
#include <projectexplorer/compileoutputwindow.h>
#include <coreplugin/outputwindow.h>
#include <QProcess>
#include <QAction>
#include <QDockWidget>
#include <QTabWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QTemporaryFile>

using namespace ProjectExplorer::Internal;
namespace TICSQtCreator {
namespace Internal {

class TICSQtCreatorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TICSQtCreator.json")

public:
    TICSQtCreatorPlugin();
    ~TICSQtCreatorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

public slots:
    void handleReadyRead();
   // void openFileLink(const QUrl & url);
    void handleTicsProcessStarted();
    void handleTicsProcessStopped(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QAction * createAction(QIcon icon, QString text, bool visible, void (TICSQtCreatorPlugin::* slot) ());

    QAction * analyzeFileAction;
    QAction * analyzeProjectAction;
    QAction * ticsConfigAction;
    QAction * cancelTicsAction;
    QTemporaryFile * filesList = nullptr;

    void analyzeFile();
    void analyzeProject();
    void configureTICS();
    void cancelAnalysis();
    QProcess ticsProcess;
    //ProjectExplorer::Internal::CompileOutputWindow * ticsOutput = new ProjectExplorer::Internal::CompileOutputWindow(nullptr);;
    QDockWidget *dock;
    TicsOutputPane * ticsOutput;
    QTabWidget * tabWidget;
    QToolBar * toolBar;
};

} // namespace Internal
} // namespace TICSQtCreator

#endif // TICSQTCREATOR_H
