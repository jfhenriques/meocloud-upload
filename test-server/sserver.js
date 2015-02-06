var http = require('http');
var url = require('url');


http.createServer(function (req, res) {
	if(req.url !== "/favicon.ico")
	{
	  var url_parts = url.parse(req.url, true);
	  var out = {url: req.url,
				method: req.method,
				params: url_parts.query,
				httpVersion: req.httpVersion,
				headers: req.headers};

	  res.writeHead(200, {'Content-Type': 'application/json; charset=utf-8'});
	  res.end(JSON.stringify(out));
	  
	  console.log("---Connection---");
	  console.log(out);
    }
}).listen(80);