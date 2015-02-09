

#include "Http.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <sstream>

using namespace std;

namespace Http {



	/********************************************************************************************
	 *
	 *	HttpParameters
	 *
	 ********************************************************************************************/
	
	HttpParameters::HttpParameters(c_str url)
	{
		this->params = NULL;
		this->url = NULL;

		this->SetUrl(url);
	}

	HttpParameters::~HttpParameters()
	{
		if( this->params != NULL )
			delete this->params;

		if( this->url != NULL )
			delete[] this->url;
	}


	void HttpParameters::Add(c_str key, c_str param)
	{
		if( this->params == NULL )
			this->params = new std::map<c_str, c_str>();

		if( key != NULL )
			(*this->params)[key] = param;
	}

	void HttpParameters::SetUrl(c_str url)
	{
		if( this->url != NULL )
			delete[] this->url;

		this->url = (url == NULL) ? NULL : StrCopy(url);
	}
	void HttpParameters::SetCTX(CurlCTX *ctx)
	{
		if( ctx != NULL )
			this->ctx = ctx;
	}

	str HttpParameters::toStr()
	{
		if( this->params->size() <= 0 )
		{
			if(this->url == NULL)
				return NULL;

			return StrCopy(this->url);
		}

		bool hadCurl;
		CURL* curlPtr = NULL;

		vector<str> outV, escList;
		bool isFirst = true;
		str tmp, outStr = NULL;
		size_t size = 1; // 1 extra byte for null-terminated string
		str curPtr;
		size_t tBytes;

		if( this->ctx != NULL && this->ctx->curl != NULL )
		{
			hadCurl = true;
			curlPtr = this->ctx->curl;
		}
		else
		{
			hadCurl = false;
			curlPtr = curl_easy_init();

			if( !curlPtr )
				throw MEOCLOUD_ERROR_HTTP;
		}

		if( this->url != NULL )
		{
			outV.push_back(this->url);
			outV.push_back((str)"?");

			size += strlen(this->url) + 1 ;

		}

		// build it

		for (map<c_str, c_str>::iterator it = this->params->begin(); it != this->params->end(); it++)
		{
			if( it->first == NULL ) continue;

			// Key

			tmp = curl_easy_escape(curlPtr, it->first, 0);
			if( tmp == NULL ) continue;

			escList.push_back( tmp );

			if( isFirst )
				isFirst = false;

			else
			{
				outV.push_back((str)"&");
				size += 1;
			}
			

			outV.push_back( tmp );
			size += strlen( tmp );


			// value

			if( it->second == NULL ) continue;

			tmp = curl_easy_escape(curlPtr, it->second, 0);
			if( tmp == NULL ) continue;

			escList.push_back( tmp );

			outV.push_back((str)"=");
			outV.push_back( tmp );

			size += 1 + strlen( tmp );
		}

		outStr = new char[size];

		// concatenate everything
		if( outStr != NULL )
		{
			curPtr = outStr;

			for (vector<str>::iterator it = outV.begin(); it != outV.end(); it++)
			{
				tBytes = strlen(*it);
				memcpy(curPtr, *it, tBytes);
					
				curPtr += tBytes;
			}

			*curPtr = 0; // null terminated the string
		}

		// cleanup
		for (vector<str>::iterator it = escList.begin(); it != escList.end(); it++)
		{
			curl_free(*it);
		}

		if( !hadCurl )
			curl_easy_cleanup(curlPtr);

		return outStr;
	}

	//void HttpParameters::ReleaseStr(str str)
	//{
	//	if( str != NULL )
	//		curl_free(str);
	//}





	/********************************************************************************************
	 *
	 *	FileHttpBody
	 *
	 ********************************************************************************************/



	FileHttpBody::FileHttpBody(FILE *stream, bool chunked, bool hasSize)
	{
		this->hasSize = hasSize;
		this->stream = stream;
		this->size = 0L;
		this->chunked = chunked;

		if( hasSize )
		{
			fseek (this->stream, 0 , SEEK_END);
			this->size = ftell (this->stream);
			rewind (this->stream);	
		}
	}

	bool FileHttpBody::HasSize()
	{
		return this->hasSize;
	}

	bool FileHttpBody::IsChunked()
	{
		return this->chunked;
	}

	long FileHttpBody::Size()
	{
		return this->size;
	}

	size_t FileHttpBody::ReadCallback(void *ptr, size_t size, size_t nmemb)
	{
		size_t sizeRead = 0;

		while(true)
		{
			if( feof(this->stream) )
				break;

			sizeRead = fread(ptr, size, nmemb, this->stream);

			if( sizeRead > 0 || this->hasRead )
				break;
		}

		this->hasRead = true;

		return sizeRead;
	}




	/********************************************************************************************
	 *
	 *	URLEncodedHttpBody
	 *
	 ********************************************************************************************/


	//class URLEncodedHttpBody: virtual public HttpBody
	//{
	//private:
	//	long size;
	//	c_str data;
	//	size_t readPtr;
	//	HttpParameters params;
	//public:
	//	URLEncodedHttpBody();

	//	void AddParam(c_str key, c_str value);
	//	void Build(CURL* cInstance = NULL, curl_slist *Hlist = NULL);

	//	bool IsChunked();
	//	bool HasSize();
	//	long Size();
	//	size_t ReadCallback(void *ptr, size_t size, size_t nmemb);
	//};

	URLEncodedHttpBody::URLEncodedHttpBody()
	{
		this->data = NULL;
		this->size = 0;
		this->readPtr = 0;
	}
	URLEncodedHttpBody::~URLEncodedHttpBody()
	{
		if( this->data != NULL )
			delete[] this->data;
	}


	void URLEncodedHttpBody::AddParam(c_str key, c_str value)
	{
		params.Add(key, value);
	}

	void URLEncodedHttpBody::Prepare(CurlCTX *ctx)
	{
		if( this->data != NULL )
		{
			delete[] this->data;

			this->data = NULL;
			this->size = 0;
			this->readPtr = 0;
		}

		this->params.SetCTX(ctx);
		this->data = this->params.toStr();

		if( this->data != NULL )
		{
			this->size = strlen( this->data );

			if( ctx != NULL )
				ctx->headers = curl_slist_append(ctx->headers, "Content-Type: application/x-www-form-urlencoded");
		}

	}

	bool URLEncodedHttpBody::IsChunked()
	{
		return false;
	}

	bool URLEncodedHttpBody::HasSize()
	{
		return true;
	}

	long URLEncodedHttpBody::Size()
	{
		return this->size;
	}


	size_t URLEncodedHttpBody::ReadCallback(void *ptr, size_t size, size_t nmemb)
	{
		if( this->data == NULL )
			return 0;

		size_t sizeRead = min((size_t)(this->size - this->readPtr), (size_t)(size * nmemb) );
		//size_t sizeRead = min((size_t)(min(this->size - this->readPtr, 10)), (size_t)(this->size * nmemb) );

		if( sizeRead > 0 )
		{
			memcpy(ptr, this->data + this->readPtr, sizeRead);
			this->readPtr += sizeRead;
		}

		return sizeRead;
	}


	/********************************************************************************************
	 *
	 *	Http
	 *
	 ********************************************************************************************/


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
			ctx->memory = (str)malloc( ctx->size + realsize + 1);
		else
			ctx->memory = (str)realloc(ctx->memory, ctx->size + realsize + 1);

		if(ctx->memory == NULL)
			throw MEOCLOUD_EXCEPTION_HTTP;
 
		memcpy(&(ctx->memory[ctx->size]), contents, realsize);
		ctx->size += realsize;
		ctx->memory[ctx->size] = 0;
 
		return realsize;
	}
	static size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp)
	{
		HttpBody *body = (HttpBody*)userp;

		return body->ReadCallback(ptr, size, nmemb);
	}



	void Http::releaseResult(HttpResult* result)
	{
		if( result != NULL )
		{
			if( result->url != NULL )
				delete[] result->url;

			if( result->ctx != NULL )
			{
				if( result->ctx->headers != NULL )
					curl_slist_free_all( result->ctx->headers);

				curl_easy_cleanup(result->ctx->curl);

				if(result->ctx->memory != NULL)
					free(result->ctx->memory);

				delete result->ctx;
			}

			delete result;
		}
	}

	HttpResult* Http::Get(HttpURL* url)
	{
		return this->Request(url, HttpMethod::M_GET, NULL);
	}
	HttpResult* Http::Put(HttpURL* url, HttpBody* body)
	{
		return this->Request(url, HttpMethod::M_PUT, body);
	}
	HttpResult* Http::Post(HttpURL* url, HttpBody* body)
	{
		return this->Request(url, HttpMethod::M_POST, body);
	}
	HttpResult* Http::Delete(HttpURL* url, HttpBody* body)
	{
		return this->Request(url, HttpMethod::M_DELETE, body);
	}



	HttpResult* Http::Request(HttpURL* url, HttpMethod method, HttpBody* body)
	{
		CurlCTX *ctx = getContext();
		HttpResult* result = getHttpResult();
		//curl_slist *slist = NULL;
		string contentLength;

		if( ctx == NULL || url == NULL || result == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		result->ctx = ctx;

		/* send all data to this function  */ 
		curl_easy_setopt(ctx->curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
		/* we pass our 'chunk' struct to the callback function */ 
		curl_easy_setopt(ctx->curl, CURLOPT_WRITEDATA, (void *)ctx);

		//curl_easy_setopt(ctx->curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(ctx->curl, CURLOPT_USERAGENT, "meocloud-uploader/1.0");

		//ctx->headers = curl_slist_append(ctx->headers, "Expect:"); 


		if( body != NULL )
		{
			body->Prepare(ctx);

			curl_easy_setopt(ctx->curl, CURLOPT_READFUNCTION, ReadCallback);
			curl_easy_setopt(ctx->curl, CURLOPT_READDATA, body);

			if( body->IsChunked() )
				ctx->headers = curl_slist_append(ctx->headers, "Transfer-Encoding: chunked");
			else
				ctx->headers = curl_slist_append(ctx->headers, "Transfer-Encoding:");
				

			if( body->HasSize() )
			{
				if( method != HttpMethod::M_PUT || body->Size() == 0 )
				{
					stringstream ss;
					ss << "Content-Length: " << body->Size();
					contentLength = ss.str();
					ctx->headers = curl_slist_append(ctx->headers, contentLength.c_str());
				}

				curl_easy_setopt(ctx->curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)body->Size());
			}
		}

		switch(method)
		{
		case HttpMethod::M_POST:
			curl_easy_setopt(ctx->curl, CURLOPT_POST, 1L);
			break;

		case HttpMethod::M_PUT:
			curl_easy_setopt(ctx->curl, CURLOPT_PUT, 1L);
			break;

		case HttpMethod::M_DELETE:
			curl_easy_setopt(ctx->curl, CURLOPT_CUSTOMREQUEST, "DELETE"); 
			break;

		default:
			break;
		}


		url->SetCTX(ctx);
		result->url = url->toStr();

		// set url
		curl_easy_setopt(ctx->curl, CURLOPT_URL, result->url);

		// set headers
		if( ctx->headers != NULL )
			curl_easy_setopt(ctx->curl, CURLOPT_HTTPHEADER, ctx->headers);

		if( Http::debug )
			curl_easy_setopt(ctx->curl, CURLOPT_VERBOSE, 1L);

		/* Perform the request, res will get the return code */ 
		result->curlStatus = curl_easy_perform(ctx->curl);

		curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &result->statusCode);

		/* Check for errors */ 
		if(result->curlStatus != CURLE_OK)
			result->curlErrorMsg = (str)curl_easy_strerror(result->curlStatus);

		return result;
	}

	// statics

	bool Http::debug = false;

	void Http::Init(void)
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}
	void Http::Terminate(void)
	{
		curl_global_cleanup();
	}
	void Http::SetDebug(bool debug)
	{
		Http::debug = debug;
	}





	/********************************************************************************************
	 *
	 *	
	 *
	 ********************************************************************************************/

}
