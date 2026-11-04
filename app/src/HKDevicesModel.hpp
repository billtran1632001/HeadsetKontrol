// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

#pragma once

#include <QAbstractListModel>
#include <QAbstractProxyModel>
#include <QObjectBindableProperty>
#include <QtQmlIntegration/qqmlintegration.h>

class HKDevicesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    enum Role {
        None = Qt::UserRole,
        DeviceId,
        DeviceName,
        DeviceVendorName,
        DeviceProductName,
        Device
    };
    Q_ENUM(Role)

    explicit HKDevicesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    using DataVector = std::vector<QPointer<class HKDevice>>;
    DataVector m_data;

    void onDevicesAdded(const QList<HKDevice *> &devices);
    void onDevicesRemoved(const QList<HKDevice *> &devices);
};

class HKDevicesProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged BINDABLE bindableSortOrder FINAL)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged BINDABLE bindableFilter FINAL)
public:
    explicit HKDevicesProxyModel(QObject *parent = nullptr);

    void setSortOrder(Qt::SortOrder value)
    {
        m_sortOrder = value;
    }

    [[nodiscard]] Qt::SortOrder sortOrder() const
    {
        return m_sortOrder;
    }

    [[nodiscard]] QBindable<Qt::SortOrder> bindableSortOrder() const
    {
        return &m_sortOrder;
    }

    void setFilter(QString value)
    {
        m_filter = value;
    }

    [[nodiscard]] QString filter() const
    {
        return m_filter;
    }

    [[nodiscard]] QBindable<QString> bindableFilter() const
    {
        return &m_filter;
    }

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

Q_SIGNALS:
    void sortOrderChanged(Qt::SortOrder);
    void filterChanged(const QString &);

private:
    std::vector<int> m_mapping;

    Q_OBJECT_BINDABLE_PROPERTY(HKDevicesProxyModel, Qt::SortOrder, m_sortOrder, &HKDevicesProxyModel::sortOrderChanged)
    Q_OBJECT_BINDABLE_PROPERTY(HKDevicesProxyModel, QString, m_filter, &HKDevicesProxyModel::filterChanged)

    bool matchRow(int sourceRow);
    void doSort();

    void sortFilter();
};