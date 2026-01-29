#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "Location.hpp"

class	ConfigServer {
	private:
		int							_port;
		std::vector<std::string>	_server_names;
		std::string					_host;
		std::string					_root;
		std::string					_index;
		unsigned int				_client_max_body_size;
		std::map<int, std::string>	_error_pages;
		std::vector<Location>		_locations;

	public:
		ConfigServer() :
			_port(8080),
			_host("127.0.0.1"),
			_root("/var/www/html"),
			_index("home.html"),
			_client_max_body_size(1000000){}

		// Setters
		void setPort(int port) { _port = port; }
		void setHost(const std::string& host) { _host = host; }
		void setRoot(const std::string& root) { _root = root; }
		void setMaxBodySize(unsigned long size) { _client_max_body_size = size; }
		void addServerName(const std::string& name) { _server_names.push_back(name); }
		void addErrorPage(int code, const std::string& path) { _error_pages[code] = path; }
		void addLocation(const Location& loc) { _locations.push_back(loc); }

		// Getters
		int getPort() const { return _port; }
		const std::string& getHost() const { return _host; }
		const std::string& getRoot() const { return _root; }
		const std::string& getIndex() const { return _index; }
		const std::vector<Location>& getLocations() const { return _locations; }
};

#endif