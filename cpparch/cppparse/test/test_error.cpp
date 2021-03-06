
typedef struct localeinfo_struct
{
	pthreadlocinfo locinfo;
	pthreadmbcinfo mbcinfo;
}
_locale_tstruct, *_locale_t; // comma is lost

template<class _Ty>
inline void swap(_Ty&_Left, _Ty&_Right)
{
	size_t _Count, _Count1; // comma is lost
	_Ty _Tmp=_Left;
	_Left=_Right, _Right=_Tmp; // comma is lost
	if(_Strbuf==0||traits_type::eq_int_type(traits_type::eof(), _Strbuf->sbumpc()))_Strbuf=0, _Got=true; // last comma is lost
}


void test()
{
	__asm int 3 __asm int 3 // one line
	__asm { int 3; } // braced
	__asm { __asm int 3; __asm int 3; }; // nested
	__asm { __asm { int 3 }; }; // nested
}

class A
{
	void member()
	{
		this->m = new X(*this->m); // new-expression without placement
	}
};


template <class _Traits>
class _Char_traits_category
{
public:
	typedef typename _Char_traits_category_helper<_Traits, _Inherits_from_char_traits_base<_Traits>::_Exists>::_Secure_char_traits _Secure_char_traits;
};

class A
{
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
	int x;
};

#if 0
// TODO: long backtrack here: 'nested-name-specifier' vs 'type-id' beginning with 'simple-template-id'
template <class _Iterator>
class checked_array_iterator
	: public iterator<
	typename iterator_traits<_Iterator>::iterator_category, 
	typename iterator_traits<_Iterator>::value_type, 
	typename iterator_traits<_Iterator>::difference_type, 
	typename iterator_traits<_Iterator>::pointer, 
	typename iterator_traits<_Iterator>::reference>
{
};
#endif


// initializer-list
const int x[] = { 0, 1, 2 };

// member operator-function-definition
struct plus
{
	T operator()(const T& _Left, const T& _Right) const
	{
	}
};


// bitfield member-declaration
struct A {
	unsigned x : 1;
};

// list of bitfield member-declarations
struct A{
	unsigned x: 1, y : 1;
};

// member-struct
struct A {
	struct B {
	};
};


class A
{
public:
	explicit A()
		: a(0)
	{
	}
	__thiscall ~A()
	{
	}
	A& operator=(const A&)
	{
		return *this;
	}
};

class A
{
	explicit A(R(T::*m)()) // 'ptr-operator' with 'nested-name-specifier' aka ptr-to-member
	{
	}
};

inline Type<R(*)(A)> function() // ambiguity: 'template-argument' may be 'type-id' or 'primary-expression'
{
}

class basic_streambuf
{
	basic_streambuf()
		: _Plocale(new(locale)) // new-expression without placement, with parenthesised type-id
	{
	}
};

class A
{
	explicit A(const char *_Locname, size_t _Refs = 0)
		: codecvt<_Elem, _Byte, _Statype>(_Locname, _Refs) // mem-initializer with template-id
	{
	}
};


char* test = "str1" "str2"; // concatenate adjacent string-literal tokens

template<class _RanIt>
class reverse_iterator
	: public _Iterator_base_secure
{
	_Checked_iterator_base_type _Checked_iterator_base() const
	{
		typename _Checked_iterator_base_type _Base(current); // allow 'nested-name-specifier' to be omitted in 'typename-specifier'
		return _Base;
	}
};


template <class _Traits>
class _Char_traits_category_helper<_Traits, true>
{
public:
	typedef typename _Traits::_Secure_char_traits _Secure_char_traits; // typename-specifier
};

template <class _Traits>
class _Inherits_from_char_traits_base
{
	static _False _Inherits(...); // parameter-declaration-clause == ellipsis
};

template<> struct char_traits<wchar_t>:
public _Char_traits_base // base-clause
{
};

template<class _Statetype>
const _Statetype fpos<_Statetype>::_Stz = _Statetype(); // nested-name-specifier with template-id

template<class _Statetype>
class fpos
{	// store arbitrary file position
	typedef fpos<_Statetype> _Myt;
};

namespace
{
void*operator new(size_t _Size)throw()
{
	return (x < y ? x : y); // conditional-expression
}
}

void function(int arg, ...)
{
	va_list args;
	(args = p); // assignment-expression
}

void function(void(*)(void));

