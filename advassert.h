#ifndef ADVASSERT_H
#define ADVASEERT_H

#ifdef NDEBUG

// if not debugging, ADV_ASSERT must be turned off
#define ADV_ASSERT(x) ((void)0)

#else /* debug on, ADV_ASSERT is turned on */

void advassert_(const char* condition,
                const char* explanation,
                const char* file,
                const int line,
                const char* vars_as_string,
                unsigned int nVars,
                ...);

#define ADV_ASSERT(cond, expl, nVars,...) ( (cond) ? ((void)0) : advassert_(#cond,          \
                                                                            (expl),         \
                                                                            __FILE__,       \
                                                                            __LINE__,       \
                                                                            #__VA_ARGS__,   \
                                                                            (nVars),        \
                                                                            __VA_ARGS__ ) )

#endif /* NEDBUG */

#endif /* ADVASSERT_H */
