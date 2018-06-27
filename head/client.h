#include "network.h"
#include <vector>

class Client
{
    public:
        Client( Address* address );
        Address* addr;
};

class ClientManager
{
    public:
        static bool AddClient( Client* client );
        static std :: vector< Client* > Clients;
};
