#ifndef TOTALMEMORY_H
#define TOTALMEMORY_H

#include "userbuildingprotocol.h"

class TotalMemory : public UserBuildingProtocol
{
private:
    UserBuildingProtocol* next;
public:
    TotalMemory( UserBuildingProtocol* next, UserBuilder* builder );
    virtual bool handle( QString line );
    ~TotalMemory();
};

#endif // TOTALMEMORY_H
