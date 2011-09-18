var Scanner = require("../lib/htmlscanner").Scanner;
var fs = require("fs");
var sys = require("sys");


var html = 
	"<!DOCTYPE html>" + 
	"<? some processing instruction ?>" +
	"<html>" + 
		"<!--some comment-->" +
		"<body class=canvas>" +
			"<input type=chechbox checked>" +
			"<![CDATA[some <div> Hallo </div> cdata]]>" +
			"<div>Normal Text</div>" +
		 "</body>" + 
	"</html>";



// open tag = 1, close tag = 2, text = 4, comment = 8 , cdata = 10, pi =12, doctype = 14, eof = 0, error = -1

var scanner = new Scanner(html);
do {
		var  x = scanner.next();
		console.dir(x);
} while(x[0])





