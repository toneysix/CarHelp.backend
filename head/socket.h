#define BOOST_ASIO_DISABLE_KQUEUE 1
#include "common.h"
#include <deque>
#include <boost/algorithm/string/regex.hpp>

typedef std::deque<io_service_ptr> ios_deque;

/**
 * Connection class, implementing async input/output
 *
 */
class connection : public boost::enable_shared_from_this<connection>
{
public:
	typedef boost::shared_ptr<connection> pointer;
	static std::vector<connection*> Connection;


	/**
	 * Create new connection
	 *
	 * @param io_service io_service in which this connection will work
	 *
	 * @return pointer to newly allocated object
	 */
	static pointer create(ba::io_service& io_service);

	/**
	 * Return socket, associated with this connection. This socket used in accept operation
	 *
	 *
	 * @return reference to socket
	 */
	ba::ip::tcp::socket& socket();
        void processPacket(std::string packet);

	/**
	 * Start input/output chain with reading of headers from browser
	 *
	 */
	void start();
	~connection();

private:
	/**
	 * Initialize connection
	 *
	 * @param io_service
	 *
	 * @return
	 */
	connection(ba::io_service& io_service);
	/**
	 * Called when data written to browser
	 *
	 * @param error object, containing information about errors
	 * @param bytes_transferred number of transferred bytes
	 */
	void handle_write(const boost::system::error_code& error,size_t bytes_transferred);
	/**
	 * Called when data readed from browser
	 *
	 * @param error object, containing information about errors
	 * @param bytes_transferred number of transferred bytes
	 */
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

	ba::ip::tcp::socket socket_; /**< socket, associated with browser */
	boost::asio::streambuf buf;  /**< buffer for request data */
	static std::string message_; /**< data, that we'll return to browser */
};


/**
 * Server class
 *
 */
class server
{
public:
/**
 * Initialize all needed data
 *
 * @param io_service reference to io_service
 * @param port port to listen on, by default - 4000
 */
	server(const ios_deque& io_services, int port=49669);

private:
	/**
	 * start connection accepting in async mode
	 *
	 */
	void start_accept();

	/**
	 * Run when new connection is accepted
	 *
	 * @param new_connection accepted connection
	 * @param error reference to error object
	 */
	void handle_accept(connection::pointer new_connection,
					   const boost::system::error_code& error);

	ios_deque io_services_;		     /**< deque of pointers to io_services */
	ba::ip::tcp::acceptor acceptor_; /**< object, that accepts new connections */
	std::vector<connection*> connections;
};
