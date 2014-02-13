/***********************************************************************
*Copyright 2010-20XX by 7ymekk
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   @author 7ymekk (7ymekk@gmail.com)
*
************************************************************************/


#include "kmsgdialog.h"
#include "ui_kmsgdialog.h"

//extern QString sdk;
//extern QString adb;
//extern QString aapt;
//extern QProcess *adbProces;
//extern QString busybox;
//extern QString fastboot;

KmsgDialog::KmsgDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QSettings settings;
    this->bufferLimit = settings.value("kmsgBufferLimit",0).toInt();
    this->spinBoxBufferLimit->setValue(this->bufferLimit);
    this->sdk = settings.value("sdkPath").toString();

    checkBoxAutoScroll->setChecked(settings.value("kmsgAutoScroll",true).toBool());
    this->tableView->setContextMenuPolicy(Qt::CustomContextMenu);


    this->contextMenu = new QMenu();

    this->contextMenu->addAction(tr("clear kmsg"),this,SLOT(on_pushButtonClearKmsg_pressed()));
    this->contextMenu->addAction(tr("copy selected to clipboard"),this,SLOT(copySelectedToClipboard()));
    this->contextMenu->addAction(tr("export selected to file"),this,SLOT(exportSelectedToFile()));


    this->restoreGeometry(settings.value("kmsgDialogGeometry").toByteArray());

    setWindowFlags( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    this->kmsgModel = new KmsgModel;
    this->filterModel = new SortFilterProxyModelKmsg;
    this->filterModel->setSourceModel(this->kmsgModel);
    this->filterModel->setFilterKeyColumn(0);
    this->filterModel->setDynamicSortFilter(true);
    this->tableView->setSelectionMode(QTableView::ExtendedSelection);
    this->tableView->setSelectionBehavior(QTableView::SelectRows);

    this->setLayout(layoutKmsg);
    this->proces=new QProcess(this);
    proces->setProcessChannelMode(QProcess::MergedChannels);
    this->setWindowTitle("Kernel Messages");
    QProcess su;
    su.start("\""+sdk+"\""+"adb shell su");
    su.waitForReadyRead();
    QString command;
    if (su.readAll().contains("su: not found"))
    {
        command="\""+sdk+"\""+"adb shell cat /proc/kmsg";
        this->pushButtonLastkmsg->setDisabled(true);
    }
    else
    {
        command="\""+sdk+"\""+"adb shell su -c 'cat /proc/kmsg'";
        this->pushButtonLastkmsg->setEnabled(true);
    }
    this->proces->start(command);
    this->tableView->setModel(this->filterModel);
    this->textBrowser->hide();
    connect(this->proces, SIGNAL(readyRead()), this, SLOT(read()));
    connect(this->checkBoxEmergency, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxCritical, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxAlert, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxError, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxWarning, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxNotice, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxInformation, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxDebug, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->checkBoxAutoScroll, SIGNAL(toggled(bool)), this, SLOT(filter()));
    connect(this->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(filter()));
    connect(this->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filter()));
    connect(this->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    //setAttribute(Qt::WA_DeleteOnClose);
}

KmsgDialog::~KmsgDialog()
{
    QSettings settings;
    settings.setValue("kmsgDialogGeometry",this->saveGeometry());
    this->proces->close();
}

void KmsgDialog::showContextMenu(QPoint point)
{
    QPoint pos2;
    pos2.setX(point.x());
    pos2.setY(point.y()+20);

    this->contextMenu->exec(this->tableView->mapToGlobal(pos2));
}

void KmsgDialog::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("kmsgDialogGeometry",this->saveGeometry());
    QWidget::closeEvent(event);
    this->proces->close();
}

void KmsgDialog::read()
{
    QString tmp = QString::fromUtf8(this->proces->readAll());
    QStringList list=tmp.split("\n");

    qApp->processEvents();
    while (list.length() > 0)
    {
        tmp=list.takeFirst();
        tmp.remove(QRegExp("\\s+$"));
        if (tmp.isEmpty())
            continue;
        KmsgMessage kmsgMessage;
        if (tmp.startsWith("<0>"))
            kmsgMessage.type = "Emergency";
        else if (tmp.startsWith("<1>"))
            kmsgMessage.type = "Alert";
        else if (tmp.startsWith("<2>"))
            kmsgMessage.type = "Critical";
        else if (tmp.startsWith("<3>"))
            kmsgMessage.type = "Error";
        else if (tmp.startsWith("<4>"))
            kmsgMessage.type = "Warning";
        else if (tmp.startsWith("<5>"))
            kmsgMessage.type = "Notice";
        else if (tmp.startsWith("<6>"))
            kmsgMessage.type = "Information";
        else if (tmp.startsWith("<7>"))
            kmsgMessage.type = "Debug";

        tmp.remove(0, tmp.indexOf("]")+1);
        tmp.trimmed();
        kmsgMessage.request = tmp.left(tmp.indexOf(":"));
     //   tmp.remove(0, tmp.indexOf("(") + 1);
        kmsgMessage.request.remove(QRegExp("\\s+$"));
      //  kmsgMessage.pid = tmp.left(tmp.indexOf(")"));
     //   tmp.remove(0, tmp.indexOf(":") + 1);
        kmsgMessage.message = tmp.right(tmp.size() - tmp.indexOf(":") - 1);
        kmsgMessage.message.remove(QRegExp("\\s+$"));
    //    kmsgMessage.pid.remove(QRegExp("\\s+$"));
     //   kmsgMessage.pid.remove(QRegExp("^\\s+"));

        QDateTime time;
        kmsgMessage.timestamp = time.currentDateTime().toString("hh:mm:ss.zzz");

//        kmsgMessage.number=QString::number(kmsgModel->rowCount());

        this->kmsgModel->insertMessage(kmsgMessage);
    }

    this->tableView->resizeColumnToContents(0);
    this->tableView->resizeColumnToContents(1);
    this->tableView->resizeColumnToContents(2);
    if (!this->tableView->horizontalScrollBar()->isVisible())
            this->tableView->setColumnWidth(3,tableView->width() - tableView->columnWidth(0) - tableView->columnWidth(1) - tableView->columnWidth(2) - 20);

    executeBufferLimitation();

    if (checkBoxAutoScroll->isChecked())
        this->tableView->scrollToBottom();

}

void KmsgDialog::filter()
{

    this->filterModel->clearFilters();
    QString pattern;
    if (this->checkBoxEmergency->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Emergency$");
    }
    if (this->checkBoxCritical->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Critical$");
    }
    if (this->checkBoxInformation->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Information$");
    }
    if (this->checkBoxAlert->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Alert$");
    }
    if (this->checkBoxError->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Error$");
    }
    if (this->checkBoxWarning->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Warning$");
    }
    if (this->checkBoxNotice->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Notice$");
    }
    if (this->checkBoxDebug->isChecked())
    {
        if (!pattern.isEmpty() && !pattern.endsWith("|"))
            pattern.append("|");
        pattern.append("^Debug$");
    }

    QRegExp regExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp);

    this->filterModel->addFilterRegExp(1,regExp);

    pattern = this->lineEdit->text();
    QRegExp regExp2(pattern, Qt::CaseInsensitive, QRegExp::RegExp);
    if (this->comboBox->currentIndex() == 0)
        this->filterModel->addFilterRegExp(2,regExp2);
    else
        this->filterModel->addFilterRegExp(3,regExp2);

    if (checkBoxAutoScroll->isChecked())
        this->tableView->scrollToBottom();
}

void KmsgDialog::startKmsg()
{
    if (this->proces->isOpen())
        this->proces->close();
    QProcess su;
    su.start("\""+sdk+"\""+"adb shell su");
    su.waitForReadyRead();
    QString command;
    if (su.readAll().contains("su: not found"))
    {
        command="\""+sdk+"\""+"adb shell cat /proc/kmsg";
        this->pushButtonLastkmsg->setDisabled(true);
    }
    else
    {
        command="\""+sdk+"\""+"adb shell su -c 'cat /proc/kmsg'";
        this->pushButtonLastkmsg->setEnabled(true);
    }
    this->proces->start(command);
}

void KmsgDialog::on_pushButtonClearKmsg_pressed()
{
    this->kmsgModel->clear();
}

void KmsgDialog::executeBufferLimitation()
{
    if (this->bufferLimit == 0)
        return;
    int rowCount = this->kmsgModel->rowCount();
    if (rowCount <= this->bufferLimit)
        return;
    this->kmsgModel->removeRows(0,rowCount-this->bufferLimit,QModelIndex());
}

void KmsgDialog::on_spinBoxBufferLimit_editingFinished()
{
    this->bufferLimit = this->spinBoxBufferLimit->value();
    QSettings settings;
    settings.setValue("kmsgBufferLimit",this->bufferLimit);
    this->executeBufferLimitation();
}

void KmsgDialog::on_checkBoxAutoScroll_toggled(bool checked)
{
    QSettings settings;
    settings.setValue("kmsgAutoScroll", checkBoxAutoScroll->isChecked());
}

void KmsgDialog::copySelectedToClipboard()
{
    QModelIndexList indexList = this->tableView->selectionModel()->selectedRows();
    QModelIndex index;
    QList<KmsgMessage> list;
    while (!indexList.isEmpty())
    {
        index = this->filterModel->mapToSource(indexList.takeFirst());
        list.append(this->kmsgModel->getRow(index.row()));
    }
    QString output;
    foreach (KmsgMessage item, list)
    {
     //   output.append(item.timestamp+" "+item.type+" "+item.sender+" "+item.pid+" "+item.message+"\n");
        output.append(item.timestamp+" "+item.type+" "+item.request+" "+item.message+"\n");
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(output);
}

void KmsgDialog::exportSelectedToFile()
{
    QModelIndexList indexList = this->tableView->selectionModel()->selectedRows();
    QModelIndex index;
    QList<KmsgMessage> list;
    while (!indexList.isEmpty())
    {
        index = this->filterModel->mapToSource(indexList.takeFirst());
        list.append(this->kmsgModel->getRow(index.row()));
    }
    QString output;
    foreach (KmsgMessage item, list)
    {
     //   output.append(item.timestamp+" "+item.type+" "+item.sender+" "+item.pid+" "+item.message+"\n");
        output.append(item.timestamp+" "+item.type+" "+item.request+" "+item.message+"\n");
    }

    QFile file;
    file.setFileName(QFileDialog::getSaveFileName(this, tr("Save File..."), "./kmsg.txt", tr("txt file")+" (*.txt)"));
    if (file.fileName().isEmpty())
        return;
    if (file.open(QFile::WriteOnly))
    {
        file.write(output.toLatin1());
        file.close();
    }
}

void KmsgDialog::on_saveButton_clicked()
{
    QList<KmsgMessage> list;
    for (int i = 0 ; i<this->kmsgModel->rowCount(); i++ )
    {
        list.append(this->kmsgModel->getRow(i));
    }
    QString output;
    foreach (KmsgMessage item, list)
    {
  //      output.append(item.timestamp+" "+item.type+" "+item.sender+" "+item.pid+" "+item.message+"\n");
        output.append(item.timestamp+" "+item.type+" "+item.request+" "+item.message+"\n");
    }

    QFile file;
    file.setFileName(QFileDialog::getSaveFileName(this, tr("Save File..."), "./kmsg.txt", tr("txt file")+" (*.txt)"));
    if (file.fileName().isEmpty())
        return;
    if (file.open(QFile::WriteOnly))
    {
        file.write(output.toLatin1());
        file.close();
    }
}

void KmsgDialog::on_pushButtonLastkmsg_clicked()
{
    QProcess su;
    su.start("\""+sdk+"\""+"adb pull /proc/last_kmsg \"" + QDir::currentPath()+"/tmp/phone/last_kmsg.txt");
    su.waitForFinished(-1);
    if (!QDesktopServices::openUrl(QUrl("file:///" + QDir::currentPath()+"/tmp/phone/last_kmsg.txt", QUrl::TolerantMode)))
        QMessageBox::information(this,"",tr("Last Kernel MSG is only available in Android."),QMessageBox::Ok);
}
