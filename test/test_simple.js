var Scanner = require("../lib/htmlscanner").Scanner;
var scanner = new Scanner("<div id=12 class=important>hello</div>");
do {
	token = scanner.next();
	console.dir(token);
} while (token[0]);

