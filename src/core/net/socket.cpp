
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

std::vector<connection*> connection :: Connection; 

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
    Connection.push_back( this );
    std::cout  << "connection created" << std::endl;
}


connection :: ~connection()
{
    Connection.erase(std::remove(Connection.begin(), Connection.end(), this), Connection.end());
    std::cout  << "connection destroyed" << std::endl;
}


void connection :: handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    std::cerr << "handle_write" << std::endl;
}

#include <codecvt>
void connection :: handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if( ( ba :: error :: eof == error ) || ( ba :: error :: connection_reset == error ) )
    {
        std::cerr << "GOOD BYE MY BOY" << std::endl;
        return;
    }

    std::istream is(&buf);
    std::string line;
    std::string packet = "";
    for (std::string line; std::getline(is, line); )
    {
	std::cout << std::endl;
        std::cout << line << std::endl;
	packet += line;
  
    }
    processPacket(packet);
    this->start();

}

void connection ::  processPacket( std::string packet )
{
    std::vector<std::string> packets;
    std::string delim = "<end>";
    boost::algorithm::split_regex( packets, packet, boost:: regex( "<end>" ) );

    std::stringstream sss;
    sss << packets[0].c_str();
    std::cout << packets[ 0 ] << std::endl;
    pt :: ptree readedRoot;
    pt::read_json(sss, readedRoot);
    std::string lat = readedRoot.get<std::string>("lat"); 
    std::string lon = readedRoot.get<std::string>("lon");

    std::stringstream ss;
    pt :: ptree root;
    root.put( "type", "HELPER_LOCATION" );
    root.put( "lat", lat );
    root.put( "lon", lon );
    // Add the new node to the root
    // root.add_child( "header", root );
    pt :: write_json( ss, root );
    ss << "<end>";


    std::cerr << "handle WRITE (json):" << ss.str() << std::endl;

    for (auto &conn : Connection) // access by reference to avoid copying
    {  
        if (conn != this)
        {
   		 ba::async_write(socket_, ba::buffer(ss.str()),
                	boost::bind(&connection::handle_write, shared_from_this(),
                            ba::placeholders::error,
                            ba::placeholders::bytes_transferred));

        }
    }    
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
