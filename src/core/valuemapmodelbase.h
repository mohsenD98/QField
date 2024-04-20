/***************************************************************************
                            valuemapmodelbase.h

                              -------------------
              begin                : March 2019
              copyright            : (C) 2019 by Matthias Kuhn
              email                : matthias@opengis.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VALUEMAPMODELBASE_H
#define VALUEMAPMODELBASE_H

#include <QAbstractListModel>
#include <QVariant>

//! \copydoc ValueMapModel
class ValueMapModelBase : public QAbstractListModel
{
    Q_OBJECT

    //! \copydoc ValueMapModel::valueMap
    Q_PROPERTY( QVariant valueMap READ map WRITE setMap NOTIFY mapChanged )

  public:
    /**
     * Create a new value map model base
     */
    explicit ValueMapModelBase( QObject *parent = nullptr );

    //! \copydoc ValueMapModel::map
    QVariant map() const;

    //! \copydoc ValueMapModel::setMap
    void setMap( const QVariant &map );

    //! \copydoc ValueMapModel::keyToIndex
    Q_INVOKABLE int keyToIndex( const QVariant &key ) const;

    //! \copydoc ValueMapModel::keyForValue
    Q_INVOKABLE QVariant keyForValue( const QString &value ) const;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QHash<int, QByteArray> roleNames() const override;

  signals:
    //! \copydoc ValueMapModel::mapChanged
    void mapChanged();

  private:
    QList<QPair<QVariant, QString>> mMap;
    QVariant mConfiguredMap;
};

#endif // VALUEMAPMODELBASE_H
