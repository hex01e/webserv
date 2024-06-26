/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-yous <mel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 11:06:06 by mel-yous          #+#    #+#             */
/*   Updated: 2024/05/02 19:34:45 by mel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()
{

}

/**
* `Parameterized constructor` that takes the path of config file.
* Tokenize the config file, check for syntax. Finally parse the servers.
* If no server found in config file I setup my default server.
* If no location found in one of the servers I setup a default location for that server.
* Check for logical errors.
* @param const std::string& configPath
* @return nothing
*/
Config::Config(const std::string& configPath)
{
    this->configPath = configPath;
}

Config::Config(const Config& obj)
{
    *this = obj;
}

Config& Config::operator=(const Config& obj)
{
    if (this == &obj)
        return *this;
    servers = obj.servers;
    tokens = obj.tokens;
    configPath = obj.configPath;
    return *this;
}

Config::~Config()
{
    tokens.clear();
    servers.clear();
    configPath.clear();
}

/**
 * The function parses a location directive in a configuration file and adds it to the server context.
 * @param tok_iter `tok_iter` is an iterator pointing to the current token in a vector of tokens. It is
 * passed by reference so that it can be updated within the function.
 * @param serverCtx The parameter `serverCtx` is of type `ServerContext` and is passed by reference to
 * the `parseLocation` function. It represents the context of the server configuration and is used to
 * store information about the server's locations & directives.
 * @return Nothing
 */
void Config::parseLocation(TokensVector::iterator& tok_iter, ServerContext& serverCtx)
{
    if (tok_iter == tokens.end())
        return ;
    t_directive d = Utils::getDirectiveFromTokenName(tok_iter->getContent());
    if (d == LOCATION)
    {
        tok_iter++;
        LocationContext locationCtx(tok_iter->getContent()); /*Create location object with -prefix-*/
        while (tok_iter != tokens.end() && tok_iter->getType() != CLOSED_BRACKET)
        {
            d = Utils::getDirectiveFromTokenName(tok_iter->getContent());
            parseDirective(tok_iter, locationCtx);
            tok_iter++;
        }
        serverCtx.addLocation(locationCtx);
    }
}

/* The `parseSingleValueDirectives` function in the `Config` class is responsible for parsing
directives that have a single value in the configuration file. It checks the type of directive based
on the token content, such as `LISTEN`, `ROOT`, `CLIENT_MAX_BODY_SIZE`, `AUTO_INDEX`, and
`UPLOAD_STORE`. */
void Config::parseSingleValueDirectives(TokensVector::iterator& tok_iter, Context& ctx)
{
    t_directive d = Utils::getDirectiveFromTokenName(tok_iter->getContent());
    std::string key = tok_iter->getContent();
    StringVector value;
    if (d == LISTEN || d == ROOT || d == CLIENT_MAX_BODY_SIZE
        || d == AUTO_INDEX || d == UPLOAD_STORE || d == CGI_MAX_TIMEOUT)
    {
        if (ctx.getDirectives().count(tok_iter->getContent()) != 0)
            throw SyntaxErrorException("`" + tok_iter->getContent() + "` directive is duplicated at line: ", tok_iter->getLineIndex());
        tok_iter++;
        value.push_back(tok_iter->getContent());
        ctx.addDirective(Directive(key, value));
    }
}

/* The `parseMultiValueDirectives` function in the `Config` class is responsible for parsing directives
that have multiple values in the configuration file. It checks the type of directive based on the
token content, such as `INDEX`, `ALLOWED_METHODS`, `SERVER_NAME`, and `RETURN`. */
void Config::parseMultiValueDirectives(TokensVector::iterator& tok_iter, Context& ctx)
{
    t_directive d = Utils::getDirectiveFromTokenName(tok_iter->getContent());
    std::string key = tok_iter->getContent();
    StringVector value;
    if (d == INDEX || d == ALLOWED_METHODS || d == SERVER_NAME || d == RETURN)
    {
        size_t j = ctx.getDirectives().count(tok_iter->getContent());
        if ((d == ALLOWED_METHODS || d == RETURN) && j > 0)
            throw SyntaxErrorException("`" + tok_iter->getContent() + "` directive is duplicated at line: ", tok_iter->getLineIndex());
        tok_iter++;
        while (tok_iter != tokens.end() && tok_iter->getType() != SEMICOLON)
            value.push_back((tok_iter++)->getContent());
        if (j > 0)
            ctx.appendDirective(Directive(key, value));
        else
            ctx.addDirective(Directive(key, value));
    }
    else if (d == ERROR_PAGE || d == CGI_ASSIGN)
    {
        tok_iter++;
        if (d == CGI_ASSIGN)
            ctx.addCGI(tok_iter->getContent(), (++tok_iter)->getContent());
        else
            ctx.addErrorPage(tok_iter->getContent(), (++tok_iter)->getContent());
    }
}

/** The `parseDirective` function is responsible for parsing a directive in the configuration file and
adding it to the server context.
*/
void Config::parseDirective(TokensVector::iterator& tok_iter, Context& ctx)
{
    if (tok_iter == tokens.end())
        return ;
    parseSingleValueDirectives(tok_iter, ctx);
    parseMultiValueDirectives(tok_iter, ctx);
}


/* The `parseServers()` function is responsible for parsing the configuration file.
* Iterate through the tokens when a server is found, an object from `ServerContext` is created
* After that the function calls `parseLocation` and `parseDirective` function
* to parse server locations & directives, finally the function add the server to the servers vector.
*/
void Config::parseServers()
{
    TokensVector::iterator tok_iter = tokens.begin();
    while (tok_iter != tokens.end())
    {
        if (tok_iter->getType() == OPEN_BRACKET)
        {
            ServerContext serverCtx;
            while (tok_iter != tokens.end() && tok_iter->getType() != CLOSED_BRACKET)
            {
                parseLocation(tok_iter, serverCtx);
                parseDirective(tok_iter, serverCtx);
                if (tok_iter != tokens.end())
                    tok_iter++;
            }
            servers.push_back(serverCtx);
        }
        if (tok_iter != tokens.end())
            tok_iter++;
    }
}

/* The `void Config::setupDefaultServer()` function is responsible for setting up a default server in
case no servers are found in the configuration file.*/
void Config::setupDefaultServer()
{
    if (servers.empty())
    {
        ServerContext defaultServer;
        servers.push_back(defaultServer);
        return;
    }
}

/* The `void Config::setupDefaultLocation()` function is responsible for setting up a default location
for each server in case no locations are found in the configuration file.*/
void Config::setupDefaultLocation()
{
    ServersVector::iterator serv_it = servers.begin();
    while (serv_it != servers.end())
    {
        if (serv_it->getLocations().empty())
            serv_it->addLocation(LocationContext("/"));
        serv_it++;
    }
}

void Config::inheritServerDirectives()
{
    ServersVector::iterator serv_it = servers.begin();
    while (serv_it != servers.end())
    {
        LocationsVector::iterator location_it = serv_it->getLocations().begin();
        while (location_it != serv_it->getLocations().end())
        {
            std::map<std::string, std::string>::iterator map_it = serv_it->getErrorPages().begin();
            while (map_it != serv_it->getErrorPages().end())
            {
                if (location_it->getErrorPages().find(map_it->first) == location_it->getErrorPages().end())
                    location_it->addErrorPage(map_it->first, map_it->second);
                map_it++;
            }
            map_it = serv_it->getCGI().begin();
            while (map_it != serv_it->getCGI().end())
            {
                if (location_it->getCGI().find(map_it->first) == location_it->getCGI().end())
                    location_it->addCGI(map_it->first, map_it->second);
                map_it++;
            }
            DirectivesMap::iterator it = serv_it->getDirectives().begin();
            while (it != serv_it->getDirectives().end())
            {
                if (it->first != "listen" && it->first != "server_name" && !location_it->getDirectives().count(it->first))
                        location_it->addDirective(*it);
                it++;
            }
            location_it++;
        }
        serv_it++;
    }
}

/* The `const ServersVector& Config::getServers() const` function is a getter method in the `Config`
class that returns a constant reference to the vector of servers (`ServersVector`). */
ServersVector& Config::getServers()
{
    return servers;
}

const std::string& Config::getConfigPath()
{
    return configPath;
}

void Config::parseConfigFile()
{
    try
    {
        tokens = Lexer::tokenize(configPath);
    }
    catch(const std::exception& e)
    {
        if (configPath == "webserv.conf")
            goto LABEL;
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    checkSyntax(tokens);
    parseServers();
    LABEL:
    setupDefaultServer();
    setupDefaultLocation();
    checkLogicalErrors(servers);
    inheritServerDirectives();
}

void Config::resetConfig()
{
    tokens.clear();
    servers.clear();
    // configPath.clear();
}