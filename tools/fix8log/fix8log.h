#ifndef FIX8LOG_H
#define FIX8LOG_H
#include <QFutureWatcher>
#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include <QMap>
#include <QString>
#include <globals.h>
#include "mainwindow.h"
#include "tableschema.h"
class QStandardItemModel;
class Database;
class SchemaEditorDialog;
class FutureReadData;
class FixMimeData;

FutureReadData * readLogFileInThread(const QString &fileName,QString &errorStr);

class Fix8Log : public QObject
{
    Q_OBJECT
public:
    explicit Fix8Log(QObject *parent = 0);
    bool init();
    bool init(QString fileNameToLoad);
    void readFileInAnotherThread(const QString &fileName,QString &errorStr);
    void readSettings();
    void writeSettings();
    bool isGlobalSchemaOn();
public slots:
    void autoSaveOnSlot(bool);
    void cancelSessionRestoreSlot();
    void createNewWindowSlot(MainWindow *mw=0);
    void copyWindowSlot(MainWindow *mw);
    void deleteMainWindowSlot(MainWindow *mw);
    void displayConsoleMessage(GUI::Message);
    void displayConsoleMessage(QString, GUI::Message::MessageType = GUI::Message::InfoMsg);
    void editSchemaSlot(MainWindow *,QUuid workSheetID);
    void exitAppSlot();
     void finishedReadingDataFileSlot();
    void lastWindowClosedSlot();
    void modelDroppedSlot(FixMimeData *);
    void setGlobalSchemaOnSlot(bool);
    void setTimeFormatSlot(GUI::Globals::TimeFormat);
    void schemaEditorFinishedSlot(int);
protected:
    QStandardItemModel *readLogFile(const QString &fileName,QString &errorStr);
    void saveSession();
    void wireSignalAndSlots(MainWindow *mw);
    QList <MainWindow *> mainWindows;
    bool firstTimeToUse;
    Database *database;
    bool autoSaveOn;
    QMap <QString, QStandardItemModel *> fileNameModelMap;
    bool cancelSessionRestore;
    SchemaEditorDialog *schemaEditorDialog;
    TableSchemaList *tableSchemaList;
    TableSchema *defaultTableSchema;
    TableSchema *worldTableSchema;
    bool globalSchemaOn;

signals:
    void notifyTimeFormatChanged(GUI::Globals::TimeFormat);
};

#endif // FIX8LOG_H
