var Scanner = require("../lib/htmlscanner").Scanner;

var snippets = [
	"<html ", " a=" , "'12332'", "CDA" , "/a>" , "asasadsajdksa" , "&hdjskdhsa dhsa jad hsakd hsa",
	"p=q=w=e=r=t=y" , "<div>Hello World</div>", "<!-- comment -> -->", "CDATA[[",
	"^&#!^*&#!^#*&!^!", "]]>-?", "href=/test/1234", "<doctype ....","p<p?dJDJKDS","/\/"

];

function getHtml() {
	var result="";
	var len = snippets.length;
	for (var i=0;i<100;i++) {
		var index = Math.floor(Math.random() * (len));
		result += snippets[index];
	}
	return result;
}

var x,scanner;
var loop = 100*100;

for (var i=0;i<loop;i++) {
	scanner = new Scanner(getHtml());
	do {
		x = scanner.next();
		// prettyPrint(x);
	} while(x[0])
}

console.log("Finished without a crash.");


