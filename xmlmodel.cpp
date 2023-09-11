#include "xmlmodel.h"
#include <QMessageBox>
#include <QAbstractItemModel>
#include <QFile>
#include <QXmlStreamReader>
#include <QStack>
#include <qfileinfo.h>

XMLModel::XMLModel(QObject* parent) : QAbstractItemModel(parent),
                                      focused() {
    mainRoot = new QStandardItem();
}


QModelIndex XMLModel::index(int row, int column, const QModelIndex& parent) const {
    QStandardItem* parentItem;
    if (!parent.isValid()) {
        parentItem = mainRoot;
    }
    else {
        parentItem = static_cast<QStandardItem*>(parent.internalPointer());
    }
    QStandardItem* descendantItem = parentItem->child(row, column);
    if (descendantItem) {
        return createIndex(row, column, descendantItem);
    }
    return {};
}

QVariant XMLModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return {};
    }
    if (role != Qt::DisplayRole) {
        return {};
    }
    auto* item = static_cast<QStandardItem*>(index.internalPointer());
    return item->data(0);
};

int XMLModel::rowCount(const QModelIndex& parent) const {
    QStandardItem* parentItem;
    if (parent.column() > 0) {
        return 0;
    }
    if (!parent.isValid()) {
        parentItem = mainRoot;
    }
    else {
        parentItem = static_cast<QStandardItem*>(parent.internalPointer());
    }
    return parentItem->rowCount();
}

QModelIndex XMLModel::parent(const QModelIndex& descendant) const {
    if (!descendant.isValid()) {
        return QModelIndex{};
    }
    auto* descendantItem = static_cast<QStandardItem*>(descendant.internalPointer());
    QStandardItem* parentItem = descendantItem->parent();
    if (parentItem == mainRoot || descendantItem == mainRoot) {
        return {};
    }
    return createIndex(parentItem->row(), 0, parentItem);
}

int XMLModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return static_cast<QStandardItem*>(parent.internalPointer())->columnCount();
    }
    return mainRoot->columnCount();
}


void XMLModel::ParseFile(const QString& fileName) {
    auto* reader = new QXmlStreamReader;
    QFile file{ fileName };
    QFileInfo f(fileName);
    QStack<int> counter;
    QStack<int> layers;


    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox message(QMessageBox::Critical, tr("ERROR!"), tr("Fail to open file!"), QMessageBox::Ok);
        message.exec();
    }

    reader->setDevice(&file);

    auto startNode = new QStandardItem(f.fileName());
    mainRoot->appendRow(startNode);
    mainRoot->setColumnCount(2);
    QStandardItem* curNode = startNode;

    int curLayer = 0;
    QXmlStreamReader::TokenType token = reader->readNext();
    while (!reader->atEnd() && !reader->hasError()) {
        if (token == QXmlStreamReader::StartElement) {
            ++curLayer;
            auto text = reader->name().toString();
            if (text == "array") {
                if (layers.isEmpty() || curLayer != layers.top()) {
                    counter.append(0);
                    layers.append(curLayer);
                }
                text = QString("Element %1").arg(counter.top() + 1);
                ++counter.top();
            }
            auto name_node = new QStandardItem(text);
            token = reader->readNext();
            if (reader->atEnd() && reader->hasError())
                break;
            if (token == QXmlStreamReader::Characters) {
                auto value = reader->text().toString();
                auto value_node = new QStandardItem(value);
                curNode->appendRow({name_node, value_node});
                if (text == "Name") {
                    setData(curNode->index(),QVariant(value),Qt::DisplayRole);
                    curNode->setData(QVariant(value), Qt::DisplayRole);
                }
            } else {
                curNode->appendRow(name_node);
            }
            curNode = name_node;
            continue;
        }

        if (token == QXmlStreamReader::EndElement) {
            --curLayer;
            curNode = curNode->parent();
            auto text = reader->name().toString();
            if (text == "array" && curLayer + 1 != layers.top()) {
                counter.pop();
                layers.pop();
            }
        }

        token = reader->readNext();
    }

    if (reader->hasError()) {
        QMessageBox message(QMessageBox::Critical, tr("Parsing error!"), tr("This XML document cannot be displayed!"), QMessageBox::Ok);
        message.exec();

    }
    file.close();
}

void XMLModel::append(const QModelIndex& index) {
    if (not index.isValid())
        return;
    beginResetModel();
    auto* clone = static_cast<QStandardItem*>(index.internalPointer())->clone();
    mainRoot->appendRow(clone);
    append(index, clone);
    endResetModel();
}

void XMLModel::append(const QModelIndex& sourceIndex, QStandardItem* item) {
    if (not sourceIndex.isValid())
        return;
    for (int i = 0; i < rowCount(sourceIndex); ++i) {
        auto child = sourceIndex.model()->index(i, 0, sourceIndex);
        auto* clone = static_cast<QStandardItem*>(child.internalPointer())->clone();
        item->appendRow(clone);
        append(child, clone);
    }
}

bool XMLModel::removeRows(int row, int count, const QModelIndex& parent) {
    QModelIndex correctParent;
    if (parent.isValid())
        correctParent = parent;
    else
        correctParent = QModelIndex();
    auto start = row;
    auto stop = start + count - 1;
    beginRemoveRows(correctParent, start, stop);
    mainRoot->removeRows(start, count);
    endRemoveRows();
    return true;
}

void XMLModel::closeAll() {
    removeRows(0, mainRoot->rowCount(), createIndex(-1, -1, nullptr));
}

QVariant XMLModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::FontRole)) {
        if (section == 0)
            return tr("Name");
        else if (section == 1)
            return tr("Value");
    }
    return QVariant();
}

bool XMLModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) {
        return false;
    }
    auto* item = static_cast<QStandardItem*>(index.internalPointer());
    if (role == Qt::EditRole) {
        item->setData(value,Qt::EditRole);
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

