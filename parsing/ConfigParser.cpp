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

void ConfigParser::parse() {
	while (_pos < _tokens.size()) {
		if (_tokens[_pos] == "server") {
			_servers.push_back(parseServer());
		} else {
			throw std::runtime_error("Configuration error: unknown or misplaced directive '" + _tokens[_pos] + "'");
		}
			_pos++;
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
			newServer.addServerName(_tokens[_pos]);
			_pos++;
			checkSemicolon();
		}
		else if (_tokens[_pos] == "root") {
			_pos++;
			newServer.setRoot(_tokens[_pos]);
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
		else if (_tokens[_pos] == "autoindex") {
			_pos++;
			newLoc.autoindex = (_tokens[_pos++] == "on");
			checkSemicolon();
		}
		else {
			throw std::runtime_error("Configuration error: unknown directive '" + _tokens[_pos] + "' in location block");
			//_pos++;
		}
	}
		
	if (_pos >= _tokens.size())
		throw std::runtime_error("Configuration error: unexpected end of file (missing closing brace '}' for location block)");
	server.addLocation(newLoc);
}