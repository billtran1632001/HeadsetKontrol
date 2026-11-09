// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

#include "HKDevicesModel.hpp"

#include "HKDevice.hpp"
#include "HKDeviceId.hpp" // IWYU pragma: keep
#include "HKHeadsetControl.hpp"
#include "HKLogging.hpp"
#include "HKNamespace.hpp"

#include <QCollator>
#include <QPointer>

using namespace Qt::StringLiterals;

HKDevicesModel::HKDevicesModel(QObject *parent)
    : QAbstractListModel{parent}
{
    connect(HKHeadsetControl::instance(), &HKHeadsetControl::devicesAdded, this, &HKDevicesModel::onDevicesAdded);
    connect(HKHeadsetControl::instance(), &HKHeadsetControl::devicesRemoved, this, &HKDevicesModel::onDevicesRemoved);

    onDevicesAdded(HKHeadsetControl::devices());
}

int HKDevicesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

QVariant HKDevicesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || std::cmp_greater_equal(index.row(), m_data.size())) {
        return {};
    }

    auto device = m_data.at(index.row());
    if (!device) {
        return {};
    }

    QVariant data;

    switch (role) {
    case Role::DeviceId:
        data = QVariant::fromValue(device->id());
        break;
    case Role::DeviceName:
        data = device->name();
        break;
    case Role::DeviceVendorName:
        data = device->vendorName();
        break;
    case Role::DeviceProductName:
        data = device->productName();
        break;
    case Role::Device:
        data = QVariant::fromValue(device.get());
        break;
    }

    return data;
}

QHash<int, QByteArray> HKDevicesModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{
        {Role::DeviceId, "deviceId"_ba},
        {Role::DeviceName, "deviceName"_ba},
        {Role::DeviceVendorName, "deviceVendorName"_ba},
        {Role::DeviceProductName, "deviceProductName"_ba},
        {Role::Device, "device"_ba},
    };
    return roles;
}

void HKDevicesModel::onDevicesAdded(const QList<HKDevice *> &devices)
{
    if (devices.empty()) {
        HK_DEBUG("Devices model cannot add devices from devices list that is empty.");
        return;
    }

    if (HK::isSumExceedMax<int>(m_data.size(), devices.size())) {
        HK_CRIT("Devices model is full!");
        return;
    }

    beginInsertRows({}, m_data.size(), m_data.size() + devices.size() - 1);

    for (auto device : devices) {
        m_data.push_back(device);
    }

    endInsertRows();
}

void HKDevicesModel::onDevicesRemoved(const QList<HKDevice *> &devices)
{
    if (devices.empty()) {
        HK_DEBUG("Devices model cannot remove devices from devices list that is empty.");
        return;
    }

    auto startRemove = m_data.end();
    for (auto it = m_data.begin(); it != m_data.end(); ++it) {
        if ((*it).isNull() || devices.contains(*it)) {
            if (startRemove == m_data.end()) {
                startRemove = it;
            }
        } else {
            if (startRemove != m_data.end()) {
                beginRemoveRows({}, std::distance(m_data.begin(), startRemove), std::distance(m_data.begin(), it - 1));
                m_data.erase(startRemove, it);
                endRemoveRows();
                startRemove = m_data.end();
            }
        }
    }
    if (startRemove != m_data.end()) {
        beginRemoveRows({}, std::distance(m_data.begin(), startRemove), std::distance(m_data.begin(), m_data.end() - 1));
        m_data.erase(startRemove, m_data.end());
        endRemoveRows();
    }
}

HKDevicesProxyModel::HKDevicesProxyModel(QObject *parent)
    : QAbstractProxyModel{parent}
    , m_sortOrder{Qt::AscendingOrder}
{
    connect(this, &HKDevicesProxyModel::filterChanged, this, &HKDevicesProxyModel::sortFilter);
    connect(this, &HKDevicesProxyModel::sortOrderChanged, this, &HKDevicesProxyModel::sortFilter);
}

void HKDevicesProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (this->sourceModel() == sourceModel) {
        return;
    }

    auto *const model = qobject_cast<HKDevicesModel *>(sourceModel);
    if (!model) {
        HK_DEBUG("Invalid source model passed into HKDevicesProxyModel.");
        return;
    }

    Q_ASSERT(model);

    if (auto *oldSourceModel = this->sourceModel()) {
        disconnect(oldSourceModel, &QAbstractItemModel::rowsInserted, this, &HKDevicesProxyModel::sortFilter);
        disconnect(oldSourceModel, &QAbstractItemModel::rowsRemoved, this, &HKDevicesProxyModel::sortFilter);
        disconnect(oldSourceModel, &QAbstractItemModel::modelReset, this, &HKDevicesProxyModel::sortFilter);
    }

    QAbstractProxyModel::setSourceModel(model);

    if (sourceModel) {
        connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &HKDevicesProxyModel::sortFilter);
        connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &HKDevicesProxyModel::sortFilter);
        connect(sourceModel, &QAbstractItemModel::modelReset, this, &HKDevicesProxyModel::sortFilter);
    }

    sortFilter();
}

QModelIndex HKDevicesProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() /* Flat list have no parent */ || row < 0 || std::cmp_greater_equal(row, m_mapping.size()) || column != 0) {
        return {};
    }
    return createIndex(row, column);
}

QModelIndex HKDevicesProxyModel::parent([[maybe_unused]] const QModelIndex &child) const
{
    return {}; // List models are flat
}

int HKDevicesProxyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) { // Flat list have no parent
        return 0;
    }
    using SizeType = decltype(m_mapping.size());
    if constexpr (std::numeric_limits<SizeType>::max() > std::numeric_limits<int>::max()) {
        return std::clamp<SizeType>(m_mapping.size(), 0, std::numeric_limits<int>::max());
    } else {
        return m_mapping.size();
    }
}

int HKDevicesProxyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) { // Flat list have no parent
        return 0;
    }
    return 1;
}

QModelIndex HKDevicesProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid() || proxyIndex.row() < 0 || std::cmp_greater_equal(proxyIndex.row(), m_mapping.size())) {
        return {};
    }
    return sourceModel()->index(m_mapping[proxyIndex.row()], 0);
}

QModelIndex HKDevicesProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid() || sourceIndex.row() < 0 || std::cmp_greater_equal(sourceIndex.row(), m_mapping.size())) {
        return {};
    }

    const auto it = std::ranges::find(std::as_const(m_mapping), sourceIndex.row());
    if (it == m_mapping.cend()) {
        return {};
    }

    return index(std::distance(m_mapping.cbegin(), it), 0);
}

void HKDevicesProxyModel::sort([[maybe_unused]] int column, Qt::SortOrder order)
{
    Q_EMIT layoutAboutToBeChanged();
    m_sortOrder = order;

    doSort();

    Q_EMIT layoutChanged();
}

bool HKDevicesProxyModel::matchRow(int sourceRow)
{
    auto model = sourceModel();
    QModelIndex idx = model->index(sourceRow, 0);

    const auto filterString = m_filter.value().toLower();

    if (model->data(idx, HKDevicesModel::DeviceProductName).toString().toLower().contains(filterString)) {
        return true;
    }

    if (model->data(idx, HKDevicesModel::DeviceVendorName).toString().toLower().contains(filterString)) {
        return true;
    }

    if (model->data(idx, HKDevicesModel::DeviceName).toString().toLower().contains(filterString)) {
        return true;
    }

    if (model->data(idx, HKDevicesModel::DeviceId).toString().toLower().contains(filterString)) {
        return true;
    }

    return false;
}

void HKDevicesProxyModel::doSort()
{
    if (auto *model = sourceModel()) {
        QCollator collator;
        std::sort(m_mapping.begin(), m_mapping.end(), [this, model, &collator](int rowA, int rowB) {
            QModelIndex idxA = model->index(rowA, 0);
            QModelIndex idxB = model->index(rowB, 0);

            auto ret = collator.compare(model->data(idxA, HKDevicesModel::DeviceProductName).toString(),
                                        model->data(idxB, HKDevicesModel::DeviceProductName).toString());
            if (ret == 0) {
                ret = collator.compare(model->data(idxA, HKDevicesModel::DeviceVendorName).toString(),
                                       model->data(idxB, HKDevicesModel::DeviceVendorName).toString());
                if (ret == 0) {
                    ret = collator.compare(model->data(idxA, HKDevicesModel::DeviceName).toString(), model->data(idxB, HKDevicesModel::DeviceName).toString());
                    if (ret == 0) {
                        auto ret =
                            model->data(idxA, HKDevicesModel::DeviceId).value<HKDeviceId>() <=> model->data(idxB, HKDevicesModel::DeviceId).value<HKDeviceId>();
                        if (ret == std::strong_ordering::equal) {
                            return false;
                        } else {
                            return m_sortOrder == Qt::AscendingOrder ? ret == std::strong_ordering::less : ret == std::strong_ordering::greater;
                        }
                    }
                }
            }

            return m_sortOrder == Qt::AscendingOrder ? ret < 0 : ret > 0;
        });
    }
}

void HKDevicesProxyModel::sortFilter()
{
    beginResetModel();
    m_mapping.clear();

    if (auto *model = sourceModel()) {
        const auto rowCount = model->rowCount();
        m_mapping.reserve(rowCount);

        if (!m_filter.value().isEmpty()) {
            for (int i = 0; i < rowCount; ++i) {
                if (matchRow(i)) {
                    m_mapping.push_back(i);
                }
            }
        } else {
            for (int i = 0; i < rowCount; ++i) {
                m_mapping.push_back(i);
            }
        }

        doSort();
    }

    endResetModel();
}
