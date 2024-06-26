/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-yous <mel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 17:22:17 by houmanso          #+#    #+#             */
/*   Updated: 2024/05/02 13:13:06 by mel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Core.hpp"

std::vector<Server>	Core::servers;

Core::Core(void)
{

}

Core::Core(const Core &cpy)
{
	*this = cpy;
}

Core::Core(Config &conf)
{
	ServersVector::iterator	it;

	config = conf;
	serversConf = conf.getServers();
	it = serversConf.begin();
	while (it != serversConf.end())
		servers.push_back(Server(*it++));
	std::sort(servers.begin(), servers.end());
}

void	Core::run(void)
{
	size_t			binded;
	StringVector	ports;
	StringVector::iterator	p;
	std::vector<Server>::iterator	it;

	binded = 0;
	size = servers.size();
	std::cout << "Listening on: " << std::endl;
	for(int i = 0; i < size; i++)
	{
		p = std::find(ports.begin(), ports.end(), servers[i].getHostPort());
		if (p != ports.end())
			continue ;
		try
		{
			servers[i].setupServer();
			fcntl(servers[i].getSocketId(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			ports.push_back(servers[i].getHost() + ":" + servers[i].getPort());
			binded++;
		}
		catch (const std::exception& e)
		{
			std::cerr << servers[i].getHostPort() << " : " << e.what() << std::endl;
		}
	}
	Request::servers = servers;
	Response::initReasonPhrases();
	Response::initMimeTypes();
	if (binded > 0)
		traceEvents();
	else
		std::cout << "Info: no servers to bind !" << std::endl;
}

void	Core::traceEvents(void)
{
	int	fd;
	int	hooks;
	size_t	i;
	servers_it	it;
	std::string tmp = Utils::get_last_modified_date(config.getConfigPath());

	while (tmp == Utils::get_last_modified_date(config.getConfigPath()))
	{
		for (it = servers.begin(); it !=  servers.end(); it++)
		{
			fd = accept(it->getSocketId(), NULL, 0);
			if (fd != -1)
			{
				clients[fd].setSockId(fd);
				clients[fd].setServerCTX(it->getServerCTX());
				checklist.push_back((pollfd){fd, POLLIN | POLLOUT | POLLHUP, 0});
				clients[fd].setServersBegin(it - servers.begin());
				while (it + 1 != servers.end() && *it == *(it + 1))
					it++;
				clients[fd].setServersEnd(it - servers.begin() + 1);
			}
			else
			{
				while (it + 1 != servers.end() && *it == *(it + 1))
					it++;
			}
		}
		if (!checklist.size())
			continue ;
		// the requests header failing  (sending response  without waiting the empty line)
		hooks = poll(checklist.data(), checklist.size(), 0);
		for (i = 0; i < checklist.size(); i++)
		{
			try
			{
				Client &client = clients[checklist[i].fd];
				if (checklist[i].revents & POLLIN)
					client.recvRequest();
				if ((checklist[i].revents & POLLOUT) && client.isRequestDone())
					client.sendResponse();
				if (checklist[i].revents & POLLHUP || client.timeout()
					|| (client.isResponseDone() && !client.isALive()))
				{
					if (client.timeout())
						client.requestTimeout();
					clients.erase(checklist[i].fd);
					checklist.erase(checklist.begin() + i--);
				}
				else
					client.reset();
			}
			catch(const Fail &e)
			{
				clients.erase(checklist[i].fd);
				checklist.erase(checklist.begin() + i--);
			}
		}
	}
}

void	Core::setConfig(Config& conf)
{
	ServersVector::iterator	it;
	config = conf;
	serversConf = conf.getServers();
	it = serversConf.begin();
	while (it != serversConf.end())
		servers.push_back(Server(*it++));
	std::sort(servers.begin(), servers.end());
}

void Core::resetCore()
{
	config.resetConfig();
	serversConf.clear();
	clients.clear();
	checklist.clear();
	servers.clear();
	size = 0;
}

Core &Core::operator=(const Core &cpy)
{
	if (this != &cpy)
	{
		size = cpy.size;
		config = cpy.config;
		clients = cpy.clients;
		servers = cpy.servers;
		checklist = cpy.checklist;
		serversConf = cpy.serversConf;
	}
	return (*this);
}

Core::~Core(void)
{
}
