/*
  This file is part of KDToolBox.

  Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: André Somers <andre.somers@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SORTPROXYMODEL_H
#define SORTPROXYMODEL_H

#include <QAbstractProxyModel>

/**
 * @brief The SortProxyModel class provides sorting with row move support
 *
 * The default QSortFilterProxyModel does not properly emit detailed signals
 * for what happens during sorting, making it hard to visually show this and
 * to keep selections stable. This proxy model provides sorting for list or
 * table type models that does provide move signals to signal what happens
 * during a sort.
 *
 * The API is similar to that of QSortFilterProxyModel for the sorting parts
 * of the API, so if you used QSortFilterProxyModel only for sorting it
 * should be a drop-in replacement.
 *
 * Note that SortProxyModel always sorts the underlying model.
 */
class SortProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int sortRole READ sortRole WRITE setSortRole NOTIFY sortRoleChanged)

public:
    explicit SortProxyModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void sort(int column, Qt::SortOrder order) override;
    QVariant data(const QModelIndex &proxyIndex, int role) const override;

    // QAbstractProxyModel interface
public:
    void setSourceModel(QAbstractItemModel *model) override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

    // QSortFilterProxyModel interface (copy, not inheriting from QSFPM)
public:
    void setSortRole(int role);
    int sortRole() const;
    void setSortCaseSensitivity(Qt::CaseSensitivity sensitivity);
    Qt::CaseSensitivity sortCaseSensitivity() const;
    int sortColumn() const;
    Qt::SortOrder sortOrder() const;

signals:
    void sortRoleChanged();
    void sortCaseSensitivityChanged();
    void sortColumnChanged();
    void sortOrderChanged();

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

protected slots:
    void resetInternalData();

private:
    void rebuildRowMap();
    void reorder();
    void sortMappingContainer(std::vector<int> &container);
    bool lessThan(int source_left_row, int source_right_row) const;

    //source model change handlers
    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, QVector<int> roles);
    void handleRowsInserted(const QModelIndex &parent, int firstNewRow, int lastNewRow);
    void handleRowsRemoved(const QModelIndex &parent, int firstRemovedRow, int lastRemovedRow);

    bool isInvalidedRow(const int row) const;

private:
    int m_sortColumn = 0;
    Qt::SortOrder m_order = Qt::AscendingOrder;
    int m_sortRole = Qt::DisplayRole;
    Qt::CaseSensitivity m_caseSensitivity = Qt::CaseSensitive;

    std::vector<int> m_rowMap;
    std::pair<std::vector<int>::iterator, std::vector<int>::iterator> m_invalidatedRows;
};

#endif // SORTPROXYMODEL_H