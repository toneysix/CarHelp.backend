#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
    #elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif

//#if PLATFORM == PLATFORM_WINDOWS
//    #pragma comment( lib, "wsock32.lib" )
//#endif

class Address
{
    public:
        Address();
        Address( const unsigned int address, const unsigned short port );
        sockaddr_in getAddress();
        unsigned short GetPort() const;
        void setAddress( const unsigned int address, const unsigned short port );
        bool operator == ( const Address & other ) const;
        bool operator != ( const Address & other ) const;
        bool operator = ( const Address & other ) const;
    private:
        unsigned int address;
        unsigned short port;
};

class Socket
{
    public:
        Socket();
        ~Socket();
        bool open( unsigned short port );
        void close();
        bool isOpen() const;
        bool send( const Address & destination, const void * data, int size );
        int receive( Address & sender, void * data, int size );
    private:
        int handle;
};

