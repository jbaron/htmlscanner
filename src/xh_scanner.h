#ifndef __MARKUP
#define __MARKUP

//| 
//| simple and fast XML/HTML scanner/tokenizer
//|
//| (C) Andrew Fedoniouk @ terrainformatica.com
//|

#include <stdlib.h> // wchar_t
#include <string.h> // strcmp, etc.

#include <stdint.h>

#include <vector>

namespace markup 
{
  typedef uint16_t wchar; // Not wchar_t since this is 4 bytes.

  struct instream 
  {
    virtual wchar get_char() = 0;
  };


  class scanner
  {
  public:
    enum token_type 
    {
      TT_ERROR = -1,
      TT_EOF = 0,

      TT_TAG_START,   // <tag ...
                      //     ^-- happens here
      TT_TAG_END,     // </tag>
                      //       ^-- happens here 
                      // <tag ... />
                      //            ^-- or here 
      TT_ATTR,        // <tag attr="value" >      
                      //                  ^-- happens here   
      TT_WORD,
      TT_SPACE,

      TT_DATA,        // Should not be used

      TT_COMMENT_START, TT_COMMENT_END, // after "<!--" and "-->"
      TT_CDATA_START, TT_CDATA_END,     // after "<![CDATA[" and "]]>"
      TT_PI_START, TT_PI_END,           // after "<?" and "?>"
      TT_DOCTYPE_START, TT_DOCTYPE_END, // after "<!DOCTYPE" and ">"
      //This scanner does not parse DTD (in DOCTYPE) so this is commented out 
      //TT_ENTITY_START, TT_ENTITY_END,   // after "<!ENTITY" and ">"
      
    };

    enum $ {  MAX_NAME_SIZE = 128 };

  public:
  
    scanner(instream& is): 
        input(is), 
        input_char(0), 
	got_tail(false) { c_scan = &scanner::scan_body; }

    // get next token
    token_type      get_token() { return (this->*c_scan)(); } 
    
    
    
    // should be overrided to resolve entities, e.g. &nbsp;
    virtual wchar   resolve_entity(const char* buf, int buf_size) { return 0; }

     std::vector<wchar> value2;	
     std::vector<wchar> tagname;	
     std::vector<wchar> attrname;	
        
  private: /* methods */

    typedef token_type (scanner::*scan)();
    scan        c_scan; // current 'reader'

    // content 'readers'
    token_type  scan_body();
    token_type  scan_head();
    token_type  scan_comment();
    token_type  scan_cdata();
    token_type  scan_pi();
    token_type  scan_tag();
    token_type  scan_entity_decl();
    token_type  scan_doctype_decl();

    wchar       skip_whitespace();
    void        push_back(wchar c);
  
    wchar       get_char();
    wchar       scan_entity();

    bool        is_whitespace(wchar c);
      

  private: /* data */

    //enum state { TEXT = 0, MARKUP = 1, COMMENT = 2, CDATA = 3, PI = 4 };
    //state       where;
    token_type  token;
    
  
    instream&   input;
    wchar       input_char; 

    bool        got_tail; // aux flag used in scan_comment, etc. 

  };
}
 
#endif
