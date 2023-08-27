#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include "advassert.h"

//! @brief Maximum variable's name's length (including '\0').
const unsigned int MAX_VAR_NAME_LEN = 3;

/*!
    @brief Parses string of variables (like "x1, x2, x3")
    into array of strings (like { "x1", "x2", "x3" }).
    @param [in] vars String of variables separated by commas and spaces. (like "x1, x2, x3").
    @param [in] nVars Number of variables in vars.
    @param [out] res Array of variables' names converted to strings (like { "x1", "x2", "x3" }).
*/
static int parse_vars(const char* vars, const unsigned int nVars, char res[][MAX_VAR_NAME_LEN]);

int main() // временно для тестов
{
    //int x1 = 19, x2 = 23, x3 = 42;

    unsigned int n = 4;
    char (*res)[MAX_VAR_NAME_LEN] = (char (*)[MAX_VAR_NAME_LEN]) calloc(n, sizeof(char [MAX_VAR_NAME_LEN]));

    parse_vars("   x1234    ,x0, pri19nt,   x123", n, res);

    for (unsigned int i = 0; i < n; i++)
    {
        printf("%s\n", res[i]);
    }

    //ADV_ASSERT(x1 < 0, "Explanation", 3, x1, x2, x3);

    free(res);

    return 0;
}

/*!
    @brief Internal function implementing advanced version of standard assert().
    @details MUSTN'T be used on its own, only as a part of the macro ADV_ASSERT().
    Please, read docs concerning this macro to learn how to properly use this macro.
    @param [in] condition Assertion condition converted to string by macro.
    @param [in] explanation Written by user explanation of the assertion condition as a constant string.
    @param [in] file File where assertion failed, passed in by macro __FILE__.
    @param [in] line Line where assertion failed, passed in by macro __LINE__.
    @param [in] vars_as_string Variables to print passed by user, converted by macro to string.
    @param [in] nVars Number of variables to print passed by user.
*/
void advassert_(const char* condition,
                const char* explanation,
                const char* file,
                const int line,
                const char* vars_as_string,
                unsigned int nVars,
                ...)
{
    fprintf(stderr, "Advanced assertion failed!\nFailed condition: %s\n"
                    "Explanation: %s\nFile: %s\nLine: %d\n", condition, explanation, file, line);

    if (nVars == 0)
    {
        printf("No variables to print passed.\n");

        abort();
    }

    // nVars != 0
    va_list vars;
    va_start(vars, nVars);

    while (nVars--)
    {

    }

    va_end(vars);
}

static int parse_vars(const char* vars, const unsigned int nVars, char res[][MAX_VAR_NAME_LEN])
{
    const char* p_curr_vars = vars; // указатель на обрабатываемый в данный момент символ vars
    int is_in_name = 0;
    char *p_curr_res = res[0];
    int ind_curr_res = 0;
    unsigned int name_letter_cnt = 0; // счетчик кол-ва букв в одном имении

    //" x1,  x2  ,x3567,    x4, x5  " lim = 3
    //"x1'\0'
    //"x35
    //"
    //"
    //"
    while (*p_curr_vars != '\0')
    {
        if ( *p_curr_vars == ',' || isspace(*p_curr_vars) )// если символ не принадлежит имени, т.е. пробел или запятая
        {
            if ( is_in_name )
            {
                // если сейчас внутри имени, значит имя закончилось
                // значит надо поставить '/0'
                // и переставить указатель на начало нового элемент res

                is_in_name = 0;
                name_letter_cnt = 0;
                *p_curr_res = '\0';
                p_curr_res = res[++ind_curr_res];
            }
        }
        else
        {
            // символ принадлежит имени
            if (!is_in_name) is_in_name = 1;

            *p_curr_res = *p_curr_vars;

            if (++name_letter_cnt == MAX_VAR_NAME_LEN)
            {
                // слишком длинное читаемое в данный момент имя, значит прекращаем его читать,
                // в следующей итерации обработается конец этого слова и добавится '\0'
                // и пропускаем буквы вплоть до пробела/запятой/конца строки vars

                while (*p_curr_vars != ',' && !isspace(*p_curr_vars) && *p_curr_vars != '\0') p_curr_vars++;

                if (*p_curr_vars == '\0')
                {
                    // в таком случае следующей итерации не будет, а значит нужно здесь добавить '\0'
                    *p_curr_res = '\0';
                }

                continue;
            }

            //все ок, передвигаем указатель по слову
            p_curr_res++;
        }

        p_curr_vars++;
    }
}
