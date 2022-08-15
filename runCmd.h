#ifndef __RUNCMD_H
#define __RUNCMD_H

class QSqlQuery;
class QString;
class QVariant;
template < typename T > class QList;
typedef QList<QVariant> QVariantList;

bool runCmd( QSqlQuery & query, bool ignoreError=false );
bool runCmd( const QString & cmd, const QList< QVariantList > & params, bool ignoreError=false );
bool runCmd( QSqlQuery & query, const QString & cmd, const QList< QVariant > & params, bool ignoreError=false );
bool runCmd( QSqlQuery & query, const QList< QVariant > & params, bool ignoreError=false );

#endif