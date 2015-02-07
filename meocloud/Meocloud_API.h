
#ifndef _MEOCLOUD_API_H_
#define _MEOCLOUD_API_H_


#include "Http.h"
#include <string>

using namespace std;

typedef const char* c_str;
typedef char* str;


namespace Meocloud {


	class API {

	private:
		c_str consumer_key;
		c_str consumer_secret;
		c_str refresh_token;
		c_str access_token;
		::Http::Http httpClient;

		//static const string OA2_AUTH = "https://meocloud.pt/oauth2/authorize";
	public:
		static const string OAUTH2_BASE;
		static const string OAUTH2_AUTH;
		static const string OAUTH2_TOKEN;

		static const string URL_BASE_PUBLIC;
		static const string URL_BASE_CONTENT;

		static const string URL_FILES;


		API(c_str cKey, c_str cSecret);
		API(c_str cKey, c_str cSecret, c_str rToken, c_str aToken);
		~API();


		const char* GetConsumerKey();
		const char* GetConsumerSecret();

		c_str GetRefreshToken();
		void SetRefreshToken(c_str rToken);

		c_str GetAccessToken();
		void SetAccessToken(c_str aToken);


		void UploadFile(FILE *stream, c_str name);

	};

}


#endif /* _MEOCLOUD_API_H_ */

