#include "xh_scanner.h"
#include "string.h"
#include <ctype.h>
#include "stdio.h"

namespace markup 
{

  
    // case sensitive string equality test
    // s_lowcase shall be lowercase string
    inline bool equal(const std::vector<wchar>* s2, const char* s1, size_t length)
    {
      wchar* s = (wchar *)s2; 	
      switch(s2->size())
      {
        case 8: if(s1[7] != s[7]) return false;
        case 7: if(s1[6] != s[6]) return false;
        case 6: if(s1[5] != s[5]) return false;
        case 5: if(s1[4] != s[4]) return false;
        case 4: if(s1[3] != s[3]) return false;
        case 3: if(s1[2] != s[2]) return false;
        case 2: if(s1[1] != s[1]) return false;
        case 1: if(s1[0] != s[0]) return false;
        case 0: return true;
        // default: return strncmp(s,s1,length) == 0;
      }
    }

        
    scanner::token_type scanner::scan_body() 
    {
      wchar c = get_char();
      value2.clear();	      
   
      bool ws = false;

      if(c == 0) return TT_EOF;
      else if(c == '<') return scan_tag();
      else if(c == '&')
         c = scan_entity();
      else
         ws = is_whitespace(c);
        
      while(true) 
      {
        value2.push_back(c);
        c = input.get_char();
        if(c == 0)  { push_back(c); break; }
        if(c == '<') { push_back(c); break; }

	ws = ws && is_whitespace(c); // Keep track of pure whitespace  
	/*
        if(c == '&') { push_back(c); break; }
        
        if(is_whitespace(c) != ws) 
        {
          push_back(c);
          break;
        }
	*/
      }

      value2.push_back(0);
      return ws? TT_SPACE:TT_WORD;
    }

    scanner::token_type scanner::scan_head()
    {
      wchar c = skip_whitespace();

      if(c == '>') { c_scan = &scanner::scan_body; return scan_body(); }
      if(c == '/')
      {
         wchar t = get_char();
         if(t == '>')   { c_scan = &scanner::scan_body; return TT_TAG_END; }
         else { push_back(t); return TT_ERROR; } // erroneous situtation - standalone '/'
      }

      value2.clear();
      attrname.clear();	

      // attribute name...
      while(c != '=') 
      {
        if( c == 0) return TT_EOF;
        if( c == '>' ) { push_back(c); return TT_ATTR; } // attribute without value (HTML style)
        if( is_whitespace(c) )
        {
          c = skip_whitespace();
          if(c != '=') { push_back(c); return TT_ATTR; } // attribute without value (HTML style)
          else break;
        }
        if( c == '<') return TT_ERROR;
	attrname.push_back(c);
        c = get_char();
      }

      c = skip_whitespace();
      // attribute value...
      
      if(c == '\"')
        while(c = get_char())
        {
            if(c == '\"') return TT_ATTR;
            value2.push_back(c);
        }
      else if(c == '\'') // allowed in html
        while(c = get_char())
        {
            if(c == '\'') return TT_ATTR;
            value2.push_back(c);
        }
      else  // scan token, allowed in html: e.g. align=center
        do
        {
            if( is_whitespace(c) ) return TT_ATTR;
            if( c == '>' ) { push_back(c); return TT_ATTR; }
            value2.push_back(c);
        } while(c = get_char());

      return TT_ERROR;
    }

    // caller already consumed '<'
    // scan header start or tag tail
    scanner::token_type scanner::scan_tag() 
    {
      tagname.clear();	

      wchar c = get_char();

      bool is_tail = c == '/';
      if(is_tail) c = get_char();
      else if( c == '?' )
      {
        c_scan = &scanner::scan_pi; 
        return TT_PI_START;
      }
      
      while(c) 
      {
        if(is_whitespace(c)) { c = skip_whitespace(); break; }
        if(c == '/' || c == '>') break;
        tagname.push_back(c);

        switch(tagname.size())
        {
        case 3: 
	  if (tagname[0] == '!' && tagname[1] == '-' && tagname[2] == '-')
          { c_scan = &scanner::scan_comment; return TT_COMMENT_START; }
          break;
        case 8:
	   if (tagname[0] == '!' && tagname[1] == '[' && tagname[2] == 'C' && tagname[3] == 'D' &&
	  tagname[4] == 'A' && tagname[5] == 'T' && tagname[6] == 'A' && tagname[7]=='[')		
		{ c_scan = &scanner::scan_cdata; return TT_CDATA_START; }
          else 	   if (tagname[0] == '!' && tagname[1] == 'D' && tagname[2] == 'O' && tagname[3] == 'C' &&
	  tagname[4] == 'T' && tagname[5] == 'Y' && tagname[6] == 'P' && tagname[7]=='E')		
		{ c_scan = &scanner::scan_doctype_decl; return TT_DOCTYPE_START; }
          break;
        /*case 7:
          if( equal(tag_name,"!ENTITY",7) ) { c_scan = &scanner::scan_entity_decl; return TT_ENTITY_START; }
          break; */
        }

        c = get_char();
      }
 
      if(c == 0) return TT_ERROR;    
              
      if(is_tail)
      {
          if(c == '>') return TT_TAG_END;
          return TT_ERROR;
      }
      else 
           push_back(c);
      
      c_scan = &scanner::scan_head;
      return TT_TAG_START;
    }

    // skip whitespaces.
    // returns first non-whitespace char
    wchar scanner::skip_whitespace() 
    {
        while(wchar c = get_char()) 
        {
            if(!is_whitespace(c)) return c;
        }
        return 0;
    }

    void    scanner::push_back(wchar c) { input_char = c; }

    wchar scanner::get_char() 
    { 
      if(input_char) { wchar t(input_char); input_char = 0; return t; }
      return input.get_char();
    }

    bool unescape(const char* str, wchar &uc)
    {
	  //caller consumed &# 
 	  wchar MSCP1252[] =
	  { 0,0, 0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,0,0x2030,
		0x0160,0x2039,0x0152,0,0,0,0,0x2018,0x2019,0x201C,0x201D,
		0x2022,0x2013,0x2014,0x02DC,0x2122,0x0161,0x203A,0x0153,0,0,0x0178 };

      uc = 0;

      //numeric value, char code
      int base = 10;
      if(*str == 'x') { str++; base = 16; }
      char *endptr = 0;
      long v = strtol( str, &endptr, base );
      if(endptr && *endptr == 0)
      {
          uc = (wchar)v;
          if( uc >= 0x80 && uc <= 0x9F )
            uc = MSCP1252[ uc - 0x80 ];
          return true;
      }
	  return false;
    }

    // caller consumed '&'
    wchar scanner::scan_entity() 
    {
      return '&'; //QQQ disable	
/*      char buf[32];
      int i = 0;
      wchar t;
      for(; i < 31 ; ++i )
      {
        t = get_char();
        if(t == 0) return TT_EOF;
        if(t == ';')
          break;
        buf[i] = char(t); 
      }
      buf[i] = 0;
      if(i == 2)  
      {
        if(equal(buf,"gt",2)) return '>';
        if(equal(buf,"lt",2)) return '<';
      }
      else if(i == 3 && equal(buf,"amp",3)) 
        return '&';
      else if(i == 4 && equal(buf,"apos",4)) 
		return '\'';
	  else if(i == 4 && equal(buf,"quot",4)) 
		return '\"';
	  else if(i > 2 && buf[0] == '#')
	  {
	    if(unescape(&buf[1],t))
		  return t;
	  }
      t = resolve_entity(buf,i);
      if(t) return t;
      // no luck ...
      value2.push_back('&');
      for(int n = 0; n < i; ++n)
        value2.push_back(buf[n]);
      return ';';
*/
    }

    bool scanner::is_whitespace(wchar c)
    {
        return c <= ' ' 
            && (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f');
    }


    scanner::token_type scanner::scan_comment()
    {
      value2.clear();	
      while (true)
      {
        wchar c = get_char();
        if( c == 0) return TT_EOF;
        value2.push_back(c);

	int value_length = value2.size()-1;
        if(value_length >= 2 
          && value2[value_length] == '>' 
          && value2[value_length - 1] == '-' 
          && value2[value_length - 2] == '-')
        {
          c_scan = &scanner::scan_body;
          value2[value_length -2] = 0;
          return TT_COMMENT_END; 
        }
      }
    }



    scanner::token_type scanner::scan_cdata()
    {
      value2.clear();	
      while (true)
      {
        wchar c = get_char();
        if( c == 0) return TT_EOF;
        value2.push_back(c);

	int value_length = value2.size()-1;
        if(value_length >= 2 
          && value2[value_length] == '>' 
          && value2[value_length - 1] == ']' 
          && value2[value_length - 2] == ']')
        {
          c_scan = &scanner::scan_body;
          value2[value_length -2] = 0;
          return TT_CDATA_END; 
        }
      }
    }

    scanner::token_type scanner::scan_pi()
    {
      value2.clear();	
      while (true)
      {
        wchar c = get_char();
        if( c == 0) return TT_EOF;
        value2.push_back(c);

	int value_length = value2.size()-1;
        if(value_length >= 1 
          && value2[value_length] == '>' 
          && value2[value_length - 1] == '?') 
        {
          c_scan = &scanner::scan_body;
          value2[value_length -1] = 0;
          return TT_PI_END; 
        }
      }
    }


    /* This tokenizer does not parse DTD so this is commented out 
    scanner::token_type scanner::scan_entity_decl()
    {
      if(got_tail)
      {
        c_scan = &scanner::scan_body;
        got_tail = false;
        return TT_ENTITY_END;
      }
      wchar t;
      unsigned int tc = 0;
      for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
      {
        t = get_char();
        if( t == 0 ) return TT_EOF;
        value[value_length] = t;
        if(t == '\"') tc++;
        else if( t == '>' && (tc & 1) == 0 )
        {
          got_tail = true;
          break;
        }
      }
      return TT_DATA;
    }*/


    scanner::token_type scanner::scan_doctype_decl()
    {
      value2.clear();
      unsigned int tc = 0;
      unsigned int bc = 0;	
      while (true)
      {
        wchar c = get_char();
        if( c == 0) return TT_EOF;
        value2.push_back(c);


	if(c == '\"') tc++;
        else if( c == '<' && (tc & 1) == 0 )
          ++bc;
        else if( c == '>' && (tc & 1) == 0 )
        {
          if( bc-- == 0 )
          {
		value2[value2.size() -1] = 0;
		c_scan = &scanner::scan_body;
          	return TT_DOCTYPE_END; 
          }
        }

      }
    }


/*

    scanner::token_type scanner::scan_doctype_decl()
    {
      if(got_tail)
      {
        c_scan = &scanner::scan_body;
        got_tail = false;
        return TT_DOCTYPE_END;
      }
      wchar t;
      unsigned int tc = 0;
      unsigned int bc = 0;
      for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
      {
        t = get_char();
        if( t == 0 ) return TT_EOF;

        value[value_length] = t;
        if(t == '\"') tc++;
        else if( t == '<' && (tc & 1) == 0 )
          ++bc;
        else if( t == '>' && (tc & 1) == 0 )
        {
          if( bc-- == 0 )
          {
            got_tail = true;
            break;
          }
        }
      }
      return TT_DATA;
    }

*/

}
 
