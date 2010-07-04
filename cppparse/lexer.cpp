
#include "lexer.h"

#include "profiler.h"

#include <string.h> // strrchr

///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#include <boost/wave.hpp>

///////////////////////////////////////////////////////////////////////////////
// Include the lexer stuff
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

//  This token type is one of the central types used throughout the library, 
//  because it is a template parameter to some of the public classes and  
//  instances of this type are returned from the iterators.
typedef boost::wave::cpplexer::lex_token<boost::wave::util::file_position_type> token_type;

//  The template boost::wave::cpplexer::lex_iterator<> is the lexer type to
//  to use as the token source for the preprocessing engine. It is 
//  parametrized with the token type.
typedef boost::wave::cpplexer::lex_iterator<token_type> lex_iterator_type;

struct LoadFile
{
    template <typename IterContextT>
	struct inner : public boost::wave::iteration_context_policies::load_file_to_string::inner<IterContextT>
	{
            template <typename PositionT>
            static void init_iterators(IterContextT &iter_ctx, 
				PositionT const &act_pos, boost::wave::language_support language)
            {
				ProfileScope profile(gProfileIo);
				boost::wave::iteration_context_policies::load_file_to_string::inner<IterContextT>::init_iterators(iter_ctx, act_pos, language);
			}
	};
};

typedef LoadFile input_policy_type;

struct LexNames
{
	typedef std::set<std::string, std::less<std::string>, DebugAllocator<std::string> > Identifiers;
	Identifiers identifiers;
	Identifiers filenames;

	const char* makeIdentifier(const char* value)
	{
		ProfileScope profile(gProfileIdentifier);
		return (*identifiers.insert(value).first).c_str();
	}
	const char* makeFilename(const char* value)
	{
		ProfileScope profile(gProfileIdentifier);
		return (*filenames.insert(value).first).c_str();
	}
	FilePosition makeFilePosition(const LexFilePosition& position)
	{
		FilePosition result = {
			makeFilename(position.get_file().c_str()),
			position.get_line(),
			position.get_column()
		};
		return result;
	}
};

const char* findFilename(const char* path)
{
	const char* result = strrchr(path, '/');
	if(result == 0)
	{
		result = path;
	}
	else
	{
		++result;
	}
	return result;
}

class Hooks : public boost::wave::context_policies::eat_whitespace<token_type>
{
public:
	LexNames& names;
	std::string includes[1024];
	size_t depth;

	size_t macroDepth;
	FilePosition macroPosition;

	Hooks(LexNames& names) : names(names), depth(1), macroDepth(0)
	{
		includes[0] = "$outer";
	}

	template <typename ContextT>
	void opened_include_file(ContextT const &ctx, 
		std::string const &relname, std::string const& absname,
		bool is_system_include)
	{
		includes[depth] = relname.c_str();
		std::cout << findFilename(includes[depth - 1].c_str()) << "  included: " << findFilename(includes[depth].c_str()) << std::endl;
		++depth;
	}
	template <typename ContextT>
	void returning_from_include_file(ContextT const &ctx)
	{
		--depth;
		std::cout << findFilename(includes[depth - 1].c_str()) << "  returned: " << findFilename(includes[depth].c_str()) << std::endl;
	}

	template<typename ContextT, typename TokenT>
	void expanding_macro(ContextT& ctx,const TokenT& macrocall)
	{
		if(++macroDepth == 1)
		{
			macroPosition = names.makeFilePosition(macrocall.get_position());
		}
	}
    template <typename ContextT, typename TokenT, typename ContainerT, typename IteratorT>
    bool 
    expanding_function_like_macro(ContextT& ctx,
        TokenT const& macrodef, std::vector<TokenT> const& formal_args, 
        ContainerT const& definition,
        TokenT const& macrocall, std::vector<ContainerT> const& arguments,
        IteratorT const& seqstart, IteratorT const& seqend) 
    {
		expanding_macro(ctx, macrocall);
		return false;
	}
    template <typename ContextT, typename TokenT, typename ContainerT>
    bool 
    expanding_object_like_macro(ContextT const& ctx, TokenT const& macro, 
        ContainerT const& definition, TokenT const& macrocall)
    {
		expanding_macro(ctx, macrocall);
		return false;
	}
    template <typename ContextT, typename ContainerT>
    void rescanned_macro(ContextT const &ctx, ContainerT const &result)
	{
		LEXER_ASSERT(macroDepth != 0);
		--macroDepth;
	}

    template <
        typename ContextT, typename TokenT, typename ParametersT, 
        typename DefinitionT
    >
    void
    defined_macro(ContextT const& ctx, TokenT const& macro_name, 
        bool is_functionlike, ParametersT const& parameters, 
        DefinitionT const& definition, bool is_predefined)
    {
		std::cout << "defined macro: " << macro_name.get_value().c_str() << std::endl;
	}

	const char* getSourcePath() const
	{
		return includes[depth - 1].c_str();
	}
};

//  This is the resulting context type to use. The first template parameter
//  should match the iterator type to be used during construction of the
//  corresponding context object (see below).
typedef boost::wave::context<std::string::iterator, lex_iterator_type, input_policy_type, Hooks>
context_type;

//  The preprocessor iterator shouldn't be constructed directly. It is 
//  to be generated through a wave::context<> object. This wave:context<> 
//  object is to be used additionally to initialize and define different 
//  parameters of the actual preprocessing (not done here).
//
//  The preprocessing of the input stream is done on the fly behind the 
//  scenes during iteration over the context_type::iterator_type stream.

struct LexContext : public context_type, public LexNames
{
	LexContext(std::string& instring, const char* input)
		: context_type(instring.begin(), instring.end(), input, *this)
	{
		set_language(boost::wave::language_support(
			boost::wave::support_normal
			| boost::wave::support_option_variadics
			| (boost::wave::support_option_mask &
				~(boost::wave::support_option_emit_line_directives
					| boost::wave::support_option_single_line
					| boost::wave::support_option_emit_pragma_directives))));
	}
};

struct LexIterator : public context_type::iterator_type
{
	LexIterator(const context_type::iterator_type& iterator)
		: context_type::iterator_type(iterator)
	{
	}
};

struct LexToken : public token_type
{
	LexToken(const token_type& token)
		: token_type(token)
	{
	}
};

LexToken& makeToken(token_type& token)
{
	return *static_cast<LexToken*>(&token);
}

const context_type::iterator_type& makeBase(const LexIterator& i)
{
	return *static_cast<const context_type::iterator_type*>(&i);
}

context_type::iterator_type& makeBase(LexIterator& i)
{
	return *static_cast<context_type::iterator_type*>(&i);
}


LexContext& createContext(std::string& instring, const char* input)
{
	return *(new LexContext(instring, input));
}

bool add_include_path(LexContext& context, char const *path)
{
	return context.add_include_path(path);
}

bool add_sysinclude_path(LexContext& context, char const *path)
{
	return context.add_sysinclude_path(path);
}

bool add_macro_definition(LexContext& context, const char* macrostring, bool is_predefined)
{
	return context.add_macro_definition(macrostring, is_predefined);
}

void release(LexContext& context)
{
	delete &context;
}

LexIterator& createBegin(LexContext& lexer)
{
	return *(new LexIterator(lexer.begin()));
}

LexIterator& createEnd(LexContext& lexer)
{
	return *(new LexIterator(lexer.end()));
}

LexIterator& cloneIterator(LexIterator& i)
{
	return *(new LexIterator(i));
}

void assignIterator(LexIterator& i, LexIterator& other)
{
	i = other;
}

void release(LexIterator& i)
{
	delete &i;
}

bool operator==(const LexIterator& l, const LexIterator& r)
{
	return makeBase(l) == makeBase(r);
}

Token* Lexer::read(Token* first, Token* last)
{
	try
	{
		ProfileScope profile(gProfileWave);
		for(; this->first != this->last; ++makeBase(this->first))
		{
			const token_type& token = *makeBase(this->first);
			if(!isWhiteSpace(token))
			{
				if(isEOF(token))
				{
					continue; // note: not passing EOF through, as Wave appears to generate EOF tokens mid-stream
				}
				if(first == last)
				{
					break;
				}
#ifdef _DEBUG
				{
					ProfileScope profile(gProfileDiagnose);
					printer.printToken(token, token.get_value().c_str());
				}
#endif
				FilePosition position = context.get_hooks().macroDepth == 0
					? context.makeFilePosition(token.get_position())
					: context.get_hooks().macroPosition;
				*first++ = Token(token, context.makeIdentifier(token.get_value().c_str()), position);
			}
		}
		// if reached end of token stream, append EOF
		if(this->first == this->last
			&& first != last)
		{
			*first++ = Token(boost::wave::T_EOF, "", FilePosition());
		}
	}
	catch (boost::wave::cpp_exception const& e) {
		// some preprocessing error
		std::cerr 
			<< e.file_name() << "(" << e.line_no() << "): "
			<< e.description() << std::endl;
		if(!boost::wave::is_recoverable(e))
		{
			throw LexError();
		}
		return read(first, last);
	}
	return first;
}

void increment(LexIterator& i)
{
	try {
		++makeBase(i);
	}
	catch (boost::wave::cpp_exception const& e) {
		// some preprocessing error
		std::cerr 
			<< e.file_name() << "(" << e.line_no() << "): "
			<< e.description() << std::endl;
		if(!boost::wave::is_recoverable(e))
		{
			throw LexError();
		}
	}
}

const LexToken& dereference(const LexIterator& i)
{
	try {
		return makeToken(*i);
	}
	catch (boost::wave::cpp_exception const& e) {
		// some preprocessing error
		std::cerr 
			<< e.file_name() << "(" << e.line_no() << "): "
			<< e.description() << std::endl;
		throw LexError();
	}
}

const char* get_value(const LexToken& token)
{
	return token.get_value().c_str();
}

LexTokenId get_id(const LexToken& token)
{
	return token;
}

const LexFilePosition& get_position(const LexToken& token)
{
	return token.get_position();
}
