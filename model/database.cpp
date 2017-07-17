#include "database.hpp"
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QMapIterator>
#include <QMap>
#include "common.h"


Database *Database::instance = NULL;

Database::Database( QObject *parent ) :
    QObject( parent )
{
    database = QSqlDatabase::addDatabase( "QSQLITE" );
}

Database *Database::getInstance()
{
    if( !instance )
    {
        instance = new Database();
    }
    return instance;
}

Database::~Database()
{
    delete instance;
}

bool Database::connectDatabase()
{
#ifdef PORTABLE
    database.setDatabaseName( "./Files/db.sqlite" );
#else
    database.setDatabaseName( APPLICATION_PATH "Files/db.sqlite" );
#endif
    return database.open();
}

void Database::disconnectDatabase()
{
    database.close();
}

int Database::checkBase()
{
    // tengo que habilitar las foreign keys, sino no funcionan
    this->ejecutarConsulta("PRAGMA foreign_keys = ON;");
    this->ejecutarConsulta("delete from vinculos;");
    if( this->connectDatabase() )
    {
        if( database.tables().contains( "vinculos" ) &&
                database.tables().contains( "jugadores" ) )
        {
            this->disconnectDatabase();
            return 1;
        }
        else
        {
            bool qv = createTableVinculos();
            bool qj = createTableJugadores();

            this->disconnectDatabase();

            return qv && qj ? 0 : -1;
        }
    }
    else
    {
        qDebug() << "ERROR: No se logro conectar a la base";
        return -1;  // No se logro conectar a la base
    }
}

bool Database::insert_into(QString table, QStringList values)
{
    if(!this->connectDatabase()) return false;
    int cantValues = getNumberOfColumns(table);
    if(values.size() != cantValues) return false;

    QString query("insert into " + table + " values (");
    for(int i = 0; i < cantValues; i++) {
        query.append(values.at(i) + ",");
    }

    query.remove(query.size() - 1, 1);
    query.append(");");

    return ejecutarConsulta(query);
}

bool Database::insert_into(QString table, QMap<QString, QString> valuesMap)
{
    if(!this->connectDatabase()) return false;
    int cantValues = getNumberOfColumns(table);

    QString columns(" ( ");
    QString values(" values ( ");
    QMapIterator<QString, QString> i(valuesMap);
    while (i.hasNext()) {
        i.next();
//        cout << i.key() << ": " << i.value() << endl;
        columns.append(i.key() + " ,");
        values.append(i.value() + " ,");
    }
    values.remove(values.size() - 1, 1);
    columns.remove(columns.size() - 1, 1);
    columns.append(" ) ");
    values.append(" );");

    QString insert("insert into "+ table + columns + values);
//    qDebug() << "insertar:" << insert;

    return ejecutarConsulta(insert);
}

int Database::getNumberOfColumns(QString table)
{
    int columns = 0;
    if( this->connectDatabase() ) {
        // obtiene info de la tabla que se quiere consultar
        QSqlQuery query = database.exec( "PRAGMA table_info(" + table + ");" );
        while ( query.next() ){
            columns++;
        }
        this->disconnectDatabase();
    }
    return columns;
}

bool Database::createTableVinculos()
{
    QString queryString( "create table vinculos                            "
                         "(                                                "
                         "    marker_id      integer         primary key,  "
                         "    nro_jugador    integer         null,         "
                         "    recurso        varchar(100)    null,         "
                         "    formato_caja   varchar(5)      null          "
                         ")" );
    return Database::getInstance()->ejecutarConsulta(queryString);
}

bool Database::createTableJugadores()
{
    QString queryString( "create table jugadores                                         "
                         "(                                                              "
                         "    nro_jugador    integer         primary key AUTOINCREMENT,  "
                         "    nom_jugador    varchar(100)    not null,                   "
                         "    foto_perfil    varchar(200)    not null                    "
                         ")" );
    return Database::getInstance()->ejecutarConsulta(queryString);
}

bool Database::ejecutarConsulta(QString q)
{
    if(!this->connectDatabase()) return false;
    QSqlQuery query( database );
    if (!query.exec(q)){
        qDebug() << "ERROR:" << query.lastError() << query.lastQuery();
        this->disconnectDatabase();
        return false;
    }
    this->disconnectDatabase();
    return true;
}

bool Database::ejecutarConsulta(QString q, QSqlQuery &query)
{
    if(!this->connectDatabase()) return false;
    query = database.exec("");
    if (!query.exec(q)){
        qDebug() << "ERROR:" << query.lastError() << query.lastQuery();
        this->disconnectDatabase();
        return false;
    }
    this->disconnectDatabase();
    return true;
}

int Database::getNumberOfRows(QString table)
{
    QSqlQuery q;
    if (!ejecutarConsulta("select count(*) from " + table, q)) return -1;
    q.next();
    return q.value(0).toInt();
}

int Database::getLastRow(QString table, QString column)
{
    QSqlQuery q;
    if (!ejecutarConsulta("select * from "+ table +" order by " + column + " desc limit 1", q)) return -1;
    q.next();
    qDebug() << "getLastRow:" << q.value(0).toInt();
    return q.value(0).toInt();
}

bool Database::saveVinculo( int marker_id, QString recurso, QString formatoCaja )
{
    if( this->connectDatabase() )
    {
        QSqlQuery query( database );

        QString queryString( "INSERT OR REPLACE INTO vinculos (marker_id, recurso, formato_caja) "
                             "VALUES(" + QString::number( marker_id ) + ", '" + recurso + "', '" + formatoCaja + "');" );

        bool ok = query.exec( queryString );

        qDebug() << "metodo saveVinculo()" << query.lastError() << query.lastQuery();

        this->disconnectDatabase();
        return ok;
    }
    else
    {
        return false;
    }
}

QVector<QStringList> Database::readVinculos()
{
    QVector<QStringList> results;

    if( this->connectDatabase() )
    {
        QSqlQuery query = database.exec( "select marker_id, recurso, formato_caja from vinculos" );

        while ( query.next() )
        {
            QStringList registro;
            registro.append( query.value( 0 ).toString() );  // marker_id
            registro.append( query.value( 1 ).toString() );  // recurso
            registro.append( query.value( 2 ).toString() );  // formato_caja
            results.append(registro);
        }

        this->disconnectDatabase();
    }

    return results;
}
