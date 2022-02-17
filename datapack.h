#ifndef DATAPACK_H
#define DATAPACK_H

#include <QTime>
#include <QDataStream>
#include <QStringList>


struct DataPack
{
    int                 type;
    QTime               time;
    QString             name_PC;
    QString             source;
    QString             target;
    QString             notify;
    QString             application_path;
    QString             working_time;
    QStringList         attributes;
    unsigned long long  file_size;
    QString             message;
};


//==============================================================================================================================================================
//перегрузка оператора потокового чтения для DataPack
//==============================================================================================================================================================
static inline QDataStream &operator >> (QDataStream &in, DataPack &data)
{
    in >> data.type;
    in >> data.time;
    in >> data.name_PC;
    in >> data.source;
    in >> data.target;
    in >> data.notify;
    in >> data.application_path;
    in >> data.working_time;
    in >> data.attributes;
    in >> data.file_size;
    in >> data.message;
    return in;
}



//==============================================================================================================================================================
//перегрузка оператора потоковой записи для DataPack
//==============================================================================================================================================================
static inline QDataStream &operator << (QDataStream &out, DataPack &data)
{
    out << data.type;
    out << data.time;
    out << data.name_PC;
    out << data.source;
    out << data.target;
    out << data.notify;
    out << data.application_path;
    out << data.working_time;
    out << data.attributes;
    out << data.file_size;
    out << data.message;
    return out;
}


#endif // DATAPACK_H
