
#ifndef _HTTP_H_
#define _HTTP_H_


#include "Utils.h"
#include "curl\curl.h"
#include <map>
#include <stdlib.h>

namespace Http {

	typedef enum HttpMethod
	{
		M_GET = 0,
		M_POST,
		M_PUT,
		M_DELETE,
	} HttpMethod;

	typedef struct CurlCTX
	{
		CURL *curl;
		char *memory;
		size_t size;
	} CurlCTX;


	typedef struct HttpResult {
		char *url;
		long statusCode;
		CURLcode curlStatus;
		char *curlErrorMsg;
		CurlCTX *ctx;
	} HttpResult;




	class HttpParameters {
	private:
		std::map<const char*, const char *> *params;
		char *url;
		CurlCTX *ctx;

	public:
		HttpParameters(const char* url = NULL);
		~HttpParameters();

		void Add(const char* key, const char* param);
		void SetUrl(const char* url);
		void SetCTX(CurlCTX *ctx);

		char* toStr();
		
	};

	typedef HttpParameters HttpURL;




	class HttpBody {
	public:
		virtual bool IsChunked() = 0;
		virtual bool HasSize() = 0;
		virtual long Size() = 0;
		virtual size_t ReadCallback(void *ptr, size_t size, size_t nmemb) = 0;
	};

	class EmptyHttpBody: public HttpBody
	{
	public:
		bool IsChunked() { return false; }
		bool HasSize() { return true; }
		long Size() { return 0; }
		size_t ReadCallback(void *ptr, size_t size, size_t nmemb) { return 0; }
	};

	class FileHttpBody: virtual public HttpBody
	{
	private:
		bool hasSize;
		bool hasRead;
		long size;
		FILE* stream;
	public:
		FileHttpBody(FILE* stream, bool hasSize);

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