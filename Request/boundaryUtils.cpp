/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   boundaryUtils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aben-nei <aben-nei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 20:12:18 by aben-nei          #+#    #+#             */
/*   Updated: 2024/04/29 13:55:04 by aben-nei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

// function to check if the key exists in the map
String& Request::isExist(Map& headers, String key)
{
	if (headers.find(key) != headers.end())
		return headers[key];
	return headers[key] = "";
}

void	Request::removeFiles()
{
	for (size_t i = 0; i < tmpFiles.size(); i++)
		std::remove(tmpFiles[i].c_str());
}

// function to prepare the filename and return it
String Request::prepareFileName(String line)
{
	String filename = "";
	String extension = "";
	size_t posFile = line.find("filename=");
	size_t posName = line.find("name=");
	if (posFile != String::npos)
	{
		filename = line.substr(posFile + 10);
		posFile = filename.find("\"");
		if (posFile != String::npos)
			filename = filename.substr(0, filename.find("\""));
		else
			return (status = InternalServerError, requestIscomplete = true, "");
	}
	else if (posName != String::npos)
	{
		filename = line.substr(posName + 6);
		posFile = filename.find("\"");
		if (posFile != String::npos)
			filename = filename.substr(0, filename.find("\""));
		else
			return (status = InternalServerError, requestIscomplete = true, "");
	}
	if (filename.empty())
		return (filename);
	extension = Utils::getFileExtension(filename);
	// int random = std::rand() % 1000;
	filename = filename.substr(0, filename.find_last_of("."));
	filename = this->_path + filename + extension;
	if (fileExists(filename))
		filename = generatePath(filename);
	return (filename);
}

// function to create the boundary tmp file
void Request::createBoundaryTmpFile()
{
	if (_pathTmpFile.empty())
		_pathTmpFile = "/goinfre/boundary_" + Utils::numberToString(rand() % 1000);
	if (tmpFile < 0)
	{
		_pathTmpFile = generatePath(_pathTmpFile);
		tmpFile = open(_pathTmpFile.c_str(), O_CREAT | O_RDWR | O_APPEND, 0666);
		tmpFiles.push_back(_pathTmpFile);
	}
	if (!fileExists(_pathTmpFile))
		return (status = InternalServerError, requestIscomplete = true, void());
	this->bodySize += _body.size();
	if (bodySize > locationCTX.getClientMaxBodySize())
		return (removeFiles(), status = RequestEntityTooLarge, requestIscomplete = true, void());
	write(tmpFile, _body.c_str(), _body.size());
	std::string boundary = _headers["content-type"].substr(30);
	std::string startBoundary = "--" + boundary;
	std::string EndBoundary = "--" + boundary + "--";
	if (_body.find(EndBoundary) != String::npos)
	{
		_body.clear();
		this->_BoundaryComplete = true;
		close(tmpFile);
		tmpFile = -1;
	}
}
