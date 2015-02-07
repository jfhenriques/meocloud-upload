
#include "Http.h"
#include "Meocloud_API.h"
#include <iostream>

using namespace std;
using namespace Http;
using namespace Meocloud;


int main(void)
{
	::Http::Http::Init();



	API meocloudAPI("", "");
	meocloudAPI.SetAccessToken("");

	FILE *in = fopen("in.txt", "rb");
	//FILE *in = fopen("img.jpg", "rb");

	meocloudAPI.UploadFile(in, "txt.txt");
	//meocloudAPI.UploadFile(, "bin.txt");

	fclose(in);

	::Http::Http::Terminate();

	return 0;
}
