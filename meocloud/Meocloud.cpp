
#include <iostream>
#include "Meocloud.h"
#include <stdlib.h>
#include <vector>


using namespace std;

namespace Meocloud {



	/***************************************************************************************/
	
	HttpParameters::HttpParameters()
	{
		this->url = NULL;
	}


	void HttpParameters::Add(char * key, char* param)
	{
		if( key != NULL )
			this->params[key] = param;
	}

	void HttpParameters::SetUrl(char* url)
	{
		if( url != NULL )
			this->url = url;
	}
	void HttpParameters::SetCTX(CurlCTX *ctx)
	{
		if( ctx != NULL )
			this->ctx = ctx;
	}

	char* HttpParameters::toStr()
	{
		if( this->params.size() <= 0 )
			return NULL;

		vector<char*> outV, escList;
		bool isFirst = true;
		char *tmp, *outStr = NULL;
		size_t size = 1; // 1 extra byte for null-terminated string
		char *curPtr;
		size_t tBytes;

		if( this->url != NULL )
		{
			outV.push_back(this->url);
			outV.push_back("?");

			size += strlen(this->url) + 1 ;

		}

		for (map<char *, char*>::iterator it = this->params.begin(); it != this->params.end(); it++)
		{
			if( it->first == NULL ) continue;

			// Key

			tmp = curl_easy_escape(this->ctx->curl, it->first, 0);
			if( tmp == NULL ) continue;

			escList.push_back( tmp );

			if( isFirst )
				isFirst = false;

			else
			{
				outV.push_back("&");
				size += 1;
			}
			

			outV.push_back( tmp );
			size += strlen( tmp );


			// value

			if( it->second == NULL ) continue;

			tmp = curl_easy_escape(this->ctx->curl, it->second, 0);
			if( tmp == NULL ) continue;

			escList.push_back( tmp );

			outV.push_back("=");
			outV.push_back( tmp );

			size += 1 + strlen( tmp );
		}

		outStr = new char[size];

		if( outStr != NULL )
		{
			curPtr = outStr;

			for (vector<char *>::iterator it = outV.begin(); it != outV.end(); it++)
			{
				tBytes = strlen(*it);
				memcpy(curPtr, *it, tBytes);
					
				curPtr += tBytes;
			}

			*curPtr = 0;
		}

		for (vector<char *>::iterator it = escList.begin(); it != escList.end(); it++)
		{
			curl_free(*it);
		}

		return outStr;
	}






	/***************************************************************************************/

	//Http::Http()
	//{
	//}

	void Http::Init(void)
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}
	void Http::Terminate(void)
	{
		curl_global_cleanup();
	}

	static HttpResult* getHttpResult()
	{
		HttpResult *result = new HttpResult();

		if( result != NULL )
			memset(result, 0, sizeof(HttpResult));

		return result;
	}
	static CurlCTX* getContext()
	{
		CurlCTX *ctx = NULL;
		CURL *tmpcurl = curl_easy_init();

		if( !tmpcurl )
			throw MEOCLOUD_EXCEPTION_HTTP;

		ctx = new CurlCTX();

		if( ctx == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		memset(ctx, 0, sizeof(CurlCTX));

		ctx->curl = tmpcurl;

		return ctx;
	}



 
	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		size_t realsize = size * nmemb;
		CurlCTX *ctx = (CurlCTX *)userp;

		if( ctx->memory == NULL )
			ctx->memory = (char *)malloc( ctx->size + realsize + 1);
		else
			ctx->memory = (char*)realloc(ctx->memory, ctx->size + realsize + 1);

		if(ctx->memory == NULL)
			throw MEOCLOUD_EXCEPTION_HTTP;
 
		memcpy(&(ctx->memory[ctx->size]), contents, realsize);
		ctx->size += realsize;
		ctx->memory[ctx->size] = 0;
 
		return realsize;
	}






	void Http::releaseResult(HttpResult* result)
	{
		if( result != NULL )
		{
			if( result->url != NULL )
				delete result->url;

			if( result->ctx != NULL )
			{
				curl_easy_cleanup(result->ctx->curl);
				if(result->ctx->memory != NULL)
					free(result->ctx->memory);

				delete result->ctx;
			}

			delete result;
		}
	}




	HttpResult* Http::Request(const char* url, HttpParameters *params)
	{
		CurlCTX *ctx = getContext();
		HttpResult* result = getHttpResult();

		if( ctx == NULL || url == NULL || result == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		result->ctx = ctx;

		/* send all data to this function  */ 
		curl_easy_setopt(ctx->curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
		/* we pass our 'chunk' struct to the callback function */ 
		curl_easy_setopt(ctx->curl, CURLOPT_WRITEDATA, (void *)ctx);

		//curl_easy_setopt(ctx->curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(ctx->curl, CURLOPT_USERAGENT, "meocloud-uploader/1.0");

		if( params == NULL )
			curl_easy_setopt(ctx->curl, CURLOPT_URL, url);

		else
		{
			params->SetCTX(ctx);
			params->SetUrl((char *)url);

			result->url = params->toStr();

			curl_easy_setopt(ctx->curl, CURLOPT_URL, result->url);
		}

				//struct curl_slist *headerlist;
		//curl_easy_setopt(ctx->curl, CURLOPT_HTTPHEADER, headerlist);
 
		/* Perform the request, res will get the return code */ 
		result->curlStatus = curl_easy_perform(ctx->curl);

		curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &result->statusCode);

		/* Check for errors */ 
		if(result->curlStatus != CURLE_OK)
			result->curlErrorMsg = (char *)curl_easy_strerror(result->curlStatus);

		else
		{


		}

		return result;
	}



	/***************************************************************************************/


}