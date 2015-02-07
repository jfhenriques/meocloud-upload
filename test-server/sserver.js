var http = require('http');
var url = require('url');
var fs = require('fs');
  
  
http.createServer(function (req, res) {
	if(req.url !== "/favicon.ico")
	{
		var body = "";

		req.on('data', function (chunk) {
			body += chunk;
		});
		req.on('end', function () {

			var url_parts = url.parse(req.url, true);
			var out = {url: req.url,
					method: req.method,
					params: url_parts.query,
					//body: body,
					httpVersion: req.httpVersion,
					headers: req.headers};	
			
			//fs.writeFile("." + url_parts.pathname, body, function(){
				res.writeHead(200, {'Content-Type': 'application/json; charset=utf-8'});
				res.end(JSON.stringify(out));

				console.log("---Connection---");
				console.log(out);
			//});
		});
    }
}).listen(80);