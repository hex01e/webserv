/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: houmanso <houmanso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 16:23:57 by houmanso          #+#    #+#             */
/*   Updated: 2024/02/14 20:06:05 by houmanso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

class Server
{
	private:
		int	sockId;
		Server	serverCTX;

	public:
		Server(void);
		Server(const Server& cpy);

		Server&	operator=(const Server& cpy);

		~Server(void);
};

#endif
