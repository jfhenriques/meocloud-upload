
#include <cstdio>
#include "Meocloud_API.h"
#include <iostream>
#include "rapidjson\document.h"
#include "rapidjson\filewritestream.h"
#include "rapidjson\filereadstream.h"
#include "rapidjson\prettywriter.h"

using namespace std;
using namespace Http;
using namespace rapidjson;


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



	bool API::UploadFile(FILE *stream, c_str name)
	{
		if( stream == NULL || name == NULL )
			return false;

		bool out = false;

		try {
			HttpURL url( (API::URL_FILES + name).c_str());
			url.Add("access_token", this->GetAccessToken() );
			url.Add("overwrite", "true");

			FileHttpBody body(stream, stream != stdin );
			HttpResult* result = httpClient.Put(&url, &body);

			if(    result != NULL
				&& result->curlStatus == CURLE_OK
				&& result->statusCode == 200
				&& result->ctx != NULL
				&& result->ctx->memory != NULL )
			{
				Document doc;
				doc.Parse(result->ctx->memory);

				if( doc.IsObject() )
				{
					if (    !body.HasSize()
						 || (
							   doc.HasMember("bytes")
							&& doc["bytes"].IsInt64()
							&& doc["bytes"].GetInt64() == body.Size() ) )
						 out = true;
				}
			}

			httpClient.releaseResult(result);
	
		} catch(exception& e) {
			cout << e.what() << endl;
		}

		return out;
	}

	string API::GetAuthorizationURL()
	{
		HttpURL url( API::OAUTH2_AUTH.c_str());
		url.Add("redirect_uri", "http://127.0.0.1/callback" );
		url.Add("response_type", "code");
		url.Add("client_id", this->consumer_key);

		str outStr = url.toStr();
		string out(outStr);

		delete[] outStr;

		return out;
	}



	rapidjson::Document* API::RequestToken(c_str code, bool isRefresh)
	{
		if( code == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		Document *docOut = NULL;

		try {

			HttpURL url( API::OAUTH2_TOKEN.c_str());
			url.Add("client_id", this->consumer_key);
			url.Add("client_secret", this->consumer_secret);
			url.Add("grant_type", "authorization_code");
			url.Add("redirect_uri", "http://127.0.0.1/callback" );

			if( isRefresh )
				url.Add("refresh_token", code);
			else
				url.Add("code", code);

			HttpResult* result = httpClient.Post(&url, new EmptyHttpBody());

			if(    result != NULL
				&& result->curlStatus == CURLE_OK
				&& result->statusCode == 200
				&& result->ctx != NULL
				&& result->ctx->memory != NULL )
			{
				str response = result->ctx->memory;
				Document* doc = new Document;
				doc->Parse(response);

				if( doc->IsObject() )
					docOut = doc;
			}
			
			httpClient.releaseResult(result);
	
		} catch(exception& e) {
			cout << e.what() << endl;
		}

		return docOut;
	}


	void API::WriteFile(c_str file)
	{
		if( file == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		FILE* fp = fopen(file, "wb");

		if( fp == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		Document dd;
		dd.SetObject();

		Value cKey(Type::kStringType);
		Value cSecret(Type::kStringType);
		Value rToken(Type::kStringType);
		Value aToken(Type::kStringType);

		cKey.SetString(this->consumer_key, strlen(this->consumer_key));
		cSecret.SetString(this->consumer_secret, strlen(this->consumer_secret));
		rToken.SetString(this->refresh_token, strlen(this->refresh_token));
		aToken.SetString(this->access_token, strlen(this->access_token));

		dd.AddMember("consumer_key", cKey, dd.GetAllocator());
		dd.AddMember("consumer_secret", cSecret, dd.GetAllocator());
		dd.AddMember("refresh_token", rToken, dd.GetAllocator());
		dd.AddMember("access_token", aToken, dd.GetAllocator());

		char writeBuffer[65536];
		FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		PrettyWriter<FileWriteStream> writer(os);
		dd.Accept(writer);

		fclose(fp);
	}




	
	API* API::FromFile(c_str file)
	{
		if( file == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		FILE* fp = fopen(file, "rb");

		if( fp == NULL )
			return NULL;

		char readBuffer[65536];
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		Document dd;
		dd.ParseStream(is);
		fclose(fp);

		if(dd.IsObject())
		{
			if(    dd.HasMember("consumer_key") && dd["consumer_key"].IsString()
				&& dd.HasMember("consumer_secret") && dd["consumer_secret"].IsString() )
			{
				API* api = new API(dd["consumer_key"].GetString(), dd["consumer_secret"].GetString());

				if( dd.HasMember("refresh_token") && dd["refresh_token"].IsString() )
					api->SetRefreshToken( dd["refresh_token"].GetString() );

				if( dd.HasMember("access_token") && dd["access_token"].IsString() )
					api->SetAccessToken( dd["access_token"].GetString() );

				return api;
			}
		}

		return NULL;
	}

}


