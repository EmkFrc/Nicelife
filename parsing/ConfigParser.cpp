#include "ConfigParser.hpp"
#include <cstdlib>

// 1. Parcourir les tokens
// 2. Detecter si on entre dans serveur ou location
// 3. Remplir les variables en gerant les erreurs (ex: lettres au lieu de chiffres, nombre trop grand...)

ConfigParser::ConfigParser(const std::vector<std::string>& tokens) : _tokens(tokens), _pos(0) {}

ConfigParser::~ConfigParser() {}

const std::vector<ConfigServer>& ConfigParser::getServers() const {
	return _servers;
}

void ConfigParser::checkSemicolon() {
	if (_pos >= _tokens.size() || _tokens[_pos] != ";")
		throw std::runtime_error("Configuration error: expected ';' after directive '" + _tokens[_pos - 1] + "'");
	_pos++;
}

void ConfigParser::checkBrace() {
	if (_pos >= _tokens.size() || _tokens[_pos] != "{")
		throw std::runtime_error("Configuration error: expected '{' to start the block");
	_pos++;
}

bool ConfigParser::isNumber(const std::string& s) {
	if (s.empty())
		return false;
	for (size_t i = 0; i < s.length(); i++) {
		if (!std::isdigit(s[i]))
			return false;
	}
	return true;
}

unsigned long ConfigParser::parseSize(std::string s) {
	if (s.empty())
		return 0;
	char unit = s[s.length() - 1];
	unsigned long multiplier = 1;

	if (unit == 'K')
		multiplier = 1024;
	else if (unit == 'M')
		multiplier = 1024 * 1024;
	else if (unit == 'G')
		multiplier = 1024 * 1024 * 1024;

	if (multiplier > 1)
		s.erase(s.length() - 1);
	if (!isNumber(s))
		throw std::runtime_error("Invalid size format: " + s);

	return static_cast<unsigned long>(std::atol(s.c_str())) * multiplier;
}

void ConfigParser::parse() {
	while (_pos < _tokens.size()) {
		if (_tokens[_pos] == "server") {
			ConfigServer newServer = parseServer();
			for (size_t i = 0; i < _servers.size(); ++i) {
				if (_servers[i].getPort() == newServer.getPort() && _servers[i].getHost() == newServer.getHost()) {
					throw std::runtime_error("Configuration error: multiple servers listening on "
						+ newServer.getHost() + ";" + static_cast<std::string>(_tokens[_pos - 1]));
				}
			}
			_servers.push_back(newServer);
		} else {
			throw std::runtime_error("Configuration error: unknown or misplaced directive '" + _tokens[_pos] + "'");
		}
	}
}

ConfigServer ConfigParser::parseServer() {
	ConfigServer newServer;
	_pos++;
		
	checkBrace();

	while (_pos < _tokens.size() && _tokens[_pos] != "}") {
		if (_tokens[_pos] == "listen") {
			_pos++;
			if (_pos >= _tokens.size() || !isNumber(_tokens[_pos]))
				throw std::runtime_error("Configuration error: 'listen' directive requires a port");

			int	port = std::atoi(_tokens[_pos].c_str());
			if (port < 0 || port > 65535)
				throw std::runtime_error("Configuration error: port out of range");

			newServer.setPort(std::atoi(_tokens[_pos].c_str()));
			_pos++;
			checkSemicolon();
		}
		else if (_tokens[_pos] == "server_name") {
			_pos++;
			if (_pos >= _tokens.size() || _tokens[_pos] == ";")
				throw std::runtime_error("Configuration error: server name needs at least one argument");
			while (_pos < _tokens.size() && _tokens[_pos] != ";") {
				newServer.addServerName(_tokens[_pos]);
				_pos++;
			}
			checkSemicolon();
		}
		else if (_tokens[_pos] == "root") {
			_pos++;
			std::string rootPath = _tokens[_pos];
			if (!rootPath.empty() && rootPath[rootPath.length() -1] != '/') {
				rootPath += "/";
			}
			newServer.setRoot(rootPath);
			_pos++;
			checkSemicolon();
		}
		else if (_tokens[_pos] == "client_max_body_size") {
			_pos++;
			if (_tokens[_pos][0] == '-') {
				throw std::runtime_error("Configuration error: client_max_body_size cannot be negative");
			}
			newServer.setClientMaxBodySize(parseSize(_tokens[_pos]));
			_pos++;
			checkSemicolon();
		}
		else if (_tokens[_pos] == "error_page") {
			_pos++;
			std::vector<int> codes;
	
			while (_pos < _tokens.size() && isNumber(_tokens[_pos])) {
				codes.push_back(std::atoi(_tokens[_pos++].c_str()));
			}
			if (codes.empty()) {
				throw std::runtime_error("Configuration error: 'error_page' directive requires at least one status code");
			}
			if (_pos >= _tokens.size() || _tokens[_pos] == ";")
				throw std::runtime_error("Configuration error: 'error_page' directive requires a path after the codes");
			std::string errorPath = _tokens[_pos++];
			for (size_t i = 0; i < codes.size(); ++i) {
				newServer.addErrorPage(codes[i], errorPath);
			}
			checkSemicolon();
		}
		else if (_tokens[_pos] == "index") {
			_pos++;
			newServer.setIndex(_tokens[_pos]);
			_pos++;
			checkSemicolon();
		}
		else if (_tokens[_pos] == "autoindex") {
			_pos++;
			newServer.setAutoIndex(_tokens[_pos] == "on");
			_pos++;
			checkSemicolon();
		}
		else if (_tokens[_pos] == "location") {
			parseLocation(newServer);
		}
		else {
			throw std::runtime_error("Configuration error: unknown directive '" + _tokens[_pos] + "' in server block");
		}
	}

	if (_pos >= _tokens.size() || _tokens[_pos] != "}")
		throw std::runtime_error("Configuration error: unexpected end of file (missing closing brace '}' for server block)");
	_pos++;
	return newServer;
}

void ConfigParser::parseLocation(ConfigServer& server) {
	Location newLoc;
		
	newLoc.root = server.getRoot(); 
		
	_pos++;
	if (_pos >= _tokens.size())
		throw std::runtime_error("Configuration error: 'location' directive requires a path");
	newLoc.path = _tokens[_pos++];
		
	checkBrace();

	while (_pos < _tokens.size() && _tokens[_pos] != "}") {
		if (_tokens[_pos] == "root") {
			_pos++;
			newLoc.root = _tokens[_pos++];
			checkSemicolon();
		}
		else if (_tokens[_pos] == "index") {
			_pos++;
			newLoc.index =_tokens[_pos++];
			checkSemicolon();
		}
		else if (_tokens[_pos] == "autoindex") {
			_pos++;
			newLoc.autoindex = (_tokens[_pos++] == "on");
			checkSemicolon();
		}
		else if (_tokens[_pos] == "allowed_methods") {
			_pos++;
			newLoc.allowed_methods["GET"] = false;
			newLoc.allowed_methods["POST"] = false;
			newLoc.allowed_methods["DELETE"] = false;
            while (_pos < _tokens.size() && _tokens[_pos] != ";") {
				std::string method = _tokens[_pos++];
				if (method == "GET" || method == "POST" || method == "DELETE") {
					newLoc.allowed_methods[method] = true;				
				}
				else {
					throw std::runtime_error("Unknow method: " + method);
				}
			}
            checkSemicolon();
        }
        else if (_tokens[_pos] == "return") {
			_pos++;
            newLoc.return_code = std::atoi(_tokens[_pos++].c_str());
            newLoc.return_url = _tokens[_pos++];
            checkSemicolon();
        }
        else if (_tokens[_pos] == "upload_enable") {
			_pos++;
            newLoc.upload_enable = (_tokens[_pos++] == "on");
            checkSemicolon();
        }
        else if (_tokens[_pos] == "upload_store") {
			_pos++;
            newLoc.upload_store = _tokens[_pos++];
            checkSemicolon();
        }
        else if (_tokens[_pos] == "cgi_extension") {
			_pos++;
            if (_pos + 1 >= _tokens.size())
				throw std::runtime_error("Configuration error: 'cgi_extension' directive requires an extension and a path");
			std::string ext = _tokens[_pos++];
			std::string bin = _tokens[_pos++];
            newLoc.cgi[ext] = bin;
            checkSemicolon();
        }
		else {
			throw std::runtime_error("Configuration error: unknown directive '" + _tokens[_pos] + "' in location block");
		}
	}
		
	if (_pos >= _tokens.size())
		throw std::runtime_error("Configuration error: unexpected end of file (missing closing brace '}' for location block)");
	_pos++;
	server.addLocation(newLoc);
}
