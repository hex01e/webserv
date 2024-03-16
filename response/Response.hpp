/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-yous <mel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:07:24 by mel-yous          #+#    #+#             */
/*   Updated: 2024/03/16 15:53:55 by mel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include "ServerContext.hpp"

// class Request;

class Response
{
    enum Status
    {
        FORBIDDEN = 403  
    };
    private:
        Request *request;
        Context *context;

        char buffer[1024];
        int fd;

        int statusCode;
        std::string headers;
        std::string body;
        std::string bodyPath;
        bool headersSent;
        bool responseDone;

        std::string generateHtmlErrorPage();
        bool checkErrorPage(const std::string& path);
        void generateResponseError();
        void prepareHeaders();
        void prepareGETBody();
        void prepareGET();

        static std::map<int, std::string> reasonPhrases;
        static std::map<std::string, std::string> mimeTypes;
    public:
        Response();
        Response(const Response& obj);
        Response& operator=(const Response& obj);
        ~Response();
        
        void setRequest(Request* request);
        void setContext(Context* context);
        void setHeadersSent(bool flag);
        static std::string getMimeType(const std::string& extension);
        const std::string& getBody() const;
        const std::string& getHeaders() const;
        bool getHeadersSent() const;
        bool responseIsDone() const;

        void prepareResponse();
        void resetResponse();

        static void initReasonPhrases();
        static void initMimeTypes();
};