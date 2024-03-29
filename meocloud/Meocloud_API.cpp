
#include <cstdio>
#include "Meocloud_API.h"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

#ifdef OS_UNIX
#include <sys/stat.h>
#endif

using namespace std;
using namespace Http;
using namespace rapidjson;


namespace Meocloud {

	const string API::OAUTH2_BASE = "https://meocloud.pt/oauth2/";
	const string API::OAUTH2_AUTH = API::OAUTH2_BASE + "authorize";
	const string API::OAUTH2_TOKEN = API::OAUTH2_BASE + "token";

	const string API::URL_BASE_PUBLIC = "https://api.meocloud.pt/1/";
	const string API::URL_BASE_CONTENT = "https://api-content.meocloud.pt/1/";


	const string API::URL_FILES = API::URL_BASE_CONTENT + "Files/";
	const string API::URL_CREATE_DIR = API::URL_BASE_PUBLIC + "Fileops/CreateFolder";
	const string API::URL_SHARE_LINK = API::URL_BASE_PUBLIC + "Shares/";
	const string API::URL_SHARE_SMALL = API::URL_BASE_PUBLIC + "ShortenLinkURL";


	static void SetContent(c_str* key, c_str value)
	{
		if( key != NULL && *key != NULL )
			delete[] *key;

		*key = ( value == NULL ) ? NULL : StrCopy(value);
	}


	API::API(c_str cKey, c_str cSecret)
	{
		this->consumer_key = NULL;
		this->consumer_secret = NULL;
		this->refresh_token = NULL;
		this->access_token = NULL;

		this->isSandbox = false;

		SetContent(&this->consumer_key, cKey);
		SetContent(&this->consumer_secret, cSecret);
	};


	API::~API()
	{
		SetContent(&this->consumer_key, NULL);
		SetContent(&this->consumer_secret, NULL);
		SetContent(&this->refresh_token, NULL);
		SetContent(&this->access_token, NULL);
	}


	bool API::IsSandbox()
	{
		return this->isSandbox;
	}
	void API::SetSandbox(bool s)
	{
		this->isSandbox = s;
	}

	c_str API::ResolveAccessLevel()
	{
		return this->isSandbox ? "sandbox" : "meocloud";
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




	int API::CreateFolder(c_str name)
	{
		int outCode = -1;

		string directory;
		string fullName(name);

		if( name == NULL || fullName.find('/') == string::npos )
			return outCode;

		HttpURL url( API::URL_CREATE_DIR.c_str());
		url.Add("access_token", this->GetAccessToken() );

		bool canContinue;

		size_t start = 0,
				end, tSize = fullName.length();

		if( fullName[0] == '/' )
			start++;

		do
		{
			end = fullName.find('/', start);

			if( end == string::npos )
				end = tSize;

			directory += "/" + string(fullName, start, end-start);
			start = end + 1;

			URLEncodedHttpBody body;
			body.AddParam("root", this->ResolveAccessLevel() );
			body.AddParam("path", directory.c_str());

			HttpResult* result = httpClient.Post(&url, &body);

			if( result == NULL || !result->IsCurlOK() )
			{
				outCode = -1;
				canContinue = false;
			}
			else
			{
				outCode = result->statusCode;
				canContinue = API::IsCreateDirectoryCodeOK(result->statusCode);
			}

			httpClient.releaseResult( result );

		} while(canContinue && end < tSize);

		return outCode;
	}

	
	int API::UploadFile(FILE *stream, FileParts &parts, bool overwriteFiles, bool createDirectories)
	{
		if( stream == NULL )
			return -1;

		int outCode;

		if( createDirectories )
		{
			outCode = this->CreateFolder(parts.directory.c_str());
			if( !API::IsCreateDirectoryCodeOK( outCode ) )
				return outCode;
		}

		outCode = -1;

		string _url = API::URL_FILES + this->ResolveAccessLevel() + parts.GetFullName();
		ReplaceInString(_url, " ", "%20");
		HttpURL url(_url.c_str());
		url.Add("access_token", this->GetAccessToken() );
		url.Add("overwrite", overwriteFiles ? "true" : "false");

		FileHttpBody body(stream, false, stream != stdin );
		HttpResult* result = httpClient.Put(&url, &body);

		if( result != NULL && result->IsCurlResponseOK() )
		{
			if( result->statusCode != 200 )
				outCode = result->statusCode;

			else
			{
				Document doc;
				doc.Parse(result->ctx->memory);

				if( doc.IsObject() )
				{
					if( !body.HasSize()
						|| (   doc.HasMember("bytes")
							&& doc["bytes"].IsInt64()
							&& doc["bytes"].GetInt64() == body.Size() ) )
						outCode = 200;
						
					if (doc.HasMember("path") && doc["path"].IsString()) {
						string fname = doc["path"].GetString();
						GetParts(fname, parts);
					}
				}
			}
		}

		if( result != NULL )
			httpClient.releaseResult(result);

		return outCode;
	}

	string API::GetAuthorizationURL()
	{

		HttpURL url( API::OAUTH2_AUTH.c_str());
		url.Add("response_type", "code");
		url.Add("client_id", this->consumer_key);

		str outStr = url.toStr();
		string out(outStr);

		delete[] outStr;

		return out;
	}


	int API::RequestToken(c_str code, APITokens& tokens, bool isRefresh)
	{
		if( code == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		int outCode = -1;

		tokens.access_token = "";
		tokens.refresh_token = "";
		tokens.type = "";
		tokens.expires = -1;

		HttpURL url( API::OAUTH2_TOKEN.c_str());
		url.Add("client_id", this->consumer_key);
		url.Add("client_secret", this->consumer_secret);
		url.Add("redirect_uri", "oob" );

		if( isRefresh )
		{
			url.Add("grant_type", "refresh_token");
			url.Add("refresh_token", code);
		}
		else
		{
			url.Add("grant_type", "authorization_code");
			url.Add("code", code);
		}

		HttpResult* result = httpClient.Post(&url, new EmptyHttpBody());

		if( result != NULL && result->IsCurlResponseOK() )
		{
			
			if( result->statusCode != 200 )
				outCode = result->statusCode;

			else
			{
				Document doc;
				doc.Parse(result->ctx->memory);

				if( doc.IsObject() )
				{
					if(	   doc.HasMember("access_token") && doc["access_token"].IsString()
						&& doc.HasMember("refresh_token") && doc["refresh_token"].IsString() )
					{
						tokens.refresh_token = doc["refresh_token"].GetString();
						tokens.access_token = doc["access_token"].GetString();

						if( !tokens.refresh_token.empty() && !tokens.access_token.empty() )
							outCode = 200;

						if( doc.HasMember("expires_in") && doc["expires_in"].IsInt64() )
							tokens.expires = doc["expires_in"].GetInt64();
					}
				}
			}
		}
			
		httpClient.releaseResult(result);

		return outCode;
	}

	int API::CrceateShareLink(c_str link, ShareLinkInfo &share, ShareLinkType type)
	{
		if( link == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		int outCode = -1;
		share.url = "";
		share.shareid = "";
		share.expires = "";
		share.url_small = "";
		share.type = ShareLinkType::NORMAL;


		string _url = API::URL_SHARE_LINK + this->ResolveAccessLevel() + link;
		ReplaceInString(_url, " ", "%20");

		HttpURL url(_url.c_str());
		url.Add("access_token", this->GetAccessToken() );

		HttpResult* result = httpClient.Post(&url, new EmptyHttpBody());

		if( result != NULL && result->IsCurlResponseOK() )
		{
			
			if( result->statusCode != 200 )
				outCode = result->statusCode;

			else
			{
				Document doc;
				doc.Parse(result->ctx->memory);

				if( doc.IsObject() )
				{
					if(	   doc.HasMember("url") && doc["url"].IsString()
						&& doc.HasMember("shareid") && doc["shareid"].IsString() )
					{
						share.url = doc["url"].GetString();
						share.shareid = doc["shareid"].GetString();
						share.expires = doc["expires"].GetString();
						share.outUrl = &share.url;

						if( !share.url.empty() && !share.shareid.empty() )
							outCode = 200;

						if( doc.HasMember("expires") && doc["expires"].IsString() )
							share.expires = doc["expires"].GetString();
					}
				}
			}
		}
			
		httpClient.releaseResult(result);

		if( outCode == 200 && type == ShareLinkType::SMALL )
		{
			HttpURL url( API::URL_SHARE_SMALL.c_str() );
			url.Add("access_token", this->GetAccessToken() );

			URLEncodedHttpBody body;
			body.AddParam("shareid", share.shareid.c_str());

			HttpResult* result = httpClient.Post(&url, &body);

			if(    result != NULL
				&& result->IsCurlResponseOK()
				&& result->statusCode == 200 )
			{
				Document doc;
				doc.Parse(result->ctx->memory);

				if(    doc.IsObject()
					&& doc.HasMember("url")
					&& doc["url"].IsString() )
				{
					share.url_small = doc["url"].GetString();
					share.outUrl = &share.url_small;
					share.type = ShareLinkType::SMALL;
				}
			}

			httpClient.releaseResult(result);
		}

		return outCode;
	}


	bool API::IsCreateDirectoryCodeOK(int code)
	{
		return code == 200 || code == 403;
	}


	void API::WriteFile(c_str file)
	{

#ifdef OS_UNIX
		c_str defPath = NULL;
		if( file == NULL )
		{
			defPath = GetUnixDefaultConfFile();
			file = defPath;
		}
#endif

		FILE* fp = GetConfFilePtr(file, true);

		if( fp == NULL )
			throw MEOCLOUD_EXCEPTION_HTTP;

		Document dd;
		dd.SetObject();

		Value cKey(Type::kStringType);
		Value cSecret(Type::kStringType);
		Value rToken(Type::kStringType);
		Value aToken(Type::kStringType);

		Value sandbox;

		cKey.SetString(this->consumer_key, strlen(this->consumer_key));
		cSecret.SetString(this->consumer_secret, strlen(this->consumer_secret));
		rToken.SetString(this->refresh_token, strlen(this->refresh_token));
		aToken.SetString(this->access_token, strlen(this->access_token));
		sandbox.SetBool(this->isSandbox);

		dd.AddMember("consumer_key", cKey, dd.GetAllocator());
		dd.AddMember("consumer_secret", cSecret, dd.GetAllocator());
		dd.AddMember("refresh_token", rToken, dd.GetAllocator());
		dd.AddMember("access_token", aToken, dd.GetAllocator());
		dd.AddMember("sandbox", sandbox, dd.GetAllocator());

		char writeBuffer[65536];
		FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		PrettyWriter<FileWriteStream> writer(os);
		dd.Accept(writer);

		fclose(fp);

#ifdef OS_UNIX
		char mode[] = "0600";
		int modeL = strtol(mode, 0, 8);
		chmod(file, modeL);

		if (defPath != NULL)
			delete[] defPath;
#endif

	}


	
	API* API::FromFile(c_str file)
	{
		//if( file == NULL )
		//	throw MEOCLOUD_EXCEPTION_HTTP;

		FILE* fp = GetConfFilePtr(file);

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

				if( dd.HasMember("sandbox") && dd["sandbox"].IsBool() )
					api->SetSandbox(dd["sandbox"].GetBool());

				return api;
			}
		}

		return NULL;
	}

}
