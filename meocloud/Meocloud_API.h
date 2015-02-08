
#ifndef _MEOCLOUD_API_H_
#define _MEOCLOUD_API_H_


#include "Http.h"
#include <string>
#include <rapidjson/document.h>

using namespace std;


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
		~API();


		c_str GetConsumerKey();
		c_str GetConsumerSecret();

		c_str GetRefreshToken();
		void SetRefreshToken(c_str rToken);

		c_str GetAccessToken();
		void SetAccessToken(c_str aToken);


		string GetAuthorizationURL();


		bool UploadFile(FILE *stream, c_str name);

		rapidjson::Document* RequestToken(c_str code, bool isRefresh = false);

		void WriteFile(c_str file);

		static API* FromFile(c_str file);

	};

}


#endif /* _MEOCLOUD_API_H_ */

