/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-yous <mel-yous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 21:57:14 by aben-nei          #+#    #+#             */
/*   Updated: 2024/04/28 10:52:17 by mel-yous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Server.hpp"
#include <fcntl.h>
#include <sys/stat.h> // For mkdir


#define BUFFER_SIZE 102400

#define CR '\r'
#define CRLF "\r\n"
typedef std::vector<std::string> Vector;
typedef std::map<std::string, std::string> Map;
typedef std::string String;

class Core;
class Config;

class Request
{
	private:
		//status
		int		status;
		bool	foundUri;
		bool	bodyDone;
		bool	chunkedBoundary;
		int		detectHost;
		bool	_setLength;
		bool	headersDone;
		bool	isComplete;
		int		receivecount;
		bool	requestLineDone;
		bool	requestInProgress;
		bool	requestIscomplete;
		bool	_requestIsWellFormed;
		bool	_chunkedComplete;
		bool	_BoundaryComplete;
		bool	requestLineInProgress;

		char	buffer[BUFFER_SIZE];
		int		tmpFile;
		// serverctxs range
		size_t	serv_end;
		size_t	serv_begin;
		//unsigned int
		long long		bodySize;
		long long		contentLength;
		size_t			remainingChunkLength;
		unsigned int	remainingChunk;
		//config
		ServerContext serverCTX;
		LocationContext locationCTX;
		// Context			*context;
		// LocationContext	locationCtx;
		//maps
		Map	_headers;
		Map	requestLine;
		//vector
		Vector	requestVector;
		Vector	tmpFiles;
		//strings
		String	_path;
		String	_pathTmpFile;
		String	_params;
		String	_body;
		String	headers;
		String	rawExtension;
		String	requestData;
		String	queryString;
		String	requestLineData;
		String	boundaryName;
		String	_chunkedName;
		//time
		time_t	lastTime;

		/* *************************** methods ********************************* */
			void			findUri();
			void			parseUri();
			void			parseBody();
			void			fillParams();
			int				preparName();
			void			removeFiles();
			void			parseBoundary();
			void			ContentLength();
			void			parseChunkedBody();
			void			parseContentType();
			void			setUploadingPath();
			void			parseContentLength();
			void 			getExtenstionOfBody();
			void			requestIsWellFormed();
			void			createChunkedTmpFile();
			void			createBoundaryTmpFile();
			void			parseTransferEncoding();
			bool			writeInfile(int fdFile);
			void			isMethodAllowedInLocation();
			void			fillHeaders(Vector headers);
			String			prepareFileName(String line);
			String			generatePath(String fileName);
			unsigned int	convertToDecimal(String hex);
			void			preparLength(String& length);
			String& 		isExist(Map& headers, String key);
			void			readBytes(int fd, ssize_t& bytesRead);
			int				takingRequests(String receivedRequest);
			bool			fileExists(const std::string& fileName);
			void			separateRequest(String receivedRequest);
			void			fillRequestLine(const String& requestLine);
			int				parseRequestLine(const String& requestLine);
			int				checkDuplicate(const String& receivedRequest);
			bool			getBoundaryName(String startBoundary, int &file);
			int				writeInFile(int& file, String eBoundary, String sBoundary);
	public:
	/* *************************** constructors ****************************** */
	
		Request();
		~Request();
		Request(const Request& obj);
		Request& operator=(const Request& obj);

		
		static std::vector<Server>	servers;

	/* *************************** methods ****************************** */
		void	resetRequest();
		time_t	getLastTime() const;
		void	selectServerContext(const String& host);
		void	setServerCTXEnd(size_t i);
		void	setServerCTXBegin(size_t i);
		void	parseRequest(const std::string& receivedRequest, ServerContext serverCTX);

	/* *************************** getters ************************************ */
		bool					isDone(void) const;
		const String 			getHost(void) const;
		const String&			getBody(void) const;
		void					setStatus(int status);
		const String 			getMethod(void) const;
		const int&				getStatus(void) const;
		const Map& 				getHeaders(void) const;
		const bool& 			getBodyDone(void) const;
		const bool& 			getFoundUri(void) const;
		const bool& 			getHeadersDone(void) const;
		const Map& 				getRequestLine(void) const;
		const std::string		getRequestPath(void) const;
		const ServerContext&	getServerCTX(void) const;
		const LocationContext&	getLocationCtx(void) const;
		bool					hostIsDetected(void) const;
		const bool& 			getRequestLineDone(void) const;
		const bool& 			getRequestIsWellFormed(void) const;
		const String 			getHeaderByName(const String& name) const;
		std::string 			getParams();

		bool			hasCgi(void);
		String			getProtocol(void);
		String			getBodyPath(void);
		String			getQueryString(void);
};
