//-------------------------------------------------------------------------------------------------
/*
Fix8logviewer is released under the GNU LESSER GENERAL PUBLIC LICENSE Version 3.

Fix8logviewer Open Source FIX Log Viewer.
Copyright (C) 2010-14 David N Boosalis dboosalis@fix8.org, David L. Dight <fix@fix8.org>

Fix8logviewer is free software: you can  redistribute it and / or modify  it under the  terms of the
GNU Lesser General  Public License as  published  by the Free  Software Foundation,  either
version 3 of the License, or (at your option) any later version.

Fix8logviewer is distributed in the hope  that it will be useful, but WITHOUT ANY WARRANTY;  without
even the  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

You should  have received a copy of the GNU Lesser General Public  License along with Fix8.
If not, see <http://www.gnu.org/licenses/>.

BECAUSE THE PROGRAM IS  LICENSED FREE OF  CHARGE, THERE IS NO  WARRANTY FOR THE PROGRAM, TO
THE EXTENT  PERMITTED  BY  APPLICABLE  LAW.  EXCEPT WHEN  OTHERWISE  STATED IN  WRITING THE
COPYRIGHT HOLDERS AND/OR OTHER PARTIES  PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY
KIND,  EITHER EXPRESSED   OR   IMPLIED,  INCLUDING,  BUT   NOT  LIMITED   TO,  THE  IMPLIED
WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO
THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE,
YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

IN NO EVENT UNLESS REQUIRED  BY APPLICABLE LAW  OR AGREED TO IN  WRITING WILL ANY COPYRIGHT
HOLDER, OR  ANY OTHER PARTY  WHO MAY MODIFY  AND/OR REDISTRIBUTE  THE PROGRAM AS  PERMITTED
ABOVE,  BE  LIABLE  TO  YOU  FOR  DAMAGES,  INCLUDING  ANY  GENERAL, SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT
NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR
THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH
HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

*/
//-------------------------------------------------------------------------------------------------
#include <iostream>

#include <fix8/f8includes.hpp>
#include "fix8/field.hpp"
#include "fix8/message.hpp"
#include <Myfix_types.hpp>
#include <Myfix_router.hpp>
#include <Myfix_classes.hpp>

#include "intItem.h"
#include "messagearea.h"
#include "messagefield.h"
#include <QtWidgets>

MessageArea::MessageArea(QWidget *parent) :
    QWidget(parent),currentMessage(0)
{
    stackLayout = new QStackedLayout();
    setLayout(stackLayout);
    model = new QStandardItemModel(this);
    model->setColumnCount(2);
    QStringList headerLabels;
    //headerLabels << "Field" <<  "Name" << "Value";
    //model->setHorizontalHeaderLabels(headerLabels);
    QWidget *workArea = new QWidget(this);
    QVBoxLayout *wBox = new QVBoxLayout();
    workArea->setLayout(wBox);
    treeView = new QTreeView(workArea);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView->setModel(model);
    treeView->setUniformRowHeights(true);
    treeView->setAlternatingRowColors(true);
    treeHeaderItem  = new QStandardItem("Field");
    valueHeaderItem  = new QStandardItem("Value");
    headerItem = new QStandardItem("Header");
    fieldItem  = new QStandardItem("Fields");
    groupItem  = new QStandardItem("Groups");
    tailItem   = new QStandardItem("Trailer");
    model->setHorizontalHeaderItem(0,treeHeaderItem);
    model->setHorizontalHeaderItem(1,valueHeaderItem);
    model->appendRow(headerItem);
    model->appendRow(fieldItem);
    model->appendRow(groupItem);
    model->appendRow(tailItem);
    //treeView->setSortingEnabled(true);

    infoArea = new QWidget(workArea);
    QFont fnt = infoArea->font();
    fnt.setBold(true);
    fnt.setPointSize(fnt.pointSize()+2);
    infoArea->setFont(fnt);
    QPalette pal = infoArea->palette();
    QColor fgColor = pal.color(QPalette::WindowText);
    QColor bgColor = pal.color(QPalette::Window);
    pal.setColor(QPalette::WindowText,bgColor);
    pal.setColor(QPalette::Window,fgColor);
    infoArea->setAutoFillBackground(true);
    infoArea->setPalette(pal);
    QFormLayout *infoForm = new QFormLayout();
    infoArea->setLayout(infoForm);
    seqNumV = new QLabel(infoArea);
    seqNumV->setAlignment(Qt::AlignCenter);
    messageTypeV = new QLabel(infoArea);
    messageTypeV->setAlignment(Qt::AlignCenter);
    infoForm->addRow("Seq Num",seqNumV);
    infoForm->addRow("Mesg Type",messageTypeV);
    wBox->addWidget(treeView,1);
    wBox->addWidget(infoArea,0,Qt::AlignBottom);
    stackLayout->insertWidget(0,workArea);
}
void MessageArea::setMessage(QMessage *m)
{
    char c[60];
    BaseField *bf;
    MessageBase *header;
    MessageBase *trailer;
    GroupBase  *groupBase;
    FieldTrait::FieldType ft;
    currentMessage = m;
    QString str;
    QString name;
    QString messageType;
    bool headerExpanded = treeView->isExpanded(headerItem->index());
    bool fieldExpanded  = treeView->isExpanded(fieldItem->index());
     bool groupExpanded  = treeView->isExpanded(groupItem->index());
    bool tailExpanded = treeView->isExpanded(tailItem->index());
    setUpdatesEnabled(false);

    headerItem->removeRows(0,headerItem->rowCount());

    fieldItem->removeRows(0,fieldItem->rowCount());
    groupItem->removeRows(0,groupItem->rowCount());
    tailItem->removeRows(0,tailItem->rowCount());
    if (currentMessage) {
        seqNumV->setText(QString::number(currentMessage->seqID));
        Message *msg = currentMessage->mesg;
        header = msg->Header();
        trailer = msg->Trailer();
        const Presence& pre(msg->get_fp().get_presence());
        messageType =  QString::fromStdString(msg->get_msgtype());
        const char *dum = msg->get_msgtype().c_str();
        BaseMsgEntry bme = TEX::ctx()._bme.find_ref(dum);
        messageTypeV->setText(bme._name);
        for (Fields::const_iterator itr(header->fields_begin()); itr != header->fields_end(); ++itr)
        {
            const FieldTrait::FieldType trait(pre.find(itr->first)->_ftype);
            name = QString::fromStdString(TEX::ctx().find_be(itr->first)->_name);
            bf = itr->second;
            memset(c,'\0',60);
            bf->print(c);
            str  = QString::fromLatin1(c);
            QStandardItem *messageItem =
                    new QStandardItem(name);
            QStandardItem *valueItem =
                    new QStandardItem(str);
            QList <QStandardItem *>items;
            items.append(messageItem);
            items.append(valueItem);
            headerItem->appendRow(items);

        }

        for (Fields::const_iterator itr(msg->fields_begin()); itr != msg->fields_end(); ++itr)
        {

            const FieldTrait::FieldType trait(pre.find(itr->first)->_ftype);
            name = QString::fromStdString(TEX::ctx().find_be(itr->first)->_name);
            bf = itr->second;
            memset(c,'\0',60);
            bf->print(c);
            str = QString::fromLatin1(c);
            QStandardItem *messageItem =
                    new QStandardItem(name);
            QStandardItem *valueItem =
                    new QStandardItem(str);
            QList <QStandardItem *>items;
            items.append(messageItem);
            items.append(valueItem);
            fieldItem->appendRow(items);

        }
        Groups groups = msg->get_groups();
        typedef std::map<unsigned short, class GroupBase *>::iterator it_type;
        for(it_type iterator = groups.begin(); iterator != groups.end(); iterator++) {
            GroupBase *gb = iterator->second;
            int  gbSize = gb->size();
            qDebug() << "\tGroup Base Size = " << gbSize;
            for(int k=0;k<gbSize;k++) {
                MessageBase *mb = gb->get_element(k);

                 for (Fields::const_iterator itr(mb->fields_begin());itr != mb->fields_end(); ++itr) {
                     const FieldTrait::FieldType trait(pre.find(itr->first)->_ftype);
                     name = QString::fromStdString(TEX::ctx().find_be(itr->first)->_name);
                     bf = itr->second;
                     memset(c,'\0',60);
                     bf->print(c);
                     str = QString::fromLatin1(c);
                     QStandardItem *messageItem =
                             new QStandardItem(name);
                     QStandardItem *valueItem =
                             new QStandardItem(str);
                     QList <QStandardItem *>items;
                     items.append(messageItem);
                     items.append(valueItem);
                     groupItem->appendRow(items);
                     qDebug() << "\t\tField Name = " << name << __FILE__ << __LINE__;
                 }
            }
        }
        qDebug() << "GROUPS COUNT = " << groups.size() << __FILE__ << __LINE__;
         //for (Fields::const_iterator itr(msg->get_groups()
        for (Fields::const_iterator itr(trailer->fields_begin()); itr != trailer->fields_end(); ++itr)
        {treeView->isExpanded(headerItem->index());
            const FieldTrait::FieldType trait(pre.find(itr->first)->_ftype);
            name = QString::fromStdString(TEX::ctx().find_be(itr->first)->_name);
            bf = itr->second;
            memset(c,'\0',60);
            bf->print(c);
            str =  QString::fromLatin1(c);
            QStandardItem *messageItem =
                    new QStandardItem(name);
            QStandardItem *valueItem =
                    new QStandardItem(str);
            QList <QStandardItem *>items;
            items.append(messageItem);
            items.append(valueItem);
            tailItem->appendRow(items);
        }
        treeView->setExpanded(headerItem->index(),headerExpanded);
        treeView->setExpanded(fieldItem->index(),fieldExpanded);
           treeView->setExpanded(groupItem->index(),groupExpanded);
        treeView->setExpanded(tailItem->index(),tailExpanded);
        setUpdatesEnabled(true);
    }
    else {
        seqNumV->setText("");
    }
}
void MessageArea::setItemExpaned(TreeItem ti,bool expanded)
{
    switch (ti) {
    case HeaderItem:
        treeView->setExpanded(headerItem->index(),expanded);
        break;
    case FieldsItem:
        treeView->setExpanded(fieldItem->index(),expanded);
        break;
    case TrailerItem:
        treeView->setExpanded(tailItem->index(),expanded);
        break;
    default:
         qWarning() << "Unknown expansion item" << __FILE__ << __LINE__;
        break;
    }

}
bool MessageArea::getExpansionState(TreeItem ti)
{
    bool isExpanded = false;
    switch(ti) {
    case HeaderItem:
        isExpanded = treeView->isExpanded(headerItem->index());
        break;
    case FieldsItem:
        isExpanded = treeView->isExpanded(fieldItem->index());
        break;
    case TrailerItem:
        isExpanded = treeView->isExpanded(tailItem->index());
        break;
    default:
        qWarning() << "Unknown request typefor expansion state" << __FILE__ << __LINE__;

    }
    return isExpanded;
}
