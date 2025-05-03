#include "ticsqtcreatorplugin.h"
#include "ticsqtcreatorconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editorview.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/outputwindow.h>
#include <coreplugin/idocument.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/modemanager.h>

#include <projectexplorer/project.h>
#include <projectexplorer/session.h>
#include <projectexplorer/compileoutputwindow.h>
#include <projectexplorer/buildstep.h>
#include <projectexplorer/taskhub.h>


#include <QProcess>
#include <QProcessEnvironment>
#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QDir>
#include <QCoreApplication>
#include <QToolTip>
#include "ticsoutputpane.h"

namespace TICSQtCreator {
namespace Internal {

static QString TICS_ENV = "";
static QString TICSCONFIG = "TICSConfig";
static const QString TICS_COMMAND = "TICS";
static const QString IDE_PARAMETER = "-ide";
static const QString IDE_NAME = "qtcreator";


TICSQtCreatorPlugin::TICSQtCreatorPlugin()
{
    TICS_ENV = QProcessEnvironment::systemEnvironment().value("TICS");

}

TICSQtCreatorPlugin::~TICSQtCreatorPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool TICSQtCreatorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    const Core::Context editorManagerContext =  Core::Context(Core::Constants::C_EDITORMANAGER);

    analyzeFileAction = createAction(QIcon(":/icons/TI_analyze_file"),tr("Analyze Current File"),true, &TICSQtCreatorPlugin::analyzeFile);
    Core::Command *analyzeFileCmd = Core::ActionManager::registerAction(analyzeFileAction, Constants::ANALYZE_FILE_ACTION_ID, editorManagerContext);

    analyzeProjectAction = createAction(QIcon(":/icons/TI_analyze_project.png"),tr("Analyze Current Project"),true, &TICSQtCreatorPlugin::analyzeProject);
    Core::Command *analyzeProjectCmd = Core::ActionManager::registerAction(analyzeProjectAction, Constants::ANALYZE_PROJECT_ACTION_ID,editorManagerContext);

    cancelTicsAction = createAction(QIcon(":/icons/TI_cancel_analysis.png"),tr("Cancel TICS Analysis"),true, &TICSQtCreatorPlugin::cancelAnalysis);
    Core::Command *cancelTicsCmd = Core::ActionManager::registerAction(cancelTicsAction, Constants::CANCEL_ANALYSIS_ACTION_ID, editorManagerContext);

    ticsConfigAction = createAction(QIcon(":/icons/TI_rulesoptions_configuration.gif"), tr("Configure TICS Client"),true, &TICSQtCreatorPlugin::configureTICS);
    Core::Command *ticsConfigCmd = Core::ActionManager::registerAction(ticsConfigAction, Constants::TICS_CONFIG_ACTION_ID, editorManagerContext);

    cancelTicsAction->setDisabled(true);

    connect(&ticsProcess,  &QProcess::readyRead, this, &TICSQtCreatorPlugin::handleReadyRead);
    connect(&ticsProcess, &QProcess::started, this, &TICSQtCreatorPlugin::handleTicsProcessStarted);

    connect(&ticsProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleTicsProcessStopped(int, QProcess::ExitStatus)));

    ticsOutput = new TicsOutputPane(this);

    Core::ActionContainer* ticsMenu = Core::ActionManager::createMenu(Constants::TICS_MENU_TITLE);
    ticsMenu->menu()->setTitle("TICS");
    ticsMenu->addAction(analyzeFileCmd);
    ticsMenu->addAction(analyzeProjectCmd);
    ticsMenu->addAction(cancelTicsCmd);
    ticsMenu->addAction(ticsConfigCmd);

    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addMenu(Core::ActionManager::actionContainer(Core::Constants::M_HELP),ticsMenu);

    //apply background to menu bar items to avoid transparent icon on hover
    QMenuBar* mainMenuBar = Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->menuBar();
    mainMenuBar->setStyleSheet("QMenuBar::item:selected {background: #888888} QMenuBar::item:pressed {background: #888888}");

    // add TICS actions to menu bar
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->appendGroup(Constants::TICS_MENU_ID);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addAction(analyzeFileCmd,Constants::TICS_MENU_ID);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addAction(analyzeProjectCmd,Constants::TICS_MENU_ID);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addAction(cancelTicsCmd,Constants::TICS_MENU_ID);
    Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->addAction(ticsConfigCmd,Constants::TICS_MENU_ID);

    if(TICS_ENV == nullptr || TICS_ENV==""){
        analyzeFileAction->setEnabled(false);
        analyzeProjectAction->setEnabled(false);
        ticsConfigAction->setEnabled(false);
        cancelTicsAction->setEnabled(false);
        QMessageBox::warning(Core::ICore::mainWindow(), tr("TICS not Configured"),
                             tr("TICS Environment Variable not set. TICS plugin actions will be disabled."));
    }

    ProjectExplorer::TaskHub::addCategory(Constants::TICS_TASK_ID, "TiCS Tasks");

    return true;
}

void TICSQtCreatorPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag TICSQtCreatorPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

QAction * TICSQtCreatorPlugin::createAction(QIcon icon, QString text, bool visible, void (TICSQtCreatorPlugin::* slot) ()){
    QAction * action = new QAction(this);

    action->setIcon(icon);
    action->setText(text);
    action->setToolTip(text);
    action->setIconVisibleInMenu(visible);
    connect(action, &QAction::triggered, this, slot);

    return action;
}

void TICSQtCreatorPlugin::analyzeFile(){

    QStringList arguments;


    Core::IDocument *document = Core::EditorManager::currentDocument();
    if (document == NULL) {
        QMessageBox::information(Core::ICore::mainWindow(),
                                 tr("Analyze File"),
                                 tr("No open file found!"));
        return;
    }
    arguments << IDE_PARAMETER << IDE_NAME;
    arguments << document->filePath().toString();
    ProjectExplorer::TaskHub::clearTasks(Constants::TICS_TASK_ID);
    ticsOutput->resetViolationDetection();
    ticsOutput->clearContents();
    ticsOutput->popup(Core::IOutputPane::ModeSwitch);
    ticsProcess.start(TICS_COMMAND, arguments); // start returns immediately

}

void TICSQtCreatorPlugin::analyzeProject(){

    ProjectExplorer::Project * project =  ProjectExplorer::SessionManager::startupProject();
    if (project == NULL) {
        QMessageBox::information(Core::ICore::mainWindow(),
                                 tr("Analyze Project"),
                                 tr("No open project found!"));
        return;
    }
    QStringList arguments;
    arguments << IDE_PARAMETER << IDE_NAME;
    filesList = new QTemporaryFile();
    //disable autoremove to retain file until TICS process exits
    filesList->setAutoRemove(false);

    //write files list to temp file and pass to TICS with @fileName argument
    if(filesList->open()){
        QString fileName = filesList->fileName();
        QTextStream out(filesList);
        foreach (Utils::FilePath srcFilePath , project->files(ProjectExplorer::Project::SourceFiles)){
            out << srcFilePath.toString() << '\n';
        }
        out.flush();
        arguments.append('@'+fileName);
    }

    /* project file can be provided as argument instead when qmake integration for TICS is available
     *
     * projfile for Qt Projects => .pro file
     * projfile for CMake Projects => CMakeLists.txt
     * projfile for QtPython Projects => .pyprojectfile
     *
     * this can be extracted as:
     *
     *
     * Utils::FilePath projFile = project->projectFilePath();
     * if(!projFile.isEmpty()){
     *      arguments << " -projfile " << project->projectFilePath().toString();
     * }
     *
     */

    ProjectExplorer::TaskHub::clearTasks(Constants::TICS_TASK_ID);
    ticsOutput->resetViolationDetection();
    ticsOutput->clearContents();
    ticsOutput->popup(Core::IOutputPane::ModeSwitch);
    ticsProcess.start(TICS_COMMAND, arguments); // start returns immediately

}

void TICSQtCreatorPlugin::handleReadyRead()
{
    while(ticsProcess.canReadLine()){
        QByteArray p_stdout = ticsProcess.readLine();
        QString s_stdout= QString::fromUtf8(p_stdout);
        ticsOutput->writeLine(s_stdout);
    }
}

void TICSQtCreatorPlugin::handleTicsProcessStarted()
{
    analyzeFileAction->setEnabled(false);
    analyzeProjectAction->setEnabled(false);
    cancelTicsAction->setEnabled(true);
}

void TICSQtCreatorPlugin::handleTicsProcessStopped(int exitCode, QProcess::ExitStatus exitStatus )
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    analyzeFileAction->setEnabled(true);
    analyzeProjectAction->setEnabled(true);
    cancelTicsAction->setEnabled(false);
    filesList = nullptr;
}

void TICSQtCreatorPlugin::configureTICS(){
    ticsOutput->popup(Core::IOutputPane::ModeSwitch);
    ticsProcess.start(TICSCONFIG); // start returns immediately

}

void TICSQtCreatorPlugin::cancelAnalysis(){
    //Check if tics process is running.
    if(ticsProcess.processId()!=0){
        ticsOutput->writeLine("Canceling Analysis...");
        ticsProcess.kill();
        ticsOutput->writeLine("Analysis cancelled!");
    }
    return;
}

} // namespace Internal
} // namespace TICSQtCreator
