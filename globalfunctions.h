#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QString>

bool boolFromString(QString str)
{
    if(str == "1")
        return true;
    else
        return false;
}


#endif // GLOBALFUNCTIONS_H
