#include "client.h"
std :: vector< Client* > ClientManager :: Clients;

Client :: Client( Address* address )
{
    this->addr = address;
}


bool ClientManager :: AddClient( Client* client )
{
    Clients.push_back( client );
    return true;
}
