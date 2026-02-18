#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "Location.hpp"
# include <poll.h>

class	ConfigServer {
	private:
		int								_port;
		std::vector<std::string>		_server_names;
		std::string						_host;
		std::string						_root;
		std::string						_index;
		unsigned long					_client_max_body_size;
		std::map<int, std::string>		_error_pages;
		std::map<std::string, Location>	_locations;
		bool							_autoindex;
		int								_listenerIndex;

	public:
		ConfigServer() :
			_port(8080),
			_host("127.0.0.1"),
			_root("/var/www/html"),
			_index("index.html"),
			_client_max_body_size(1000000),
			_autoindex(false) {}

		// Setters
		void setPort(int port) { _port = port; }
		void setHost(const std::string& host) { _host = host; }
		void setRoot(const std::string& root) { _root = root; }
		void setClientMaxBodySize(unsigned long size) { _client_max_body_size = size; }
		void setIndex(const std::string& index) { _index = index; }
		void setAutoIndex(bool value) { _autoindex = value; }
		void setListener(int listener) { _listenerIndex = listener; }

		void addServerName(const std::string& name) { _server_names.push_back(name); }
		void addErrorPage(int code, const std::string& path) { _error_pages[code] = path; }
		void addLocation(const Location& loc) { _locations[loc.path] = loc; }

		// Getters
		int getPort() const { return _port; }
		const std::string& getHost() const { return _host; }
		const std::string& getRoot() const { return _root; }
		const std::string& getIndex() const { return _index; }
		unsigned long getClientMaxBodySize() const { return _client_max_body_size; }
		bool getAutoIndex() const { return _autoindex; }
		const std::vector<std::string>& getServerNames() const { return _server_names; }
		const std::map<int, std::string>& getErrorPages() const { return _error_pages; }
		const std::map<std::string, Location>& getLocations() const { return _locations; }
		int getListener() { return _listenerIndex; }
};

#endif