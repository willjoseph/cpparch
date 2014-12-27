
namespace N515 // test substitution of initializers containing two expressions, the first being value-dependent
{
	struct A
	{
		A(int, bool);

		int m;
	};

	template<typename T>
	int f(T t, bool b)
	{
		A a(t, b);
		return a.m;
	}

	template<typename T>
	int g(T t, bool b)
	{
		A a = { t, b };
		return a.m;
	}

	int x = f(0, false);
	int y = g(0, false);
}

namespace N514
{
	template<int>
	struct A
	{
	};

	template<typename T>
	void f(const T)
	{
		static const int i(sizeof(T));
		A<i> a;
	};
}

namespace N513
{
	template<typename T, typename U>
	class A
	{
		void f()
		{
			struct B
			{
				T t;
			} m;
			static const int i = sizeof(m.t);
		}
	};
}

namespace N512 // test determination of dependentness of 'm'
{
	template<typename T>
	struct C
	{
		enum E {};
		static const E m = (E)0;
	};

	struct B : C<int>
	{

	};

	template<typename T>
	struct A : public B
	{
		void f(int a=m)
		{
		}
	};
}


namespace N511
{
	template<class T>
	T f(T t)
	{
		return f(t); // should correctly determine that the type of 'f(t)' is dependent
	}

	template<typename T>
	struct A
	{
		static T f(T t)
		{
			return t; // dependent
		}

		template<typename U>
		static U g(U u)
		{
			return u; // dependent
		}

		static const T m1 = sizeof(f(0)); // initializer is dependent
		static const T m2 = sizeof(g(0)); // initializer not dependent, does not depend on template parameter at this depth
	};

	int i = A<int>::m1; // initializer can't be dependent because: not within a template, qualified by non-dependent
}

namespace N510
{
	const int n = 1;
	const int m = 1;

	static const int i = n + m; // can determine type and value

	template<int N>
	struct A
	{
		template<int M>
		void f(int[N + M]); // when instantiating A, must substitute expression
	};
}

namespace N508 // test instantiation of member function template declaration with member-pointer parameter
{
	template<typename T>
	struct B
	{
		template<class U>
		static int f(void(U::*)(void));

		typedef int Type;
	};

	typedef B<int>::Type Type;
}

namespace N509 // test instantiation of dependent expression within declaration of member function template
{
	template<typename T>
	struct B { };
	typedef B<int> T;

	template<typename T>
	struct A
	{
		template<typename U>
		void f(U[sizeof(sizeof(T))]);

		typedef int Type;
	};

	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N507 // test instantiation of class template with out of line member function template definition
{
	template<typename T>
	struct A
	{
		template<typename U>
		void f(U);

		typedef int Type;
	};

	template<typename T>
	template<typename U>
	void A<T>::f(U)
	{
	}

	typedef A<int>::Type Type;
}

namespace N506 // test that dependent friend functions are not treated as redeclarations 
{
	template<class T>
	struct A
	{
		friend void f(T);
	};

	template<class T>
	struct B
	{
		friend void f(T);
	};
}

namespace N505
{
	template<typename T>
	struct B { };
	typedef B<int> T;

	template<typename T>
	struct A
	{
		static const int m = sizeof(sizeof(T));
		typedef int Type;
	};

	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N504
{
	template<typename T>
	struct B { };
	typedef B<int> T;

	template<typename T>
	struct A
	{
		static const int m;
		typedef int Type;
	};

	template<typename T>
	const int A<T>::m = sizeof(sizeof(T));

	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(!__is_instantiated(T), "");
	int i = A<T>::m;
	static_assert(__is_instantiated(T), "");
}

namespace N503 // test that type of extern declaration is not required to be complete
{
	struct U;

	extern U u; // not a definition
}

namespace N501 // test instantiation of declaration within member function
{
	template<typename T>
	struct B { };
	typedef B<int> T;

	struct A
	{
		static int f()
		{
			T x;
			return 0;
		}
	};

	int i = A::f();
	static_assert(__is_instantiated(T), "");
}

#if 0 // TODO
namespace N502 // test instantiation of declaration with dependent type within member function
{
	template<typename T>
	struct B { };
	typedef B<int> T;

	template<class T>
	struct A
	{
		static int f()
		{
			T x;
			return 0;
		}
	};

	int i = A<T>::f();
	static_assert(__is_instantiated(T), "");
}
#endif

namespace N500 // test instantation of dependent default-argument
{
	template<typename T>
	struct B { };
	typedef B<int> T;

	template<class T>
	struct A
	{
		static int f(int x = sizeof(T))
		{
		}
	};

#if 0 // TODO
	int i = A<T>::f(); // instantiate dependent default argument
	static_assert(__is_instantiated(T), "");
#endif
}

namespace N499
{
	template<typename T>
	struct A
	{
		int m;
		void f()
		{
			int i = m;
			return (i);
		}
	};
}

namespace N498
{
	template<typename T>
	struct B { };
	typedef B<int> T;

	template<typename T>
	struct A
	{
		static const T m;
	};

	template<typename T>
	const T A<T>::m = sizeof(T);

	static_assert(!__is_instantiated(A<T>::m), "");
	int i = A<T>::m;
	static_assert(__is_instantiated(A<T>::m), "");
	static_assert(__is_instantiated(T), "");
}

#if 0 // TODO: explicit specialization of class member
namespace N497
{
	template<typename T>
	struct A
	{
		static const T m;
	};

	template<typename T>
	const T A<T>::m = 0;

	template<>
	const char A<char>::m = 1;

	static_assert(A<char>::m == 1, ""); // TODO: find most recently declared specialization and search back from there
	static_assert(A<int>::m == 0, "");
}
#endif

namespace N494
{
	struct A
	{
		static int m;
	};
	static_assert(!__is_instantiated(A::m), "");
	int i = A::m;
	static_assert(__is_instantiated(A::m), "");
}

namespace N496
{
	template<typename T>
	struct A
	{
		static const T m;
	};

	template<typename T>
	const T A<T>::m = 0;

	static_assert(!__is_instantiated(A<int>::m), "");
	int i = A<int>::m;
	static_assert(__is_instantiated(A<int>::m), "");
}

namespace N493 // test instantiation of template parameter used in qualified name lookup within member anonymous union definition
{
	template<typename T>
	struct B
	{
		static const int value = 1;
	};
	template<typename T>
	struct A
	{
		union
		{
			int m[T::value];
		};
		typedef int Type;
	};
	typedef B<int> T;
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N492 // test instantiation of template parameter used in qualified name lookup within member unscoped enum definition
{
	template<typename T>
	struct B
	{
		static const int value = 0;
	};
	template<typename T>
	struct A
	{
		enum E
		{
			X = T::value
		};
		typedef int Type;
	};
	typedef B<int> T;
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N491 // test instantiation of template parameter used in qualified name lookup within member class definition
{
	template<typename T>
	struct B
	{
	};
	template<typename T>
	struct A
	{
		class C
		{
			T m;
		};
		typedef int Type;
	};
	typedef B<int> T;
	typedef A<T>::Type Type;
	static_assert(!__is_instantiated(T), "");
}

namespace N490 // test instantiation of template parameter used in qualified name lookup within parameter declaration of member declaration
{
	template<typename T>
	struct B
	{
		typedef int Type;
	};
	template<typename T>
	struct A
	{
		void mf(typename T::Type);
		typedef int Type;
	};
	typedef B<int> T;
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N489 // test instantiation of template parameter used in parameter declaration of member declaration
{
	template<typename T>
	struct B
	{
	};
	template<typename T>
	struct A
	{
		void mf(T);
		typedef int Type;
	};
	typedef B<int> T;
	typedef A<T>::Type Type;
	static_assert(!__is_instantiated(T), "");
}

namespace N488 // test instantiation of template parameter used in member declaration
{
	template<typename T>
	struct B
	{
	};
	template<typename T>
	struct A
	{
		T m;
		typedef int Type;
	};
	typedef B<int> T;
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N486 // test parse of member-initializer containing dependent expression
{
	template<bool b>
	struct B
	{
	};

	template<bool b>
	struct A : B<b>
	{
		A() : B<b>()
		{
		}
	};
}

namespace N81 // test parse of template declaration with template-parameter-clause containing default-argument
{
	template<typename T>
	struct Bool
	{
		enum { value = false };
	};

	struct S
	{
		template<bool b = Bool<int>::value> // default template-argument parse should not be deferred - should be parsed immediately
		class M;
	};
}


namespace N485 // test instantiation of template parameter when used in double sizeof within function-style cast within member declaration
{
	template<int>
	struct I
	{
		typedef int Type;
		static const int value = 0;
	};
	template<typename T>
	struct A
	{
		typedef int Type;
		int m[typename I<sizeof(sizeof(T))>::Type(0)];
	};
	template<typename T>
	struct B
	{
		typedef T Type;
	};
	typedef B<int> T;
	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
	static_assert(__is_instantiated(I<sizeof(sizeof(T))>), "");
}

namespace N484 // test instantiation of template parameter when used in double sizeof within member declaration
{
	template<int>
	struct I
	{
	};
	template<typename T>
	struct A
	{
		typedef I<sizeof(sizeof(T))> Type;
	};
	template<typename T>
	struct B
	{
		typedef T Type;
	};
	typedef B<int> T;
	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N483 // test instantiation of template parameter when used in double sizeof within member declaration
{
	template<typename T>
	struct A
	{
		typedef int Type;
		static int m[sizeof(sizeof(T))];
	};
	template<typename T>
	struct B
	{
		typedef T Type;
	};
	typedef B<int> T;
	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}


namespace N478 // test instantiation of class containing member template function with non-type template-parameter and dependent return-type
{
	template<typename T>
	struct A
	{
		template<int N>
		struct B
		{
			typedef int type;
		};
		template<int N>
		typename B<N>::type& get();
	};

	typedef A<int>::B<0>::type type;
}

namespace N481 // test instantiation of template parameter when used in sizeof within member declaration
{
	template<typename T>
	struct A
	{
		static int m[sizeof(T)];
	};
	template<typename T>
	struct B
	{
		typedef T Type;
	};
	typedef B<int> T;
	static_assert(!__is_instantiated(T), "");
	int i = A<T>::m[0];
	static_assert(__is_instantiated(T), "");
}

namespace N480 // test instantiation of template parameter when used in member typedef
{
	template<typename T>
	struct A
	{
		typedef typename T::Type Type;
	};
	template<typename T>
	struct B
	{
		typedef T Type;
	};
	typedef B<int> T;
	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(__is_instantiated(T), "");
}

namespace N482 // test instantiation of template parameter when NOT used in member typedef
{
	template<typename T>
	struct A
	{
		typedef int Type;
	};
	template<typename T>
	struct B
	{
		typedef T Type;
	};
	typedef B<int> T;
	static_assert(!__is_instantiated(T), "");
	typedef A<T>::Type Type;
	static_assert(!__is_instantiated(T), "");
}

namespace N479 // test is-instantiated
{
	struct A
	{
	};
	static_assert(__is_instantiated(A), "");
}

namespace N487 // test instantiation of template used in qualified name lookup
{
	template<typename T>
	struct B
	{
		typedef T Type;
	};
	static_assert(!__is_instantiated(B<int>), "");
	typedef B<int>::Type Type;
	static_assert(__is_instantiated(B<int>), "");
}

namespace N474 // test substitution of dependent member types when class template is implicitly instantiated
{
	template<typename T>
	struct A
	{
		typedef T Type; // typedef
		T m; // non-static data member
		static T sm; // static data member 
		T mf(); // non-static member function
		static T smf(); // non-static member function
		template<typename U>
		T tmf(U); // member template function
		int bm : sizeof(T); // bitfield
	};

	typedef A<int>::Type Type;
}

#if 0 // TODO: C++11: explicit specialization of enum
namespace N477 // test parse of explicit specialization
{
	template<typename T>
	struct A
	{
		enum E { X };
	};

	template<>
	enum A<int>::E { Y };

	int i = A<float>::X;
	int j = A<int>::Y;
}
#endif


namespace N476 // test that template default-argument is not substituted unless used
{
	template<typename T, typename U = typename T::Type>
	struct A
	{
	};

	A<int, int> a;
}

namespace N475 // test parse of qualified template-id containing type name
{
	namespace M
	{
		template<typename>
		struct A
		{
		};
	}

	template<class T, int N>
	struct B
	{
		void f(M::A<T[N]>); // TODO: causes unnecessary backtrack?
	};
}

namespace N473 // test using-declaration naming function found via ADL
{
	namespace N
	{
		struct A
		{
		};
	}

	namespace M
	{
		int f(N::A);
	}

	namespace N
	{
		using M::f;
	}

	int i = f(N::A());
}

namespace N472 // test behaviour of dependent using-declaration naming member function of class template
{
	template<typename T>
	struct A
	{
		static T f(T);
	};

	template<typename T>
	struct B : A<T>
	{
		using A<T>::f;
	};

	static_assert(sizeof(B<int>::f(int())) == sizeof(int), "");
}

#if 0 // TODO: fails in Clang
namespace N467 // test that ADL pulls in all overloads
{
	struct B
	{
	};

	struct A : B
	{
		using B::f;
#if 1
		static int f(const void*)
		{
			return 0;
		}
#endif
		friend char f(A*)
		{
			return 0;
		}
	};

	const A a = A();
	static_assert(sizeof(f(&a)) == sizeof(int), "");
}
#endif


#if 0 // TODO: using-declaration
namespace N454 // test behaviour of using-declaration naming member of base class
{
	struct A
	{
		static int f(int);
		static int g(int);
	};

	struct B : A
	{
		using A::f;
		using A::g;
		static char f(int); // hides A::f
		static char g(char); // overload resolution finds A::g
	};

	static_assert(sizeof(B::f(int())) == sizeof(char), "");
	static_assert(sizeof(B::g(int())) == sizeof(int), "");
}
#endif


#if 0 // TODO: dependent using-declaration
namespace N455 // test behaviour of using-declaration naming member of dependent base class
{
	struct A
	{
		static int f(int);
		static int g(int);
	};

	template<typename T>
	struct B : T
	{
		using T::f;
		using T::g;
		static char f(int); // hides A::f
		static char g(char); // overload resolution finds A::g
	};

	static_assert(sizeof(B<A>::f(int())) == sizeof(char), "");
	static_assert(sizeof(B<A>::g(int())) == sizeof(int), "");
}
#endif

namespace N471 // test that name lookup finds the correct set of overloads when member function definitions are not in same order as declarations
{
	class B
	{
		struct A
		{
		};
		bool f(bool)const;
		bool f(A)const;
	};

	inline bool B::f(A)const
	{
		return false;
	}
	inline bool B::f(bool)const
	{
		return f(A());
	}
}

namespace N139 // test elaborated type-specifier naming enum hidden by typedef
{
	typedef enum E { } E;

	typedef enum E E;
}

namespace N138 // test elaborated type-specifier naming class hidden by typedef
{
	typedef struct S { } S;

	typedef struct S S;
}

#if 0 // TODO
namespace N470 // test redeclaration of type introduced by using-declaration
{
	namespace M
	{
		class A
		{
		};
	}
	namespace N
	{
		using M::A;
		class A; // TODO: should redeclare M::A ?
	}

	static_assert(sizeof(N::A) == sizeof(M::A), ""); // Clang fails here
}
#endif

namespace N469
{
	namespace M
	{
		typedef int I;
	}
	namespace N
	{
		using M::I;
	}

	static_assert(sizeof(N::I) == sizeof(int), "");
}

namespace N466 // test that ADL pulls in all overloads
{
	struct A
	{
		friend int f(const void*)
		{
			return 0;
		}
		friend char f(A*)
		{
			return 0;
		}
	};

	const A a = A();
	static_assert(sizeof(f(&a)) == sizeof(int), "");

}

namespace N468 // test behaviour of using-declaration naming template member function
{
	struct A
	{
		template<typename T>
		static T f(T);
	};

	struct B : A
	{
		using A::f;
	};

	static_assert(sizeof(B::f(int())) == sizeof(int), "");
}

namespace N464 // test dependent non-type using-declaration as class-member
{
	template<typename T>
	struct A : T
	{
		using T::i;

		static const int j = i;
	};

	struct B
	{
		static const int i = 0;
	};

	template<typename T>
	struct C
	{
		static const T i = 0;
	};

	int i = A<B>::j;
	int j = A<C<int> >::j;
}

namespace N459 // test dependent non-type using-declaration as class-member
{
	template<typename T>
	struct C
	{
		static const T i = 0;
	};

	template<typename T>
	struct A : C<T>
	{
		using C<T>::i;

		static const int j = i;
	};

	int i = A<int>::j;
}

namespace N463 // test non-type using-declaration as class-member
{
	struct B
	{
		static const int i = 0;
	};
	struct A : B
	{
		using B::i;

		static const int j = i;
	};
}

namespace N458 // test non-type using-declaration as class-member, chained
{
	struct C
	{
		static const int i = 0;
	};
	struct B : C
	{
		using C::i;
	};
	struct A : B
	{
		using B::i;

		static const int j = i;
	};
}


namespace N457 // test non-type using-declaration in function-body
{
	namespace N
	{
		int i;
	}
	void f()
	{
		using N::i;

		int j = i;
	}
}

namespace N465 // test dependent using-declaration naming a class template
{
	template<typename T>
	struct A
	{
	protected:
		template<typename U>
		struct Type
		{
		};
	};


	template<typename T>
	struct B : A<T>
	{
		using A<T>::Type;
		typedef typename B::template Type<int> IntType;
	};

	B<int>::IntType x;
}

namespace N462 // test dependent using-declaration naming a dependent using-declaration naming a class template
{
	template<typename T>
	struct C
	{
	protected:
		template<typename U>
		struct Type
		{
		};
	};

	template<typename T>
	struct A : C<T>
	{
	protected:
		using C<T>::Type;
	};


	template<typename T>
	struct B : A<T>
	{
		using A<T>::Type;
		typedef typename B::template Type<int> IntType;
	};

	B<int>::IntType x;
}

namespace N461 // test using-declaration naming a class template, used in dependent typedef
{
	struct A
	{
	protected:
		template<typename U>
		struct Type
		{
		};
	};


	template<typename T>
	struct B : T
	{
		using A::Type;
		typedef typename B::template Type<int> IntType;
	};

	B<A>::IntType x;
}


namespace N460 // test dependent typedef naming a class template
{
	template<typename T>
	struct A
	{
	protected:
		template<typename U>
		struct Type
		{
		};
	};


	template<typename T>
	struct B : A<T>
	{
		typedef typename A<T>::template Type<int> IntType;
	};

	B<int>::IntType x;
}

namespace N456 // test dependent using-declaration naming a class
{
	template<typename T>
	struct A
	{
	protected:
		struct Type
		{
		};
	};


	template<typename T>
	struct B : A<T>
	{
		using typename A<T>::Type; // typename is required
	};

	B<int>::Type x;
}

namespace N457 // test using-declaration naming a class template
{
	namespace N
	{
		template<typename T>
		struct Tmpl
		{
		};
	}

	using N::Tmpl;

	struct S : Tmpl<int>
	{
	};
}


#if 0 // TODO: explicit specialization of static data member
namespace N453
{
	template<int i>
	struct A
	{
		static const int value;
	};

	template<int i>
	const int A<i>::value = 0;

	template<>
	const int A<1>::value = 7;

	static_assert(A<1>::value == 7, "");
}
#endif

#if 0 // TODO: clang fails to compile this
namespace N451
{
	template<int i>
	struct A
	{
	};

	int f(int);
	char f(char);

	template<typename T>
	struct B
	{
		void f(A<sizeof(f(T()))>);
	};

	template<typename T>
	void B<T>::f(A<sizeof(f(T()))>)
	{
	}
}
#endif

#if 0
namespace N450
{
	template<class T>
	void g();

	template<class T, int N>
	void f()
	{
		g<T[N]>();
	}
}
#endif

namespace N449 // TODO: exercising bitfield parse path: constant_expression in SemaMemberDeclaratorBitfield
{
	typedef struct _LDT_ENTRY {
		short    LimitLow;
		short    BaseLow;
		union {
			struct {
				char    BaseMid;
				char    Flags1;     // Declare as bytes to avoid alignment
				char    Flags2;     // Problems.
				char    BaseHi;
			} Bytes;
			struct {
				int   BaseMid : 8;
				int   Type : 5;
				int   Dpl : 2;
				int   Pres : 1;
				int   LimitHi : 4;
				int   Sys : 1;
				int   Reserved_0 : 1;
				int   Default_Big : 1;
				int   Granularity : 1;
				int   BaseHi : 8;
			} Bits;
		} HighWord;
	} LDT_ENTRY, *PLDT_ENTRY;
}

#if 1 // TODO: check that all dependent expressions in initializer are substituted at point of instantiation
namespace N448
{
	template<int n>
	struct A
	{
		static const int i = (n + 0, n + 1, n);
	};

	int i = A<0>::i;
}
#endif

namespace N446
{
	int f(int);
	static_assert(sizeof(f('\0')) == sizeof(int), "");

	char f(char);
	static_assert(sizeof(f('\0')) == sizeof(char), "");
};

namespace N445
{
	template<typename T>
	void f()
	{
		static_assert(sizeof(T) == 4, "");
	}
};

namespace N444 // test parse of explicit specialization of function template
{
	template<typename T>
	int f(T);

	template<>
	int f(int); // T is deduced to be int

	template<>
	int f<>(int); // T is deduced to be int

	template<>
	int f<int>(int); // T is explicitly specified to be int
}

namespace N443 // test type of array of unknown size with initializer-list
{
	static int data[] ={ 0, 1, 2 };
	static_assert(sizeof(decltype(data)) == sizeof(int) * 3, "");
}

namespace N441 // test deduction of non-type template parameter
{
	template<typename T, int N>
	int f(T(&)[N])
	{
	}

	static_assert(sizeof(f("..")) == sizeof(int), "");
}

namespace N440 // test deduction of non-type template parameter
{
	template<typename T>
	struct is_array
	{
		static const bool value = false;
	};
	template<typename T, int N>
	struct is_array<T[N]>
	{
		static const bool value = true;
	};

	static_assert(is_array<char[3]>::value, "");
}

namespace N438 // evaluation of sizeof for string literals
{
	static_assert(sizeof("" "") == sizeof(char[1]), "");
	static_assert(sizeof(L"" "") == sizeof(wchar_t[1]), "");
	static_assert(sizeof("\n") == sizeof(char[2]), "");
	static_assert(sizeof("\t") == sizeof(char[2]), "");
	static_assert(sizeof("\v") == sizeof(char[2]), "");
	static_assert(sizeof("\b") == sizeof(char[2]), "");
	static_assert(sizeof("\r") == sizeof(char[2]), "");
	static_assert(sizeof("\f") == sizeof(char[2]), "");
	static_assert(sizeof("\a") == sizeof(char[2]), "");
	static_assert(sizeof("\\") == sizeof(char[2]), "");
	static_assert(sizeof("\?") == sizeof(char[2]), "");
	static_assert(sizeof("\'") == sizeof(char[2]), "");
	static_assert(sizeof("\"") == sizeof(char[2]), "");
	static_assert(sizeof("\xA") == sizeof(char[2]), "");
	static_assert(sizeof("\xAx") == sizeof(char[3]), "");
	static_assert(sizeof("\xAAx") == sizeof(char[3]), "");
	static_assert(sizeof(L"\xAAAx") == sizeof(wchar_t[3]), "");
}

namespace N437 // evaluation of sizeof for literals
{
	static_assert(sizeof("" "") == sizeof(char[1]), "");
	static_assert(sizeof('c') == sizeof(char), "");
	static_assert(sizeof(L'c') == sizeof(wchar_t), "");
	static_assert(sizeof(0) == sizeof(int), "");
	static_assert(sizeof(0.f) == sizeof(float), "");
	static_assert(sizeof(0.) == sizeof(double), "");
	static_assert(sizeof("") == sizeof(char[1]), "");
	static_assert(sizeof(L"") == sizeof(wchar_t[1]), "");
}

namespace N436 // evaluation of sizeof for union
{
	union A
	{
		char c;
		int i;
	};

	union B
	{
		char c;
		char d;
	};

	static_assert(sizeof(A) == 4, "");
	static_assert(sizeof(B) == 1, "");
}

namespace N435 // evaluation of sizeof for struct: padding and alignment
{
	struct A
	{
		char c;
		int t;
	};

	struct B
	{
		int t;
		char c;
	};

	struct C 
	{
		B b;
		char c;
	};

	struct D : B
	{
		char c;
	};

	static_assert(sizeof(A) == 8, "");
	static_assert(sizeof(B) == 8, "");
	static_assert(sizeof(C) == 12, "");
	static_assert(sizeof(D) == 12, "");
}

namespace N434 // deferred evaluation of sizeof for struct: padding and alignment
{
	template<typename T>
	struct A
	{
		char c;
		T t;
	};

	template<typename T>
	struct B
	{
		T t;
		char c;
	};

	static_assert(sizeof(A<char>) == 2, "");
	static_assert(sizeof(A<int>) == 8, "");
	static_assert(sizeof(B<char>) == 2, "");
	static_assert(sizeof(B<int>) == 8, "");
}

namespace N433
{
	template<typename T, void(*)(T*)>
	struct A
	{
	};

	void f(int*);

	void g()
	{
		A<int, f> a;
	}
}

namespace N432
{
	template<typename T>
	void f(T*);

	template<typename T, void(*)(T*)>
	struct A
	{
	};

	template<typename T>
	void g()
	{
		A<T, f> a;
	}
}

namespace N427
{
	template<typename T>
	struct A
	{
	};

	template<bool b>
	struct B
	{
		static const int value = b;
	};

	template<>
	struct A<const int>
	{
		typedef B<false> type;
		static const int value = A::type::value;
	};
}


#if 0 // TODO: perform overload resolution when name of overloaded function is used as template parameter
namespace N431
{
	void f(int);
	void f(float);

	template<void(int)>
	struct S
	{
	};

	typedef S<f> Type; // overload resolution selects 'f'
}
#endif

namespace N430
{
	template<typename T>
	struct A
	{
		enum Type { N = 0 }; // 'Type' is dependent: aka A<T>::Type
		static const Type m = (Type)0;
	};

	template<typename T>
	const typename A<T>::Type A<T>::m;

	struct B : A<int>
	{
		void f()
		{
			int a = m;
		}
	};
}

namespace N142
{
	template<typename T>
	struct A
	{
		enum Type { N = 0 }; // 'Type' is dependent: aka A<T>::Type
		static const Type m = (Type)0;
	};

	template<typename T>
	const typename A<T>::Type A<T>::m;

	struct B : A<int>
	{
		void f(int a = m)
		{
		}
	};
}



namespace N403
{
	enum file_type
	{
		status_error, status_unknown=status_error, file_not_found
	};
}

namespace N428
{
	struct A
	{
		static const int VALUE=0;
	};
	A a;
	int i = a.VALUE;
}

namespace N429
{
	template<typename T>
	struct A
	{
		static const T VALUE=0;
	};
	A<int> a;
	int i = a.VALUE;
}



namespace N426
{
	template<class T>
	struct A
	{
		static const T value = 1;
		static char c[value];
	};

	template<class T>
	char A<T>::c[A<T>::value];

	A<int> a;
}

#if 0 // TODO: need to allow this?
namespace N425
{
	int f(void*);

	template<class T>
	struct A
	{
		static const T value = 0;
		// this is allowed by clang in C++98 mode, but not C++11?
		// related to DR 903?
		static const int x = sizeof(f(value)); // 'value' is type-dependent, so we don't know if it's a constant-expression

	};

	A<int> a;
}
#endif

namespace N424
{
	template<int x>
	struct B
	{
		static const int value = x;
	};

	template<class T>
	struct A
	{
		static const T value = 0;
		typedef B<(int)value> Type; // 'value' is type-dependent, so we don't know if it's a constant-expression
	};

	int x = A<int>::Type::value;
}

#if 0 // TODO: correct evaluation of constant-ness (C++11)
namespace N423
{
	template<int i>
	struct A
	{
	};

	int x;
	A<(x, 0)> a; // comma operator
	A<(true ? 0 : x)> b; // conditional
	A<(int())> a; // default-construct
}
#endif

namespace N422 // type-dependent expression as template-argument
{
	template<bool b>
	struct A
	{
	};

	template<typename T>
	struct B
	{
		static const T value = 0; // T may be integral or not

		// 'value' is a value-dependent id-expression
		// .. which cannot be determined to be an integral-constant-expression because it is type-dependent
		void f(A<value>);
	};

	template<typename T>
	void B<T>::f(A<B::value>) // should match declaration in B according to the ODR
	{
	};
}

#if 0 // TODO: msvc 12.0 accepts this, but it appears to be non-standard
namespace N410 // workaround implementation of boost::is_base_and_derived specifically for msvc 7.1
{
	struct yes_type
	{
		char c;
	};
	struct no_type
	{
		int m;
	};

	struct B
	{
	};

	struct D : public B
	{
	};

	struct bd_helper
	{
		static yes_type check_sig(D*, long);
		static no_type check_sig(B* const&, int);
	};


	struct is_base_and_derived_impl2
	{
		struct Host
		{
			operator B* const&()const;
			operator D*();
		};
		static_assert(sizeof(bd_helper::check_sig(Host(), 0)) == sizeof(yes_type), "");
	};
}
#endif

#if 0
namespace N418
{
	template<typename>
	struct B
	{
	};

	template<typename F>
	struct S;
	template<typename F>
	struct S<F()> : B<F()> // bug: incorrectly recognises the F() in the base-specifier as the definition of a template function
	{
	};
}
#endif

namespace N417
{
	class Empty
	{
		typedef union // c-style, NOT anonymous
		{
			int m; // member of 'U'
		} U;
	};
	class NonEmpty
	{
		union // anonymous
		{
			int m; // becomes a member of enclosing class
		};
	};
	static_assert(__is_empty(Empty), "");
	static_assert(!__is_empty(NonEmpty), "");
}

namespace N415
{
	struct Empty
	{
	};

	struct NonEmpty
	{
		int m;
	};

	static_assert(__is_empty(Empty), "");
	static_assert(!__is_empty(NonEmpty), "");
}

namespace N416 // testing handling of c-style struct definition 
{
	typedef struct
	{
		int m;
	}
	A;
}


namespace N414
{
	struct Abstract
	{
		virtual void f() = 0;
	};

	struct Regular
	{
		virtual void f();
	};

	static_assert(__is_abstract(Abstract), "");
	static_assert(!__is_abstract(Regular), "");
}

namespace N413
{
	struct Polymorphic
	{
		virtual void f();
	};

	struct Regular
	{
		void f();
	};

	static_assert(__is_polymorphic(Polymorphic), "");
	static_assert(!__is_polymorphic(Regular), "");
}

namespace N411 // type traits __is_convertible_to
{
	static_assert(__is_convertible_to(int, float), "");
	static_assert(!__is_convertible_to(const int*, int*), "");
}

namespace N412
{
	struct VirtualDestructor
	{
		virtual ~VirtualDestructor();
	};

	struct NonVirtualDestructor
	{
		~NonVirtualDestructor();
	};

	static_assert(__has_virtual_destructor(VirtualDestructor), "");
	static_assert(!__has_virtual_destructor(NonVirtualDestructor), "");
}



namespace N408 // technique used in boost::is_base_or_derived
{
	struct yes_type
	{
		char c;
	};
	struct no_type
	{
		int m;
	};

	template<bool b>
	struct Bool
	{
		static const bool value = b;
	};

	template<typename B, typename D>
	struct bd_helper
	{
		template<typename T>
		static yes_type check_sig(D*, T);
		static no_type check_sig(B*, int);
	};
	template<typename B, typename D>
	struct is_base_and_derived_impl2
	{
		struct Host
		{
			operator B*()const;
			operator D*();
		};
		static const bool value=sizeof(bd_helper<B, D>::check_sig(Host(), 0))==sizeof(yes_type);
	};

	template<typename Base, typename Derived>
	struct is_base_and_derived : public Bool<(is_base_and_derived_impl2<Base, Derived>::value)>
	{
	public:

	};

	struct B
	{
	};

	struct D : private B
	{
	};

	static_assert(is_base_and_derived<B, D>::value == true, "");
}

namespace N407
{

	template<typename T>
	struct A
	{
		static int f(T);
		static bool f();
	};
	template<typename T>
	struct B
	{
		static const bool value = sizeof(A<T>::f(0)) == sizeof(int); // evaluation of dependent function-call expression
	};

	static_assert(B<int>::value == true, "");

}

namespace N406
{
	struct A
	{
		typedef void(*unspecified_bool_type)();
		operator unspecified_bool_type() const;
	};
	A a;
	bool b = a || 0; // overload resolution finds built-in operator|| via conversion operator
}
namespace N405
{
	enum E
	{
		value
	};
	inline E& operator&=(E& x, E y);
	struct A
	{
		E m;
		void f()
		{
			m &= value; // 'm' is non-static and should be transformed to a class-member-access
		}
	};
}

namespace N404
{
	class A
	{
	};
	namespace N
	{
		void f(A a);
	}
	void f(A a)
	{
		return N::f(a); // should NOT find '::f' via argument dependent lookup
	}
}

namespace N402 // test name lookup and expression evaluation in out-of-line nested class definition
{
	template<typename T>
	struct B
	{
		struct A;
		static int f();
		template<int>
		struct C
		{
		};
	};
	template<typename T>
	struct B<T>::A : C<sizeof(f())> // 'C' and 'f' found via enclosing type 'B'
	{
		void mf()
		{
			f(); // 'f' found via enclosing type 'B'
		}
	};
}

namespace N401 // test name lookup and expression evaluation in out-of-line nested class definition
{
	struct B
	{
		struct A;
		static int f();
		template<int>
		struct C
		{
		};
	};
	struct B::A : C<sizeof(f())> // 'C' and 'f' found via enclosing type 'B'
	{
		void mf()
		{
			f(); // 'f' found via enclosing type 'B'
		}
	};
}

namespace N398 // technique used in boost::is_convertible_to
{
	template<typename T, typename U>
	struct A
	{
		static int f(...);
		static bool f(T);

		static const U& m;
		static const bool value = sizeof(f(m)) == sizeof(bool);
	};

	struct C
	{
	};

	static_assert(A<int, int>::value == true, "");
	static_assert(A<int, C>::value == false, "");
}

namespace N390 // technique used in boost::is_class
{
	template<bool b>
	struct A
	{
		static const bool value = b;
	};

	template<typename T>
	struct B
	{
		template<class U>
		static int f(void(U::*)(void));
		template<class U>
		static bool f(...);
		static const bool value = sizeof(f<T>(0)) == sizeof(int); // SFINAE during overload resolution for static member B<T>::f
	};

	struct C
	{
	};

	static_assert(B<C>::value == true, "");
	static_assert(B<int>::value == false, "");
}

namespace N389
{
	template<int i>
	struct A
	{
	};

	template<int i>
	struct B
	{
		typedef typename A<int(i)>::Type Type; // 'int(i)' is value-dependent
	};
}

#if 0 // TODO: bug: failure to perform implicit conversion
namespace N388
{
	template<bool b>
	struct A
	{
	};

	template<bool b, int N>
	struct C;

	template<bool b>
	struct C<b, 1>
	{
		C(A<b>)
		{
		}
	};
	template<bool b>
	struct C<b, 2>
	{
		typedef A<b> Value;
		C(Value, Value)
		{
		}
	};

	struct B : A<true>
	{
	};

	template<bool b>
	bool f(C<b, 1>);

	bool b = f<true>(B());
}
#endif

#if 0 // TODO: bug: failure to resolve overloaded built-in operator==(int, int)
namespace N387
{
	struct A
	{
		enum { VALUE } id;
	};

	struct B
	{
		enum { VALUE } id;
	};

	A a;

	bool b = a.id == B::VALUE; // should silently succeed, though a warning would be nice..
}
#endif

namespace N386
{
#define STATIC_ASSERT_SIZEOF(e, expected) static_assert(sizeof(e) == expected, "")
	STATIC_ASSERT_SIZEOF(bool, 1);
	STATIC_ASSERT_SIZEOF(char, 1);
	STATIC_ASSERT_SIZEOF(short, 2);
	STATIC_ASSERT_SIZEOF(int, 4);
	STATIC_ASSERT_SIZEOF(float, 4);
	STATIC_ASSERT_SIZEOF(double, 8);
	STATIC_ASSERT_SIZEOF(long long int, 8);

	enum E
	{
	};
	STATIC_ASSERT_SIZEOF(E, 4);

	struct S
	{
		int i;
	};
	STATIC_ASSERT_SIZEOF(S, 4);

	struct A
	{
		float f;
		int i;
	};
	STATIC_ASSERT_SIZEOF(A, 8);
}

namespace N372
{
	// using decltype-specifier in simple-type-specifier
	decltype(0) a;

	struct A
	{
		void f()
		{
#ifdef _CPPP_TEST // msvc 2010 fails this test
			// using decltype-specifier in nested-name-specifier and unqualified-id
			decltype(A())::~decltype(A())();
#endif
		}
	};

	void f()
	{
#ifdef _CPPP_TEST // msvc 2010 fails this test
		// using decltype-specifier in psuedo-destructor-name
		(0).~decltype(0)();
#endif
		// using decltype-specifier in simple-type-specifier
		decltype(A()) a;
#ifdef _CPPP_TEST // msvc 2010 fails this test
		// using decltype-specifier in psuedo-destructor-name
		a.~decltype(A())();
#endif
	};

#ifdef _CPPP_TEST // msvc 2010 fails this test
	struct B : decltype(A()) // using decltype-specifier in base-specifier
	{
		B() : decltype(A())(A()) // using decltype-specifier in mem-initializer
		{
		}
	};
#endif
}

#ifdef _CPPP_TEST // causes static_assert to fire
namespace N371
{
	static_assert(true, "");
	static_assert(false, "?false");

	class A
	{
		static_assert(true, "");
		static_assert(false, "?false");
	};

	void f()
	{
		static_assert(true, "");
		static_assert(false, "?false");
	}
}
#endif

namespace N385 // test deferred evaluation of static_assert
{
	template<bool b>
	class A
	{
		static_assert(!b, "");
		static_assert(b, "?false");

		typedef int Type;
	};

	typedef A<false>::Type Type;
}

namespace cppp
{
	template<class T, class U>
	struct is_same
	{
		static const bool value = false;
	};

	template<class T>
	struct is_same<T, T> // TODO: unit test for partial specialization
	{
		static const bool value = true;
	};

}

#define STATIC_ASSERT(e) static_assert(e, "")
#define IS_SAME(T, U) cppp::is_same<T, U>::value
#define STATIC_ASSERT_IS_SAME(T, U) STATIC_ASSERT(IS_SAME(T, U))
#define STATIC_ASSERT_IS_DIFFERENT(T, U) STATIC_ASSERT(!IS_SAME(T, U))
#define ASSERT_EXPRESSION_TYPE(e, T) STATIC_ASSERT_IS_SAME(decltype(e), T)

namespace N382
{
	STATIC_ASSERT_IS_SAME(int, int);
	STATIC_ASSERT_IS_DIFFERENT(int, bool);
	STATIC_ASSERT_IS_SAME(int*, int*);
	STATIC_ASSERT_IS_DIFFERENT(int, int*);
	STATIC_ASSERT_IS_SAME(int&, int&);
	STATIC_ASSERT_IS_DIFFERENT(int, int&);
	STATIC_ASSERT_IS_SAME(const int&, const int&);
	STATIC_ASSERT_IS_DIFFERENT(int, const int&);
	STATIC_ASSERT_IS_SAME(const int, const int);
	STATIC_ASSERT_IS_DIFFERENT(int, const int);
	STATIC_ASSERT_IS_SAME(unsigned int, unsigned int);
	STATIC_ASSERT_IS_DIFFERENT(int, unsigned int);
}

#if 0 // TODO: offsetof expression should be usable in a template argument
namespace N434
{
	struct S
	{
		int m;
	};

	template<int>
	struct A
	{
	};

	STATIC_ASSERT_IS_SAME(A<(int)&(((S*)0)->m)>, A<(int)&(((S*)0)->m)>); // offsetof should produce a unique expression
}
#endif


namespace N495
{
	struct A
	{
		int f(int);
	};

	STATIC_ASSERT_IS_SAME(decltype(&A::f), int(A::*)(int));
}

#if 0 // TODO: evaluate type of pointer to member via dependent using-declaration
namespace N494 // test behaviour of dependent using-declaration naming member function of class template, used in pointer to member
{
	template<typename T>
	struct A
	{
		T f(T);
	};

	template<typename T>
	struct B : A<T>
	{
		using A<T>::f;
	};

	STATIC_ASSERT_IS_SAME(decltype(&B<int>::f), int(A<int>::*)(int));
}
#endif

namespace N383
{
	ASSERT_EXPRESSION_TYPE(true, bool);
	ASSERT_EXPRESSION_TYPE(false, bool);
	ASSERT_EXPRESSION_TYPE(0, int);
	ASSERT_EXPRESSION_TYPE(0u, unsigned);
	ASSERT_EXPRESSION_TYPE(0l, long);
	ASSERT_EXPRESSION_TYPE(0ul, unsigned long);
	ASSERT_EXPRESSION_TYPE(0U, unsigned);
	ASSERT_EXPRESSION_TYPE(0L, long);
	ASSERT_EXPRESSION_TYPE(0UL, unsigned long);
	ASSERT_EXPRESSION_TYPE(0.f, float);
	ASSERT_EXPRESSION_TYPE(0.0, double);
	ASSERT_EXPRESSION_TYPE(0.l, long double);
	// ASSERT_EXPRESSION_TYPE(0ll, long long); // TODO: long long
	ASSERT_EXPRESSION_TYPE('\0', char);
	ASSERT_EXPRESSION_TYPE(L'\0', wchar_t);
	//ASSERT_EXPRESSION_TYPE("", const char(&)[1]); // TODO: array size for string literal
	//ASSERT_EXPRESSION_TYPE(L"", const wchar_t*(&)[1]);
	ASSERT_EXPRESSION_TYPE(9 * (8.2), double);
	ASSERT_EXPRESSION_TYPE(9 * 8.2, double);
	ASSERT_EXPRESSION_TYPE(3 * (9 * 8.2), double);
	ASSERT_EXPRESSION_TYPE(1 * 2U * 3.0, double);

	ASSERT_EXPRESSION_TYPE(int(), int);

	// The type denoted by decltype(e) is defined as follows:
	//  - if e is an unparenthesized id-expression or an unparenthesized class member access (5.2.5), decltype(e)
	//  	is the type of the entity named by e. If there is no such entity, or if e names a set of overloaded functions,
	//  	the program is ill-formed;
	//  - otherwise, if e is an xvalue, decltype(e) is T&&, where T is the type of e;
	//  - otherwise, if e is an lvalue, decltype(e) is T&, where T is the type of e;
	//  - otherwise, decltype(e) is the type of e.

	struct A
	{
		int f();
		int m;
		mutable int mm;
		const int cm = 0;
	};

	int f();
	int& r();
	const int cf();
	int i;
	const int ci = 0;
	A a;
	const A ca = A();
	A* p;
	int A::* pm;
	int (A::* pmf)();

	enum E { VALUE };

	ASSERT_EXPRESSION_TYPE(i, int); // unparenthesized id-expression
	ASSERT_EXPRESSION_TYPE(ci, const int); // unparenthesized id-expression
	ASSERT_EXPRESSION_TYPE(f, int()); // unparenthesized id-expression (not overloaded)
	ASSERT_EXPRESSION_TYPE(a.m, int); // unparenthesized class-member-access
	ASSERT_EXPRESSION_TYPE(VALUE, E); // unparenthesized id-expression
	ASSERT_EXPRESSION_TYPE(p->m, int); // unparenthesized class-member-access
	ASSERT_EXPRESSION_TYPE(a.cm, const int);
	ASSERT_EXPRESSION_TYPE(ca.m, int);
	ASSERT_EXPRESSION_TYPE(ca.cm, const int);
	ASSERT_EXPRESSION_TYPE(ca.mm, int);


	ASSERT_EXPRESSION_TYPE((i), int&); // yields a reference because 'i' is an lvalue
	ASSERT_EXPRESSION_TYPE((ci), const int&); // yields a reference because 'ci' is an lvalue
	ASSERT_EXPRESSION_TYPE((f), int(&)()); // yields a reference because 'f' is an lvalue
	ASSERT_EXPRESSION_TYPE((VALUE), E); // does not yield a reference because 'VALUE' is not an lvalue
	ASSERT_EXPRESSION_TYPE((a.m), int&); // yields a reference because 'a.m' is an lvalue
	ASSERT_EXPRESSION_TYPE((p->m), int&);
	ASSERT_EXPRESSION_TYPE((A().m), int); // not an lvalue because 'A()' is not an lvalue
	ASSERT_EXPRESSION_TYPE((a.cm), const int&);
	ASSERT_EXPRESSION_TYPE((ca.m), const int&);
	ASSERT_EXPRESSION_TYPE((ca.cm), const int&);
	ASSERT_EXPRESSION_TYPE((ca.mm), int&);

	struct C
	{
		void f()
		{
			ASSERT_EXPRESSION_TYPE(&(C::m), int*);
			ASSERT_EXPRESSION_TYPE(&C::m, int(C::*));

			ASSERT_EXPRESSION_TYPE(this, C*); // not an lvalue
			ASSERT_EXPRESSION_TYPE(m, int);
			ASSERT_EXPRESSION_TYPE((m), int&);
			ASSERT_EXPRESSION_TYPE(C::m, int);
			ASSERT_EXPRESSION_TYPE((C::m), int&);
		}
		void f() const
		{
			ASSERT_EXPRESSION_TYPE(this, const C*); // not an lvalue
			ASSERT_EXPRESSION_TYPE(m, int);
			ASSERT_EXPRESSION_TYPE((m), const int&);
			ASSERT_EXPRESSION_TYPE(C::m, int);
			ASSERT_EXPRESSION_TYPE((C::m), const int&);
		}
		static void s();
		int m;     
		ASSERT_EXPRESSION_TYPE(m, int);
		ASSERT_EXPRESSION_TYPE((m), int&);

	};
	C c;

	// postfix expressions
	ASSERT_EXPRESSION_TYPE(p[0], A&); // lvalue
	ASSERT_EXPRESSION_TYPE(f(), int); // not an lvalue
	ASSERT_EXPRESSION_TYPE(cf(), int); // not an lvalue
	ASSERT_EXPRESSION_TYPE(r(), int&); // lvalue
	ASSERT_EXPRESSION_TYPE(c.f(), void); // not an lvalue
	ASSERT_EXPRESSION_TYPE(int(), int);
	ASSERT_EXPRESSION_TYPE(A(), A);
	ASSERT_EXPRESSION_TYPE(E(), E);
	ASSERT_EXPRESSION_TYPE(E(), E);
	//ASSERT_EXPRESSION_TYPE(c.s, void()); // illegal: type is 'unresolved function overload set'
	//ASSERT_EXPRESSION_TYPE((c.s), void()); // illegal: type is 'unresolved function overload set'
	ASSERT_EXPRESSION_TYPE(p++, A*);
	ASSERT_EXPRESSION_TYPE(p--, A*);
	ASSERT_EXPRESSION_TYPE(i++, int);
	ASSERT_EXPRESSION_TYPE(i--, int);
	ASSERT_EXPRESSION_TYPE(int(0), int); // not an lvalue
	ASSERT_EXPRESSION_TYPE((int)0, int); // not an lvalue
	ASSERT_EXPRESSION_TYPE((const int)0, int); // not an lvalue

	struct B : A
	{
		virtual ~B();
	};
	B* b;
	ASSERT_EXPRESSION_TYPE(dynamic_cast<A*>(b), A*);
	ASSERT_EXPRESSION_TYPE(dynamic_cast<A&>(*b), A&);
	ASSERT_EXPRESSION_TYPE(static_cast<float>(0), float);
	ASSERT_EXPRESSION_TYPE(static_cast<A*>(b), A*);
	ASSERT_EXPRESSION_TYPE(static_cast<A&>(*b), A&);
	ASSERT_EXPRESSION_TYPE(reinterpret_cast<A*>(b), A*);
	ASSERT_EXPRESSION_TYPE(reinterpret_cast<A&>(*b), A&);
	ASSERT_EXPRESSION_TYPE(const_cast<const A*>(p), const A*);
	ASSERT_EXPRESSION_TYPE(const_cast<const A&>(*p), const A&);

	// unary expressions
	ASSERT_EXPRESSION_TYPE(++p, A*&);
	ASSERT_EXPRESSION_TYPE(--p, A*&);
	ASSERT_EXPRESSION_TYPE(++i, int&);
	ASSERT_EXPRESSION_TYPE(--i, int&);
	ASSERT_EXPRESSION_TYPE(*p, A&);
	ASSERT_EXPRESSION_TYPE(&a, A*);
	ASSERT_EXPRESSION_TYPE(+i, int);
	ASSERT_EXPRESSION_TYPE(-i, int);
	ASSERT_EXPRESSION_TYPE(!i, bool);
	ASSERT_EXPRESSION_TYPE(~i, int);
	ASSERT_EXPRESSION_TYPE(sizeof(0), size_t);
	ASSERT_EXPRESSION_TYPE(sizeof(int), size_t);
	ASSERT_EXPRESSION_TYPE(new int, int*);
	ASSERT_EXPRESSION_TYPE(new int[1], int*);
	ASSERT_EXPRESSION_TYPE(delete p, void);

	// cast
	ASSERT_EXPRESSION_TYPE((A*)b, A*);
	ASSERT_EXPRESSION_TYPE((A&)*b, A&);

	// pm
	ASSERT_EXPRESSION_TYPE(a.*pm, int&);
	ASSERT_EXPRESSION_TYPE(p->*pm, int&);
	ASSERT_EXPRESSION_TYPE((a.*pmf)(), int);
	ASSERT_EXPRESSION_TYPE((p->*pmf)(), int);

	ASSERT_EXPRESSION_TYPE(i * 3, int);
	ASSERT_EXPRESSION_TYPE(i + 3, int);
	ASSERT_EXPRESSION_TYPE(p + 3, A*);
	//ASSERT_EXPRESSION_TYPE(p - p, ptrdiff_t); // TODO: ptrdiff_t

	ASSERT_EXPRESSION_TYPE(i << 3, int);
	ASSERT_EXPRESSION_TYPE(i < 3, bool);
	ASSERT_EXPRESSION_TYPE(i == 3, bool);
	ASSERT_EXPRESSION_TYPE(i & 3, int);

	ASSERT_EXPRESSION_TYPE(i * VALUE, int);
	ASSERT_EXPRESSION_TYPE(i + VALUE, int);
	ASSERT_EXPRESSION_TYPE(p + VALUE, A*);

	ASSERT_EXPRESSION_TYPE(i << VALUE, int);
	ASSERT_EXPRESSION_TYPE(i < VALUE, bool);
	ASSERT_EXPRESSION_TYPE(i == VALUE, bool);
	ASSERT_EXPRESSION_TYPE(i & VALUE, int);

	ASSERT_EXPRESSION_TYPE(i && 3, bool);

	ASSERT_EXPRESSION_TYPE(false ? 0 : 0, int);

	ASSERT_EXPRESSION_TYPE(i = 0, int&);

	ASSERT_EXPRESSION_TYPE((i, 0), int); // comma operator
	ASSERT_EXPRESSION_TYPE((0, i), int&); // comma operator

}

namespace N342
{
	template<int>
	struct B
	{
	};

	struct A
	{
		static const int VALUE = 0;
		typedef B<VALUE> Type; // 'VALUE' is an integral constant expression
		void f()
		{
			// typedef B<(*this).VALUE> Type; // '(*this).VALUE' is an integral constant expression  (C++11)
			typedef B<VALUE> Type; // 'VALUE' is an integral constant expression
  			typedef B<A::VALUE> Type; // 'A::VALUE' is an integral constant expression        
		}
	};
	typedef B<A::VALUE> Type; // 'A::VALUE' is an integral constant expression
	A a;
	//typedef B<a.VALUE> Type; // 'a.VALUE' is an integral constant expression (C++11)
	//typedef B<A().VALUE> Type; // 'A().VALUE' is an integral constant expression in (C++11)

	ASSERT_EXPRESSION_TYPE(A::VALUE, const int);
	ASSERT_EXPRESSION_TYPE(a.VALUE, const int);

	template<typename T>
	struct C
	{
		static const T VALUE = 0;
		typedef B<VALUE> Type; // 'VALUE' is an integral constant expression
		void f()
		{
			// typedef B<(*this).VALUE> Type; // '(*this).VALUE' is an integral constant expression (C++11)
			typedef B<VALUE> Type; // 'VALUE' is an integral constant expression
			typedef B<C::VALUE> Type; // 'C::VALUE' is an integral constant expression
			// typedef B<f().VALUE> Type4; // ill-formed? no diagnostic from clang/gcc
		}
	};
	C<int> c;
	//typedef B<c.VALUE> Type; // 'c.VALUE' is an integral constant expression (C++11)

	typedef C<int>::Type Type; // force instantiation
}

namespace N394
{
	template<typename T>
	struct A
	{
		T m;
	};
	A<int> a;
	ASSERT_EXPRESSION_TYPE(A<int>::m, int);
	ASSERT_EXPRESSION_TYPE(a.m, int);
}



#if 0 // TODO: determine type of template-id which identifies a single function template specialization
namespace N395 // [temp.arg.explicit]
{
	template<typename T>
	T f();

	ASSERT_EXPRESSION_TYPE(f<int>, int());
	ASSERT_EXPRESSION_TYPE(&f<int>, int(*)());
}

namespace N393 // [temp.arg.explicit]
{
	struct A
	{
		template<typename T>
		T f();
	};

	ASSERT_EXPRESSION_TYPE(&A::f<int>, int(A::*)());
}

namespace N396 // test SFINAE for function template id
{
	template<typename T, typename U = int() >
	struct B
	{
		typedef int Type;
	};

	template<typename T>
	int f1();

	template<typename T, typename U>
	int f2();

	template<int i>
	int f3();

	// SFINAE: too few template parameters
	template<typename T>
	struct B<T, decltype(f2<T>)>
	{
	};

	// SFINAE: too many template parameters
	template<typename T>
	struct B<T, decltype(f1<T, T>)>
	{
	};

	// SFINAE: expected non-type parameter
	template<typename T>
	struct B<T, decltype(f3<T>)>
	{
	};


	STATIC_ASSERT_IS_SAME(B<int>::Type, int);
}


namespace N397 // [temp.arg.explicit] test evaluation of type of dependent function template id
{   
	template<typename T>
	T f();

	template<typename T>
	struct A
	{
		typedef int Type;
		ASSERT_EXPRESSION_TYPE(f<T>, int());
		ASSERT_EXPRESSION_TYPE(&f<T>, int(*)());
	};

	typedef A<int>::Type Type; // force instantiation
}

#endif

namespace N384 // defer evaluation of type of dependent expression to point of instantiation
{
	int f(int);
	int& fr(int);

	template<typename T>
	struct C
	{
		T m;
		T mf();
		T& mfr();
		T f();
		static T smf();
	};

	template<typename T, T i>
	struct A
	{
		typedef A<int, 0> Self;
		T m;
		const T cm = 0;
		static T sm;
		static T* p;
		static C<T> c;
		static C<T>* pc;
		static C<T>& rc;


		ASSERT_EXPRESSION_TYPE(i, int); // unparenthesized id-expression
		ASSERT_EXPRESSION_TYPE(m, int);
		ASSERT_EXPRESSION_TYPE(cm, const int);
		ASSERT_EXPRESSION_TYPE(sm, int);
		ASSERT_EXPRESSION_TYPE(c.m, int); // TODO: fails
		ASSERT_EXPRESSION_TYPE(pc->m, int);
		ASSERT_EXPRESSION_TYPE(rc.m, int);
		//ASSERT_EXPRESSION_TYPE(c.smf, int()); // illegal: type is 'unresolved function overload set'

		ASSERT_EXPRESSION_TYPE((i), int); // not lvalue
		ASSERT_EXPRESSION_TYPE((m), int&);
		ASSERT_EXPRESSION_TYPE((cm), const int&);
		ASSERT_EXPRESSION_TYPE((sm), int&);
		ASSERT_EXPRESSION_TYPE((c.m), int&);
		ASSERT_EXPRESSION_TYPE((pc->m), int&);
		ASSERT_EXPRESSION_TYPE((rc.m), int&);
		//ASSERT_EXPRESSION_TYPE((c.smf), int()); // illegal: type is 'unresolved function overload set'

		int mf()
		{
			ASSERT_EXPRESSION_TYPE(this, Self*);
		}
		int mfc() const
		{
			ASSERT_EXPRESSION_TYPE(this, const Self*);
		}

		ASSERT_EXPRESSION_TYPE(p[0], int&);
		ASSERT_EXPRESSION_TYPE(f(i), int);
		ASSERT_EXPRESSION_TYPE(fr(i), int&);
		ASSERT_EXPRESSION_TYPE(c.f(), int);
		ASSERT_EXPRESSION_TYPE(T(), int);
		ASSERT_EXPRESSION_TYPE(C<T>(), C<int>);
		ASSERT_EXPRESSION_TYPE(p++, int*);
		ASSERT_EXPRESSION_TYPE(p--, int*);
		ASSERT_EXPRESSION_TYPE(sm++, int);
		ASSERT_EXPRESSION_TYPE(sm--, int);

		typedef int Type;
	};

	// instantiate
	typedef A<int, 0>::Type Type;
	A<int, 0> a;
	int i = a.mf();
	int j = a.mfc();
}

#if 0 // TODO: defer evaluation of type of dependent pointer to member
namespace N392
{
	struct C
	{
		int m;
		static int ms;
	};

	template<typename T>
	struct A
	{
		ASSERT_EXPRESSION_TYPE(&T::m, int(C::*));
		ASSERT_EXPRESSION_TYPE(&T::ms, int*);
		typedef int Type;
	};

	typedef A<C>::Type Type;
}

#endif

namespace N344 // non-dependent non-type-template-parameter
{
	template<bool b, bool func()>
	void f()
	{
		static_assert(sizeof(b), "?evaluated"); // type of 'b' is not dependent and can be resolved during initial parse
		static_assert(sizeof(func()), "?evaluated"); // type of 'func' is not dependent and can be resolved during initial parse
	}

	template<bool b, bool func()>
	struct A
	{
#ifdef _CPPP_TEST // msvc 2010 fails this test
		static_assert(sizeof(b), "?evaluated"); // type of 'b' is not dependent and can be resolved during initial parse
		static_assert(sizeof(func()), "?evaluated"); // type of 'func' is not dependent and can be resolved during initial parse
#endif
	};
}

namespace N352 // test nondependent non-type template parameter in non-type template parameter default argument
{
	template<bool b, typename T = decltype(b)> // type of 'b' is not dependent and evaluation of 'decltype(b)' is not deferred
	struct A
	{
		static const T value = b;
	};

	ASSERT_EXPRESSION_TYPE(A<false>::value, const bool);
}

namespace N351 // test nondependent non-type template parameter in constant-expression
{
	template<bool b>
	struct A
	{
		static const int value = sizeof(b);
	};

	ASSERT_EXPRESSION_TYPE(A<false>::value, const int); // check that the expression type is successfully evaluated
}

namespace N317 // test evaluation of type of conditional expression with null-pointer and pointer
{
	int* p;
	ASSERT_EXPRESSION_TYPE(true ? 0 : p, int*);
}

namespace N368 // test evaluation of type of conditional expression with null-pointer and function-pointer
{
	void f();
	ASSERT_EXPRESSION_TYPE(false ? 0 : f, void(*)()); // 'f' is not overloaded, its type can be resolved to 'void()'
}


namespace N323 // test evaluation of type of additive expression with pointer operand
{
	enum E
	{
		VALUE
	};

	const char p[1] = "";
	ASSERT_EXPRESSION_TYPE(p + VALUE, const char*);
}

namespace N319 // test evaluation of type of additive expression with enum operand
{
	enum E
	{
	};

	ASSERT_EXPRESSION_TYPE(E() + 1, int);
}


namespace N310 // test evaluation of type of expression involving overloaded operator->
{
	struct B
	{
		int m;
	};

	struct A
	{
		B* operator->();
	};

	A a;
	ASSERT_EXPRESSION_TYPE((a->m), int&);
};

namespace N322 // test overload resolution
{
	int f(const char*);
	void f(bool);

	ASSERT_EXPRESSION_TYPE(f(""), int); // calls 'f(const char*)'
}

namespace N391 // test overload resolution based on cv-qualification of implicit object argument
{
	struct A
	{
		float f();
		int f() const;
	};

	A a;
	ASSERT_EXPRESSION_TYPE(a.f(), float);
	const A ca = A();
	ASSERT_EXPRESSION_TYPE(ca.f(), int);
}

namespace N333 // test overload resolution with direct-reference-binding and user-defined-conversion
{
	struct A
	{
		operator const int&()const;
		operator int&();
	};

	int f(const int& x, const A& y);
	bool f(int, int);


	int i;
	A a;
	ASSERT_EXPRESSION_TYPE(f(i, a), int); // calls 'f(const int&, const A&)' - because it is a better candidate than 'f(int, int)'
}

namespace N313 // test overload resolution with user-defined-conversion to const reference
{
	struct S
	{
		operator const int&();
	};

	int f(const int&);

	ASSERT_EXPRESSION_TYPE(f(S()), int); // overload resolution chooses conversion 'S::operator const int&()'
}

namespace N308 // test overload resolution with viable and non-viable user-defined-conversion
{
	struct S
	{
		operator int*();
		operator int();
	};

	int f(int);

	ASSERT_EXPRESSION_TYPE(f(S()), int); // overload resolution chooses conversion 'S::operator int()'
}

namespace N312 // test overload resolution with user-defined-conversion to pointer
{
	struct B
	{
		operator int*();
	};


	int f(int*);

	ASSERT_EXPRESSION_TYPE(f(B()), int); // overload resolution chooses conversion 'B::operator int*()'
}

namespace N282 // test overload resolution with multiple converting constructors
{
	struct S
	{
		S(int);
		S(float);
		~S();
	};

	int f(S);

	ASSERT_EXPRESSION_TYPE(f(0), int); // calls 'S(int)'
}

namespace N304 // test overload resolution with multiple converting constructor templates
{
	template<class T>
	class A
	{
	public:
		typedef A<T> _Myt;
		typedef const T* const_pointer;
		A();
		A(const _Myt&);
		A(const T*);
		template<class _It>
		A(_It, _It);
		template<class _It>
		A(const_pointer, const_pointer);
	};

	int f(A<char>);

	const char* s = "";
	ASSERT_EXPRESSION_TYPE(f(s), int); // calls 'A<char>(const char*)'
}

namespace N305 // test overload resolution with converting constructor template
{
	template<class T>
	struct A
	{
		A();
		A(const T*);
	};

	int f(A<char>);

	const char* s = "";
	ASSERT_EXPRESSION_TYPE(f(s), int); // calls 'A<char>(const char*)'
}

namespace N311 // test overload resolution with user-defined-conversion template
{
	template<typename T>
	struct B
	{
		operator T*() const;
	};

	struct D : B<int>
	{
	};

	int f(int*);

	const D& d = D();
	ASSERT_EXPRESSION_TYPE(f(d), int); // overload resolution chooses conversion 'B<int>::operator int*()'
}

namespace N314 // test overload resolution with overloaded function templates
{
	template<typename X>
	int* f(X&, int*);
	template<typename X, typename T>
	void* f(X&, T*);

	int x;
	int* p;
	ASSERT_EXPRESSION_TYPE(f(x, p), int*); // overload resolution chooses 'f<int>(int&, int*)'
}

namespace N315 // test overload resolution with overloaded member function templates
{
	struct A
	{
		template<typename X>
		static int* f(X&, int*);
		template<typename X, typename T>
		void* f(X&, T*);
	};

	int x;
	A a;
	int* p;
	ASSERT_EXPRESSION_TYPE(a.f(x, p), int*); // overload resolution chooses 'A::f<int>(int&, int*)'
}

namespace N316 // test overload resolution with overloaded function templates and class hierarchy
{
	template<typename>
	struct C
	{
	};

	struct I
	{
	};

	struct B : I
	{
	};

	struct D : B
	{
	};

	struct A
	{
		template<typename X, typename T>
		static T* f(C<X>&, T*);
		template<typename X>
		B* f(C<X>&, B*);
		template<typename X>
		I* f(C<X>&, I*);
	};

	C<int> c;
	A a;
	D* p;
	ASSERT_EXPRESSION_TYPE(a.f(c, p), D*); // overload resolution chooses 'A::f<int, D>(C<int>&, D*)'
}

namespace N367 // test overload resolution of overloaded operator+ with pointer-to-overloaded-function argument
{
	struct A
	{
	};

	int operator+(A, void(*)(int));
	float operator+(A, void(*)(float));

	void f();
	void f(int);

	void g()
	{
		A a;
		ASSERT_EXPRESSION_TYPE(a + f, int); // 'f' is overloaded, the correct overload 'f(int)' should be chosen
#ifdef _CPPP_TEST // msvc 2010 crashes with this test
		ASSERT_EXPRESSION_TYPE(a + &f, int); // 'f' is overloaded, the correct overload 'f(int)' should be chosen
#endif
	}
}

namespace N348 // test call to overloaded operator++
{
	struct A
	{
		A& operator++(int);
	};

	A a;
	ASSERT_EXPRESSION_TYPE(a++, A&); // calls 'A::operator++(int)'
}

namespace N271 // test call to built-in operator++(char*) followed by built-in operator*(char*)
{
	void f()
	{
		char* _Ptr = 0;
		ASSERT_EXPRESSION_TYPE(*_Ptr++, char&);
	}
}

namespace N366 // test function call with pointer-to-overloaded-function argument
{
	void f();
	void f(int);

	int g(void(*)(int));

	ASSERT_EXPRESSION_TYPE(g(f), int); // 'f' is overloaded, the correct overload 'f(int)' should be chosen
}

namespace N281 // test template argument deduction and return-type substitution in call to function-template
{
	template<class U>
	inline U f(const U&_Val);

	ASSERT_EXPRESSION_TYPE(f(char(0xba)), char);
}

namespace N284 // test template argument deduction and return-type substitution in call to function-template
{
	template<class T>
	int f(const T&);

	const int& i = 0;
	ASSERT_EXPRESSION_TYPE(f(i), int);
}

namespace N298 // test template argument deduction and return-type substitution in call to function-template 
{
	template<typename T>
	T f(const T*);

	int a;
	ASSERT_EXPRESSION_TYPE(f(&a), int); // calls f(const int*)
}

namespace N296 // test template argument deduction for pointer to class-template parameter
{
	template<typename T>
	struct S
	{
	};

	template<typename T>
	T f(const S<T>*);

	struct A : S<int>
	{
	};

	A a;
	ASSERT_EXPRESSION_TYPE(f(&a), int); // calls f(const S<int>*)
}

namespace N293 // test template argument deduction for reference to class-template parameter
{
	template<typename T>
	struct S
	{
	};

	template<typename T>
	T f(const S<T>&);

	S<int> s;
	ASSERT_EXPRESSION_TYPE(f(s), int); // calls f(const S<int>&)
}

namespace N294 // test template argument deduction for reference to class-template parameter
{
	template<typename T>
	struct S
	{
	};

	template<typename T>
	T f(const S<T>&);

	struct A : S<int>
	{
	};

	A a;
	ASSERT_EXPRESSION_TYPE(f(a), int); // calls f(const S<int>&)
}

namespace N295 // test template argument deduction for overloaded operator*
{
	struct S
	{
	};

	template<typename T>
	int operator*(const T&);

	S s;
	ASSERT_EXPRESSION_TYPE(*s, int); // calls operator*(const S&);
}


namespace N300 // test template argument deduction for parameter with additional cv-qualification
{
	template<typename T>
	T f(const T*const*);

	int** a;
	ASSERT_EXPRESSION_TYPE(f(a), int); // calls f(const int*const*)
}

namespace N326 // test non-type template argument deduction and return-type substitution in call to function template
{
	template<int i>
	struct A
	{
	};

	template<int i>
	bool f(A<i>& a);

	A<0> a;
	ASSERT_EXPRESSION_TYPE(f(a), bool); // calls f(A<0>&)
}

namespace N350 // test template argument deduction and return-type substitution in call to function-template, from parameter type with a defaulted template argument
{
	template<typename T, typename U = T>
	struct A
	{
	};

	template<typename T>
	bool g(A<T>);

	A<int> a;
	ASSERT_EXPRESSION_TYPE(g(a), bool); // calls 'g(A<int, int>)'
}

namespace N331 // test argument dependent lookup of friend function definition
{
	template<class T>
	struct A
	{
		friend T f(const T&);
	};

	struct B : A<B>
	{
	};

	B b;
	ASSERT_EXPRESSION_TYPE(f(b), B); // name lookup should find 'A<B>::f'
}

namespace N334 // test template argument deduction during argument dependent lookup of friend function template definition
{
	template<class T>
	struct A
	{
		template<typename U>
		friend U f(const U&);
	};

	struct B : A<B>
	{
	};

	B b;
	ASSERT_EXPRESSION_TYPE(f(b), B); // name lookup should find 'A<B>::f<B>'
}

namespace N332 // test overload resolution of friend function overloaded operator< against built-in operator
{
	template<class T, class U>
	struct B
	{
	private:
		friend int operator<(const U&, const T&);
	};
	struct A : B<A, int>
	{
		operator const int&()const;
		operator int&();
	};

	int i;
	A a;
	ASSERT_EXPRESSION_TYPE(i < a, int); // calls 'B<A>::operator<(const int&, const A&)' via argument dependent lookup - because it is a better candidate than built-in 'operator<(int, int)'
}

namespace N330 // test viability of built-in operator< called with an object convertible to built in type
{
	struct A
	{
		operator const int&()const;
		operator int&(); // implicit-object parameter is better conversion from 'A&' because no 'added qualification'
	};

	int i;
	A a;
	ASSERT_EXPRESSION_TYPE(i < a, bool); // calls built-in operator<(int, int), via conversion function 'A::operator int&()'
}

namespace N325 // test viability of built-in operator&& called with an object convertible to built in type
{
	struct A
	{
		bool operator()();
	};

	A a;

	ASSERT_EXPRESSION_TYPE(true && a(), bool);
}

namespace N324 // test viability of built-in operator!= called with an object convertible to built in type
{
	struct A
	{
		operator int*();
	};

	A a;

	ASSERT_EXPRESSION_TYPE(a != 0, bool);
}


//-----------------------------------------------------------------------------
// explicit specialization

namespace N157 // test choice of correct explicit specialization by function type
{
	template<class R>
	struct S
	{
		typedef int Primary;
	};
	template<>
	struct S<void()>
	{
		typedef int Spec1;
	};
	template<>
	struct S<void(...)>
	{
		typedef int Spec2;
	};

	S<int>::Primary p;
	S<void()>::Spec1 s1;
	S<void(...)>::Spec2 s2;
}


//-----------------------------------------------------------------------------
// name lookup for friend declaration

namespace N376 // test name lookup for namespace-scope redeclaration of function first declared as friend
{
	struct A
	{
		friend int f();
	};

	int f(); // redeclares 'f'

	int c = f(); // unqualified name lookup finds 'f'
}

namespace N377 // test name lookup for namespace-scope declaration of function redeclared as friend
{
	int f();

	struct A
	{
		friend int f(); // redeclares 'f'
	};

	int c = f(); // unqualified name lookup finds 'f'
}

namespace N379 // test name lookup for namespace-scope declaration of class redeclared as friend
{
	struct B;

	struct A
	{
		friend struct B; // redeclares 'B'
	};

	B* b; // unqualified name lookup finds 'B'
}

namespace N378 // test name lookup for namespace-scope redeclaration of class first declared as friend
{
	struct A
	{
		friend struct B;
	};

	struct B; // redeclares 'B'

	B* b; // unqualified name lookup finds 'B'
}


namespace N329 // test name lookup for overloaded operator declared as friend
{
	template<class T>
	struct A
	{
		friend int operator==(const T&x, const T&y);
	};

	enum E
	{
	};

	E e;
	ASSERT_EXPRESSION_TYPE(e == e, bool); // name lookup should not find 'A<T>::operator=='
}

//-----------------------------------------------------------------------------
// argument dependent name lookup

namespace N380 // test argument dependent name lookup for function declared only as friend
{
	struct A
	{
		friend int f(A); // redeclares 'f'
	};

	int c = f(A()); // argument dependent name lookup finds 'f'
}

namespace N321 // test argument dependent name lookup for function declared in nested namespace
{
	namespace N
	{
		struct A
		{
		};
		int f(A);
	}

	N::A a;
	int x = f(a); // lookup of unqualified id 'f' should be deferred until arguments are known, then looked up via ADL
}

//-----------------------------------------------------------------------------
// explicit template argument specification

namespace N257 // test parse of explicit template argument specification in call to 'operator()'
{
	struct S
	{
		template<bool>
		void operator()()
		{
		}
	};
	void f()
	{
		enum { CONSTANT = 0 };
		S s;
		s.operator()<true>();
		s.operator()<CONSTANT < 0>(); // older versions of Comeau fail to compile this
	}
}

namespace N347 // test interaction of explicit template argument specification and template argument deduction
{
	template<typename First, typename Second>
	struct C
	{
	};

	template<typename First, typename Second>
	C<First, Second> f(const Second& src);

	struct A
	{
	};

	struct B
	{
	};

	A a;
	B b;

	typedef C<B, A> Expected;

	// first template argument is explicitly specified as 'B'
	ASSERT_EXPRESSION_TYPE(f<B>(a), Expected); // second template argument is deduced as 'A'
}

//-----------------------------------------------------------------------------
// class template partial specialization

namespace N381 // test selection of correct specialization when both template arguments are the same type
{
	template<typename Target, typename Src>
	struct C
	{
		typedef Target type;
	};
	template<typename Src>
	struct C<Src, Src>
	{
		typedef const Src& type;
	};

	typedef C<int, float> Different;
	typedef C<int, int> Same;

	STATIC_ASSERT_IS_SAME(Different::type, int);
	STATIC_ASSERT_IS_SAME(Same::type, const int&);
}


//-----------------------------------------------------------------------------
// type of class-member-access

namespace N345 // test call of pointer to function named by class-member-access
{
	struct A
	{
		A* f()
		{
			ASSERT_EXPRESSION_TYPE(f()->m(), void);
		}
		void(*m)();
	};
}

//-----------------------------------------------------------------------------
// type of expression involving pointer-to-member

namespace N337 // check type of function-call expression with class-member-access expression with pointer-to-member as object expression
{
	struct A
	{
	};

	struct B
	{
		int (A::* m)();
	};

	A a;
	B b;
	ASSERT_EXPRESSION_TYPE((a.*b.m)(), int); // not lvalue
}

namespace N336 // check type of class-member-access expression with pointer-to-member as object expression
{
	struct A
	{
	};

	struct B
	{
		int A::* m;
	};

	A a;
	B b;
	ASSERT_EXPRESSION_TYPE(a.*b.m, int&); // lvalue
	ASSERT_EXPRESSION_TYPE(A().*b.m, int); // not lvalue
}

namespace N335 // check type of pointer-to-member id-expression
{
	struct A
	{
		int m;
	};

	int A::* m = &A::m;

	A a;
	ASSERT_EXPRESSION_TYPE(a.*m, int&); // lvalue
	ASSERT_EXPRESSION_TYPE((&a)->*m, int&); // lvalue
	ASSERT_EXPRESSION_TYPE(A().*m, int); // not lvalue
}

namespace N328 // test SFINAE to remove overload candidate with enable_if in function call
{
	template<bool B, class T=void>
	struct enable_if_c
	{
		typedef T type;
	};
	template<class T>
	struct enable_if_c<false, T>
	{
	};
	template<class Cond, class T=void>
	struct enable_if: public enable_if_c<Cond::value, T>
	{
	};

	template< class T >
	struct is_int { static const bool value = false; };

	template<>
	struct is_int<int>
	{
		static const bool value=true;

	};

	template<class T>
	int f(T, typename enable_if<is_int<T> >::type* = 0); // SFINAE

	bool f(float);

	ASSERT_EXPRESSION_TYPE(f(false), bool); // calls 'f(float)' because argument is not int
	ASSERT_EXPRESSION_TYPE(f(0), int); // calls 'f<int>(int)'
}


namespace N297 // test template argument deduction with template base class parameter
{
	template<typename T>
	struct C
	{
	};
	template<typename T>
	struct B : public C<T>
	{
	};
	struct A : public B<A>
	{
	};

	template<typename T>
	T operator*(const C<T>&);

	A a;
	ASSERT_EXPRESSION_TYPE(*a, A); // calls operator*(const C<A>)
}


namespace N279 // test template argument deduction with argument types that both decay to pointer
{
	template<class T>
	T f(T, T);

	char buffer[1];
	ASSERT_EXPRESSION_TYPE(f(buffer, buffer+1), char*); // calls f<char*>(char*, char*)
}


namespace N276 // test dereference of pointer to function before calling it
{
	typedef int(*F)();

	F p;
	ASSERT_EXPRESSION_TYPE((*p)(), int);
}

namespace N280 // test overload resolution with enum promotion to int
{
	enum E
	{
	};

	int f(int);
	bool f(unsigned int);

	ASSERT_EXPRESSION_TYPE(f(E(0)), int); // f(int) is a better match
}

namespace N275 // test template argument deduction with parameter T* and function-pointer argument
{
	template<class T>
	inline T* f(T*);

	typedef void(*F)();

	F p;
	ASSERT_EXPRESSION_TYPE(f(p), F); // calls f<void(*)()>(void(*)())
}

namespace N273 // test call of overloaded operator[]
{
	struct A
	{
		int operator[](int);
	};

	A a;
	ASSERT_EXPRESSION_TYPE(a[0], int);
}

namespace N272 // test call of function with ellipsis parameter list
{
	int f(...);

	ASSERT_EXPRESSION_TYPE(f(1), int);
}


namespace N264 // test partial ordering of function templates
{
	template<typename T>
	int f(const T*);

	template<typename T>
	bool f(T*);

	const char* p;
	ASSERT_EXPRESSION_TYPE(f(p), int); // calls f<char>(const char*)
}


namespace N234 // test SFINAE with explicitly specified template argument for function template overloads with both type and non-type parameter.
{
	template<int i>
	int f();

	template<typename T>
	T f();

	ASSERT_EXPRESSION_TYPE(f<bool>(), bool);
	ASSERT_EXPRESSION_TYPE(f<0>(), int);
}

namespace N123 // test type substitution for return type that depends on a template parameter
{
	template<typename T>
	struct S
	{
		typedef T Type;

		Type f();
	};

	template<typename U>
	typename S<U>::Type S<U>::f();

	S<int> s;
	ASSERT_EXPRESSION_TYPE(s.f(), int);
}


namespace N235 // test template argument deduction with TT<T> parameter, with explicitly specified template argument for T
{
	template<typename T, template<class> class TT>
	TT<T> f(TT<T>);

	template<class T>
	struct S
	{
	};

	S<int> s;
	ASSERT_EXPRESSION_TYPE(f<int>(s), S<int>);
}

namespace N177 // test template argument deduction with TT<int> parameter
{
	template<template<class> class TT>
	TT<int> f(TT<int>);

	template<class T>
	struct S
	{
	};

	S<int> s;
	ASSERT_EXPRESSION_TYPE(f(s), S<int>);
}

namespace N233 // test implicit conversion of null-pointer-constant to pointer in function call expression
{
	int f(char*);
	ASSERT_EXPRESSION_TYPE(f(0), int); // null-pointer-constant matches T*
}

namespace N154 // test template argument deduction with T parameter
{
	template<typename T>
	T f();

	ASSERT_EXPRESSION_TYPE(f<int>(), int); // calls 'f<int>()'
};


namespace N124 // test type substitution in function call expression for function with dependent type
{
	struct S
	{
		typedef int Type;
	};

	template<typename T>
	typename T::Type f(T);

	S s;
	ASSERT_EXPRESSION_TYPE(f(s), int); // return type is 'int'
}

namespace N127 // test type substitution in function call expression for member function which depends on a template parameter of the enclosing class template
{
	template<typename T>
	struct S
	{
		static T f(T t)
		{
			return t;
		}
	};

	ASSERT_EXPRESSION_TYPE(S<int>::f(0), int); // type of 'S<int>::f' should be 'int(int)'
}


namespace N112 // test type substitution in function call expression for member function which indirectly depends on a template parameter of the enclosing class template
{
	template<typename T>
	struct S
	{
		typedef typename T::Type Type;
		int f(Type);
	};

	struct A
	{
		typedef int Type;
	};

	S<A> s;
	ASSERT_EXPRESSION_TYPE(s.f(0), int); // calls S::f(int)
}

namespace N104
{
	int const f(); // const is ignored
	ASSERT_EXPRESSION_TYPE(f, int const()); // const is also ignored here!
}

namespace N103
{
	int const f(); // const is ignored
	ASSERT_EXPRESSION_TYPE(f(), int);
}

namespace N102
{
	const int& f();
	ASSERT_EXPRESSION_TYPE(f(), const int&);
}


namespace N100 // test using declaration
{
	namespace A
	{
		int f(int);
	}

	namespace B
	{
		using A::f;
		float f(float);
		ASSERT_EXPRESSION_TYPE(f(0), int); // overload resolution should choose A::f(int)
	}
}


namespace N089
{
	void f(bool, bool = false);
	int f(int = 0);

	ASSERT_EXPRESSION_TYPE(f(0), int); // should call f(int)
}

namespace N094
{
	void f(float, ...);
	int f(float, float);
	void f(int, float);
	ASSERT_EXPRESSION_TYPE(f(0.f, 0.f), int); // overload resolution should choose f(float, float)
}

namespace N093
{
	void f(float, float);
	int f(float, int = 37);
	void f(int);
	ASSERT_EXPRESSION_TYPE(f(0.f), int); // overload resolution should choose f(float, int)
}

namespace N088
{
	int f(const wchar_t*);
	void f();
	ASSERT_EXPRESSION_TYPE(f(L""), int); /// overload resolution should pick 'f(const wchar_t*)'
}
