#include <stdio.h>

#include <stdarg.h>
#include "advassert.h"

//#define ADV_ASSERT(cond, msg, n,...) test(#cond, msg, #__VA_ARGS__, n, __VA_ARGS__)

//void test(const char* cond, const char* msg, const char* args_str, int n,...);

int main() // временно для тестов
{
    int x1 = 19, x2 = 23, x3 = 42;
    ADV_ASSERT(x1 < 0, "Explanation", 3, x1, x2, x3);
    return 0;
}

//void test(const char* cond, const char* msg, const char* args_str, int n,...)
//{
//    va_list args;
//    va_start(args, n);
//
//    printf("%s\n", cond);
//    printf("%s\n", msg);
//    printf("%s\n", args_str);
//    printf("%d\n", n);
//    printf("%d\n", va_arg(args, int));
//    printf("%d\n", va_arg(args, int));
//    printf("%d\n", va_arg(args, int));
//
//    va_end(args);
//}

void advassert_(const char* condition,
                const char* explanation,
                const char* file,
                int line,
                const char* args_as_string,
                unsigned int nArgs,
                ...)
{
    va_list args;
    va_start(args, nArgs);



    va_end(args);

    abort();
}
