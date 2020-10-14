#include "ticsoutputpane.h"
#include "ticsqtcreatorplugin.h"
#include <coreplugin/editormanager/editormanager.h>

namespace TICSQtCreator {
namespace Internal {


TicsOutputPane::TicsOutputPane(QObject* parent): IOutputPane( parent )
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

void TicsOutputPane::writeText(QString text){
    qDebug() << "write text"<<endl;
    //matches violation trace file paths
    QRegularExpression re("^(.*)\\(([0-9]+)\\):$",QRegularExpression::MultilineOption);
    QRegularExpressionMatch match = re.match(text);

    while(match.hasMatch()){
        QString textBeforeLink = text.left(match.capturedStart(0));
        QString textAfterLink = text.mid(match.capturedEnd(0),text.size());
        QString fileLink = "<a href=\""+match.captured(1)+"#"+match.captured(2)+"\">"+match.captured(0)+"</a>";

        textEdit->append(textBeforeLink);
        textEdit->append(fileLink);
        qDebug()<< "Transforming fileLink in violations summary: " << match.captured(0) << endl;

        text = textAfterLink;
        match = re.match(text);
    }

    textEdit->append(text);
}

void TicsOutputPane::openFileLink(const QUrl & url)
{
    qInfo()<< "Opening file" << url.path() << "at line " << url.fragment();
    Core::EditorManager::openEditorAt(url.path(),url.fragment().toInt());
}

}
}
