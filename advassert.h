#ifndef ADVASSERT_H
#define ADVASEERT_H

#ifdef NDEBUG

// if not debugging, ADV_ASSERT must be turned off
#define ADV_ASSERT(x) ((void)0)

#else /* debug on, ADV_ASSERT is turned on */

void advassert_(const char* condition,
                const char* explanation,
                const char* file,
                int line,
                const char* args_as_string,
                unsigned int nArgs,
                ...);

//TEST!!!
//#define ADV_ASSERT(cond, msg, n,...) test(#cond, msg, #__VA_ARGS__, n, __VA_ARGS__)

#define ADV_ASSERT(cond, expl, nArgs,...) ( (cond) ? ((void)0) : advassert_(#cond,          \
                                                                            (expl),         \
                                                                            __FILE__,       \
                                                                            __LINE__,       \
                                                                            #__VA_ARGS__,   \
                                                                            (nArgs),        \
                                                                            __VA_ARGS__ ) )

#endif /* NEDBUG */

#endif /* ADVASSERT_H */
