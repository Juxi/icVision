#ifndef DISPLMATRIX_H
#define DISPLMATRIX_H

#include <QMatrix4x4>
#include <math.h>

class DisplMatrix : public QMatrix4x4
{
public:
    DisplMatrix()
    {
        this->setToIdentity();
    }
    DisplMatrix( const QVector3D& rotationAxis,
                 const QVector3D& translationAxis,
                 const qreal angle,
                 const qreal distance )
    {
        QVector3D rAxis = rotationAxis,
                  tAxis = translationAxis;
        if ( !qFuzzyCompare( rAxis.length(), 1 ) ) { rAxis.normalize(); }
        if ( !qFuzzyCompare( tAxis.length(), 1 ) ) { tAxis.normalize(); }
        const qreal Cos=cos( angle ),
                    Sin=sin( angle ),
                    Vers=1-Cos;

        QVector4D row;
        row.setX( rAxis.x()*rAxis.x()*Vers + Cos );
        row.setY( rAxis.x()*rAxis.y()*Vers - rAxis.z()*Sin );
        row.setZ( rAxis.x()*rAxis.z()*Vers + rAxis.y()*Sin );
        row.setW( distance*tAxis.x() );
        this->setRow( 0, row );

        row.setX( rAxis.x()*rAxis.y()*Vers + rAxis.z()*Sin );
        row.setY( rAxis.y()*rAxis.y()*Vers + Cos );
        row.setZ( rAxis.y()*rAxis.z()*Vers - rAxis.x()*Sin );
        row.setW( distance*tAxis.y() );
        this->setRow( 1, row );

        row.setX( rAxis.x()*rAxis.z()*Vers - rAxis.y()*Sin );
        row.setY( rAxis.y()*rAxis.z()*Vers + rAxis.x()*Sin );
        row.setZ( rAxis.z()*rAxis.z()*Vers + Cos );
        row.setW( distance*tAxis.z() );
        this->setRow( 2, row );

        row.setX( 0 ); row.setY( 0 ); row.setZ( 0 ); row.setW( 1 );
        this->setRow( 3, row );
    }
};

#endif // DISPLMATRIX_H
