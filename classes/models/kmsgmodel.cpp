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


#include "kmsgmodel.h"

KmsgMessage::KmsgMessage(QObject *parent)
    : QObject(parent)
{

}

KmsgMessage::KmsgMessage(const KmsgMessage& kmsgMessage)
{
    this->type = kmsgMessage.type;
    this->message = kmsgMessage.message;
    this->request = kmsgMessage.request;
    this->timestamp = kmsgMessage.timestamp;
//    this->number = kmsgMessage.number;
}

KmsgMessage& KmsgMessage::operator =(const KmsgMessage& kmsgMessage)
{
    this->type = kmsgMessage.type;
    this->message = kmsgMessage.message;
    this->request = kmsgMessage.request;
    this->timestamp = kmsgMessage.timestamp;
//    this->number = kmsgMessage.number;
    return *this;
}

KmsgModel::KmsgModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

KmsgModel::KmsgModel(QList< KmsgMessage > kmsgList, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->kmsgList = kmsgList;
}

int KmsgModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->kmsgList.size();
}

KmsgMessage KmsgModel::getRow(int row)
{
    return this->kmsgList.value(row);
}

int KmsgModel::rowCount()
{
    return this->kmsgList.size();
}


int KmsgModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant KmsgModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->kmsgList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        int col = index.column();
        switch (col)
        {
        case 0:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 1:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 2:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 3:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
         default:
            return QVariant();
        }
    }
    else if (role == Qt::DisplayRole)
    {
        KmsgMessage kmsgMessage = this->kmsgList.at(index.row());

        int col = index.column();
        switch (col)
        {
        case 0:
            return kmsgMessage.timestamp;
        case 1:
            return kmsgMessage.type;
        case 2:
            return kmsgMessage.request;
        case 3:
            return kmsgMessage.message;
         default:
            return QVariant();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        KmsgMessage kmsgMessage = this->kmsgList.at(index.row());
        if (kmsgMessage.type == "Debug")
            return QVariant::fromValue(QBrush(Qt::darkBlue, Qt::SolidPattern));
        else if (kmsgMessage.type == "Error" || kmsgMessage.type == "Emergency" || kmsgMessage.type == "Alert" || kmsgMessage.type == "Critical")
            return QVariant::fromValue(QBrush(Qt::red, Qt::SolidPattern));
        else if (kmsgMessage.type == "Information")
            return QVariant::fromValue(QBrush(Qt::darkGreen, Qt::SolidPattern));
        else if (kmsgMessage.type == "Notice")
            return QVariant::fromValue(QBrush(Qt::darkYellow, Qt::SolidPattern));
        else if (kmsgMessage.type == "Warning")
            return QVariant::fromValue(QBrush(QColor(255,165,0), Qt::SolidPattern));
        else
            return QVariant::fromValue(QBrush(Qt::black, Qt::SolidPattern));
    }
    return QVariant();

}

QVariant KmsgModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Time", "kmsg header");
        case 1:
            return tr("Type", "kmsg header");
        case 2:
            return tr("Function", "kmsg header");
        case 3:
            return tr("Message", "kmsg header");
        default:
            return QVariant();
        }
    }
    return QVariant();

}

Qt::ItemFlags KmsgModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);

}

bool KmsgModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        KmsgMessage kmsgMessage = this->kmsgList.value(row);

        int col = index.column();
        switch (col)
        {
        case 0:
            break;
//            kmsgMessage.number = value.toString();
        case 1:
            kmsgMessage.type = value.toString();
            break;
        case 2:
            kmsgMessage.request = value.toString();
            break;
        case 3:
            kmsgMessage.message = value.toString();
            break;
        default:
            return false;
            break;
        }

        this->kmsgList.replace(row, kmsgMessage);
        emit(dataChanged(index, index));

        return true;
    }

    return false;

}

bool KmsgModel::insertMessage(int position, KmsgMessage kmsgMessage)
{
    beginInsertRows(QModelIndex(), position, position);

    this->kmsgList.insert(position, kmsgMessage);

    endInsertRows();
    return true;
}

bool KmsgModel::insertMessage(KmsgMessage kmsgMessage)
{
    beginInsertRows(QModelIndex(), kmsgList.size(), kmsgList.size());

    this->kmsgList.append(kmsgMessage);

    endInsertRows();
    return true;
}

bool KmsgModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        KmsgMessage kmsgMessage;
        kmsgMessage.type = "";
        kmsgMessage.request = "";
        kmsgMessage.message = "";
//        kmsgMessage.number = "";

        this->kmsgList.insert(position, kmsgMessage);
    }

    endInsertRows();
    return true;

}


bool KmsgModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
       this->kmsgList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

QList< KmsgMessage > KmsgModel::getList()
{
    return this->kmsgList;
}

bool KmsgModel::clear()
{
    if (!this->kmsgList.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->kmsgList.size()-1);

        this->kmsgList.clear();

        endRemoveRows();

        return true;
    }
    return false;
}


SortFilterProxyModelKmsg::SortFilterProxyModelKmsg(QObject *parent) : QSortFilterProxyModel(parent)
{
    int i;
    for (i = 0; i < 4; i++)
        this->regExp.append(QRegExp());
}

void SortFilterProxyModelKmsg::addFilterRegExp(qint32 column, const QRegExp &pattern)
{
    this->regExp[column] = pattern;
    invalidateFilter();
}

bool SortFilterProxyModelKmsg::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(regExp.isEmpty())
        return true;

    bool ret = false;

    int i;
    for(i = 0; i < this->columnCount(QModelIndex()); i++)
    {
        QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);
        QString str = index.data().toString();
        QString pat = regExp.at(i).pattern();
        ret = (str.contains(regExp.at(i)));

        if(!ret)
            return ret;
    }

    return ret;
}

void SortFilterProxyModelKmsg::clearFilters()
{
    this->regExp.clear();
    int i;
    for (i = 0; i < 4; i++)
        this->regExp.append(QRegExp());
}
