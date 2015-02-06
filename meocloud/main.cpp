
#include <iostream>
#include "Meocloud.h"

using namespace std;
using namespace Meocloud;


int main(void)
{
	Http::Init();

	//CURL *curl;
	//CURLcode res;
	//struct curl_slist *headerlist=NULL;
	//const char buf[] = "Authorization: Bearer 123";
 //
	//curl_global_init(CURL_GLOBAL_ALL);
	//headerlist = curl_slist_append(headerlist, buf);


	//curl = curl_easy_init();

	//if(curl) {
	//	/* what URL that receives this POST */ 
	//	curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1");
	//	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
 //
	//	/* Perform the request, res will get the return code */ 
	//	res = curl_easy_perform(curl);
	//	/* Check for errors */ 
	//	if(res != CURLE_OK)
	//	  fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
 //
	//	/* always cleanup */ 
	//	curl_easy_cleanup(curl);
	//}

	try {

		Http http;
		HttpParameters params;

		params.Add("var1","aaaa");
		params.Add("var2","cont*=a3s$?&");
		params.Add("va r3","bbbb");

		HttpResult * result = http.Request("http://127.0.0.1", &params);

		http.releaseResult(result);

	
	} catch(exception& e) {
		cout << e.what() << endl;
	}

	Http::Terminate();

	return 0;
}





