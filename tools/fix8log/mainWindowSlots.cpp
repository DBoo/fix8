#include "mainwindow.h"
#include "messagearea.h"
#include "globals.h"
#include "worksheet.h"
#include <QDebug>
#include "globals.h"
#include <QtWidgets>
//#include <QStandardItemModel>

void MainWindow::quitSlot()
{
    writeSettings();
    emit exitApp();
    //qApp->quit();
}
void MainWindow::hideColumnActionSlot(QAction *action)
{
    WorkSheet *workSheet;
    if (!action)
        return;
    QVariant var =  action->data();
    qint32 colNum = var.toInt();

    for(int i =0;i< tabW->count();i++) {
        workSheet = qobject_cast <WorkSheet *> (tabW->widget(i));
        if (workSheet)
            workSheet->hideColumn(colNum,action->isChecked());
    }
}
void MainWindow::iconStyleSlot(QAction *action)
{
    QSettings settings("fix8","logviewer");
    Qt::ToolButtonStyle  tbs = Qt::ToolButtonTextOnly;
    tbs = Qt::ToolButtonTextOnly;
    if (action == iconsOnlyA)
        tbs = Qt::ToolButtonIconOnly;
    else if (action == iconsWithTextA)
        tbs = Qt::ToolButtonTextUnderIcon;
    setToolButtonStyle(tbs);
    settings.setValue("ToolButtonStyle",tbs);
}
void MainWindow::iconSizeSlot(QAction *action)
{
    QSettings settings("fix8","logviewer");
    QSize is = GUI::Globals::regIconSize;
    if (action == iconSizeSmallA)
        is = GUI::Globals::smallIconSize;
    else if (action == iconSizeLargeA)
        is  = GUI::Globals::largeIconSize;
    mainToolBar->setIconSize(is);
    searchToolBar->setIconSize(is);
    settings.setValue("ToolButtonSize",is);
}
void MainWindow::configSlot()
{
    int status;
    QSettings settings("fix8","logviewer");
    QPalette pal = mainMenuBar->palette();
    menubarColor = pal.color(QPalette::Background);
    QColorDialog *colorDialog = new QColorDialog();
    colorDialog->setWindowTitle(tr("Select Menu Bar Color"));
    colorDialog->setCurrentColor(menubarColor);
    colorDialog->setCustomColor(0,GUI::Globals::menubarDefaultColor);
    connect(colorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(setColorSlot(QColor)));
    connect(colorDialog,SIGNAL(currentColorChanged(QColor)),this,SLOT(currentColorChangedSlot(QColor)));
    status = colorDialog->exec();
    if (status != QDialog::Accepted) {
        pal.setColor(QPalette::Background,menubarColor);
        mainMenuBar->setPalette(pal);
    }
    else {
        settings.setValue("MenuBarColor",menubarColor);
    }

    colorDialog->deleteLater();
}
void MainWindow::setColorSlot(QColor color)
{
    QPalette pal = mainMenuBar->palette();
    pal.setColor(QPalette::Background,color);
    //pal.setColor(QPalette::Base,color);
    mainMenuBar->setPalette(pal);
    menubarColor = color;
}
void MainWindow::currentColorChangedSlot(QColor color)
{
    QPalette pal = mainMenuBar->palette();
    pal.setColor(QPalette::Background,color);
    mainMenuBar->setPalette(pal);
}
void MainWindow::editTabNameSlot(bool isOn)
{
    QString tabName;
    int index = tabW->currentIndex();
    if (isOn) {
        tabName =  tabW->tabText(index);
        cancelEditTabNamePB->show();
        tabNameLineEdit->show();
        tabNameLineEdit->setFocus();
        editTabNamePB->setToolTip("Save");
        tabNameLineEdit->setText(tabName);
        tabNameLineEdit->selectAll();
    }
    else  {
        cancelEditTabNamePB->hide();
        tabNameLineEdit->hide();
        editTabNamePB->setToolTip("Edit current tab name");
        tabName = tabNameLineEdit->text();
        tabW->setTabText(index,tabName);
    }
}
void MainWindow::cancelTabNameSlot()
{
    cancelEditTabNamePB->hide();
    tabNameLineEdit->hide();
    editTabNamePB->setToolTip("Edit current tab name");
    editTabNamePB->setChecked(false);
    editTabNamePB->setEnabled(true);
}
void MainWindow::tabNameModifiedSlot(QString str)
{
    bool isValid = false;
    if (str.length() >0)
        isValid = true;
    editTabNamePB->setEnabled(isValid);
    if(isValid)
        tabNameLineEdit->setToolTip(tr("Enter return to change name and exit edit mode"));
    else
         tabNameLineEdit->setToolTip(tr("Enter tab name"));
}
void MainWindow::tabNameReturnKeySlot()
{
    QString tabName;
    WorkSheet *worksheet;
    int index = tabW->currentIndex();
    tabName = tabNameLineEdit->text();
    if (tabName.length() < 1)
        return;
    cancelEditTabNamePB->hide();
    tabNameLineEdit->hide();
    editTabNamePB->setChecked(false);
    editTabNamePB->setToolTip("Edit current tab name");
    tabW->setTabText(index,tabName);
    worksheet = qobject_cast <WorkSheet *> (tabW->widget(index));
    worksheet->setAlias(tabName);
}
void MainWindow::tabCurentChangedSlot(int)
{
    if (!editTabNamePB->isChecked())
        return;
    cancelTabNameSlot();
}
void MainWindow::showMessageArea(bool bstatus)
{
    WorkSheet *workSheet;
    for(int i=0;i < tabW->count();i++) {
        workSheet = qobject_cast <WorkSheet *> (tabW->widget(i));
        if(bstatus)
            workSheet->messageArea->hide();
        else
            workSheet->messageArea->show();
    }
}
void MainWindow::autoSaveOnSlot(bool isOn)
{
    emit autoSaveOn(isOn);
}
void MainWindow::cancelSessionRestoreSlot()
{
    qDebug()  << "Cancel Session Restore Slot " << __FILE__ << __LINE__;
    emit cancelSessionRestore();
}
void MainWindow::setTimeSlotFromWorkSheet(GUI::Globals::TimeFormat tf)
{
    emit notifyTimeFormatChanged(tf);
}
void MainWindow::setTimeFormatSlot(GUI::Globals::TimeFormat tf)
{
   qDebug() << "MainWindow::setTimeFormat Slot " << tf << __FILE__ << __LINE__;
   WorkSheet *workSheet;
   for(int i=0;i < tabW->count();i++) {
       workSheet = qobject_cast <WorkSheet *> (tabW->widget(i));
       if (workSheet)
           workSheet->setTimeFormat(tf);
   }
}
