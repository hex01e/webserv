/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fillRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: houmanso <houmanso@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:26:57 by aben-nei          #+#    #+#             */
/*   Updated: 2024/03/15 17:26:35 by houmanso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

//function to fill the headers to the map
void	Request::fillHeaders(Vector headers)
{
	int checkHostIsFound = 0;
	Vector::iterator it;
	it = headers.begin();
	this->requestInProgress == false ? it++ : it;
	for (; it != headers.end(); it++)
	{
		Utils::toLower(*it);
		String key;
		String value;
		size_t pos =  it->find(':');
		if (pos != String::npos)
		{
			key = it->substr(0, pos);
			if (key == "host")
				checkHostIsFound++;
			value = Utils::strTrim(Utils::strTrim(it->substr(pos + 1), CR), ' ');
			if (key == "host")
			{
				if (value.empty())
					return(this->status = BadRequest, requestIscomplete = true, void());
				selectServerContext();
			}
			this->_headers[key] = value;
		}
	}
	if (checkHostIsFound != 1)
	{
		this->status = BadRequest;
		requestIscomplete = true;
		return;
	}
	this->headersDone = true;
}

//function to Check if the request Line is well formed and fill it to the map
void	Request::fillRequestLine(const String& requestLine)
{
	if (!requestLine.size())
		return(this->status = BadRequest, requestIscomplete = true, void());
	Vector requestLineVector;
	Map requestLineMap;
	requestLineVector = Utils::split(requestLine, ' ');
	if (requestLineVector.size() != 3)
		return(this->status = BadRequest, requestIscomplete = true, void());
	if (requestLineVector.front() != "GET" && requestLineVector.front() != "POST"
		&& requestLineVector.front() != "DELETE")
	{
		if (requestLineVector.front() != "HEAD" && requestLineVector.front() != "PUT"
			&& requestLineVector.front() != "CONNECT" && requestLineVector.front() != "OPTIONS"
				&& requestLineVector.front() != "TRACE")
				return(this->status = MethodNotAllowed, requestIscomplete = true, void());
		return(this->status = NotImplemented, requestIscomplete = true, void());
	}
	if (requestLineVector[1].front() != '/' || requestLineVector[2] != "HTTP/1.1")
		return (this->status = BadRequest, requestIscomplete = true, void());
	requestLineMap["method"] = requestLineVector[0];
	requestLineMap["path"] = requestLineVector[1];
	this->requestLineDone = true;
	this->requestLine = requestLineMap;	
	fillParams();
}

void	Request::fillParams()
{
	Vector sepPath;
	String path = this->requestLine["path"];
	if (path.find('?') == String::npos)
		return;
	sepPath = Utils::split(path, '?');
	this->requestLine["path"] = sepPath[0];
	if (sepPath.size() > 1)
	{
		Vector sepParams;
		sepParams = Utils::split(sepPath[1], '&');
		for (size_t i = 0; i < sepParams.size(); i++)
		{
			Vector sepParam;
			sepParam = Utils::split(sepParams[i], '=');
			if (sepParam.size() == 2)
				this->params[sepParam[0]] = sepParam[1];
		}
	}
}