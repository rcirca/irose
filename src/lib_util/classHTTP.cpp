
//#include "stdAFX.h"
// #include <VCL.h>
#include <windows.h>
#include "classLOG.h"
#include "classHTTP.h"

#pragma	comment (lib, "winINet.lib")

//-------------------------------------------------------------------------------------------------
classHTTP::classHTTP ()
{
	m_hInternet = NULL;
	m_hURL		= NULL;
//	m_hHttp		= NULL;
}
classHTTP::~classHTTP ()
{
	if ( m_hURL )
		::InternetCloseHandle( m_hURL );

	if ( m_hInternet )
		::InternetCloseHandle( m_hInternet );

	//if ( m_hHttp )
	//	::InternetCloseHandle( m_hHttp );
}

//-------------------------------------------------------------------------------------------------
bool classHTTP::Open ()
{
	// 인터넷 연결.
	m_hInternet = ::InternetOpen ("httpFILE", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if ( m_hInternet == NULL )
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------
void classHTTP::Close ()
{
    this->CloseURL ();
    
	if ( m_hInternet ) {
		::InternetCloseHandle( m_hInternet );
		m_hInternet = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
bool classHTTP::OpenURL (char *szFileURL)
{
	if ( m_hInternet == NULL )
		return false;
        
    if ( m_hURL != NULL ) {
        this->CloseURL ();
    }

	// URL을 연다.
	m_hURL = ::InternetOpenUrl (m_hInternet, szFileURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if ( m_hURL ) {
		DWORD dwBufLen=sizeof(DWORD), dwIndex=0;

		if ( ::HttpQueryInfo(m_hURL, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &m_dwStatusCODE, &dwBufLen, &dwIndex) ) {
			if ( m_dwStatusCODE == HTTP_STATUS_OK ) {
                return true;
            }
		}
	}

	this->CloseURL ();

	return false;
}

//-------------------------------------------------------------------------------------------------
void classHTTP::CloseURL ()
{
	if ( m_hURL ) {
		::InternetCloseHandle( m_hURL );
		m_hURL = NULL;
	}

  //  if ( m_hHttp ) {
		//::InternetCloseHandle( m_hHttp );
		//m_hHttp = NULL;
  //  }
}

//-------------------------------------------------------------------------------------------------
char *classHTTP::StatusCodeString (DWORD dwCode)
{
	switch( dwCode ) {
	case HTTP_STATUS_CONTINUE :	// (100)
		return "The request can be continued.";
	case HTTP_STATUS_SWITCH_PROTOCOLS :	// (101)
		return "The server has switched protocols in an upgrade header.";
	case HTTP_STATUS_OK :		// (200)
		return "The request completed successfully.";
	case HTTP_STATUS_CREATED :	// (201)
		return "The request has been fulfilled and resulted in the creation of a new resource.";
	case HTTP_STATUS_ACCEPTED :	// (202)
		return "The request has been accepted for processing, but the processing has not been completed.";
	case HTTP_STATUS_PARTIAL :	// (203)
		return "The returned meta information in the entity-header is not the definitive set available from the origin server.";
	case HTTP_STATUS_NO_CONTENT :	//  (204)
		return "The server has fulfilled the request, but there is no new information to send back.";
	case HTTP_STATUS_RESET_CONTENT :	// (205)
		return "The request has been completed, and the client program should reset the document view that caused the request to be sent to allow the user to easily initiate another input action.";
	case HTTP_STATUS_PARTIAL_CONTENT :	//  (206)
		return "The server has fulfilled the partial GET request for the resource.";
	case HTTP_STATUS_AMBIGUOUS :	//  (300)
		return "The server couldn't decide what to return.";
	case HTTP_STATUS_MOVED :	// (301)
		return "The requested resource has been assigned to a new permanent URI (Uniform Resource Identifier), and any future references to this resource should be done using one of the returned URIs.";
	case HTTP_STATUS_REDIRECT :	//  (302)
		return "The requested resource resides temporarily under a different URI (Uniform Resource Identifier).";
	case HTTP_STATUS_REDIRECT_METHOD :	//  (303)
		return "The response to the request can be found under a different URI (Uniform Resource Identifier) and should be retrieved using a GET HTTP verb on that resource.";
	case HTTP_STATUS_NOT_MODIFIED : // (304)
		return "The requested resource has not been modified.";
	case HTTP_STATUS_USE_PROXY : // (305)
		return "The requested resource must be accessed through the proxy given by the location field.";
	case HTTP_STATUS_REDIRECT_KEEP_VERB : // (307)
		return "The redirected request keeps the same HTTP verb. HTTP/1.1 behavior.";
	case HTTP_STATUS_BAD_REQUEST : // (400)
		return "The request could not be processed by the server due to invalid syntax.";
	case HTTP_STATUS_DENIED : // (401)
		return "The requested resource requires user authentication.";
	case HTTP_STATUS_PAYMENT_REQ : // (402)
		return "Not currently implemented in the HTTP protocol.";
	case HTTP_STATUS_FORBIDDEN : // (403)
		return "The server understood the request, but is refusing to fulfill it.";
	case HTTP_STATUS_NOT_FOUND : // (404)
		return "The server has not found anything matching the requested URI (Uniform Resource Identifier).";
	case HTTP_STATUS_BAD_METHOD : // (405)
		return "The HTTP verb used is not allowed.";
	case HTTP_STATUS_NONE_ACCEPTABLE : // (406)
		return "No responses acceptable to the client were found.";
	case HTTP_STATUS_PROXY_AUTH_REQ : // (407)
		return "Proxy authentication required.";
	case HTTP_STATUS_REQUEST_TIMEOUT : // (408)
		return "The server timed out waiting for the request.";
	case HTTP_STATUS_CONFLICT : // (409)
		return "The request could not be completed due to a conflict with the current state of the resource. The user should resubmit with more information.";
	case HTTP_STATUS_GONE : // (410)
		return "The requested resource is no longer available at the server, and no forwarding address is known.";
	case HTTP_STATUS_LENGTH_REQUIRED : // (411)
		return "The server refuses to accept the request without a defined content length.";
	case HTTP_STATUS_PRECOND_FAILED : // (412)
		return "The precondition given in one or more of the request header fields evaluated to false when it was tested on the server.";
	case HTTP_STATUS_REQUEST_TOO_LARGE : // (413)
		return "The server is refusing to process a request because the request entity is larger than the server is willing or able to process.";
	case HTTP_STATUS_URI_TOO_LONG : // (414)
		return "The server is refusing to service the request because the request URI (Uniform Resource Identifier) is longer than the server is willing to interpret.";
	case HTTP_STATUS_UNSUPPORTED_MEDIA : // (415)
		return "The server is refusing to service the request because the entity of the request is in a format not supported by the requested resource for the requested method.";
/*
	case HTTP_STATUS_RETRY_WITH : // (449)
		return "The request should be retried after doing the appropriate action.";
		break;
*/
	case HTTP_STATUS_SERVER_ERROR : // (500)
		return "The server encountered an unexpected condition that prevented it from fulfilling the request.";
	case HTTP_STATUS_NOT_SUPPORTED : // (501)
		return "The server does not support the functionality required to fulfill the request.";
	case HTTP_STATUS_BAD_GATEWAY : // (502)
		return "The server, while acting as a gateway or proxy, received an invalid response from the upstream server it accessed in attempting to fulfill the request.";
	case HTTP_STATUS_SERVICE_UNAVAIL : // (503)
		return "The service is temporarily overloaded.";
	case HTTP_STATUS_GATEWAY_TIMEOUT : // (504)
		return "The request was timed out waiting for a gateway.";
	case HTTP_STATUS_VERSION_NOT_SUP : // (505)
		return "The server does not support, or refuses to support, the HTTP protocol version that was used in the request message.";
	}

	return NULL;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
classHTTPDOWN::classHTTPDOWN ()
{
	;
}
classHTTPDOWN::~classHTTPDOWN ()
{
	;
}

//-------------------------------------------------------------------------------------------------
bool classHTTPDOWN::OpenURL (char *szFileURL)
{
	m_dwTotalSIZE = 0;
	m_dwTotalRECV = 0;

	if ( m_hInternet == NULL )
		return false;

    if ( m_hURL != NULL ) {
        this->CloseURL ();
    }

	// URL을 연다.
	m_hURL = ::InternetOpenUrl (m_hInternet, szFileURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if ( m_hURL ) {
		DWORD dwBufLen=sizeof(DWORD), dwIndex=0;

		if ( ::HttpQueryInfo(m_hURL, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &m_dwStatusCODE, &dwBufLen, &dwIndex) ) {
			if ( m_dwStatusCODE == HTTP_STATUS_OK ) {
				dwBufLen=sizeof(DWORD), dwIndex=0;
				if ( ::HttpQueryInfo(m_hURL, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &m_dwTotalSIZE, &dwBufLen, &dwIndex) )
					return true;
			}

			Log_String( 0xffff, "ERROR in OpenURL( %s ), Status: %s \n", this->StatusCodeString( this->m_dwStatusCODE ) );
		}
	}

	this->CloseURL ();
	return false;
}


//-------------------------------------------------------------------------------------------------
//bool classHTTPDOWN::OpenURL (char *szServer, char *szFile )
//{
//    m_hHttp = ::InternetConnect( m_hInternet, szServer, 0, "", "", INTERNET_SERVICE_HTTP, 0, 0 );
//    if ( NULL == m_hHttp )
//        return false;
//
//    m_hURL = ::HttpOpenRequest( m_hHttp, "GET", szFile, NULL, NULL, NULL, 0, 0 );
//    if ( m_hURL ) {
//        if ( ::HttpSendRequest( m_hURL, NULL, 0, NULL, 0 ) ) {
//            DWORD dwBufLen=sizeof(DWORD), dwIndex=0;
//
//            if ( ::HttpQueryInfo(m_hURL, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &m_dwStatusCODE, &dwBufLen, &dwIndex) ) {
//                if ( m_dwStatusCODE == HTTP_STATUS_OK ) {
//                    dwBufLen=sizeof(DWORD), dwIndex=0;
//                    if ( ::HttpQueryInfo(m_hURL, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &m_dwTotalSIZE, &dwBufLen, &dwIndex) )
//                        return true;
//                }
//
//                Log_String( 0xffff, "ERROR in OpenURL( %s ), Status: %s \n", this->StatusCodeString( this->m_dwStatusCODE ) );
//            }
//        }
//    }
//
//    this->CloseURL ();
//    return false;
//}

/*
bool classHTTP::OpenURL (char *szFileURL)
{
	if ( m_hInternet == NULL )
		return false;

    if ( m_hURL != NULL ) {
        this->CloseURL ();
    }

	// URL을 연다.
	m_hURL = ::InternetOpenUrl (m_hInternet, szFileURL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if ( m_hURL ) {
		DWORD dwBufLen=sizeof(DWORD), dwIndex=0;

		if ( ::HttpQueryInfo(m_hURL, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &m_dwStatusCODE, &dwBufLen, &dwIndex) ) {
			if ( m_dwStatusCODE == HTTP_STATUS_OK ) {
                return true;
            }
		}
	}

	this->CloseURL ();

	return false;
}*/
//-------------------------------------------------------------------------------------------------
/*
DWORD classHTTPDOWN::ReadFILE ()
{
	if ( m_hURL == NULL )
		return 0;

	DWORD dwSize;

	::InternetQueryDataAvailable( m_hURL, &dwSize, 0, 0 );
	if ( dwSize > MAX_READ_BUFF_SIZE ) dwSize = MAX_READ_BUFF_SIZE;

	::InternetReadFile (m_hURL, m_btBUFF, dwSize, &m_dwREAD);
	m_dwTotalRECV += m_dwREAD;

	return m_dwREAD;
}
*/
//-------------------------------------------------------------------------------------------------
DWORD classHTTPDOWN::ReadFILE (void *pBUFF, DWORD dwBuffSIZE)
{
	if ( m_hURL == NULL )
		return 0;
    if ( pBUFF == NULL )
        return 0;

	DWORD dwSize;

	::InternetQueryDataAvailable( m_hURL, &dwSize, 0, 0 );
	if ( dwSize > dwBuffSIZE ) dwSize = dwBuffSIZE;

	::InternetReadFile (m_hURL, pBUFF, dwSize, &m_dwREAD);
	m_dwTotalRECV += m_dwREAD;

	return m_dwREAD;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
