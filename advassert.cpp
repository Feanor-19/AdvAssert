#include <stdio.h>

#include <stdarg.h>
#include <assert.h>

//! @brief Amount of actual arguments needed to represent one AdvAssertArg.
//! For example, if AdvAssertArg is represented by AdvAssertType and variable itself,
//! this constant must equal two.
const int ARG_MULTIPLIER = 2;

enum class AdvAssertType
{
    INT         = 0,
    CHAR        = 1,
    DOUBLE      = 2,
    FLOAT       = 3,
    STRING      = 4,
    LONG_INT    = 5,
    POINTER     = 7,
};

void advassert_(int condition, int nArgs,...);

#define ADV_ASSERT(COND, ...)  advassert_(COND, __VA_ARGS__); assert(COND)

int main() // временно для тестов
{
    int x = 23;
    double some_double = 19.42;
    ADV_ASSERT(x < 19 && "X is too big; x, some_double", 2, AdvAssertType::INT, x, AdvAssertType::DOUBLE, some_double);
    return 0;
}

/*!
    @brief Internal function called before standard assert().
    @details This function MUSTN'T be called on its own, only as a part of the #define.
    It DOESN'T call assert() itself. It is used to print values which variables have right
    before its call.
    @note HOW TO USE: instead of assert(cond); (where "cond" is the condition to check)
    use ADV_ASSERT(cond, *N*, type1, var1, type2, var2, ... typeN, varN);
    ---type1 is AdvAssertType corresponding to var1 type, written in capitals.
    ---var1 is any variable, supported by AdvAssertType.
    ---You can pass any number of arguments the same way as type1 and var1
    up till typeN, varN, where N is the number of arguments passed.
    ---It is recommended to write "cond" as follows: fact_cond && "explaining msg; arg1name, arg2name...",
    where fact_cond is the real condition to check.
    @note EXAMPLES: ADV_ASSERT(pointer != NULL && "Pointer is null; some_int", 1, AdvAssertType::INT, some_int);
    @note ADV_ASSERT(x < 19 && "X is too big; x, some_double", 2, AdvAssertType::INT, x, AdvAssertType::DOUBLE, some_double);

*/
void advassert_(int condition, int nArgs,...)
{
    if (!condition)
    {
        nArgs *= ARG_MULTIPLIER; // потому что передается не фактическое количество аргументов, а сколько нужно вывести

        va_list args;
        va_start(args, nArgs);

        printf("!!!ADV_ASSERT!!!\n");

        if (nArgs == 0) fprintf(stderr, "No arguments to print.\n");

        for (int n = nArgs; n > 0; n-=ARG_MULTIPLIER)
        {
            AdvAssertType type = (AdvAssertType) va_arg(args, int); // 'AdvAssertType' is promoted to 'int' when passed through '...'
            switch(type)
            {
                case AdvAssertType::INT:
                    fprintf(stderr, "%d\n", va_arg(args, int));
                    break;
                case AdvAssertType::CHAR:
                    fprintf(stderr, "%c\n", (char) va_arg(args, int)); // 'char' is promoted to 'int' when passed through '...'
                    break;
                case AdvAssertType::DOUBLE:
                case AdvAssertType::FLOAT:      // 'float' is promoted to 'double' when passed through '...'
                    fprintf(stderr, "%f\n", va_arg(args, double));
                    break;
                case AdvAssertType::STRING:
                    fprintf(stderr, "%s\n", va_arg(args, char *));
                    break;
                case AdvAssertType::LONG_INT:
                    fprintf(stderr, "%li\n", va_arg(args, long int));
                    break;
                case AdvAssertType::POINTER:
                    fprintf(stderr, "%p\n", va_arg(args, void*));
                    break;
                default:
                    printf("ADV_ASSERT: Unknown AdvAssertType. Can't read arguments...\n");
                    break;

            }
        }

        va_end(args);
    }
}