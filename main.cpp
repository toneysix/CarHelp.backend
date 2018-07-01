#include <iostream>
#include "socket.h"
#include <unistd.h>

using namespace std;

int main()
{
    #if PLATFORM == PLATFORM_WINDOWS
    SetConsoleCP( 1251 );
    SetConsoleOutputCP( 1251 );
    #endif

	try
	{
		int thread_num=10;
		// read number of threads with io_services from command line, if provided
        thread_num=boost::lexical_cast<int>(1);
		ios_deque io_services;

		boost::thread_group thr_grp;
		// create threads for each io_service
		for (int i = 0; i < thread_num; ++i)
        {
			io_service_ptr ios(new ba::io_service);
			io_services.push_back( ios );
			// run io_service in their own thread
			thr_grp.create_thread( boost::bind( &ba::io_service::run, ios ) );
		}
		// create server
		server server(io_services);

		#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
		sleep( 1 ); // it might help, but no way
		#endif

		// wait until all thread will finished
		std::cout << "Waiting for incoming connections \n";
		thr_grp.join_all();
        while( true )
        {
            std :: cout << "Wrong start, please, restart the server \n";
            #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
            usleep( 50000 );
            #endif
            #if PLATFORM == PLATFORM_WINDOWS
            Sleep( 500 );
            #endif
        }
	}
	catch( std::exception& e )
	{
		std::cerr << e.what() << std::endl;
	}

    return 0;
}
