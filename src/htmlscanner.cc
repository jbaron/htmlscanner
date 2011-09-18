#include "stdio.h"
#include "xh_scanner.h"

#include <node.h>
#include <node_version.h>
#include <node_events.h>
#include <node_buffer.h>

using namespace v8;
using namespace node;



struct wchar_istream: public markup::instream
{
	markup::wchar* data;
   	int   pos;

       wchar_istream(markup::wchar* text) : data(text), pos(0) {
				// printf("Created stream of length :%i\n",data.len);		
	}

       virtual markup::wchar get_char() { 
		// markup::wchar result =  pos < data.len ? data.start[pos++] : 0; 
		markup::wchar result =  data[pos]; 
		if (result != 0) {
			pos++;
		}
		return result;
	}

	~wchar_istream() {
		// delete[] data.start;
	}

};



/*
 * Do the binding stuff with V8
 */ 
class HTMLScanner: ObjectWrap
{
private:
  markup::scanner* scanner;	
  markup::instream* si;	
  int pendingToken;
  markup::wchar* html;

public:

  static Persistent<FunctionTemplate> s_ct;	
  static void Initialize(Handle<Object> target)
  {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);

    s_ct = Persistent<FunctionTemplate>::New(t);
    s_ct->InstanceTemplate()->SetInternalFieldCount(1);
    s_ct->SetClassName(String::NewSymbol("Scanner"));

    NODE_SET_PROTOTYPE_METHOD(t, "next", Next);
    target->Set(String::NewSymbol("Scanner"),s_ct->GetFunction());
  }

  HTMLScanner(String &str) : pendingToken(-99) 
  {
    html = new markup::wchar[str.Length()+1]; 
    str.Write(html);
    si = new wchar_istream(html);
    scanner = new markup::scanner(*si);
     
  }

  ~HTMLScanner()
  {
	delete scanner;
	delete si;
	delete[] html;
  }

  static Handle<Value> New(const Arguments& args)
  {
    HandleScope scope;
    Local<String> str;

    if (args.Length() >= 1 && args[0]->IsString()) 
    {
        str = args[0]->ToString();
        HTMLScanner* xhs = new HTMLScanner(**str);
        xhs->Wrap(args.This());
    }
		
		
    return args.This();
  }

  /*
   * This is the main method that is invoked
   */
  static Handle<Value> Next(const Arguments& args)
  {
    HandleScope scope;
    HTMLScanner* xhs = ObjectWrap::Unwrap<HTMLScanner>(args.This());
    markup::scanner* sc = xhs->scanner;

  bool found = false;
  Local<Object> result2 = Array::New();
  int t;

  while(! found)
  {

    // Check if there is still a pending token from last time this function was called.
    if (xhs->pendingToken > -2) {
	t = xhs->pendingToken;
	xhs->pendingToken = -99;
    } else {
	t = sc->get_token();   
    }

    int counter = 0;
    found = true;

    switch(t)
    {
      case markup::scanner::TT_ERROR:	
      case markup::scanner::TT_EOF:
	result2->Set(counter++, Integer::New(t) );	 
	break;	

      case markup::scanner::TT_TAG_START:
	result2->Set(counter++, Integer::New(t) );	 
	sc->tagname.push_back(0); // Just make sure there is a 0.
	result2->Set(counter++,	String::New(&sc->tagname[0]));

	// Loop to get all the attributes
	t = sc->get_token();
	while (t == markup::scanner::TT_ATTR) {
		sc->attrname.push_back(0); // Just make sure there is a 0.
		result2->Set(counter++,	String::New(&sc->attrname[0]));
		sc->value2.push_back(0); // Just make sure there is a 0.
		result2->Set(counter++,	String::New(&sc->value2[0]));
		t = sc->get_token();
	}
	xhs->pendingToken = t; 
        break;

      case markup::scanner::TT_TAG_END:
	result2->Set(counter++, Integer::New(t) );	 
	sc->tagname.push_back(0); // Just make sure there is a 0.
	result2->Set(counter++,	String::New(&sc->tagname[0]));
        break;

      case markup::scanner::TT_DOCTYPE_END:	
      case markup::scanner::TT_WORD:
      case markup::scanner::TT_PI_END:	
      case markup::scanner::TT_COMMENT_END:	
      case markup::scanner::TT_CDATA_END:
	result2->Set(counter++, Integer::New(t) );
	sc->value2.push_back(0); // Just make sure there is a 0.
	result2->Set(counter++,	String::New(&sc->value2[0]));
     	break;	

      
 
       default:
	found = false;

    }
  }


    return scope.Close(result2);
  

}
};

Persistent<FunctionTemplate> HTMLScanner::s_ct;
extern "C" void init(Handle<Object> target)
{
  HandleScope scope;
  HTMLScanner::Initialize(target);
}



