#include "ticsoutputpane.h"
#include "ticsqtcreatorconstants.h"
#include "ticsqtcreatorplugin.h"
#include <coreplugin/editormanager/editormanager.h>
#include <projectexplorer/task.h>
#include <projectexplorer/taskhub.h>
#include <utils/fileutils.h>

namespace TICSQtCreator {
namespace Internal {


TicsOutputPane::TicsOutputPane(QObject* parent): IOutputPane( parent )
    ,violationDetected{false}
    ,lineNumber{0}
{
    this->textEdit = new QTextBrowser();
    textEdit->setOpenLinks(false);
    connect(textEdit, &QTextBrowser::anchorClicked,this,&TicsOutputPane::openFileLink);
}

QWidget* TicsOutputPane::outputWidget( QWidget* parent ){
    Q_UNUSED( parent );
    return this->textEdit;

}
QList<QWidget*> TicsOutputPane::toolBarWidgets() const{
    QList<QWidget*> widgets;
    return widgets;
}
QString TicsOutputPane::displayName() const{
    return "TICS Output";
}
int TicsOutputPane::priorityInStatusBar() const{
    return 1;
}
void TicsOutputPane::clearContents(){
    textEdit->clear();
    textEdit->document()->clear();
}
void TicsOutputPane::visibilityChanged( bool visible ){
    Q_UNUSED( visible );
}
void TicsOutputPane::setFocus(){

}
bool TicsOutputPane::hasFocus() const{
    return true;
}
bool TicsOutputPane::canFocus() const{
    return true;
}
bool TicsOutputPane::canNavigate() const{
    return true;
}
bool TicsOutputPane::canNext() const{
    return false;
}
bool TicsOutputPane::canPrevious() const{
    return false;
}
void TicsOutputPane::goToNext(){

}
void TicsOutputPane::goToPrev(){

}

void TicsOutputPane::writeLine(QString line){
    qDebug() << "write text"<<endl;
    if(violationDetected){
        //matches violation description line after violation trace file path
        QRegularExpression re("^ {4}.*$");
        QRegularExpressionMatch match = re.match(line);

        if(match.hasMatch()){
            description += line;
        }
        else{
            violationDetected = false;
            ProjectExplorer::TaskHub::addTask(ProjectExplorer::Task(ProjectExplorer::Task::Warning,description,Utils::FilePath::fromUserInput(filePath),lineNumber,TICSQtCreator::Constants::TICS_TASK_ID));
        }
    }
    else{
        //matches violation trace file paths
        QRegularExpression re("^(.*)\\(([0-9]+)\\):$");
        QRegularExpressionMatch match = re.match(line);

        if(match.hasMatch()){
            QString textBeforeLink = line.left(match.capturedStart(0));
            QString textAfterLink = line.mid(match.capturedEnd(0),line.size());
            QString fileLink = "<a href=\""+match.captured(1)+"#"+match.captured(2)+"\">"+match.captured(0)+"</a>";
            line = textBeforeLink + fileLink + textAfterLink;
            qDebug()<< "Transforming fileLink in violations summary: " << match.captured(0) << endl;

            violationDetected = true;
            filePath = match.captured(1);
            lineNumber = match.captured(2).toInt();
            description = "";
        }
    }
    //Remove trailing newline since append adds one
    textEdit->append(line.chopped(1));
}

void TicsOutputPane::resetViolationDetection()
{
    violationDetected = false;
}

void TicsOutputPane::openFileLink(const QUrl & url)
{
    qInfo()<< "Opening file" << url.path() << "at line " << url.fragment();
    Core::EditorManager::openEditorAt(url.path(),url.fragment().toInt());
}

}
}
