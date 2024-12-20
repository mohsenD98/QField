/***************************************************************************
  gnsspositioninformation.cpp - GnssPositionInformation
 ---------------------
 begin                : 1.12.2020
 copyright            : (C) 2020 by David Signer
 email                : david (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gnsspositioninformation.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QIODevice>
#include <QStringList>
#include <QTime>


GnssPositionInformation::GnssPositionInformation( double latitude, double longitude, double elevation, double speed, double direction,
                                                  const QList<QgsSatelliteInfo> &satellitesInView, double pdop, double hdop, double vdop, double hacc, double vacc,
                                                  QDateTime utcDateTime, QChar fixMode, int fixType, int quality, int satellitesUsed, QChar status, const QList<int> &satPrn,
                                                  bool satInfoComplete, double verticalSpeed, double magneticVariation, int averagedCount, const QString &sourceName,
                                                  bool imuCorrection, double orientation )
  : mLatitude( latitude )
  , mLongitude( longitude )
  , mElevation( elevation )
  , mSpeed( speed )
  , mDirection( direction )
  , mSatellitesInView( satellitesInView )
  , mPdop( pdop )
  , mHdop( hdop )
  , mVdop( vdop )
  , mHacc( hacc )
  , mVacc( vacc )
  , mHvacc( sqrt( ( pow( hacc, 2 ) + pow( hacc, 2 ) + pow( vacc, 2 ) ) / 3 ) )
  , mUtcDateTime( utcDateTime )
  , mFixMode( fixMode )
  , mFixType( fixType )
  , mQuality( quality )
  , mSatellitesUsed( satellitesUsed )
  , mStatus( status )
  , mSatPrn( satPrn )
  , mSatInfoComplete( satInfoComplete )
  , mVerticalSpeed( verticalSpeed )
  , mMagneticVariation( magneticVariation )
  , mAveragedCount( averagedCount )
  , mSourceName( sourceName )
  , mImuCorrection( imuCorrection )
  , mOrientation( orientation )
{
}

bool GnssPositionInformation::operator==( const GnssPositionInformation &other ) const
{
  // clang-format off
  return mLatitude == other.mLatitude &&
         mLongitude == other.mLongitude &&
         mElevation == other.mElevation &&
         mSpeed == other.mSpeed &&
         mDirection == other.mDirection &&
         mPdop == other.mPdop &&
         mHdop == other.mHdop &&
         mVdop == other.mVdop &&
         mHacc == other.mHacc &&
         mVacc == other.mVacc &&
         mUtcDateTime == other.mUtcDateTime &&
         mFixMode == other.mFixMode &&
         mQuality == other.mQuality &&
         mStatus == other.mStatus &&
         mSatPrn == other.mSatPrn &&
         mSatInfoComplete == other.mSatInfoComplete &&
         mVerticalSpeed == other.mVerticalSpeed &&
         mMagneticVariation == other.mMagneticVariation &&
         mSourceName == other.mSourceName &&
         mImuCorrection== other.mImuCorrection &&
         mOrientation == other.mOrientation;
  // clang-format on
}

bool GnssPositionInformation::isValid() const
{
  bool valid = false;
  if ( mStatus == 'V' || mFixType == NMEA_FIX_BAD || mQuality == 0 ) // some sources say that 'V' indicates position fix, but is below acceptable quality
  {
    valid = false;
  }
  else if ( mFixType == NMEA_FIX_2D )
  {
    valid = true;
  }
  else if ( mStatus == 'A' || mFixType == NMEA_FIX_3D || mQuality > 0 ) // good
  {
    valid = true;
  }

  return valid;
}

GnssPositionInformation::FixStatus GnssPositionInformation::fixStatus() const
{
  FixStatus fixStatus = NoData;

  // no fix if any of the three report bad; default values are invalid values and won't be changed if the corresponding NMEA msg is not received
  if ( mStatus == 'V' || mFixType == NMEA_FIX_BAD || mQuality == 0 ) // some sources say that 'V' indicates position fix, but is below acceptable quality
  {
    fixStatus = NoFix;
  }
  else if ( mFixType == NMEA_FIX_2D ) // 2D indication (from GGA)
  {
    fixStatus = Fix2D;
  }
  else if ( mStatus == 'A' || mFixType == NMEA_FIX_3D || mQuality > 0 ) // good
  {
    fixStatus = Fix3D;
  }
  return fixStatus;
}

QString GnssPositionInformation::qualityDescription() const
{
  QString quality;
  switch ( mQuality )
  {
    case 8:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Simulation mode" );
      break;
    case 7:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Manual input mode" );
      break;
    case 6:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Estimated" );
      break;
    case 5:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Float RTK" );
      break;
    case 4:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Fixed RTK" );
      break;
    case 3:
      quality = QCoreApplication::translate( "QgsGpsInformation", "PPS" );
      break;
    case 2:
      quality = QCoreApplication::translate( "QgsGpsInformation", "DGPS" );
      break;
    case 1:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Autonomous" );
      break;
    case 0:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Invalid" );
      break;
    default:
      quality = QCoreApplication::translate( "QgsGpsInformation", "Unknown (%1)" ).arg( QString::number( mQuality ) );
  }

  if ( mImuCorrection )
    quality.append( QCoreApplication::translate( "QgsGpsInformation", " + IMU" ) );

  return quality;
}

QString GnssPositionInformation::fixStatusDescription() const
{
  return QString( QMetaEnum::fromType<FixStatus>().valueToKey( fixStatus() ) );
}

QDataStream &operator<<( QDataStream &stream, const GnssPositionInformation &position )
{
  return stream << position.latitude() << position.longitude() << position.elevation() << position.speed() << position.direction()
                << position.satellitesInView() << position.hdop() << position.vdop() << position.pdop()
                << position.hacc() << position.vacc() << position.hvacc() << position.utcDateTime()
                << position.fixMode() << position.fixType() << position.quality()
                << position.satellitesUsed() << position.status() << position.satPrn() << position.satInfoComplete()
                << position.verticalSpeed() << position.magneticVariation() << position.sourceName()
                << position.averagedCount() << position.imuCorrection() << position.orientation();
}

QDataStream &operator>>( QDataStream &stream, GnssPositionInformation &position )
{
  bool boolValue = false;
  int intValue = 0;
  double doubleValue = 0.0;
  QString stringValue;
  QDateTime dateTimeValue;
  QChar charValue;

  stream >> doubleValue;
  position.setLatitude( doubleValue );
  stream >> doubleValue;
  position.setLongitude( doubleValue );
  stream >> doubleValue;
  position.setElevation( doubleValue );
  stream >> doubleValue;
  position.setSpeed( doubleValue );
  stream >> doubleValue;
  position.setDirection( doubleValue );

  QList<QgsSatelliteInfo> satellitesInView;
  stream >> satellitesInView;
  position.setSatellitesInView( satellitesInView );

  stream >> doubleValue;
  position.setHdop( doubleValue );
  stream >> doubleValue;
  position.setVdop( doubleValue );
  stream >> doubleValue;
  position.setPdop( doubleValue );

  stream >> doubleValue;
  position.setHacc( doubleValue );
  stream >> doubleValue;
  position.setVacc( doubleValue );
  stream >> doubleValue;
  position.setHVacc( doubleValue );

  stream >> dateTimeValue;
  position.setUtcDateTime( dateTimeValue );

  stream >> charValue;
  position.setFixMode( charValue );

  stream >> intValue;
  position.setFixType( intValue );
  stream >> intValue;
  position.setQuality( intValue );
  stream >> intValue;
  position.setSatellitesUsed( intValue );

  stream >> charValue;
  position.setStatus( charValue );

  QList<int> satPrn;
  stream >> satPrn;
  position.setSatPrn( satPrn );

  stream >> boolValue;
  position.setSatInfoComplete( boolValue );
  stream >> doubleValue;
  position.setVerticalSpeed( doubleValue );
  stream >> doubleValue;
  position.setMagneticVaritation( doubleValue );
  stream >> stringValue;
  position.setSourceName( stringValue );
  stream >> intValue;
  position.setAveragedCount( intValue );
  stream >> boolValue;
  position.setImuCorrection( boolValue );
  stream >> doubleValue;
  position.setOrientation( doubleValue );

  return stream;
}

QDataStream &operator<<( QDataStream &stream, const QgsSatelliteInfo &satelliteInfo )
{
  return stream << satelliteInfo.azimuth << satelliteInfo.elevation << satelliteInfo.id << satelliteInfo.inUse << satelliteInfo.satType << satelliteInfo.signal;
}

QDataStream &operator>>( QDataStream &stream, QgsSatelliteInfo &satelliteInfo )
{
  return stream >> satelliteInfo.azimuth >> satelliteInfo.elevation >> satelliteInfo.id >> satelliteInfo.inUse >> satelliteInfo.satType >> satelliteInfo.signal;
}
