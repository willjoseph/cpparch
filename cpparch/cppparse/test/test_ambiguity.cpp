
template<typename T>
class X
{
	T t;
};



void f()
{
	_Facptr = (_Facet *)&std::use_facet<_Facet>(_Loc);
	const std::collate<_Elem>& _Coll_fac =
			std::use_facet<std::collate<_Elem> >(*this);

	enum { E = A < B > 0 == X < Y > 0 };
	enum { E = A < B > C == X < Y > Z };

	(A < B > 0);
	A < (A < B > 0) > B;
	A < B > 0 == X < Y > 0;
	A < B > 0 == X < Y > Z;

	A < B, C > 0; // relational-expression
	A<B>(C)(X<Y>(Z));

	::A<B>C;
	A<B>::C;
	// A < (B > ::C)
	// A<B>::C
	A<B>::C<D>::E; //argh!
	// (((A < B) > ::C) < D ) > E
	// (A<B>::C < D) > ::E
	// A<B>::C<D>::E
	// A < ((B > ::C) < D) >::E

	_Facetptr<_Facet>::_Psave = _Psave;

	A::X<B::Y>(C::Z);

	const locale::facet *_Psave = _Facetptr < _Facet > :: _Psave;
	auto_ptr_ref < _Other > _Ans ( & _Myptr ) ;

	// simple-declaration, or relational-expression
	A < B > C;
	A < B > C();
	A < B > C[1];
	A < B > C(D);
	A < B > C(D());
	A < B > C(D[1]);


	A<B>(C)[X<Y>(Z)];
	A<B>(C)++;
	A<B>(C) &
		X<Y>(Z);
	A<B>(C) ,
		X<Y>(Z);
	A < B > 0; // relational-expression
	A < (X < Y >(Z)) >(C);
	A<B>(C) ==
		X<Y>(Z);
	A<B>(C) >
		X<Y>(Z);
	A<B>(C) ^
		X<Y>(Z);
	A<B>(C) |
		X<Y>(Z);
	A<B>(C) &&
		X<Y>(Z);
	A<B>(C) ||
		X<Y>(Z);
	A<B>(C) ?
		X<Y>(Z) : NULL;
	A<B>(C) =
		X<Y>(Z);

	A < B >(C); // named-template-specialization with function-call postfix, or simple-declaration, or relational-expression
	_N < _M; // relational-expression
	_Len - _P0 < _M; // relational-expression
	A<B> *C; // simple-declaration, or relational-expression
	A<B> **C; // simple-declaration, or relational-expression
	~_Mask();
	(A)(x)(args);
	( _Charref ) ( * _Pnode ) . _Color ;
	_Destroy ( & * _Next ) ;
}

// explicit-instantiation
template int X<int>::f();

// cast-expression or function-call
void f()
{
	(A)(x);
	(A)(x)->member;
	(A)(x)[index];
	(A)(x)(args);
	(A)(x)++;
	(A)(x());
	(A)(x(y));
	(A)(x(y()));
	(A)(x[1]);
	(A)(x(y[1]));
	(A)(x[1][1]);
	(A)(x(y[1][1]));
	(A)(x, y);
	(A)(x(), y());
	(A)(x(z), y(w));
	(A)(x(z()), y(w()));
	(A)(x[1], x[1]);
	(A)(x(y[1]), x(y[1]));
	(A)(x[1][1], x[1][1]);
	(A)(x(y[1][1]), x(y[1][1]));
	(A)(x(y, z));
	(A)(x(y(), z()));
	(A)(x(y[1], z[1]));
	(A)(x(y[1][1], z[1][1]));
}

// cast-expression with unary-expression RHS, or binary-expression
void f()
{
	(a)-b;
	(a)+b;
	(a)*b;
	(a)&b;
}

// cast '++b' to 'a' OR call 'a++' with parameter 'b
void f()
{
	(a)++(b);
	(a())++(b());
	(a(c))++(b(c));
	(a(c()))++(b(c()));
	(a[1])++(b[1]);
	(a[1][1])++(b[1][1]);
	(a(c[1]))++(b(c[1]));
}

// either declares functions or init-declares variables
void f()
{
	A a(x);
	A a(x());
	A a(x(y));
	A a(x(y()));
	A a(x[1]);
	A a(x(y[1]));
	A a(x[1][1]);
	A a(x(y[1][1]));
	A a(x, y);
	A a(x(), y());
	A a(x(z), y(w));
	A a(x(z()), y(w()));
	A a(x[1], x[1]);
	A a(x(y[1]), x(y[1]));
	A a(x[1][1], x[1][1]);
	A a(x(y[1][1]), x(y[1][1]));
	A a(x(y, z));
	A a(x(y(), z()));
	A a(x(y[1], z[1]));
	A a(x(y[1][1], z[1][1]));
}


// function-style-cast or function-call or parenthesised variable
void f()
{
	A(x);
	A(*x);
	A(x());
	A(x(y));
	A(x(*y));
	A(x(y()));
	A(x[1]);
	A(x(y[1]));
	A(x[1][1]);
	A(x(y[1][1]));
	A(x, y);
	A(x(), y());
	A(x(z), y(w));
	A(x(z()), y(w()));
	A(x[1], x[1]);
	A(x(y[1]), x(y[1]));
	A(x[1][1], x[1][1]);
	A(x(y[1][1]), x(y[1][1]));
	A(x(y, z));
	A(x(y(), z()));
	A(x(y[1], z[1]));
	A(x(y[1][1], z[1][1]));
}

// constructor-declaration or parenthesised variable-declaration
class A
{
	A(x);
	A(x());
	A(x(y));
	A(x(y()));
	A(x[1]);
	A(x(y[1]));
	A(x[1][1]);
	A(x(y[1][1]));
	A(x, y);
	A(x(), y());
	A(x(z), y(w));
	A(x(z()), y(w()));
	A(x[1], x[1]);
	A(x(y[1]), x(y[1]));
	A(x[1][1], x[1][1]);
	A(x(y[1][1]), x(y[1][1]));
	A(x(y, z));
	A(x(y(), z()));
	A(x(y[1], z[1]));
	A(x(y[1][1], z[1][1]));
};

void append(_Tr::length(_S))
{
	(size_type)(_P + _N);
}

template<class A = B()>
class C
{
};


// template-template-parameter
template<template<typename X> class A>
class C
{
	A a;
};

void f()
{
	_Iosarray * _Ptr , * _Q ; 

	_Fmtfl = (fmtflags)(((int)_Fmtfl & (int)~_Mask)
		| ((int)_Newfmtflags & (int)_Mask & (int)_Fmtmask));
}

class A
{
	A(*X);
	A(*X());
	A(*X[1]);

	A(*X){}
	A(*X()){}
	A(*X[1]){}

	int(x);
	int(y());
	int(z[1]);

	int(*x);
	int(*y());
	int(*z[1]);

	enum _Mref{_FROZEN=255};
};

void f()
{
	for(_Iterator_base * * _Pnext = ( _Iterator_base * * ) & _Myfirstiter ; ;)
	{
	}
	_Iterator_base * * _Pnext = ( _Iterator_base * * ) & _Mycont -> _Myfirstiter ; 
	sizeof ( _Inherits ( _Make_traits ( ) ) );
	A* B = C;
}

class C
{
	int  operator*() const
	{
	}

};

enum {_MIN_SIZE = sizeof (_E) };

namespace std
{
	class basic_string{
	public:
		_Myt& append(const _Myt& _X, size_type _P, size_type _M)
		{
			if(0)
			{
				_Eos(_N);
			}
			return (*this);
		}
		explicit basic_string();
		basic_string(_It _F, _It _L, const _A& _Al = _A())
			: allocator(_Al)
		{
			_Tidy();
			assign(_F, _L);
		}
	};
}

struct threadlocaleinfostruct;

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif


#include <xstddef>

		// TEMPLATE CLASS basic_string
template<class _E,
	class _Tr = char_traits<_E>,
	class _A = allocator<_E> >
	class basic_string {
public:
	typedef basic_string<_E, _Tr, _A> _Myt;
	typedef typename _A::size_type size_type;
	typedef typename _A::difference_type difference_type;
	typedef typename _A::pointer pointer;
	typedef typename _A::const_pointer const_pointer;
	typedef typename _A::reference reference;
	typedef typename _A::const_reference const_reference;
	typedef typename _A::value_type value_type;
	typedef typename _A::pointer iterator;
	typedef typename _A::const_pointer const_iterator;
	typedef reverse_iterator<const_iterator, value_type,
		const_reference, const_pointer, difference_type>
			const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type,
		reference, pointer, difference_type>
			reverse_iterator;
	explicit basic_string(const _A& _Al = _A())
		: allocator(_Al) {_Tidy(); }
	basic_string(const _Myt& _X)
		: allocator(_X.allocator)
		{_Tidy(), assign(_X, 0, npos); }
	basic_string(const _Myt& _X, size_type _P, size_type _M,
		const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_X, _P, _M); }
	basic_string(const _E *_S, size_type _N,
		const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_S, _N); }
	basic_string(const _E *_S, const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_S); }
	basic_string(size_type _N, _E _C, const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_N, _C); }
	typedef const_iterator _It;
	basic_string(_It _F, _It _L, const _A& _Al = _A())
		: allocator(_Al) {_Tidy(); assign(_F, _L); }
	~basic_string()
		{_Tidy(true); }
	typedef _Tr traits_type;
	typedef _A allocator_type;
	enum _Mref {_FROZEN = 255};
	static const size_type npos;
	_Myt& operator=(const _Myt& _X)
		{return (assign(_X)); }
	_Myt& operator=(const _E *_S)
		{return (assign(_S)); }
	_Myt& operator=(_E _C)
		{return (assign(1, _C)); }
	_Myt& operator+=(const _Myt& _X)
		{return (append(_X)); }
	_Myt& operator+=(const _E *_S)
		{return (append(_S)); }
	_Myt& operator+=(_E _C)
		{return (append(1, _C)); }
	_Myt& append(const _Myt& _X)
		{return (append(_X, 0, npos)); }
	_Myt& append(const _Myt& _X, size_type _P, size_type _M)
		{if (_X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _Len <= _M)
			_Xlen();
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::copy(_Ptr + _Len, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& append(const _E *_S, size_type _M)
		{if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::copy(_Ptr + _Len, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& append(const _E *_S)
		{return (append(_S, _Tr::length(_S))); }
	_Myt& append(size_type _M, _E _C)
		{if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::assign(_Ptr + _Len, _M, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& append(_It _F, _It _L)
		{return (replace(end(), end(), _F, _L)); }
	_Myt& assign(const _Myt& _X)
		{return (assign(_X, 0, npos)); }
	_Myt& assign(const _Myt& _X, size_type _P, size_type _M)
		{if (_X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_M < _N)
			_N = _M;
		if (this == &_X)
			erase((size_type)(_P + _N)), erase(0, _P);
		else if (0 < _N && _N == _X.size()
			&& _Refcnt(_X.c_str()) < _FROZEN - 1
			&& allocator == _X.allocator)
			{_Tidy(true);
			_Ptr = (_E *)_X.c_str();
			_Len = _X.size();
			_Res = _X.capacity();
			++_Refcnt(_Ptr); }
		else if (_Grow(_N, true))
			{_Tr::copy(_Ptr, &_X.c_str()[_P], _N);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(const _E *_S, size_type _N)
		{if (_Grow(_N, true))
			{_Tr::copy(_Ptr, _S, _N);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(const _E *_S)
		{return (assign(_S, _Tr::length(_S))); }
	_Myt& assign(size_type _N, _E _C)
		{if (_N == npos)
			_Xlen();
		if (_Grow(_N, true))
			{_Tr::assign(_Ptr, _N, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(_It _F, _It _L)
		{return (replace(begin(), end(), _F, _L)); }
	_Myt& insert(size_type _P0, const _Myt& _X)
		{return (insert(_P0, _X, 0, npos)); }
	_Myt& insert(size_type _P0, const _Myt& _X, size_type _P,
		size_type _M)
		{if (_Len < _P0 || _X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _Len <= _M)
			_Xlen();
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::copy(_Ptr + _P0, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& insert(size_type _P0, const _E *_S, size_type _M)
		{if (_Len < _P0)
			_Xran();
		if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::copy(_Ptr + _P0, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& insert(size_type _P0, const _E *_S)
		{return (insert(_P0, _S, _Tr::length(_S))); }
	_Myt& insert(size_type _P0, size_type _M, _E _C)
		{if (_Len < _P0)
			_Xran();
		if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::assign(_Ptr + _P0, _M, _C);
			_Eos(_N); }
		return (*this); }
	iterator insert(iterator _P, _E _C)
		{size_type _P0 = _Pdif(_P, begin());
		insert(_P0, 1, _C);
		return (begin() + _P0); }
	void insert(iterator _P, size_type _M, _E _C)
		{size_type _P0 = _Pdif(_P, begin());
		insert(_P0, _M, _C); }
	void insert(iterator _P, _It _F, _It _L)
		{replace(_P, _P, _F, _L); }
	_Myt& erase(size_type _P0 = 0, size_type _M = npos)
		{if (_Len < _P0)
			_Xran();
		_Split();
		if (_Len - _P0 < _M)
			_M = _Len - _P0;
		if (0 < _M)
			{_Tr::move(_Ptr + _P0, _Ptr + _P0 + _M,
				_Len - _P0 - _M);
			size_type _N = _Len - _M;
			if (_Grow(_N))
				_Eos(_N); }
		return (*this); }
	iterator erase(iterator _P)
		{size_t _M = _Pdif(_P, begin());
		erase(_M, 1);
		return (_Psum(_Ptr, _M)); }
	iterator erase(iterator _F, iterator _L)
		{size_t _M = _Pdif(_F, begin());
		erase(_M, _Pdif(_L, _F));
		return (_Psum(_Ptr, _M)); }
	_Myt& replace(size_type _P0, size_type _N0, const _Myt& _X)
		{return (replace(_P0, _N0, _X, 0, npos)); }
	_Myt& replace(size_type _P0, size_type _N0, const _Myt& _X,
		size_type _P, size_type _M)
		{if (_Len < _P0 || _X.size() < _P)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
			_Tr::copy(_Ptr + _P0, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(size_type _P0, size_type _N0, const _E *_S,
		size_type _M)
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		size_type _N;
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
			_Tr::copy(_Ptr + _P0, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(size_type _P0, size_type _N0, const _E *_S)
		{return (replace(_P0, _N0, _S, _Tr::length(_S))); }
	_Myt& replace(size_type _P0, size_type _N0,
		size_type _M, _E _C)
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		size_type _N;
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0,
					_Nm);
			_Tr::assign(_Ptr + _P0, _M, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(iterator _F, iterator _L, const _Myt& _X)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _X)); }
	_Myt& replace(iterator _F, iterator _L, const _E *_S,
		size_type _M)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _S, _M)); }
	_Myt& replace(iterator _F, iterator _L, const _E *_S)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _S)); }
	_Myt& replace(iterator _F, iterator _L,	size_type _M, _E _C)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _M, _C)); }
	_Myt& replace(iterator _F1, iterator _L1,
		_It _F2, _It _L2)
		{size_type _P0 = _Pdif(_F1, begin());
		size_type _M = 0;
		_Distance(_F2, _L2, _M);
		replace(_P0, _Pdif(_L1, _F1), _M, _E(0));
		for (_F1 = begin() + _P0; 0 < _M; ++_F1, ++_F2, --_M)
			*_F1 = *_F2;
		return (*this); }
	iterator begin()
		{_Freeze();
		return (_Ptr); }
	const_iterator begin() const
		{return (_Ptr); }
	iterator end()
		{_Freeze();
		return ((iterator)_Psum(_Ptr, _Len)); }
	const_iterator end() const
		{return ((const_iterator)_Psum(_Ptr, _Len)); }
	reverse_iterator rbegin()
		{return (reverse_iterator(end())); }
	const_reverse_iterator rbegin() const
		{return (const_reverse_iterator(end())); }
	reverse_iterator rend()
		{return (reverse_iterator(begin())); }
	const_reverse_iterator rend() const
		{return (const_reverse_iterator(begin())); }
	reference at(size_type _P0)
		{if (_Len <= _P0)
			_Xran();
		_Freeze();
		return (_Ptr[_P0]); }
	const_reference at(size_type _P0) const
		{if (_Len <= _P0)
			_Xran();
		return (_Ptr[_P0]); }
	reference operator[](size_type _P0)
		{if (_Len < _P0 || _Ptr == 0)
			return ((reference)*_Nullstr());
		_Freeze();
		return (_Ptr[_P0]); }
	const_reference operator[](size_type _P0) const
		{if (_Ptr == 0)
			return (*_Nullstr());
		else
			return (_Ptr[_P0]); }
	const _E *c_str() const
		{return (_Ptr == 0 ? _Nullstr() : _Ptr); }
	const _E *data() const
		{return (c_str()); }
	size_type length() const
		{return (_Len); }
	size_type size() const
		{return (_Len); }
	size_type max_size() const
		{size_type _N = allocator.max_size();
		return (_N <= 2 ? 1 : _N - 2); }
	void resize(size_type _N, _E _C)
		{_N <= _Len ? erase(_N) : append(_N - _Len, _C); }
	void resize(size_type _N)
		{_N <= _Len ? erase(_N) : append(_N - _Len, _E(0)); }
	size_type capacity() const
		{return (_Res); }
	void reserve(size_type _N = 0)
		{if (_Res < _N)
			_Grow(_N); }
	bool empty() const
		{return (_Len == 0); }
	size_type copy(_E *_S, size_type _N, size_type _P0 = 0) const
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N)
			_N = _Len - _P0;
		if (0 < _N)
			_Tr::copy(_S, _Ptr + _P0, _N);
		return (_N); }
	void swap(_Myt& _X)
		{if (allocator == _X.allocator)
			{std::swap(_Ptr, _X._Ptr);
			std::swap(_Len, _X._Len);
			std::swap(_Res, _X._Res); }
		else
			{_Myt _Ts = *this; *this = _X, _X = _Ts; }}
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	size_type find(const _Myt& _X, size_type _P = 0) const
		{return (find(_X.c_str(), _P, _X.size())); }
	size_type find(const _E *_S, size_type _P,
		size_type _N) const
		{if (_N == 0 && _P <= _Len)
			return (_P);
		size_type _Nm;
		if (_P < _Len && _N <= (_Nm = _Len - _P))
			{const _E *_U, *_V;
			for (_Nm -= _N - 1, _V = _Ptr + _P;
				(_U = _Tr::find(_V, _Nm, *_S)) != 0;
				_Nm -= (size_type) (_U - _V + 1), _V = _U + 1)
				if (_Tr::compare(_U, _S, _N) == 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find(const _E *_S, size_type _P = 0) const
		{return (find(_S, _P, _Tr::length(_S))); }
	size_type find(_E _C, size_type _P = 0) const
		{return (find((const _E *)&_C, _P, 1)); }
	size_type rfind(const _Myt& _X, size_type _P = npos) const
		{return (rfind(_X.c_str(), _P, _X.size())); }
	size_type rfind(const _E *_S, size_type _P,
		size_type _N) const
		{if (_N == 0)
			return (_P < _Len ? _P : _Len);
		if (_N <= _Len)
			for (const _E *_U = _Ptr +
				+ (_P < _Len - _N ? _P : _Len - _N); ; --_U)
				if (_Tr::eq(*_U, *_S)
					&& _Tr::compare(_U, _S, _N) == 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type rfind(const _E *_S, size_type _P = npos) const
		{return (rfind(_S, _P, _Tr::length(_S))); }
	size_type rfind(_E _C, size_type _P = npos) const
		{return (rfind((const _E *)&_C, _P, 1)); }
	size_type find_first_of(const _Myt& _X,
		size_type _P = 0) const
		{return (find_first_of(_X.c_str(), _P, _X.size())); }
	size_type find_first_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (0 < _N && _P < _Len)
			{const _E *const _V = _Ptr + _Len;
			for (const _E *_U = _Ptr + _P; _U < _V; ++_U)
				if (_Tr::find(_S, _N, *_U) != 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find_first_of(const _E *_S, size_type _P = 0) const
		{return (find_first_of(_S, _P, _Tr::length(_S))); }
	size_type find_first_of(_E _C, size_type _P = 0) const
		{return (find((const _E *)&_C, _P, 1)); }
	size_type find_last_of(const _Myt& _X,
		size_type _P = npos) const
		{return (find_last_of(_X.c_str(), _P, _X.size())); }
	size_type find_last_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (0 < _N && 0 < _Len)
			for (const _E *_U = _Ptr
				+ (_P < _Len ? _P : _Len - 1); ; --_U)
				if (_Tr::find(_S, _N, *_U) != 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type find_last_of(const _E *_S,
		size_type _P = npos) const
		{return (find_last_of(_S, _P, _Tr::length(_S))); }
	size_type find_last_of(_E _C, size_type _P = npos) const
		{return (rfind((const _E *)&_C, _P, 1)); }
	size_type find_first_not_of(const _Myt& _X,
		size_type _P = 0) const
		{return (find_first_not_of(_X.c_str(), _P,
			_X.size())); }
	size_type find_first_not_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (_P < _Len)
			{const _E *const _V = _Ptr + _Len;
			for (const _E *_U = _Ptr + _P; _U < _V; ++_U)
				if (_Tr::find(_S, _N, *_U) == 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find_first_not_of(const _E *_S,
		size_type _P = 0) const
		{return (find_first_not_of(_S, _P, _Tr::length(_S))); }
	size_type find_first_not_of(_E _C, size_type _P = 0) const
		{return (find_first_not_of((const _E *)&_C, _P, 1)); }
	size_type find_last_not_of(const _Myt& _X,
		size_type _P = npos) const
		{return (find_last_not_of(_X.c_str(), _P, _X.size())); }
	size_type find_last_not_of(const _E *_S, size_type _P,
		 size_type _N) const
		{if (0 < _Len)
			for (const _E *_U = _Ptr
				+ (_P < _Len ? _P : _Len - 1); ; --_U)
				if (_Tr::find(_S, _N, *_U) == 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type find_last_not_of(const _E *_S,
		size_type _P = npos) const
		{return (find_last_not_of(_S, _P, _Tr::length(_S))); }
	size_type find_last_not_of(_E _C, size_type _P = npos) const
		{return (find_last_not_of((const _E *)&_C, _P, 1)); }
	_Myt substr(size_type _P = 0, size_type _M = npos) const
		{return (_Myt(*this, _P, _M)); }
	int compare(const _Myt& _X) const
		{return (compare(0, _Len, _X.c_str(), _X.size())); }
	int compare(size_type _P0, size_type _N0,
		const _Myt& _X) const
		{return (compare(_P0, _N0, _X, 0, npos)); }
	int compare(size_type _P0, size_type _N0, const _Myt& _X,
		size_type _P, size_type _M) const
		{if (_X.size() < _P)
			_Xran();
		if (_X._Len - _P < _M)
			_M = _X._Len - _P;
		return (compare(_P0, _N0, _X.c_str() + _P, _M)); }
	int compare(const _E *_S) const
		{return (compare(0, _Len, _S, _Tr::length(_S))); }
	int compare(size_type _P0, size_type _N0, const _E *_S) const
		{return (compare(_P0, _N0, _S, _Tr::length(_S))); }
	int compare(size_type _P0, size_type _N0, const _E *_S,
		size_type _M) const
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		size_type _Ans = _Tr::compare(_Psum(_Ptr, _P0), _S,
			_N0 < _M ? _N0 : _M);
		return (int)(_Ans != 0 ? _Ans : _N0 < _M ? -1
			: _N0 == _M ? 0 : +1); }
	_A get_allocator() const
		{return (allocator); }
protected:
	_A allocator;
private:
	enum {_MIN_SIZE = sizeof (_E) <= 32 ? 31 : 7};
	void _Copy(size_type _N)
		{size_type _Ns = _N | _MIN_SIZE;
		if (max_size() < _Ns)
			_Ns = _N;
		_E *_S;
		_TRY_BEGIN
			_S = allocator.allocate(_Ns + 2, (void *)0);
		_CATCH_ALL
			_Ns = _N;
			_S = allocator.allocate(_Ns + 2, (void *)0);
		_CATCH_END
		if (0 < _Len)
			_Tr::copy(_S + 1, _Ptr,_Len>_Ns?_Ns:_Len);
		size_type _Olen = _Len;
		_Tidy(true);
		_Ptr = _S + 1;
		_Refcnt(_Ptr) = 0;
		_Res = _Ns;
		_Eos(_Olen>_Ns?_Ns:_Olen); }
	void _Eos(size_type _N)
		{_Tr::assign(_Ptr[_Len = _N], _E(0)); }
	void _Freeze()
		{if (_Ptr != 0
			&& _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			_Grow(_Len);
		if (_Ptr != 0)
			_Refcnt(_Ptr) = _FROZEN; }
	bool _Grow(size_type _N, bool _Trim = false)
		{if (max_size() < _N)
			_Xlen();
		if (_Ptr != 0
			&& _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			if (_N == 0)
				{--_Refcnt(_Ptr), _Tidy();
				return (false); }
			else
				{_Copy(_N);
				return (true); }
		if (_N == 0)
			{if (_Trim)
				_Tidy(true);
			else if (_Ptr != 0)
				_Eos(0);
			return (false); }
		else
			{if (_Trim && (_MIN_SIZE < _Res || _Res < _N))
				{_Tidy(true);
				_Copy(_N); }
			else if (!_Trim && _Res < _N)
				_Copy(_N);
			return (true); }}
	static const _E * __cdecl _Nullstr()
		{static const _E _C = _E(0);
		return (&_C); }
	static size_type __cdecl _Pdif(const_pointer _P2, const_pointer _P1)
		{return (_P2 == 0 ? 0 : _P2 - _P1); }
	static const_pointer __cdecl _Psum(const_pointer _P, size_type _N)
		{return (_P == 0 ? 0 : _P + _N); }
	static pointer __cdecl _Psum(pointer _P, size_type _N)
		{return (_P == 0 ? 0 : _P + _N); }
	unsigned char& _Refcnt(const _E *_U)
		{return (((unsigned char *)_U)[-1]); }
	void _Split()
		{if (_Ptr != 0 && _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			{_E *_Temp = _Ptr;
			_Tidy(true);
			assign(_Temp); }}
	void _Tidy(bool _Built = false)
		{if (!_Built || _Ptr == 0)
			;
		else if (_Refcnt(_Ptr) == 0 || _Refcnt(_Ptr) == _FROZEN)
			allocator.deallocate(_Ptr - 1, _Res + 2);
		else
			--_Refcnt(_Ptr);
		_Ptr = 0, _Len = 0, _Res = 0; }
	_E *_Ptr;
	size_type _Len, _Res;
	};
template<class _E, class _Tr, class _A>
	const typename basic_string<_E, _Tr, _A>::size_type
		basic_string<_E, _Tr, _A>::npos = (size_type)-1;



// TEMPLATE CLASS reverse_iterator
template<class _RanIt>
	class reverse_iterator
		: public _Iterator_base_secure
	{	// wrap iterator to run it backwards
public:
	typedef reverse_iterator<_RanIt> _Myt;
	typedef typename iterator_traits<_RanIt>::iterator_category iterator_category;
	typedef typename iterator_traits<_RanIt>::value_type value_type;
	typedef typename iterator_traits<_RanIt>::difference_type difference_type;
	typedef typename iterator_traits<_RanIt>::difference_type distance_type;	// retained
	typedef typename iterator_traits<_RanIt>::pointer pointer;
	typedef typename iterator_traits<_RanIt>::reference reference;
	typedef _RanIt iterator_type;

	 reverse_iterator()
		{	// construct with default wrapped iterator
		}

	explicit  reverse_iterator(_RanIt _Right)
		: current(_Right)
		{	// construct wrapped iterator from _Right
		}

	template<class _Other>
		 reverse_iterator(const reverse_iterator<_Other>& _Right)
		: current(_Right.base())
		{	// initialize with compatible base%%
		}

	_RanIt  base() const
		{	// return wrapped iterator
		return (current);
		}

	reference  operator*() const
		{	// return designated value
		_RanIt _Tmp = current;
		return (*--_Tmp);
		}

	pointer  operator->() const
		{	// return pointer to class object
		return (&**this);
		}

	_Myt&  operator++()
		{	// preincrement
		--current;
		return (*this);
		}

	_Myt  operator++(int)
		{	// postincrement
		_Myt _Tmp = *this;
		--current;
		return (_Tmp);
		}

	_Myt&  operator--()
		{	// predecrement
		++current;
		return (*this);
		}

	_Myt  operator--(int)
		{	// postdecrement
		_Myt _Tmp = *this;
		++current;
		return (_Tmp);
		}

	template<class _Other>
		bool  _Equal(const reverse_iterator<_Other>& _Right) const
		{	// test for iterator equality
		return (current == _Right.base());
		}

// N.B. functions valid for random-access iterators only beyond this point

	_Myt&  operator+=(difference_type _Off)
		{	// increment by integer
		current -= _Off;
		return (*this);
		}

	_Myt  operator+(difference_type _Off) const
		{	// return this + integer
		return (_Myt(current - _Off));
		}

	_Myt&  operator-=(difference_type _Off)
		{	// decrement by integer
		current += _Off;
		return (*this);
		}

	_Myt  operator-(difference_type _Off) const
		{	// return this - integer
		return (_Myt(current + _Off));
		}

	reference  operator[](difference_type _Off) const
		{	// subscript
		return (*(*this + _Off));
		}

	template<class _Other>
		bool  _Less(const reverse_iterator<_Other>& _Right) const
		{	// test if this < _Right
		return (_Right.base() < current);
		}

	template<class _Other>
		difference_type  _Minus(const reverse_iterator<_Other>& _Right) const
		{	// return difference of iterators
		return (_Right.base() - current);
		}

protected:
	_RanIt current;	// the wrapped iterator
	};

		// reverse_iterator TEMPLATE OPERATORS
template<class _RanIt,
	class _Diff> inline
	reverse_iterator<_RanIt>  operator+(_Diff _Off,
		const reverse_iterator<_RanIt>& _Right)
	{	// return reverse_iterator + integer
	return (_Right + _Off);
	}

template<class _RanIt1,
	class _RanIt2> inline
	typename reverse_iterator<_RanIt1>::difference_type
		 operator-(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	// return difference of reverse_iterators
	return (_Left._Minus(_Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool  operator==(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	// test for reverse_iterator equality
	return (_Left._Equal(_Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool  operator!=(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	// test for reverse_iterator inequality
	return (!(_Left == _Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool  operator<(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	// test for reverse_iterator < reverse_iterator
	return (_Left._Less(_Right));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool  operator>(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	// test for reverse_iterator > reverse_iterator
	return (_Right < _Left);
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool  operator<=(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	// test for reverse_iterator <= reverse_iterator
	return (!(_Right < _Left));
	}

template<class _RanIt1,
	class _RanIt2> inline
	bool  operator>=(const reverse_iterator<_RanIt1>& _Left,
		const reverse_iterator<_RanIt2>& _Right)
	{	// test for reverse_iterator >= reverse_iterator
	return (!(_Left < _Right));
	}

namespace GNamespace
{
	typedef int NType;
	NType nVar;
}

typedef GNamespace::NType GType;

GType gVar = GNamespace::nVar;

GType gFunc(GType param)
{
	typedef GType FType;
	FType lVar;
	if(FType lVar = gVar)
	{
	}
}

class GClass
{
	typedef GType CType;
	GClass(CType param) : mVar(param)
	{
	}
	CType mFunc(CType param)
	{
		typedef CType FType;
		FType fVar = param;
		if(FType cVar = mVar)
		{
			typedef FType LType;
			LType lVar;
		}

		class FClass
		{
			FClass(FType param) : mVar(param)
			{
			}
			FType mFunc(FType param)
			{
				typedef CType FType;
				FType fVar = param;
				if(FType cVar = mVar)
				{
					typedef FType LType;
					LType lVar;
				}
			}
			FType mVar;
		} fVar2;
	}
	CType mVar;

	class MClass
	{
		MClass(CType param) : mVar(param)
		{
		}
		CType mFunc(CType param)
		{
			typedef CType FType;
			FType fVar = param;
			if(FType cVar = mVar)
			{
				typedef FType LType;
				LType lVar;
			}
		}
		CType mVar;
	} mVar2;
} gVar2;
