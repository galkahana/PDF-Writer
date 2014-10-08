/*
   Source File : PDFPageContentParser.cpp


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/

#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include "ContentBasicTypes.h"
#include "IByteReader.h"
#include "PDFParserTokenizer.h"
#include "IPDFPageContentHandler.h"
#include "Trace.h"

// anonymous namespace
namespace
{

	typedef std::vector<std::string> StackType;

	bool is_string_literal( std::string const& token )
	{
		return token[0] == '(' && token[token.size()-1] == ')';
	}

	bool is_hex_literal( std::string const& token )
	{
		return token[0] == '<' && token[token.size()-1] == '>';
	}

	std::string get_literal( std::string const& token )
	{
		//precondition( is_string_literal( token ) || is_hex_literal( token ) );

		// @todo: unescape
		return token.substr( 1, token.size()-2 );
	}

	bool pop_string( StackType& stack, std::string& result )
	{
		if ( stack.empty() )
			return false;
		std::swap( result, stack.back() );
		stack.pop_back();
		return true;
	}

	bool get_double( char const* str, double& result ) 
	{
		char* endptr = NULL;
		result = strtod( str, &endptr );
		if (*endptr)
			return false;
		return true;
	}

	bool pop_double( StackType& stack, double& result ) 
	{
		if ( stack.empty() )
			return false;
		if ( !get_double( stack.back().c_str(), result ) )
			return false;
		stack.pop_back();
		return true;
	}

	bool get_int( char const* str, int& result )
	{
		char* endptr = NULL;
		result = static_cast<int>( strtol( str, &endptr, 10 ) );
		if (*endptr)
			return false;
		return true;
	}

	bool pop_int( StackType& stack, int& result )
	{ 
		if ( stack.empty() )
			return false;
		if ( !get_int( stack.back().c_str(), result ) )
			return false;
		stack.pop_back();
		return true;
	}

	bool get_resource( char const* str, std::string& result ) 
	{
		if ( !str || str[0] != '/' || !str[1] )
			return false;
		result.assign( str+1 );
		return true;
	}

	bool pop_resource( StackType& stack, std::string& result )
	{
		if ( stack.empty() )
			return false;
		if ( !get_resource( stack.back().c_str(), result ) )
			return false;
		stack.pop_back();
		return true;
	}

	bool pop_double_array( StackType& stack, std::vector<double>& result )
	{
		std::string token;
		if ( !pop_string( stack, token ) )
			return false;
		if ( token != "]" )
			return false;
		if ( !pop_string( stack, token ) )
			return false;
		while ( token != "[" )
		{
			double val = 0.0;
			if ( !get_double( token.c_str(), val ) )
				return false;
			result.push_back( val );
			if ( !pop_string( stack, token ) )
				return false;
		}
		std::reverse( result.begin(), result.end() );
		return true;
	}

	bool pop_double_components( StackType& stack, 
								std::vector<double>& components, 
								std::string* resource = NULL )
	{
		if ( stack.empty() )
			return false;
		if ( resource )
		{
			if ( get_resource( stack.back().c_str(), *resource ) )
				stack.pop_back();
			else
				resource->clear();
		}
		while ( !stack.empty() )
		{
			double component = 0.0;
			if ( !pop_double( stack, component ) )
				return false;
			components.push_back( component );
		}
		std::reverse( components.begin(), components.end() );
		return !components.empty();
	}

	bool pop_string_or_double_list( StackType& stack, StringOrDoubleList& result, bool& hex )
	{
		std::string token;
		if ( !pop_string( stack, token ) )
			return false;
		if ( token != "]" )
			return false;
		if ( !pop_string( stack, token ) )
			return false;
		while ( token != "[" )
		{
			if ( is_string_literal( token ) )
			{
				hex = false;
				result.push_back( get_literal( token ) );
			}
			else if ( is_hex_literal( token ) )
			{
				hex = true;
				result.push_back( get_literal( token ) );
			}
			else
			{
				double val = 0.0;
				if ( !get_double( token.c_str(), val ) )
					return false;
				result.push_back( val );
			}
			if ( stack.empty() )
				return false;

			if ( !pop_string( stack, token ) )
				return false;
		}
		std::reverse( result.begin(), result.end() );
		return true;
	}

	template <class Handler>
	struct ParserTableInitializer
	{
		ParserTableInitializer();
	};
 
	template <class Handler>
	class ContentParserImpl
	{
		friend class ParserTableInitializer<Handler>;

	public:
		ContentParserImpl( Handler& inHandler )
			: mHandler( inHandler )
		{
		}

		PDFHummus::EStatusCode parse( IByteReader& reader )
		{
			PDFParserTokenizer tokenizer;
			tokenizer.SetReadStream( &reader );
		
			for ( ;; )
			{
				BoolAndString token = tokenizer.GetNextToken();
				if(!token.first)
				{
					if ( !mStack.empty() )
					{
						TRACE_LOG("ContentParserImpl::parse, unknown/incomplete command or trailing arguments");
						return PDFHummus::eFailure;
					}
					break;
				}
				//std::cout << token.second << std::endl;
				HandlerTableIterator it = ContentParserImpl::sHandlerTable.find( token.second );
				if ( it != ContentParserImpl::sHandlerTable.end() )
				{
					HandlerType const fnHandler = it->second;
					PDFHummus::EStatusCode const result = (this->*fnHandler)();
					if ( result != PDFHummus::eSuccess )
					{
						TRACE_LOG1("ContentParserImpl::parse, handler method '%s' call failed",it->first.c_str());
						return PDFHummus::eFailure;
					}
					
					if ( !mStack.empty() )
					{
						TRACE_LOG1("ContentParserImpl::parse, too much arguments for handler method '%s'",it->first.c_str());
						return PDFHummus::eFailure;
					}
				}
				else
					mStack.push_back( token.second );
			}
			//std::cout << "EOS" << std::endl;
			return PDFHummus::eSuccess;
		}

	private:
		Handler& mHandler;
		std::vector<std::string> mStack;

		typedef PDFHummus::EStatusCode (ContentParserImpl::*HandlerType)();
		typedef std::map<std::string, HandlerType> HandlerTableType;
		typedef typename HandlerTableType::const_iterator HandlerTableIterator;
		static HandlerTableType sHandlerTable;

	private:
		// path stroke/fill
		PDFHummus::EStatusCode b() { return mHandler.b(); }
		PDFHummus::EStatusCode B() { return mHandler.B(); }
		PDFHummus::EStatusCode bStar() { return mHandler.bStar(); }
		PDFHummus::EStatusCode BStar() { return mHandler.BStar(); }
		PDFHummus::EStatusCode s() { return mHandler.s(); }
		PDFHummus::EStatusCode S() { return mHandler.S(); }
		PDFHummus::EStatusCode f() { return mHandler.f(); }
		PDFHummus::EStatusCode F() { return mHandler.F(); }
		PDFHummus::EStatusCode fStar() { return mHandler.fStar(); }
		PDFHummus::EStatusCode n() { return mHandler.n(); }

		// path construction
		PDFHummus::EStatusCode m()
		{ 
			double y = 0.0;
			double x = 0.0;
			if ( !( pop_double( mStack, y ) && pop_double( mStack, x ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.m( x, y );
		}
		PDFHummus::EStatusCode l()
		{ 
			double x = 0.0;
			double y = 0.0;
			if ( !( pop_double( mStack, y ) && pop_double( mStack, x ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.l( x, y );
		}
		PDFHummus::EStatusCode c()
		{ 
			double x1 = 0.0;
			double y1 = 0.0;
			double x2 = 0.0;
			double y2 = 0.0;
			double x3 = 0.0;
			double y3 = 0.0;
			if ( !( pop_double( mStack, y3 ) && pop_double( mStack, x3 )
					&& pop_double( mStack, y2 ) && pop_double( mStack, x2 )
					&& pop_double( mStack, y1 ) && pop_double( mStack, x1 ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.c( x1, y1, x2, y2, x3, y3 );
		}
		PDFHummus::EStatusCode v()
		{ 
			double x2 = 0.0;
			double y2 = 0.0;
			double x3 = 0.0;
			double y3 = 0.0;
			if ( !( pop_double( mStack, y3 ) && pop_double( mStack, x3 )
					&& pop_double( mStack, y2 ) && pop_double( mStack, x2 ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.v( x2, y2, x3, y3 );
		}
		PDFHummus::EStatusCode h() { return mHandler.h(); }

	// graphic state
		PDFHummus::EStatusCode q() { return mHandler.q(); }
		PDFHummus::EStatusCode Q() { return mHandler.Q();	}
		PDFHummus::EStatusCode cm()
		{ 
			double a = 0.0;
			double b = 0.0;
			double c = 0.0;
			double d = 0.0;
			double e = 0.0;
			double f = 0.0;
			if ( !( pop_double( mStack, f ) && pop_double( mStack, e )
					&& pop_double( mStack, d ) && pop_double( mStack, c )
					&& pop_double( mStack, b ) && pop_double( mStack, a ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.cm( a, b, c, d, e, f );
		}
		PDFHummus::EStatusCode w() 
		{
			double lw = 0.0;
			if ( !pop_double( mStack, lw ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.w( lw ); 
		} 
		PDFHummus::EStatusCode J() 
		{ 
			double lcs = 0.0;
			if ( !pop_double( mStack, lcs ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.J( lcs ); 
		} 
		PDFHummus::EStatusCode j() 
		{ 
			double ljs = 0.0;
			if ( !pop_double( mStack, ljs ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.j( ljs ); 
		} 
		PDFHummus::EStatusCode M() 
		{ 
			double ml = 0.0;
			if ( !pop_double( mStack, ml ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.M( ml ); 
		} 
		PDFHummus::EStatusCode d()
		{ 
			double phase = 0.0;
			std::vector<double> segments;
			if ( !( pop_double( mStack, phase ) 
					&& pop_double_array( mStack, segments ) 
					&& !segments.empty() ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.d( &segments[0], static_cast<int>( segments.size() ), phase );
		}
		PDFHummus::EStatusCode ri() 
		{ 
			std::string intent;
			if ( !pop_string( mStack, intent ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.ri( intent ); 
		}
		PDFHummus::EStatusCode i() 
		{
			int flatness = 0;
			if ( !pop_int( mStack, flatness ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.i( flatness ); 
		} 
		PDFHummus::EStatusCode gs() 
		{		
			std::string gsname;
			if ( !pop_string( mStack, gsname ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.gs( gsname );
		}

		// color operators
		PDFHummus::EStatusCode CS() 
		{ 
			std::string csname;
			if ( !pop_resource( mStack, csname ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.CS( csname );
		}
		PDFHummus::EStatusCode cs() 
		{ 
			std::string csname;
			if ( !pop_resource( mStack, csname ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.cs( csname );
		}
		PDFHummus::EStatusCode SC() 
		{
			std::vector<double> components;
			if ( !pop_double_components( mStack, components ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.SC( &components[0], components.size() );
		}
		PDFHummus::EStatusCode SCN() 
		{
			std::string pattern;
			std::vector<double> components;
			if ( !pop_double_components( mStack, components, &pattern ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return pattern.empty()
				? mHandler.SCN( &components[0], components.size() )
				: mHandler.SCN( &components[0], components.size(), pattern );
		}
		PDFHummus::EStatusCode sc() 
		{ 
			std::vector<double> components;
			if ( !pop_double_components( mStack, components ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.sc( &components[0], components.size() );
		}
		PDFHummus::EStatusCode scn()
		{ 
			std::string pattern;
			std::vector<double> components;
			if ( !pop_double_components( mStack, components, &pattern ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return pattern.empty()
				? mHandler.scn( &components[0], components.size() )
				: mHandler.scn( &components[0], components.size(), pattern );
		}
		PDFHummus::EStatusCode G() 
		{ 
			double gray = 0.0;
			if ( !pop_double( mStack, gray ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.G( gray ); 
		}
		PDFHummus::EStatusCode g() 
		{ 
			double gray = 0.0;
			if ( !pop_double( mStack, gray ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.g( gray ); 
		}
		
		PDFHummus::EStatusCode RG()
		{ 
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;
			if ( !( pop_double( mStack, b ) 
					&& pop_double( mStack, g ) 
					&& pop_double( mStack, r ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.RG( r, g, b );
		}
		PDFHummus::EStatusCode rg()
		{ 
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;
			if ( !( pop_double( mStack, b ) 
					&& pop_double( mStack, g ) 
					&& pop_double( mStack, r ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.rg( r, g, b );
		}
		PDFHummus::EStatusCode K()
		{ 
			double c = 0.0;
			double m = 0.0;
			double y = 0.0;
			double k = 0.0;
			if ( !( pop_double( mStack, k ) 
					&& pop_double( mStack, y ) 
					&& pop_double( mStack, m ) 
					&& pop_double( mStack, c ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.K( c, m, y, k );
		}
		PDFHummus::EStatusCode k()
		{ 
			double c = 0.0;
			double m = 0.0;
			double y = 0.0;
			double k = 0.0;
			if ( !( pop_double( mStack, k ) 
					&& pop_double( mStack, y ) 
					&& pop_double( mStack, m ) 
					&& pop_double( mStack, c ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.k( c, m, y, k );
		}

		// clip operators
		PDFHummus::EStatusCode W() { return mHandler.W(); } 
		PDFHummus::EStatusCode WStar() { return mHandler.WStar(); } 

		// XObject usage
		PDFHummus::EStatusCode Do() 
		{
			std::string xobj;
			if ( !pop_resource( mStack, xobj ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.Do( xobj ); 
		} 

		// Text state operators
		PDFHummus::EStatusCode Tc() 
		{ 
			double charspace = 0.0;
			if ( !pop_double( mStack, charspace ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.Tc( charspace ); 
		} 
		PDFHummus::EStatusCode Tw() 
		{ 
			double wordspace = 0.0;
			if ( !pop_double( mStack, wordspace ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.Tw( wordspace ); 
		} 
		PDFHummus::EStatusCode Tz() 
		{ 
			int hscaling = 0;
			if ( !pop_int( mStack, hscaling ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.Tz( hscaling ); 
		} 
		PDFHummus::EStatusCode TL() 
		{ 
			double leading = 0.0;
			if ( !pop_double( mStack, leading ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.TL( leading ); 
		} 
		PDFHummus::EStatusCode Tr() 
		{ 
			int mode = 0;
			if ( !pop_int( mStack, mode ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.Tr( mode ); 
		} 
		PDFHummus::EStatusCode Ts() 
		{ 
			double rise = 0.0;
			if ( !pop_double( mStack, rise ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.Ts( rise ); 
		} 

		// Text object operators
		PDFHummus::EStatusCode BT() { return mHandler.BT(); }
		PDFHummus::EStatusCode ET() { return mHandler.ET(); } 

		// Text positioning operators
		PDFHummus::EStatusCode Td()
		{ 
			double x = 0.0;
			double y = 0.0;
			if ( !( pop_double( mStack, y ) && pop_double( mStack, x ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.Td( x, y );
		}
		PDFHummus::EStatusCode TD()
		{ 
			double x = 0.0;
			double y = 0.0;
			if ( !( pop_double( mStack, y ) && pop_double( mStack, x ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.TD( x, y );
		}
		PDFHummus::EStatusCode Tm()
		{ 
			double a = 0.0;
			double b = 0.0;
			double c = 0.0;
			double d = 0.0;
			double e = 0.0;
			double f = 0.0;
			if ( !( pop_double( mStack, f ) && pop_double( mStack, e )
					&& pop_double( mStack, d ) && pop_double( mStack, c )
					&& pop_double( mStack, b ) && pop_double( mStack, a ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.Tm( a, b, c, d, e, f );
		}

		PDFHummus::EStatusCode TStar() { return mHandler.TStar(); } 

		// Text showing operators using the library handling of fonts with unicode text using UTF 16
		PDFHummus::EStatusCode Tf()
		{ 
			double size = 0.0;
			std::string font;
			if ( !( pop_double( mStack, size ) && pop_resource( mStack, font ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			return mHandler.TfLow( font, size );
		}
		PDFHummus::EStatusCode Tj() 
		{ 
			std::string token;
			if ( !pop_string( mStack, token ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			if ( is_string_literal( token ) )
				return mHandler.TjLow( get_literal( token ) );
			if ( is_hex_literal( token ) )
				return mHandler.TjHexLow( get_literal( token ) );
			TRACE_LOG("ContentParserImpl::parse, Invalid text format" );
			return PDFHummus::eFailure;
		}

		// The rest of the text operators, handled by the library handing of font. text is in UTF8
		PDFHummus::EStatusCode Quote() 
		{ 
			std::string txt;
			if ( !pop_string( mStack, txt ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid argument");
				return PDFHummus::eFailure;
			}
			return mHandler.QuoteLow( txt ); 
		} 
		PDFHummus::EStatusCode DoubleQuote()
		{ 
			std::string token;
			double c = 0.0;
			double w = 0.0;
			if ( !( pop_string( mStack, token ) && pop_double( mStack, c ) && pop_double( mStack, w ) ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid arguments");
				return PDFHummus::eFailure;
			}
			if ( is_string_literal( token ) )
				return mHandler.DoubleQuoteLow( w, c, get_literal( token ) );
			if ( is_hex_literal( token ) )
				return mHandler.DoubleQuoteHexLow( w, c, get_literal( token ) );
			TRACE_LOG("ContentParserImpl::parse, Invalid text format" );
			return PDFHummus::eFailure;
		} 
		PDFHummus::EStatusCode TJ()
		{
			StringOrDoubleList lst;
			bool hex = true;
			if ( !pop_string_or_double_list( mStack, lst, hex ) )
			{
				TRACE_LOG("ContentParserImpl::parse, Invalid array argument");
				return PDFHummus::eFailure;
			}
			return hex
				? mHandler.TJHexLow( lst )
				: mHandler.TJLow( lst );
		} 

	};

	template <class Handler>
	typename ContentParserImpl<Handler>::HandlerTableType ContentParserImpl<Handler>::sHandlerTable;

	template <class Handler>
	ParserTableInitializer<Handler>::ParserTableInitializer()
	{
#define DEFINE_HANDLER_FN( KEY, METHOD ) ContentParserImpl<Handler>::sHandlerTable[KEY] = &ContentParserImpl<Handler>::METHOD

		// path stroke/fill
		DEFINE_HANDLER_FN( "B", B );
		DEFINE_HANDLER_FN( "b", b );
		DEFINE_HANDLER_FN( "b*", bStar );
		DEFINE_HANDLER_FN( "B*", BStar );
		DEFINE_HANDLER_FN( "s", s );
		DEFINE_HANDLER_FN( "S", S );
		DEFINE_HANDLER_FN( "f", f );
		DEFINE_HANDLER_FN( "F", F );
		DEFINE_HANDLER_FN( "f*",fStar );
		DEFINE_HANDLER_FN( "n", n );

		// path construction
		DEFINE_HANDLER_FN( "m", m );
		DEFINE_HANDLER_FN( "l", l );
		DEFINE_HANDLER_FN( "c", c );
		DEFINE_HANDLER_FN( "v", v );
		DEFINE_HANDLER_FN( "h", h );

		// graphic state
		DEFINE_HANDLER_FN( "q", q );
		DEFINE_HANDLER_FN( "Q", Q );
		DEFINE_HANDLER_FN( "cm", cm );
		DEFINE_HANDLER_FN( "w", w );
		DEFINE_HANDLER_FN("J", J );
		DEFINE_HANDLER_FN( "j", j );
		DEFINE_HANDLER_FN( "M", M );
		DEFINE_HANDLER_FN( "d", d );
		DEFINE_HANDLER_FN( "ri", ri );
		DEFINE_HANDLER_FN( "i", i );
		DEFINE_HANDLER_FN( "gs", gs );

		// color operators
		DEFINE_HANDLER_FN( "CS", CS );
		DEFINE_HANDLER_FN( "cs", cs );
		DEFINE_HANDLER_FN( "SC", SC );
		DEFINE_HANDLER_FN( "SCN", SCN );
		DEFINE_HANDLER_FN( "sc", sc );
		DEFINE_HANDLER_FN( "scn", scn );
		DEFINE_HANDLER_FN( "G", G );
		DEFINE_HANDLER_FN( "g", g );
		DEFINE_HANDLER_FN( "RG", RG );
		DEFINE_HANDLER_FN( "rg", rg );
		DEFINE_HANDLER_FN( "K", K );
		DEFINE_HANDLER_FN( "k", k );

		// clip operators
		DEFINE_HANDLER_FN( "W", W );
		DEFINE_HANDLER_FN( "W*", WStar );

		// XObject usage
		DEFINE_HANDLER_FN( "Do", Do );

		// Text state operators
		DEFINE_HANDLER_FN( "Tc", Tc );
		DEFINE_HANDLER_FN( "Tw", Tw );
		DEFINE_HANDLER_FN( "Tz", Tz );
		DEFINE_HANDLER_FN( "TL", TL );
		DEFINE_HANDLER_FN( "Tr", Tr );
		DEFINE_HANDLER_FN( "Ts", Ts );

		// Text object operators
		DEFINE_HANDLER_FN( "BT", BT );
		DEFINE_HANDLER_FN( "ET", ET );

		// Text positioning operators
		DEFINE_HANDLER_FN( "Td", Td );
		DEFINE_HANDLER_FN( "TD", TD );
		DEFINE_HANDLER_FN( "Tm", Tm );
		DEFINE_HANDLER_FN( "T*", TStar );

		// Text showing operators using the library handling of fonts with unicode text using UTF 16
		DEFINE_HANDLER_FN( "Tf", Tf );
		DEFINE_HANDLER_FN( "Tj", Tj );

		// The rest of the text operators, handled by the library handing of font. text is in UTF8
		DEFINE_HANDLER_FN( "'", Quote );
		DEFINE_HANDLER_FN( "\"", DoubleQuote );
		DEFINE_HANDLER_FN( "TJ", TJ );

#undef DEFINE_HANDLER_FN
	}

	template class ContentParserImpl<IPDFPageContentHandler>;
	ParserTableInitializer<IPDFPageContentHandler> gParserTableInit;

} // anonymous namespace


PDFHummus::EStatusCode ParsePDFPageContent( IByteReader& inStreamReader, IPDFPageContentHandler& handler ) 
{
	ContentParserImpl<IPDFPageContentHandler> parser( handler );
	return parser.parse( inStreamReader );
}
