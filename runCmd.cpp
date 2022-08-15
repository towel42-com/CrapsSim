#include "runCmd.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool runCmd( QSqlQuery & query, bool ignoreError )
{
    if ( !query.exec() )
    {
        if ( !ignoreError )
        {
            qDebug() << query.lastError().driverText();
            qDebug() << query.lastError().databaseText();
            Q_ASSERT( 0 );
        }
        return false;
    }

    return true;
}
bool runCmd( const QString & cmd, const QList< QVariantList > & params, bool ignoreError )
{
    QSqlQuery query;
    query.clear();
    if ( !query.prepare( cmd ) )
    {
        if ( !ignoreError )
        {
            qDebug() << query.lastError().driverText();
            qDebug() << query.lastError().databaseText();
            Q_ASSERT( 0 );
        }
        return false;
    }

    for( int ii = 0; ii < params.count(); ++ii )
        query.addBindValue( params[ ii ] );
    
    if ( !query.execBatch() )
    {
        if ( !ignoreError )
        {
            qDebug() << query.lastError().driverText();
            qDebug() << query.lastError().databaseText();
            Q_ASSERT( 0 );
        }
        return false;
    }
    return true;
}


bool runCmd( QSqlQuery & query, const QString & cmd, const QList< QVariant > & params, bool ignoreError )
{
    query.clear();
    if ( !query.prepare( cmd ) )
    {
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().databaseText();
        Q_ASSERT( 0 );
        return false;
    }

    for( int ii = 0; ii < params.count(); ++ii )
        query.bindValue( ii, params[ ii ] );
    
    return runCmd( query, ignoreError );
}

bool runCmd( QSqlQuery & query, const QList< QVariant > & params, bool ignoreError )
{

    for( int ii = 0; ii < params.count(); ++ii )
        query.bindValue( ii, params[ ii ] );
    
    return runCmd( query, ignoreError );
}
