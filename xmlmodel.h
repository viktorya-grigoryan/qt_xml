#pragma once
#include <QAbstractItemModel>
#include <QFile>
#include <QMessageBox>
#include <QModelIndex>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStack>
#include <QPersistentModelIndex>
#include <QMessageBox>
#include <QStandardItem>
#include <QVector>
#include <QVariant>
#include <QtXml/QDomDocument>

class XMLModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit XMLModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    void append(const QModelIndex& index);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    void ParseFile(const QString& fileName);
    void closeAll();

private:
    QStandardItem* mainRoot;
    QPersistentModelIndex focused;
    void append(const QModelIndex& index, QStandardItem*);
};



