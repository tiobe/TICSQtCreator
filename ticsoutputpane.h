#ifndef TICSOUTPUTWINDOW_H
#define TICSOUTPUTWINDOW_H

#include "coreplugin/outputwindow.h"
#include "coreplugin/ioutputpane.h"
#include <QTextBrowser>
#include <QRegularExpression>

namespace TICSQtCreator {
namespace Internal {
class TicsOutputPane : public Core::IOutputPane
{
public:
    TicsOutputPane(QObject * parent);
    QWidget* outputWidget( QWidget* parent );
    QList<QWidget*> toolBarWidgets() const;
    QString displayName() const;
    int priorityInStatusBar() const;
    void clearContents();
    void visibilityChanged( bool visible );
    void setFocus();
    bool hasFocus() const;
    bool canFocus() const;
    bool canNavigate() const;
    bool canNext() const;
    bool canPrevious() const;
    void goToNext();
    void goToPrev();
    QTextBrowser *textEdit;
    void writeLine(QString line);
    void clear();
    void resetViolationDetection();
public slots:
    void openFileLink(const QUrl & url);

private:
    bool violationDetected;
    int lineNumber;
    QString filePath;
    QString description;
};
}
}

#endif // TICSOUTPUTWINDOW_H
