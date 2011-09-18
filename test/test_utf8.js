var Scanner = require("../lib/htmlscanner").Scanner;
var fs = require("fs");
var sys = require("sys");

var html = fs.readFileSync("UTF-8-demo.html","utf-8");


function prettyPrint(x) {
		switch (x[0]) {
			case -1:
				sys.print("error");
			case 1: // TAG start
				sys.print("\n<" + x[1]);
				// Iterate over the attributes;
				for (var i=2;i<x.length;) {
					sys.print(" " + x[i++]);
					sys.print('="' + x[i++] + '"');
				} 
				sys.print(">");			
				break;
			case 2: // Tag end
				sys.print("\n</" + x[1] + ">");
				break;
			
			case 4: // Text
				sys.print(x[1]);
				break
			case 10:
				sys.print("<![CDATA[" + x[1] + "]]>");
				break;
		}
}


var x,scanner,start;

scanner = new Scanner(html);
do {
		x = scanner.next();
		// console.log(x[1]);
		prettyPrint(x);
} while(x[0])



console.log("\n\nPlease wait a moment......");
start = Date.now();
var loop = 10000;
for (var n=0;n<loop;n++) {
	scanner = new Scanner(html);
	do {
		x = scanner.next();
	} while(x[0])		
}

var stop = Date.now();
console.log("\n\nTime spent for "+ loop + " scans: " + (stop - start) +"ms.");
console.log("Throughput per second: "+ Math.round(loop*html.length*1000/((stop-start)*1000)) + " KB.");
	


