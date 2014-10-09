
#ifndef INCLUDED_CPPPARSE_CORE_LITERAL_H
#define INCLUDED_CPPPARSE_CORE_LITERAL_H

#include "Fundamental.h"
#include "Ast/ExpressionImpl.h"
#include <string>

inline bool isHexadecimalLiteral(const char* value)
{
	return *value++ == '0'
		&& (*value == 'x' || *value == 'X');
}

inline bool isFloatingLiteral(const char* value)
{
	if(!isHexadecimalLiteral(value))
	{
		const char* p = value;
		for(; *p != '\0'; ++p)
		{
			if(std::strchr(".eE", *p) != 0)
			{
				return true;
			}
		}
	}
	return false;
}


inline const char* getIntegerLiteralSuffix(const char* value)
{
	const char* p = value;
	for(; *p != '\0'; ++p)
	{
		if(std::strchr("ulUL", *p) != 0)
		{
			break;
		}
	}
	return p;
}

inline const UniqueTypeId& getIntegerLiteralSuffixType(const char* suffix)
{
	if(*suffix == '\0')
	{
		return gSignedInt; // TODO: return long on overflow
	}
	if(*(suffix + 1) == '\0') // u U l L
	{
		return *suffix == 'u' || *suffix == 'U' ? gUnsignedInt : gSignedLongInt; // TODO: return long/unsigned on overflow
	}
	if(*(suffix + 2) == '\0') // ul lu uL Lu Ul lU UL LU
	{
		return gUnsignedLongInt; // TODO: long long
	}
	throw SymbolsError();
}

inline const UniqueTypeId& getIntegerLiteralType(const char* value)
{
	return getIntegerLiteralSuffixType(getIntegerLiteralSuffix(value));
}

inline IntegralConstantExpression parseIntegerLiteral(const char* value)
{
	char* suffix;
	IntegralConstant result(strtol(value, &suffix, 0)); // TODO: handle overflow
	return IntegralConstantExpression(ExpressionType(getIntegerLiteralSuffixType(suffix), false), result);
}

inline const char* getFloatingLiteralSuffix(const char* value)
{
	const char* p = value;
	for(; *p != '\0'; ++p)
	{
		if(std::strchr("flFL", *p) != 0)
		{
			break;
		}
	}
	return p;
}

inline const UniqueTypeId& getFloatingLiteralSuffixType(const char* suffix)
{
	if(*suffix == '\0')
	{
		return gDouble;
	}
	if(*(suffix + 1) == '\0') // f F l L
	{
		return *suffix == 'f' || *suffix == 'F' ? gFloat : gLongDouble;
	}
	throw SymbolsError();
}

inline const UniqueTypeId& getFloatingLiteralType(const char* value)
{
	return getFloatingLiteralSuffixType(getFloatingLiteralSuffix(value));
}

inline IntegralConstantExpression parseFloatingLiteral(const char* value)
{
	char* suffix;
	IntegralConstant result(strtod(value, &suffix)); // TODO: handle overflow
	return IntegralConstantExpression(ExpressionType(getFloatingLiteralSuffixType(suffix), false), result);
}

inline const UniqueTypeId& getCharacterLiteralType(const char* value)
{
	// [lex.ccon]
	// An ordinary character literal that contains a single c-char has type char.
	// A wide-character literal has type wchar_t.
	return *value == 'L' ? gWCharT : gChar; // TODO: multicharacter literal
}

inline IntegralConstantExpression parseCharacterLiteral(const char* value)
{
	IntegralConstant result;
	// TODO: parse character value
	return IntegralConstantExpression(ExpressionType(getCharacterLiteralType(value), false), result);
}

inline const UniqueTypeId& getNumericLiteralType(cpp::numeric_literal* symbol)
{
	const char* value = symbol->value.value.c_str();
	switch(symbol->id)
	{
	case cpp::numeric_literal::INTEGER: return getIntegerLiteralType(value);
	case cpp::numeric_literal::CHARACTER: return getCharacterLiteralType(value);
	case cpp::numeric_literal::FLOATING: return getFloatingLiteralType(value);
	case cpp::numeric_literal::BOOLEAN: return gBool;
	default: break;
	}
	throw SymbolsError();
}

inline IntegralConstantExpression parseBooleanLiteral(const char* value)
{
	return IntegralConstantExpression(ExpressionType(gBool, false), IntegralConstant(*value == 't' ? 1 : 0));
}

inline IntegralConstantExpression parseNumericLiteral(cpp::numeric_literal* symbol)
{
	const char* value = symbol->value.value.c_str();
	switch(symbol->id)
	{
	case cpp::numeric_literal::INTEGER: return parseIntegerLiteral(value);
	case cpp::numeric_literal::CHARACTER: return parseCharacterLiteral(value);
	case cpp::numeric_literal::FLOATING: return parseFloatingLiteral(value);
	case cpp::numeric_literal::BOOLEAN: return parseBooleanLiteral(value);
	default: break;
	}
	throw SymbolsError();
}

inline bool isOctal(char c)
{
	return (c >= '0' && c <= '7');
}

inline bool isHexadecimal(char c)
{
	return isNumber(c)
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}

struct StringLiteral
{
	std::size_t length;
	bool isWide;
	StringLiteral(std::size_t length, bool isWide)
		: length(length), isWide(isWide)
	{
	}
};

inline StringLiteral parseStringLiteral(cpp::string_literal* symbol, StringLiteral preceding)
{
	const char* value = symbol->value.value.c_str();
	bool isWide = false;
	std::size_t length = 0;
	const char* p = value;
	if(*p == 'L')
	{
		isWide = true;
		++p;
	}
	SYMBOLS_ASSERT(*p == '"'); // TODO: non-fatal error: expected '"'
	++p;
	for(; *p != '"';)
	{
		SYMBOLS_ASSERT(*p != '\0'); // TODO: non-fatal error: unterminated string literal
		if(*p != '\\')
		{
			++p; // normal character
		}
		else
		{
			++p; // escape character
			if(*p == 'n' || *p == 't' || *p == 'v' || *p == 'b' || *p == 'r' || *p == 'f'
				|| *p == 'a' || *p == '\\' || *p == '?' || *p == '\'' || *p == '"')
			{
				++p; // code
			}
			else if(*p == 'x')
			{
				++p; // x
				SYMBOLS_ASSERT(isHexadecimal(*p)); // TODO: non-fatal error: invalid escape sequence
				if(isHexadecimal(*++p) // if the second is hex
					&& isHexadecimal(*++p)) // and the third is hex
				{
					++p; // skip the third
				}
			}
			else
			{
				SYMBOLS_ASSERT(isOctal(*p)); // TODO: non-fatal error: invalid escape sequence
				if(isOctal(*++p) // if the second is octal
					&& isOctal(*++p)) // and the third is octal
				{
					++p; // skip the third
				}
			}
		}
		++length;
	}
	return StringLiteral(preceding.length + length, preceding.isWide | isWide);
}

#endif
