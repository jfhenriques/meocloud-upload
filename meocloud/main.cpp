
#include "Http.h"
#include "Meocloud_API.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace Http;
using namespace Meocloud;
using namespace rapidjson;


static c_str filename = NULL;
static c_str storename = NULL;
static c_str confFile = NULL;
static bool initMode = false;
static bool createDirectories = false;
static bool overwriteFiles = false;
static bool debugMode = false;
static bool createShare = false;
static bool createShareSmall = false;
static bool showHelp = false;


static str parse_argument(int argc, str argv[])
{
	bool isNotLast;

	for (int i = 1; i < argc; i++)
	{
		isNotLast = i < (argc-1);

		if ( strcmp(argv[i], "--init") == 0 )
			initMode = true;

		else
		if (   strcmp(argv[i], "-h") == 0
			|| strcmp(argv[i], "--help") == 0)
			showHelp = true;

		else
		if (   strcmp(argv[i], "-v") == 0
			|| strcmp(argv[i], "--debug") == 0 )
			debugMode = true;

		else
		if (   strcmp(argv[i], "-y") == 0
			|| strcmp(argv[i], "--overwrite") == 0 )
			overwriteFiles = true;

		else
		if (   strcmp(argv[i], "-d") == 0
			|| strcmp(argv[i], "--createdirs") == 0 )
			createDirectories = true;

		else
		if (   strcmp(argv[i], "-s") == 0
			|| strcmp(argv[i], "--share") == 0 )
			createShare = true;

		else
		if (   strcmp(argv[i], "-ss") == 0
			|| strcmp(argv[i], "--sharesmall") == 0 )
		{
			createShare = true;
			createShareSmall = true;
		}
		else
		if( isNotLast )
		{
			if (   strcmp(argv[i], "-f"   ) == 0
			    || strcmp(argv[i], "--file") == 0 )
				filename = argv[++i];

			else
			if (   strcmp(argv[i], "-n"   ) == 0
			    || strcmp(argv[i], "--name") == 0 )
				storename = argv[++i];

			else
			if (   strcmp(argv[i], "-c"   ) == 0
			    || strcmp(argv[i], "--conf") == 0 )
				confFile = argv[++i];

			else
				return argv[i];
		}
		else
			return argv[i];
	}

	return NULL;
}



int main(int argc, str argv[])
{
	stringstream error_stream;
	str error;
	int retCode = 0;

	try {


		::Http::Http::Init();


		if( (error = parse_argument(argc, argv)) != NULL )
		{
			error_stream << "Parametro desconhecido: '" << error << "'";

			throw 1;
		}

		::Http::Http::SetDebug(debugMode);


	/********************************************************************************************
	*
	*	Help mode
	*
	********************************************************************************************/

		if (showHelp)
		{

			cout <<"meocloud [Opts] [--init|-f FICHEIRO] " << endl << endl;

			cout << "  --init\t\t Inicializa e configura a aplicacao" << endl;
			cout << "  -f,  --file FICHEIRO\t Ficheiro a ser enviado para a Meocloud" << endl << endl;

			cout << " Parametros Opcionais:" << endl;

			cout << "  -c,  --conf CONFIG\t Caminho do ficheiro de configuracao" << endl;
			cout << "  \t\t\t  Quando omitido utiliza por defeito:" << endl;
			cout << "  \t\t\t  - %APPDATA%\\MeocloudUpload\\meocloud.conf no Windows" << endl;
			cout << "  \t\t\t  - /etc/meocloud.conf em Linux" << endl ;

			cout << "  -n,  --name CAMINHO\t Caminho a ser guardado na meocloud" << endl;
			cout << "  \t\t\t  Pode ser um directorio e/ou o nome de ficheiro" << endl;
			cout << "  \t\t\t  Quando omitido é guardado na raiz da Meocloud" << endl;
			cout << "  \t\t\t  com o mesmo nome do ficheiro de origem" << endl;

			cout << "  -d,  --createdirs\t Se necessario tenta criar na Meocloud" << endl;
			cout << "  \t\t\t  os directorios definidos com com a flag -n " << endl;

			cout << "  -s,  --share\t\t Cria e imprime um link de partilha" << endl;
			cout << "  \t\t\t  para o ficheiro enviado" << endl;

			cout << "  -ss, --sharesmall\t Cria e imprime um link minimizado de partilha" << endl;
			cout << "  \t\t\t  para o ficheiro enviado" << endl;

			cout << "  -y,  --overwrite\t Reescreve o ficheiro de destino caso ja exista" << endl;

			cout << "  -v,  --debug\t\t Activa o modo de debug" << endl;
			cout << "  -h,  --help\t\t Mostra esta ajuda" << endl << endl;

			cout << " Exemplos de utilizacao:" << endl;
			cout << "  meocloud --init" << endl;
			cout << "  meocloud -f backup.zip" << endl;
			cout << "  meocloud -f IMG123.jpg -n /Fotos/Ferias/Foto_123.jpg -d -ss -y" << endl;
		}


	/********************************************************************************************
	 *
	 *	initialize mode
	 *
	 ********************************************************************************************/

		else
		if( initMode )
		{
			string consumer_key;
			string consumer_secret;
			string code;

			string cont;
			string sandbox;
			bool isSandbox = false;

			cout << "Consumer key: ";
			cin >> consumer_key;

			cout << "Consumer secret: ";
			cin >> consumer_secret;

			cout << "Nivel de acesso: Meocloud/Sandbox? [M/s] ";
			cin >> sandbox;

			isSandbox = ( sandbox[0] == 's' || sandbox[0] == 'S' );

			cout << endl;
			cout << "Consumer key introduzida: '" << consumer_key << "'" << endl;
			cout << "Consumer secret introduzido: '" << consumer_secret << "'" << endl;
			cout << "Nivel de acesso: " << ( isSandbox ? "Sandbox" : "Meocloud" ) << endl << endl;

			cout << "Deseja continuar? [s/N] ";


			cin >> cont;

			if( cont[0] != 's' && cont[0] != 'S' )
			{
				error_stream << "Abortado pelo utilizador.";
				throw 1;
			}


			API meocloudAPI(consumer_key.c_str(), consumer_secret.c_str());


			cout << endl << "Insira o seguinte link no browser e autorize a aplicacao:" << endl << endl;

			cout << meocloudAPI.GetAuthorizationURL() << endl << endl;

			cout << "Apos concluir sera apresentado um PIN" << endl;
			cout << "Copie este PIN e introduza-o de seguida: " << endl << endl;

			cout << "PIN: ";

			cin >> code;
			
			APITokens tokens;
			int statusCode = meocloudAPI.RequestToken(code.c_str(), tokens);

			if( statusCode == -1 )
			{
				error_stream << "Ocorreu um problema ao receber a resposta da meocloud. Tente novamente.";
				throw 1;
			}

			if( statusCode != 200 )
			{
				error_stream << "Probema ao autorizar. Verifique os dados inseridos e tente novamente.";
				throw 1;
			}


			cout << endl;
			cout << "Refresh Token: " << tokens.refresh_token << endl;
			cout << "Access Token: " << tokens.access_token << endl;

			if( tokens.expires > 0 )
			{
				cout << endl << "Token recebido expira dentro de " << tokens.expires << " segundos" << endl;

			}
			
			meocloudAPI.SetRefreshToken(tokens.refresh_token.c_str());
			meocloudAPI.SetAccessToken(tokens.access_token.c_str());

			meocloudAPI.WriteFile(confFile);

			cout << endl << "Aplicacao configurada com sucesso" << endl;
		}



	/********************************************************************************************
	 *
	 *	upload mode
	 *
	 ********************************************************************************************/

		else
		{
			API* meocloudAPI = API::FromFile(confFile);

			if( meocloudAPI == NULL )
			{
				error_stream << "API nao configurada. Execute a aplicacao com a flag '--init'";
				throw 1;
			}


			if( filename == NULL )
			{
				error_stream << "Nome do ficheiro para upload nao definido. Utilize a flag '--file' ou '-f'";
				throw 1;
			}

			FILE *in = fopen(filename, "rb");
			if( in == NULL )
			{
				error_stream << "Ficheiro para upload nao existe.";
				throw 1;
			}
			

			bool notgood = true;
			bool firstTime = true;
			bool _continueTry = false;
			int statusCode;
			FileParts parts;
			int retry = 3;
			GetParts(storename, parts);
			APITokens rTokens;
			string extra;

			if( parts.filename.empty() )
			{
				FileParts oName;
				GetParts(filename, oName);
				parts.filename = oName.filename;
			}

			do
			{
				_continueTry = false;

				// try first without creating directories
				switch( meocloudAPI->UploadFile(in, parts, overwriteFiles) )
				{
				// all ok
				case 200:
					notgood = false;
					break;

				// token expired? try to renew
				case 401:
					
					statusCode = meocloudAPI->RequestToken(meocloudAPI->GetRefreshToken(), rTokens, true);
					if( statusCode == 200 )
					{
						meocloudAPI->SetAccessToken(rTokens.access_token.c_str());
						meocloudAPI->SetRefreshToken(rTokens.refresh_token.c_str());
						meocloudAPI->WriteFile(confFile);

						cout << "Os token de acesso e refresh foram renovados" << endl;
					
						_continueTry = true;
					}

					break;

				// in case of dir not exist, give a try to create dirs
				case 404:

					if( firstTime )
					{
						firstTime = false;

						if(    createDirectories
							&& API::IsCreateDirectoryCodeOK( meocloudAPI->CreateFolder( parts.directory.c_str() ) ) )
							_continueTry = true;
					}

					break;

				// fallback and retry
				default:

					retry--;
					if( retry > 0 )
					{
						_continueTry = true;
						cout << "Ocorreu um problema. Tentando novamente enviar o ficheiro" << endl;
					}
					else
						_continueTry = false;

					break;
				}

			} while(_continueTry);

			fclose(in);

			if( notgood )
			{
				error_stream << "O ficheiro nao foi enviado";
				throw 1;
			}

			if( createShare )
			{
				ShareLinkInfo share;

				if( meocloudAPI->CrceateShareLink(parts.GetFullName().c_str(), share,
								createShareSmall ? ShareLinkType::SMALL: ShareLinkType::NORMAL ) == 200 )
					extra = "Share link: " + (*share.outUrl);
				else
					extra = "Nao foi possível criar o share link";
			}

			cout << "Guardado na meocloud em '" << parts.GetFullName() << "'" << endl;
			
			if( !extra.empty() )
				cout << extra << endl;
		}



	} catch(exception &e) {

		cerr << "Exception: " << e.what() << endl;
		retCode = 1;

	} catch(...) {

		if( error_stream.eof() )
			cerr << "Erro inesperado" << endl;
		else
			cerr << "[Erro] " << error_stream.str() << endl;

		retCode = 2;
	}

	::Http::Http::Terminate();

	return retCode;
}
