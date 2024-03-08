/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-nei <aben-nei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 19:50:55 by houmanso          #+#    #+#             */
/*   Updated: 2024/03/08 03:20:54 by aben-nei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Client
{
	private:
		ssize_t	len;

		int		sockId;
		char	buff[1024];
		bool	requestDone;
		bool	responseDone;

		Config	config;
		Request request;
		Response response;
		ServerContext serverCTX;
	public:
		Client(void);
		Client(int sock);
		Client(const Client& cpy);

		int		recvRequest(void);
		void	sendResponse(void);

		void	setSockId(int sock);
		void	setServerCTX(const ServerContext& serverCTX);

		bool	isRequestDone(void) const;
		bool	isResponseDone(void) const;

		Client&	operator=(const Client& cpy);

		~Client(void);
};

#endif
