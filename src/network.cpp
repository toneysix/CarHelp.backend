#include "network.h"
#include "settings.h"
#include <stdio.h>

Socket :: Socket()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    WSAStartup( MAKEWORD(2,2), &WsaData );
#endif
}

Socket :: ~Socket()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif
}

bool Socket :: open( unsigned short port )
{
    handle = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( handle <= 0 )
    {
        printf( "failed to create socket\n" );
        return false;
    }

    if( NETWORK_DEBUG_ENABLED )
        printf( "Socket has been created with handle: %d \n", handle );

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl( ( 192 << 24 ) | ( 168 << 16 ) | ( 0 << 8 ) | 105 );
    address.sin_port = htons( ( unsigned short ) port );

    int bindRtrnCode;
    if ( ( bindRtrnCode = bind( handle, ( const sockaddr* ) &address, sizeof( sockaddr_in ) ) ) < 0 )
    {
        printf( "failed to bind socket\n" );
        return false;
    }

     if( NETWORK_DEBUG_ENABLED )
        printf( "Bind has been binded with return code: %d \n", bindRtrnCode );

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    int nonBlocking = 1;
    if ( fcntl( handle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
    {
        printf( "failed to set non-blocking socket\n" );
        return false;
    }
#elif PLATFORM == PLATFORM_WINDOWS
    DWORD nonBlocking = 1;
    if ( ioctlsocket( handle, FIONBIO, &nonBlocking ) != 0 )
    {
        printf( "failed to set non-blocking socket\n" );
        return false;
    }
#endif
    return true;
}

void Socket :: close()
{

}

bool Socket :: send( const Address & destination, const void * data, int size )
{
    connect( handle, ( const struct sockaddr* ) &( destination.getAddress() ), sizeof( sockaddr ) );
    int sent_bytes = sendto( handle, (const char*)data, size, 0, ( const struct sockaddr* ) &( destination.getAddress() ), sizeof( sockaddr ) );

    if ( sent_bytes != size )
    {
        printf( "failed to send packet: return value = %d, error: %d\n", sent_bytes, WSAGetLastError() );
        return false;
    }
    return true;
}

int Socket :: receive( Address & sender, void * data, int size )
{
#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif

    sockaddr_in from;
    socklen_t fromLength = sizeof( from );

    int received_bytes = recvfrom( handle, ( char* ) data, size, 0, ( sockaddr* ) &from, &fromLength );


    unsigned int from_address = ntohl( from.sin_addr.s_addr );
    unsigned int from_port = ntohs( from.sin_port );
    sender.setAddress( from_address, from_port );

    return received_bytes;
}

Address :: Address( const unsigned int address, const unsigned short port )
{
    this->address = address;
    this->port = port;
}

Address :: Address()
{

}

void Address :: setAddress( const unsigned int address, const unsigned short port )
{
    this->address = address;
    this->port = port;
}

sockaddr_in Address :: getAddress()
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( this->address );
    addr.sin_port = htons( this->port );
    return addr;
}




