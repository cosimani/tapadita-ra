#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QObject>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QVector>
#include <QStringList>

class Database: public QObject
{
    Q_OBJECT

private:
    static Database *instance;
    explicit Database( QObject *parent = NULL );

    QSqlDatabase database;

    bool connectDatabase();
    void disconnectDatabase();

public:
    static Database *getInstance();
    ~Database();

    /**
     * @brief checkBase crea las tablas sino estan creadas,
     * sirve para controlar estado de la bd
     * @return 1 si todo estaba creado, 0 si pudo crear las tablas,
     * -1 si no pudo crear las tablas o no se pudo conectar a la base de datos
     */
    int checkBase();

    /**
     * @brief insert_into inserta en la tabla que se la pasa como parametro
     * los valores en values. TIENEN QUE VENIR EN EL ORDEN CORRECTO
     * @param table tabla donde se inserta
     * @param values valores a insertar. si se quiere insertar null
     * @return
     */
    bool insert_into(QString table, QStringList values);


    /**
     * @brief insert_into inserta en table el mapa que viene como
     * parametro
     * @param table es el nombre de tabla
     * @param values mapa con key-values de la forma:
     * [nombres de columnas] = "valor a insertar"
     * @return true si todos lograron insertarse, false si alguno fallo
     * pero tal vez otros si pudieron insertarse
     */
    bool insert_into(QString table, QMap<QString, QString> values);

    /**
     * @brief getNumberOfColumns obtiene la cantidad de columnas de la tabla indicada
     */
    int getNumberOfColumns(QString table);

    /**
     * @brief ejecuta la consutla que se pasa por parametro, abre y cierra
     * conexion a la base de datos
    */
    bool ejecutarConsulta(QString q);
    bool ejecutarConsulta(QString q, QSqlQuery &query);


    /**
     * @brief getNumberOfRows cuenta la cantidad de filas de la tabla que se indica
     * @param table es la tabla donde verifica
     * @return cantidad de filas, -1 en caso de error
     */
    int getNumberOfRows(QString table);


    int getLastRow(QString table, QString column);

    // creadores de tablas
    bool createTableVinculos();
    bool createTableJugadores();

    bool saveVinculo(int marker_id, QString recurso , QString formatoCaja = "n");
    QVector<QStringList> readVinculos();
};

#endif // DATABASE_HPP
