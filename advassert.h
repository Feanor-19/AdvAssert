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

/*!
    @brief Advanced version of standard assert, capable of printing variables' values and names.
    @details If passed condition is true, nothing happens. Otherwise internal function advassert_()
    is called, which prints passed variables' names and values and calls standard abort(). If NEDBUG
    is defined, all asserts are turned off.
    @param cond Condition to check.
    @param expl Constant string message, explaining what this assert means.
    @param nVars Number of variables being passed.
    @param ... Variables, which names and values must be printed, written as follows: (type) var_name
    @note USAGE EXAMPLE 1: ADV_ASSERT(i1 > 0, "i1 must be positive", 1, (int) i1); i1 has type int.
    @note USAGE EXAMPLE 2: ADV_ASSERT(p != NULL, "null pointer p", 2, (int *) p, (int) some_int);
    @note USAGE EXAMPLE 3: ADV_ASSERT(0, "Unreachable line", 0, ""); If you don't want to pass
    any variables, pass 0 as number of variables and anything (for example empty string) instead of variables.
*/
#define ADV_ASSERT(cond, expl, nVars,...) ( (cond) ? ((void)0) : advassert_(#cond,          \
                                                                            (expl),         \
                                                                            __FILE__,       \
                                                                            __LINE__,       \
                                                                            #__VA_ARGS__,   \
                                                                            (nVars),        \
                                                                            __VA_ARGS__ ) )

#endif /* NEDBUG */

#endif /* ADVASSERT_H */
