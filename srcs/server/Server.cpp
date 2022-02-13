#include "../webserv.hpp"

Server::Server() : _socket_fd(-1), _addr(), _clients() {}

Server::Server(Server const &src)
{
	*this = src;
}

Server::Server(ServerConfig &config) : _config(config), _socket_fd(-1), _addr(), _clients() {}

Server::~Server()
{
}

Server &	Server::operator=(Server const &src)
{
	this->_addr = src._addr;
	this->_clients = src._clients;
	this->_config = src._config;
	this->_max_fd = src._max_fd;
	this->_socket_fd = src._socket_fd;
	return (*this);
}

void	Server::init()
{
	int _enable = 1;

	memset(&this->_addr, 0, sizeof(this->_addr));
	this->_addr.sin_family = AF_INET; 
	this->_addr.sin_port = htons(this->_config.getPort());
	this->_addr.sin_addr = this->_config.getAddress();
	if ((this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw ServerException("socket()", std::string(strerror(errno)));
	if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEPORT, (char*)&_enable, sizeof(_enable)) < 0)
	// if (setsockopt(this->_socket_fd, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT, (char*)&_enable, sizeof(_enable)) < 0)
        throw ServerException("setsockopt()", std::string(strerror(errno)));
	if (bind(this->_socket_fd, (struct sockaddr*)&this->_addr, sizeof(this->_addr)) < 0)
        throw ServerException("bind()", std::string(strerror(errno)));
	if (listen(this->_socket_fd, 256) < 0)
        throw ServerException("listen()", std::string(strerror(errno)));
	if (fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK) < 0)
        throw ServerException("fcntl()", std::string(strerror(errno)));

	this->_max_fd = this->_socket_fd;

	std::cout << "[Server] Server listening on " << this->_config.getHost() << "..." << std::endl;
}

void	Server::accept_client(fd_set *rset)
{
	int	client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	memset(&client_addr, 0, client_len);
	if ((client_fd = accept(this->_socket_fd, (struct sockaddr*)&client_addr, &client_len)) == -1)
		throw ServerException("select()", std::string(strerror(errno)));
	if (client_fd > this->_max_fd)
		this->_max_fd = client_fd;
	getsockname(client_fd, (struct sockaddr*)&client_addr, &client_len);
	
	this->_clients.push_back(new Client(this->_config.getServerName(), this->_config.getHost(), client_fd, client_addr));

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
		throw ServerException("fcntl()", std::string(strerror(errno)));
	
	FD_SET(client_fd, rset);
	
	std::cout << "[Server] New client (FD: " << client_fd << ") connected to server " << this->_config.getServerName() << " (Host: " << this->_config.getHost() << ")" << std::endl;
}

void	Server::close_client(std::vector<Client*>::iterator &it)
{
	std::cout << "[Server] Client (FD: " <<  (*it)->getClientFD() << ") has been disconnected from server " << this->_config.getServerName() << " (Host: " << this->_config.getHost() << ")" << std::endl;
	delete *it;
	it = this->_clients.erase(it);
}

bool	Server::client_request(Client *client)
{
	std::string http_request = read_fd(client->getClientFD());
	
	if (!http_request.empty())
	{
		std::cout << "[Server] Client (FD: " <<  client->getClientFD() << ") has sent a request to server " << this->_config.getServerName() << " (Host: " << this->_config.getHost() << ")" << std::endl;
		client->setCurrentTime(get_current_time());
		this->_client_handler.handleRequest(http_request, *client, *this);
		return (true);
	}
	return (false);	
}

bool	Server::client_response(Client *client)
{
	this->_client_handler.handleResponse(*client, *this);
	std::cout << "[Server] Client (FD: " <<  client->getClientFD() << ") received response " << client->getResponse().getResponseCode() << " from server " << this->_config.getServerName() << " (Host: " << this->_config.getHost() << ") in " << (get_current_time() - client->getClientTime()) << "ms" << std::endl;
	client->setCurrentTime(get_current_time());
	write(client->getClientFD(), client->getResponse().getRawHeader().c_str(), client->getResponse().getRawHeader().size());
	std::vector<unsigned char> &body = client->getResponse().getBody();

	for (std::vector<unsigned char>::iterator it = body.begin();it != body.end();it++)
		write(client->getClientFD(), &(*it), 1);
	return (true);
}

void	Server::run(fd_set *rset, fd_set *wset)
{
	if (FD_ISSET(this->_socket_fd, rset))
	{
		try
		{
			this->accept_client(rset);
		}
		catch(std::exception const &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	for (std::vector<Client*>::iterator it = this->_clients.begin();it != this->_clients.end();)
	{
		if (FD_ISSET((*it)->getClientFD(), rset))
		{
			if (!this->client_request(*it))
			{
				this->close_client(it);
				continue ;
			}
			FD_SET((*it)->getClientFD(), wset);
		}
		if (FD_ISSET((*it)->getClientFD(), wset))
		{
			if (!this->client_response(*it))
			{
				this->close_client(it);
				continue ;
			}
		}
		if (!(*it)->isKeepAlive() || get_current_time() - (*it)->getClientTime() > 1)
			this->close_client(it);
		else
			++it;
	}

	FD_SET(this->_socket_fd, rset);
}