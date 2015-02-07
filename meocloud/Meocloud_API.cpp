

#include "Meocloud_API.h"
#include <iostream>

using namespace std;
using namespace Http;



namespace Meocloud {

	const string API::OAUTH2_BASE = "https://meocloud.pt/oauth2/";
	const string API::OAUTH2_AUTH = API::OAUTH2_BASE + "authorize";
	const string API::OAUTH2_TOKEN = API::OAUTH2_BASE + "token";

	const string API::URL_BASE_PUBLIC = "https://publicapi.meocloud.pt/";
	const string API::URL_BASE_CONTENT = "https://api-content.meocloud.pt/";


	const string API::URL_FILES = API::URL_BASE_CONTENT + "1/Files/sandbox/";


	static void SetContent(c_str* key, c_str value)
	{
		if( key != NULL && *key != NULL )
			delete[] *key;

		*key = ( value == NULL ) ? NULL : StrCopy(value);
	}


	API::API(c_str cKey, c_str cSecret)
	{
		SetContent(&this->consumer_key, cKey);
		SetContent(&this->consumer_secret, cSecret);
	};

	API::API(c_str cKey, c_str cSecret, c_str rToken, c_str aToken)
	{
		SetContent(&this->consumer_key, cKey);
		SetContent(&this->consumer_secret, cSecret);

		this->SetRefreshToken(rToken);
		this->SetAccessToken(aToken);
	};
	API::~API()
	{
		SetContent(&this->consumer_key, NULL);
		SetContent(&this->consumer_secret, NULL);
		SetContent(&this->refresh_token, NULL);
		SetContent(&this->access_token, NULL);
	}



	c_str API::GetConsumerKey()
	{
		return this->consumer_key;
	}
	c_str API::GetConsumerSecret()
	{
		return this->consumer_secret;
	}


	c_str API::GetRefreshToken()
	{
		return this->refresh_token;
	}
	void API::SetRefreshToken(c_str rToken)
	{
		SetContent(&this->refresh_token, rToken);
	}

	c_str API::GetAccessToken()
	{
		return this->access_token;
	}
	void API::SetAccessToken(c_str aToken)
	{
		SetContent(&this->access_token, aToken);
	}



	void API::UploadFile(FILE *stream, c_str name)
	{
		if( stream == NULL || name == NULL )
			return;

		try {
			HttpURL url( (API::URL_FILES + name).c_str());
			url.Add("access_token", this->GetAccessToken() );
			url.Add("overwrite", "true");

			FileHttpBody body(stream, stream != stdin );
			HttpResult* result = httpClient.Put(&url, &body);

			httpClient.releaseResult(result);
	
		} catch(exception& e) {
			cout << e.what() << endl;
		}
	}

}
