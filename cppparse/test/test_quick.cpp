

/* 3.4.1-8
A name used in the definition of a member function (9.3) of class X following the function�s declarator-id 33) shall be
declared in one of the following ways:
...
� if X is a nested class of class Y (9.7), shall be a member of Y, or shall be a member of a base class of Y (this lookup
applies in turn to Y �s enclosing classes, starting with the innermost enclosing class),34) or
*/
struct Enclosing
{
	struct Nested
	{
		int x;
		Nested(int a = m) : x(m)
		{
			int l = m;
		}
	};
	static const int m = 0;
};

void testFor()
{
	for(int i = 0; i != 1; ++i)
	{
	}
#if 0
	for (_Iterator_base **_Pnext = (_Iterator_base **)&_Myfirstiter;
			*_Pnext != 0; *_Pnext = (*_Pnext)->_Mynextiter)
			(*_Pnext)->_Mycont = 0;
#endif
}
#if 1
/* 3.4.1-7
A name used in the definition of a class X outside of a member function body or nested class definition26
shall be declared in one of the following ways:
� before its use in class X or be a member of a base class of X (10.2)
*/
struct Base
{
	struct T
	{
		typedef int I;
	};
};

namespace N5
{
	/* 10-2
	The class-name in a base-specifier shall not be an incompletely defined class (Clause class); this class is
	called a direct base class for the class being defined. During the lookup for a base class name, non-type
	names are ignored (3.3.10)
	*/
	int Base;

	struct Derived : public Base, public Base::T
	{
		T t;
		I i;
	};

	template<typename T>
	struct Derived3 : public T
	{
	};
}
#endif

// testing name-lookup within declarator-suffix
class C6
{
	enum { SIZE = 1 };
	static int m[SIZE];

	typedef int I;
	void f(I);
};

int C6::m[SIZE];
void C6::f(I)
{
}
namespace N
{
	template<typename T>
	void f()
	{
	}
	typedef int I;
	template<>
	void f<I>();
}

template<>
void N::f<N::I>()
{
}


template<typename X>
class C2
{
	template<typename T>
	class C3
	{
		typedef T I;
		static T m;
		static T f(I);
	};
};

template<typename X>
template<typename T>
T C2<X>::C3<T>::m = I();

template<typename X>
template<typename T>
T C2<X>::C3<T>::f(I)
{
	I i;
}

namespace std
{

	template<class _Ty> inline
		void swap(_Ty& _Left, _Ty& _Right)
	{	// exchange values stored at _Left and _Right
		_Ty _Tmp = _Left;
		_Left = _Right, _Right = _Tmp;
	}

	// TEMPLATE STRUCT pair
	template<class _Ty1,
	class _Ty2> struct pair
	{	// store a pair of values
		typedef pair<_Ty1, _Ty2> _Myt;
		typedef _Ty1 first_type;
		typedef _Ty2 second_type;

		void swap(_Myt& _Right)
		{	// exchange contents with _Right
			std::swap(first, _Right.first);
			std::swap((_Ty2)second, (_Ty2)_Right.second);
		}

		template<typename T>
		void f(_Myt t)
		{
			swap((int)t, (_Ty1)t);
		}

		_Ty1 first;	// the first stored value
		_Ty2 second;	// the second stored value
	};
}

template<typename A>
class Template7
{
	template<typename B>
	void f(B);
};

template<typename X>
template<typename Y>
void Template7::f(Y)
{
}


template<typename First, typename Second>
int f(First x, Second y);

struct S
{
};

int x = f(S(), int());

template<typename Second, typename First>
int f(Second a, First b)
{
	return b;
}





struct Base3
{
	typedef int I;
};

template<typename T>
struct Spec : public Base3
{
	I m;
};

template<>
struct Spec<int> : public Base3
{
	I m;
	typedef Spec<float> other;
};


typedef struct _MIDL_SYNTAX_INFO MIDL_SYNTAX_INFO;

typedef struct _MIDL_SYNTAX_INFO
{
} MIDL_SYNTAX_INFO;



void f()
{
	{
		typedef int T;
		T i;
	}
	{
		typedef float T;
		T f;
	}
}






int f(int i)
{
	int x(f(i));
}




/* 3.4.4-1
An elaborated-type-specifier (7.1.6.3) may be used to refer to a previously declared class-name or enum-name
even though the name has been hidden by a non-type declaration (3.3.10).
*/
struct Hidden
{
	typedef int T;
};
namespace N3
{
	int Hidden;
	struct S
	{
		struct Hidden h;
	};
	Hidden::T i;
}

/* 3.4.4-1
An elaborated-type-specifier (7.1.6.3) may be used to refer to a previously declared class-name or enum-name
even though the name has been hidden by a non-type declaration (3.3.10).
*/
enum HiddenE
{
	VALUE
};
namespace N5
{
	namespace HiddenE
	{
	}
	enum HiddenE e;
}

/* 3.4.3-1
During the lookup for a name preceding the :: scope resolution
operator, object, function, and enumerator names are ignored.
*/
namespace N2
{
	typedef int T;
}

namespace N4
{
	int N2;
	N2::T i;
}

namespace N
{
#if 1
	template<typename T>
	class undeclared;
#endif

	template<typename T>
	class C
	{
		template<typename X>
		void f()
		{
			const N::undeclared<X> x; // compiles in msvc, but should be error
			/* 14.6-10
			If a name does not depend on a template-parameter (as defined in 14.6.2), a declaration (or set of declarations)
			for that name shall be in scope at the point where the name appears in the template definition; the name is
			bound to the declaration (or declarations) found at that point and this binding is not affected by declarations
			that are visible at the point of instantiation.
			*/
		}
	};
}

namespace N
{
	class X;
}

namespace N
{
	X x;
}

struct S3
{
	typedef S3 Type;
	typedef int T;
};


template<typename T>
class Template3
{
	void f()
	{
		typename T::dependent x;
	}
};



template<typename T>
class Template2;

template<typename T>
class Template2
{
	static int x;
};

template<typename T>
int Template2<T>::x;


struct Incomplete;

template<typename T>
class Template
{
	void f()
	{
		Incomplete::X(); // should interpret this as a function-call
	};
};

int f(int a)
{
	int t(a); // function-declaration or variable declaration with init
	typedef int T;
	int (*x)(T)(f);
	// illegal
	// int (*t)(T)[1];
	// int t[1](a, b);
	// int t(a)(b);
	// int t(a)[1];
}

template<class T>
class Template1
{
	friend class Template1;
	typedef Template1<int> Type;
};

typedef struct { typedef int T; } S1;
typedef struct { typedef int T; } S2;

S1::T s1t;
S2::T s2t;


void f()
{
	struct S;
	S *p;
}


class C3
{
	void reset(C3*)
	{
	}
	void f()
	{
		C3 *_Ptr;
		reset(_Ptr);
		return (*this);
	}
};


template<typename T>
class Tmpl
{
	template<typename Q>
	void func()
	{
		Q q;
	}
};

enum { ENUM1 };
enum { ENUM2 };

template<typename T>
class C1
{
	static T t;
};

template<typename T>
T C1<T>::t;

template<>
class C1<int>
{
};


namespace std
{
	class _Lockit
	{
	public:
		explicit _Lockit();
		explicit _Lockit(int);
		~_Lockit();
		static void _Lockit_ctor(int);
		static void _Lockit_dtor(int);
	private:
		static void _Lockit_ctor(_Lockit*);
		static void _Lockit_ctor(_Lockit*,int);
		static void _Lockit_dtor(_Lockit*);
		_Lockit(const _Lockit&);
		_Lockit&operator=(const _Lockit&);
		int _Locktype;
	};

	class _Mutex
	{	// lock under program control

	};
}

typedef struct _div_t {
} div_t;

typedef struct _ldiv_t {
} ldiv_t;

inline ldiv_t  div()
{
}

div_t  __cdecl div();


struct threadlocaleinfostruct;
typedef struct threadlocaleinfostruct * pthreadlocinfo;

typedef enum IMAGE_AUX_SYMBOL_TYPE {
	IMAGE_AUX_SYMBOL_TYPE_TOKEN_DEF = 1,
} IMAGE_AUX_SYMBOL_TYPE;

typedef struct fd_set
{
} fd_set;

typedef struct fd_set FD_SET;
typedef struct fd_set *PFD_SET;


class type_info;

const type_info& type;


template<class T>
class C
{
	friend class C;
	C()
	{
	}
	~C()
	{
	}
};


class A
{
	void f();
};

class B
{
	friend class A;
};

void A::f()
{
}




typedef unsigned int size_t;

template<size_t _Size>
inline char*gets_s(char(&_Buffer)[_Size])
{
}

template<size_t _Size>
inline int _snprintf_s(char(&_Dest)[_Size], size_t __Size, const char*_Format, ...)
{
};


template<typename A>
struct X
{
	template<typename B>
	struct Y
	{
		template<typename C>
		struct Z
		{
			A a;
			B b;
			C c;
		};
	};
};

X<int>::Y<int>::Z<int> p;


template<typename A>
struct Y
{
};



