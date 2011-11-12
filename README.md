# Introduction

HTMLScanner is a fast HTML/XML scanner/tokenizer for node.js. The scanner tries to be forgiven and is ideal those messy HTML documents. It should parse most HTML files and ofcourse also valid XML files. 

Please note there is no explicit support for namespaces. If you need a full blown XML parser, there are already many good alternatives available for Node.js. 

The core of the scanner module is a fast C++ module and is for 80% based on the excelent XHScanner created by Andrew Fedoniouk, see also [http://www.codeproject.com/KB/recipes/HTML_XML_Scanner.aspx]. Without this module HTMLScanner would not be here today.

## Installation

Just run the npm install command:

```bash
$ npm install htmlscanner
```

Or if you like to do it yourself:

```bash
$ git clone git@github.com:jbaron/htmlscanner.git
$ cd htmlscanner
$ node-waf configure build install
```


You should now have a file called **htmlscanner.node** in the lib directory. We use node-waf to build this module. Please note that older versions of node-waf use a different build directory. In that case you should find the file somewhere under the build/default directory. There are also some simple test cases included with this module. Just type for example:

```bash
$ node test/test_simple.js
```

## Usage

The usage is straight forward:

```javascript
var Scanner = require("../lib/htmlscanner").Scanner;
var scanner = new Scanner("<div id=12 class=important>hello</div>");
do {
	token = scanner.next();
	console.dir(token);
} while (token[0]);
```

The token you get back from the scanner.next() call contains all the info. The above sample would produce the following output.

```javascript
[1,"div","id","12","class","important"] // Type 1 indicates OPEN TAG. Attribute key/value pairs are also included.
[4,"hello"]				// Type 3 indicates TEXT
[2,"div"] 				// Type 2 indicates CLOSE TAG
[0]					// Type 0 indicates END OF FILE
```

The first element in the array is the type, the other elements in the array depend on the first one.

## TODO

There are several things still to do:

 * Entity decoding of text. Although much of the code is already there, it is not yet Unicode ready.
 * Add routines for entity encoding.
 * Add support for Buffers. Right now only Strings are supported.
 * Add some additional robustness checks.
 * Compile on other platforms besides Linux. The code should be portable, but has never been tested on any other platform besides Linux. So if you have success compiling and using this on OSX or Windows please let us know.

## Background

There is not much that cannot be done in plain JavaScript. The Chrome team did a great job making the V8 engine a very fast JavaScript solution. However one area that could become a bottleneck is when you start having to iterate over String, character at the time. For example when peforming encodings or parsing of XML Strings. And to be honest this is not only a problem that is specific to JavaScript. For example when you profile a highly optimized Java program that does a lot of XML parsing and serializing, you see these same type of methods at the top of the CPU usage. So for these types of operations this library contains a set of optimized C/C++ modules to speed up these tasks within V8.

