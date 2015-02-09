
#ifndef _HTTP_H_
#define _HTTP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.h"
#include <curl/curl.h>
#include <map>



namespace Http {

	typedef enum
	{
		M_GET = 0,
		M_POST,
		M_PUT,
		M_DELETE,
	} HttpMethod;

	typedef struct CurlCTX
	{
		CURL* curl;
		char* memory;
		size_t size;
		curl_slist* headers;
	} CurlCTX;


	typedef struct HttpResult {
		str url;
		long statusCode;
		CURLcode curlStatus;
		str curlErrorMsg;
		CurlCTX* ctx;
	} HttpResult;




	class HttpParameters {
	private:
		std::map<c_str, c_str> *params;
		str url;
		CurlCTX* ctx;

	public:
		HttpParameters(c_str url = NULL);
		~HttpParameters();

		void Add(c_str key, c_str param);
		void SetUrl(c_str url);
		void SetCTX(CurlCTX *ctx);

		str toStr();
		
	};

	typedef HttpParameters HttpURL;




	class HttpBody {
	public:
		virtual bool IsChunked() = 0;
		virtual bool HasSize() = 0;
		virtual long Size() = 0;
		virtual void Prepare(CurlCTX *ctx = NULL) { (void)ctx; };
		virtual size_t ReadCallback(void *ptr, size_t size, size_t nmemb) = 0;
	};

	class EmptyHttpBody: virtual public HttpBody
	{
	public:
		bool IsChunked() { return false; }
		bool HasSize() { return true; }
		long Size() { return 0; };
	
		size_t ReadCallback(void *ptr, size_t size, size_t nmemb) {
			(void)ptr; (void)size; (void)nmemb;
			return 0;
		}
	};

	class FileHttpBody: virtual public HttpBody
	{
	private:
		bool hasSize;
		bool chunked;
		bool hasRead;
		long size;
		FILE* stream;
	public:
		FileHttpBody(FILE* stream, bool chunked = false, bool hasSize = false);

		bool IsChunked();
		bool HasSize();
		long Size();
		size_t ReadCallback(void *ptr, size_t size, size_t nmemb);
	};


	class URLEncodedHttpBody: virtual public HttpBody
	{
	private:
		long size;
		c_str data;
		size_t readPtr;
		HttpParameters params;
	public:
		URLEncodedHttpBody();
		~URLEncodedHttpBody();

		void AddParam(c_str key, c_str value);
		void Prepare(CurlCTX *ctx = NULL);

		bool IsChunked();
		bool HasSize();
		long Size();
		size_t ReadCallback(void *ptr, size_t size, size_t nmemb);
	};



	class Http {

	private:

	public:
		Http() {};
		~Http() {};
		//~Http(void);

		void releaseResult(HttpResult* result);
		HttpResult* Request(HttpURL* url, HttpMethod method, HttpBody *body = NULL);

		HttpResult* Get(HttpURL* url);
		HttpResult* Delete(HttpURL* url, HttpBody *body = NULL);
		HttpResult* Put(HttpURL* url, HttpBody *body = NULL);
		HttpResult* Post(HttpURL* url, HttpBody *body = NULL);

		static void Init(void);
		static void Terminate(void);

	};


}

#endif /* _HTTP_H_ */