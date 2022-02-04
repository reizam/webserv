#include "../webserv.hpp"

Client::Client() : _current_server_name(), _current_server_host(),_client_fd(-1), _read_fd(-1), _write_fd(-1) {}

Client::Client(std::string current_server_name, std::string current_server_host, int client_fd, struct sockaddr_in addr) : _current_server_name(current_server_name), _current_server_host(current_server_host), _client_fd(client_fd), _addr(addr), _read_fd(-1), _write_fd(-1) {}

Client::Client(Client const &src)
{
	(void)src;
}

Client::~Client()
{
	(void)this->_addr;
	if (this->_client_fd != -1)
		close(this->_client_fd);
}