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


#ifndef KMSGDIALOG_H
#define KMSGDIALOG_H

#include <QDialog>
#include <QtGui>
#include "../classes/models/kmsgmodel.h"
#include "ui_kmsgdialog.h"

class KmsgDialog : public QDialog, public Ui::KmsgDialog
{
    Q_OBJECT

public:
    KmsgDialog(QWidget *parent = 0);
    ~KmsgDialog();

protected:
    void closeEvent(QCloseEvent *event);

private:
    QProcess *proces;
    KmsgModel *kmsgModel;
    SortFilterProxyModelKmsg *filterModel;
    void executeBufferLimitation();
    int bufferLimit;
    QMenu *contextMenu;
    QString sdk;
public slots:
    void read();
    void filter();
    void startKmsg();
private slots:
    void on_saveButton_clicked();
    void on_pushButtonClearKmsg_pressed();
    void on_pushButtonLastkmsg_clicked();
    void on_spinBoxBufferLimit_editingFinished();
    void on_checkBoxAutoScroll_toggled(bool checked);
    void showContextMenu(QPoint point);
    void copySelectedToClipboard();
    void exportSelectedToFile();
};

#endif // KMSGDIALOG_H
