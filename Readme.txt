meocloud [Opts] [--init|-f FICHEIRO]

  --init                 Inicializa e configura a aplicacao
  -f,  --file FICHEIRO   Ficheiro a ser enviado para a Meocloud

 Parametros Opcionais:
  -c,  --conf CONFIG     Caminho do ficheiro de configuracao
                          Quando omitido utiliza por defeito:
                          - %APPDATA%\MeocloudUpload\meocloud.conf no Windows
                          - /etc/meocloud.conf em Linux com o utilizador root
                          - ~/.meocloud.conf em Linux com outros utilizadores
  -n,  --name CAMINHO    Caminho a ser guardado na meocloud
                          Pode ser um directorio e/ou o nome de ficheiro
                          Quando omitido e guardado na raiz da Meocloud
                          com o mesmo nome do ficheiro de origem
  -d,  --createdirs      Se necessario tenta criar na Meocloud
                          os directorios definidos com com a flag -n
  -s,  --share           Cria e imprime um link de partilha
                          para o ficheiro enviado
  -ss, --sharesmall      Cria e imprime um link minimizado de partilha
                          para o ficheiro enviado
  -y,  --overwrite       Reescreve o ficheiro de destino caso ja exista
  -v,  --debug           Activa o modo de debug
  -h,  --help            Mostra esta ajuda

 Exemplos de utilizacao:
  meocloud --init
  meocloud -f backup.zip
  meocloud -f IMG123.jpg -n /Fotos/Ferias/Foto_123.jpg -d -ss -y
