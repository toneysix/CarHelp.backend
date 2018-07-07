
 /**
 * @file   test-mcmt.cpp
 * @author Alex Ott <alexott@gmail.com>
 *
 * @brief  Impelements many threads - many connections strategy of connections handling.
 * All input/output is async. Server use several io_service objects to scale to work on
 * multiprocessor/multicore systems
 *
 *
 */

#include "socket.h"


static connection :: pointer connection :: create(ba::io_service& io_service)
{
    return pointer(new connection(io_service));
}

ba::ip::tcp::socket& connection :: socket()
{
    return socket_;
}

void connection :: start()
{
    // start reading of headers from browser
    boost::asio::async_read_until(socket_, buf, boost::regex("\\W*(<end>)\\W*"),
        boost::bind(&connection::handle_read, shared_from_this(),
                ba::placeholders::error,
                ba::placeholders::bytes_transferred));
    std::cout << "connection start: " << std::endl;
}


connection :: connection(ba::io_service& io_service) : socket_(io_service)
{
    std::cout  << "connection created" << std::endl;
}


void connection :: handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    std::cerr << "handle_write" << std::endl;
}

#include <codecvt>
void connection :: handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    std::stringstream ss;

    pt :: ptree root;
    root.put( "type", "HELPER_LOCATION" );
    root.put( "lat", "54.736071" );
    root.put( "lon", "55.992301" );
    // Add the new node to the root
    // root.add_child( "header", root );
    pt :: write_json( ss, root );


    std::cerr << "handle WRITE (json):" << ss.str() << std::endl;


	std::string str = ss.str();
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstr = converter.from_bytes(str);
	std::string utf8 = converter.to_bytes(wstr);


    ba::async_write(socket_, ba::buffer(str),
                boost::bind(&connection::handle_write, shared_from_this(),
                            ba::placeholders::error,
                            ba::placeholders::bytes_transferred));
    std::istream is(&buf);
    std::string line;
    while( std::getline(is, line) )
   	 std::cout << "handle read: " << line << std::endl;

}

server :: server(const ios_deque& io_services, int port=49669) : io_services_(io_services), acceptor_(*io_services.front(), ba::ip::tcp::endpoint(ba::ip::address::from_string("192.168.0.106"), 4000))
{
    std::cerr << "server object has been created" << std::endl;
    /*boost::system::error_code ec;
    acceptor_.listen( boost :: asio :: socket_base :: max_connections, ec );
    if(ec)
    {
        std::cerr << "listen error: " << ec << std::endl;
    }
    else
        std::cerr << "listen ok: " << std::endl;*/
    start_accept();
}

void server :: start_accept()
{
    std::cerr << "start_accept" << std::endl;
    // select next io_service object
    io_services_.push_back(io_services_.front());
    io_services_.pop_front();
    // create new connection
    connection::pointer new_connection = connection::create(*io_services_.front());
    // start acceptor in async mode
    std::cerr << "start_accept 3" << std::endl;
    acceptor_.async_accept(new_connection->socket(), boost::bind(&server::handle_accept, this, new_connection, ba::placeholders::error));
    std::cerr << "start_accept 4" << std::endl;
}

void server :: handle_accept(connection::pointer new_connection, const boost::system::error_code& error)
{
    std::cerr << "handle_accept" << std::endl;
    if (!error)
    {
        new_connection->start();
        start_accept();
    }
}
