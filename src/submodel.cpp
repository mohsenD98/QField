/***************************************************************************
  submodel.cpp - SubModel

 ---------------------
 begin                : 16.9.2016
 copyright            : (C) 2016 by Matthias Kuhn
 email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "submodel.h"
#include <QDebug>

SubModel::SubModel( QObject* parent )
  : QAbstractItemModel( parent )
{

}

QModelIndex SubModel::index( int row, int column, const QModelIndex& parent ) const
{
  QModelIndex sourceIndex = mModel->index( row, column, parent.isValid() ? parent : static_cast<QModelIndex>( mRootIndex ) );
  return sourceIndex;
}

QModelIndex SubModel::parent( const QModelIndex& child ) const
{
  QModelIndex idx = mModel->parent( child );
  if ( idx == mRootIndex )
    return QModelIndex();
  else
    return idx;
}

int SubModel::rowCount( const QModelIndex& parent ) const
{
  return mModel->rowCount( parent.isValid() ? parent : static_cast<QModelIndex>( mRootIndex ) );
}

int SubModel::columnCount( const QModelIndex& parent ) const
{
  return mModel->columnCount( parent.isValid() ? parent : static_cast<QModelIndex>( mRootIndex ) );
}

QVariant SubModel::data( const QModelIndex& index, int role ) const
{
  return mModel->data( index, role );
}

bool SubModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
  return mModel->setData( index, value, role );
}

bool SubModel::setModelData( int row, const QVariant& value, int role )
{
  return setData( mModel->index( row, 0, mRootIndex ), value, role );
}

QHash<int, QByteArray> SubModel::roleNames() const
{
  return mModel->roleNames();
}

QModelIndex SubModel::rootIndex() const
{
  return mRootIndex;
}

void SubModel::setRootIndex( const QModelIndex& rootIndex )
{
  if ( rootIndex == mRootIndex )
    return;

  beginResetModel();
  mRootIndex = rootIndex;
  endResetModel();
  emit rootIndexChanged();
}

QAbstractItemModel* SubModel::model() const
{
  return mModel;
}

void SubModel::setModel( QAbstractItemModel* model )
{
  if ( model == mModel )
    return;

  connect( model, &QAbstractItemModel::rowsAboutToBeInserted, this, &SubModel::onRowsAboutToBeInserted );
  connect( model, &QAbstractItemModel::rowsInserted, this, &SubModel::onRowsInserted );
  connect( model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &SubModel::onRowsAboutToBeRemoved );
  connect( model, &QAbstractItemModel::rowsRemoved, this, &SubModel::onRowsRemoved );
  connect( model, &QAbstractItemModel::modelAboutToBeReset, this, &QAbstractItemModel::modelAboutToBeReset );
  connect( model, &QAbstractItemModel::modelReset, this, &QAbstractItemModel::modelReset );
  connect( model, &QAbstractItemModel::dataChanged, this, &SubModel::onDataChanged );

  mModel = model;
  emit modelChanged();
}

void SubModel::onRowsAboutToBeInserted( const QModelIndex& parent, int first, int last )
{
  emit beginInsertRows( parent, first, last );
}

void SubModel::onRowsInserted( const QModelIndex& parent, int first, int last )
{
  Q_UNUSED( parent )
  Q_UNUSED( first )
  Q_UNUSED( last )
  emit endInsertRows();
}

void SubModel::onRowsAboutToBeRemoved( const QModelIndex& parent, int first, int last )
{
  emit beginRemoveRows( parent, first, last );
}

void SubModel::onRowsRemoved( const QModelIndex& parent, int first, int last )
{
  Q_UNUSED( parent )
  Q_UNUSED( first )
  Q_UNUSED( last )
  emit endRemoveRows();
}

void SubModel::onDataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles )
{
  emit dataChanged( topLeft, bottomRight, roles );
}
#if 0
QModelIndex SubModel::fromSourceIndex( const QModelIndex& sourceIndex ) const
{
  return createIndex( sourceIndex.row(), sourceIndex.column(), sourceIndex.internalId() );
}

QModelIndex SubModel::toSourceIndex( const QModelIndex& index ) const
{
  if ( !index.isValid() )
    return mRootIndex;

  return mModel->index( index.row(), index.column(), toSourceIndex( index.parent() ) );
}
#endif
