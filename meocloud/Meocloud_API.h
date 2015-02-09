
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
		bool isSandbox;
		::Http::Http httpClient;

	public:

		static const string OAUTH2_BASE;
		static const string OAUTH2_AUTH;
		static const string OAUTH2_TOKEN;

		static const string URL_BASE_PUBLIC;
		static const string URL_BASE_CONTENT;

		static const string URL_FILES;
		static const string URL_CREATE_DIR;


		// ctor & dtor

		API(c_str cKey, c_str cSecret);
		~API();

		// Auth level

		bool IsSandbox();
		void SetSandbox(bool s);

		c_str ResolveAccessLevel();


		// Tokens

		c_str GetConsumerKey();
		c_str GetConsumerSecret();

		c_str GetRefreshToken();
		void SetRefreshToken(c_str rToken);

		c_str GetAccessToken();
		void SetAccessToken(c_str aToken);


		string GetAuthorizationURL();


		// Operations

		int CreateFolder(c_str name);
		int UploadFile(FILE *stream, FileParts parts, bool overwriteFiles = false, bool createDirectories = false);

		rapidjson::Document* RequestToken(c_str code, bool isRefresh = false);


		// file save/load

		void WriteFile(c_str file);
		static API* FromFile(c_str file);


		static bool IsCreateDirectoryCodeOK(int code);

	};

}


#endif /* _MEOCLOUD_API_H_ */

