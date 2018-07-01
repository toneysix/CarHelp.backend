#include <iostream>
#include "socket.h"
#include <unistd.h>

using namespace std;

int main()
{
//    SetConsoleCP (1251);
//    SetConsoleOutputCP(1251);

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
		sleep(1);
		// wait until all thread will finished
		thr_grp.join_all();
       		std::cout << "Waiting for incoming connections \n";
        	while( true )
        	{
        		usleep( 50000 );
        	}
	}
	catch( std::exception& e )
	{
		std::cerr << e.what() << std::endl;
	}

    return 0;
}
