/***************************************************************************
                            bookmarkmodel.cpp
                              -------------------
              begin                : 12.12.2021
              copyright            : (C) 2021 by Mathieu Pellerin
              email                : mathieu (at) opengis.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bookmarkmodel.h"

#include <qgscoordinatetransform.h>
#include <qgsgeometry.h>
#include <qgsproject.h>

BookmarkModel::BookmarkModel( QgsBookmarkManager *manager, QgsBookmarkManager *projectManager, QObject *parent )
  : QSortFilterProxyModel( parent )
  , mModel( std::make_unique<QgsBookmarkManagerModel>( manager, projectManager, this ) )
  , mManager( manager )
{
  setSourceModel( mModel.get() );
}

QVariant BookmarkModel::data( const QModelIndex &index, int role ) const
{
  QModelIndex sourceIndex = mapToSource( index );
  if ( !sourceIndex.isValid() )
    return QVariant();

  switch ( role )
  {
    case BookmarkModel::BookmarkId:
      return mModel->data( sourceIndex, QgsBookmarkManagerModel::RoleId );

    case BookmarkModel::BookmarkName:
      return mModel->data( sourceIndex, QgsBookmarkManagerModel::RoleName );

    case BookmarkModel::BookmarkPoint:
    {
      QgsReferencedRectangle rect = mModel->data( sourceIndex, QgsBookmarkManagerModel::RoleExtent ).value<QgsReferencedRectangle>();
      QgsGeometry geom( new QgsPoint( rect.center() ) );
      return geom;
    }

    case BookmarkModel::BookmarkCrs:
    {
      QgsReferencedRectangle rect = mModel->data( sourceIndex, QgsBookmarkManagerModel::RoleExtent ).value<QgsReferencedRectangle>();
      return rect.crs();
    }
  }

  return QVariant();
}

QHash<int, QByteArray> BookmarkModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractProxyModel::roleNames();
  roleNames[BookmarkModel::BookmarkId] = "BookmarkId";
  roleNames[BookmarkModel::BookmarkName] = "BookmarkName";
  roleNames[BookmarkModel::BookmarkPoint] = "BookmarkPoint";
  roleNames[BookmarkModel::BookmarkCrs] = "BookmarkCrs";
  return roleNames;
}

void BookmarkModel::setMapSettings( QgsQuickMapSettings *mapSettings )
{
  if ( mMapSettings == mapSettings )
    return;

  mMapSettings = mapSettings;

  emit mapSettingsChanged();
}

void BookmarkModel::setExtentFromBookmark( const QModelIndex &index )
{
  QModelIndex sourceIndex = mapToSource( index );
  if ( !sourceIndex.isValid() || !mMapSettings )
    return;

  QgsReferencedRectangle rect = mModel->data( sourceIndex, QgsBookmarkManagerModel::RoleExtent ).value<QgsReferencedRectangle>();

  QgsCoordinateTransform transform( rect.crs(), mMapSettings->destinationCrs(), QgsProject::instance()->transformContext() );
  QgsRectangle transformedRect;
  try
  {
    transformedRect = transform.transform( rect );
  }
  catch ( const QgsException &e )
  {
    Q_UNUSED( e )
    return;
  }
  catch ( ... )
  {
    // catch any other errors
    return;
  }

  mMapSettings->setExtent( transformedRect );
}

QString BookmarkModel::addBookmarkAtPoint( QgsPoint point, QString name )
{
  if ( !mMapSettings )
    return QString();

  QgsRectangle extent = mMapSettings->extent();
  const QgsPointXY center = extent.center();

  const double xDiff = point.x() - center.x();
  const double yDiff = point.y() - center.y();

  extent.setXMinimum( extent.xMinimum() + xDiff );
  extent.setXMaximum( extent.xMaximum() + xDiff );
  extent.setYMinimum( extent.yMinimum() + yDiff );
  extent.setYMaximum( extent.yMaximum() + yDiff );

  QgsBookmark bookmark;
  bookmark.setExtent( QgsReferencedRectangle( extent, mMapSettings->destinationCrs() ) );
  bookmark.setName( name );
  return mManager->addBookmark( bookmark );
}

void BookmarkModel::updateBookmarkDetails( QString id, QString name )
{
  QgsBookmark bookmark = mManager->bookmarkById( id );
  bookmark.setName( name );
  mManager->updateBookmark( bookmark );
}

void BookmarkModel::removeBookmark( QString id )
{
  mManager->removeBookmark( id );
}
