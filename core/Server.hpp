/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: houmanso <houmanso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 16:23:57 by houmanso          #+#    #+#             */
/*   Updated: 2024/02/17 16:51:59 by houmanso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ServerContext.hpp"

class Server
{
	private:
		int	port;
		int	sockID;
		sockaddr_in	addr;
		ServerContext	serverCTX;

	public:
		Server(void);
		Server(const Server& cpy);
		Server(const ServerContext& _serverCTX);

		std::string	getPORT_tmp(void) const;

		Server&	operator=(const Server& cpy);

		~Server(void);
};

#endif
