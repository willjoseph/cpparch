
// offsetof implementation
#if 1
#define offsetof(s,m)   (unsigned int)&(((s *)0)->m)
#else
#define offsetof(S, m) __builtin_offsetof(S, m)
#endif

struct S
{
	int a, b;
};

char c[offsetof(S, b)]; // offsetof should yield a constant expression



