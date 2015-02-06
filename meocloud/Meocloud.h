#ifndef MEOCLOUD_H_
#define MEOCLOUD_H_


#include "curl\curl.h"
#include <map>

#define MEOCLOUD_OK 0
#define MEOCLOUD_ERROR_HTTP 1
#define MEOCLOUD_EXCEPTION_HTTP 2


namespace Meocloud {

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
		std::map<char*, char *> params;
		char *url;
		CurlCTX *ctx;

	public:
		HttpParameters();
		~HttpParameters() {};

		void Add(char* key, char* param);
		void SetUrl(char* url);
		void SetCTX(CurlCTX *ctx);

		char* toStr();
		
	};



	class Http {

	private:
		

	public:
		Http() {};
		~Http() {};
		//~Http(void);

		void releaseResult(HttpResult* result);
		HttpResult* Request(const char* url, HttpParameters *params = NULL);

		static void Init(void);
		static void Terminate(void);

	};




	class Meocloud {

	private:


	public:


	};

}


#endif /* MEOCLOUD_H_ */