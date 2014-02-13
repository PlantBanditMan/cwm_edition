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


#ifndef KMSGMODEL_H
#define KMSGMODEL_H

#include <QtGui>

class KmsgMessage : public QObject{
public:
    KmsgMessage(QObject *parent=0);
    KmsgMessage(const KmsgMessage&);
    QString type;
    QString request;
    QString message;
    QString timestamp;
//    QString number;
    KmsgMessage& operator =(const KmsgMessage&);
};


Q_DECLARE_METATYPE(KmsgMessage)

class KmsgModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    KmsgModel(QObject *parent=0);
    KmsgModel(QList< KmsgMessage > fileList, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int rowCount();
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertMessage(int position, KmsgMessage kmsgMessage);
    bool insertMessage(KmsgMessage kmsgMessage);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    KmsgMessage getRow(int row);
    QList< KmsgMessage > getList();
    bool clear();

private:
    QList< KmsgMessage > kmsgList;

};

class SortFilterProxyModelKmsg : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxyModelKmsg(QObject *parent = 0);
    void addFilterRegExp(qint32 column, const QRegExp &pattern);
    void clearFilters();
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QList<QRegExp> regExp;
};
#endif // KMSGMODEL_H
