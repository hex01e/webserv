/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-yous <mel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 12:09:37 by mel-yous          #+#    #+#             */
/*   Updated: 2024/02/23 10:26:51 by mel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <poll.h>

class Core
{
    private:
        Config config;
        std::vector<Server> servers;
        std::vector<Client> clients;
    public:
        Core();
        Core(const Config& _config);
        Core(const Core& obj);
        Core& operator=(const Core& obj);
        ~Core();

        void startWorking();
};