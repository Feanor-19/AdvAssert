#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include "advassert.h"

//! @brief Maximum variable's name's length (including '\0').
const unsigned int MAX_VAR_NAME_LEN = 101;

/*!
    @details Parses string of variables (like "(int) i1, (double) d2, (char) c3")
    into array of strings (like { "i1", "d2", "c3" }) and array of vars' specifiers
    for printf() (like {AdvAssertType::INT, AdvAssertType::FLOAT, AdvAssertType::CHAR}).
    @param [in] vars String of variables separated by commas and spaces. (like "x1, x2, x3").
    @param [in] nVars Number of variables in vars.
    @param [out] parsed_vars Array of variables' names converted to strings (like { "i1", "d2", "c3" }).
    @param [out] parsed_types Array of variables' types converted as
    {AdvAssertType::INT, AdvAssertType::FLOAT, AdvAssertType::CHAR}
    @return 1 (true) if no errors occurred, 0 (false) otherwise. If 0 is returned, contains of

*/
static int parse_vars(  const char* vars_as_string,
                        const unsigned int nVars,
                        char parsed_vars[][MAX_VAR_NAME_LEN],
                        AdvAssertType parsed_types[]);

int main() // временно для тестов
{
    int x1 = 19;
    double eji = 23.42;

    ADV_ASSERT(x1 < 0, "Explanation", 0, "");

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
    printf("Passed variables:\n");

    va_list vars;
    va_start(vars, nVars);

    char (*parsed_vars)[MAX_VAR_NAME_LEN] = (char (*)[MAX_VAR_NAME_LEN]) calloc(nVars, sizeof(char [MAX_VAR_NAME_LEN]));
    AdvAssertType *parsed_types = (AdvAssertType*) calloc(nVars, sizeof(AdvAssertType));

    int is_parse_fine = parse_vars(  vars_as_string,
                                    nVars,
                                    parsed_vars,
                                    parsed_types);

    if (is_parse_fine)
    {
        for (unsigned int i = 0; i < nVars; ++i)
        {
            switch( parsed_types[i] )
            {
                case AdvAssertType::INT:
                    fprintf(stderr, "%s = %d\n", parsed_vars[i], va_arg(vars, int));
                    break;
                case AdvAssertType::CHAR:
                    fprintf(stderr, "%s = %c\n", parsed_vars[i], (char) va_arg(vars, int)); // 'char' is promoted to 'int' when passed through '...'
                    break;
                case AdvAssertType::DOUBLE:
                case AdvAssertType::FLOAT:      // 'float' is promoted to 'double' when passed through '...'
                    fprintf(stderr, "%s = %f\n", parsed_vars[i], va_arg(vars, double));
                    break;
                case AdvAssertType::LONG_INT:
                    fprintf(stderr, "%s = %li\n", parsed_vars[i], va_arg(vars, long int));
                    break;
                case AdvAssertType::POINTER:
                    fprintf(stderr, "%s = %p\n", parsed_vars[i], va_arg(vars, void*));
                    break;
                default:
                    printf("ADV_ASSERT: %s has unknown AdvAssertType. "
                    "Can't read arguments...\n",  parsed_vars[i]);
                    break;
            }
        }
    }
    else
    {
        printf("Some error occurred during parsing variables of the assert.\n");
    }

    free(parsed_vars);
    free(parsed_types);

    va_end(vars);
}

static int parse_vars(  const char* vars_as_string,
                        const unsigned int nVars,
                        char parsed_vars[][MAX_VAR_NAME_LEN],
                        AdvAssertType parsed_types[])
{
    const char* p_curr_vars = vars_as_string; // указатель на обрабатываемый в данный момент символ vars
    int is_in_name = 0;
    char *p_curr_parsed_vars = parsed_vars[0];
    int ind_curr_parsed_vars = 0; // совпадает с количеством прочитанных на данный момент аргументов
    unsigned int name_letter_cnt = 0; // счетчик кол-ва букв в одном имени

    // !!!ЧТОБЫ РАСПАРСИТЬ ТИП, ДОСТАТОЧНО ПРОЧИТАТЬ ПЕРВУЮ БУКВУ!!!
    // А ЗАТЕМ СВИЧОМ ОТОБРАТЬ НУЖНЫЙ ADVASSERTTYPE
    // И ПОМЕСТИТЬ В МАССИВ

    while (*p_curr_vars != '\0')
    {
        //printf("!!! %c\n", *p_curr_vars);
        if ( *p_curr_vars == ',' || isspace(*p_curr_vars) )// если символ не принадлежит имени, т.е. пробел или запятая
        {
            if ( is_in_name )
            {
                // если сейчас внутри имени, значит имя закончилось
                // значит надо поставить '/0'
                // и переставить указатель на начало нового элемент res

                is_in_name = 0;
                name_letter_cnt = 0;
                *p_curr_parsed_vars = '\0';
                p_curr_parsed_vars = parsed_vars[++ind_curr_parsed_vars];
                //printf("!!!  %d\n", ind_curr_parsed_vars);
            }
        }
        else if ( *p_curr_vars == '(' )
        {
            // начинается "(тип)", нужно все это обработать до выхода из данного блока

            // пропускаем возможные пробелы между '(' и первой буквой

            p_curr_vars++;
            while( isspace( *(p_curr_vars) ) ) p_curr_vars++;

            //printf("!!! %c\n", *p_curr_vars);

            switch(*(p_curr_vars)) // читаем первую букву и по ней определяем тип
            {
                case 'i':
                    *(parsed_types++) = AdvAssertType::INT;
                    break;
                case 'c':
                    *(parsed_types++) = AdvAssertType::CHAR;
                    break;
                case 'd':
                    *(parsed_types++) = AdvAssertType::DOUBLE;
                    break;
                case 'f':
                    *(parsed_types++) = AdvAssertType::FLOAT;
                    break;
                case 'l':
                    *(parsed_types++) = AdvAssertType::LONG_INT;
                    break;
                case 'p':
                    *(parsed_types++) = AdvAssertType::POINTER;
                    break;
                default:
                    return 0; // ОШИБКА ЧТЕНИЯ
            }

            // пропускаем все символы до закрывающей скобки
            while ( *p_curr_vars != ')' ) p_curr_vars++;
        }
        else
        {
            // символ принадлежит имени
            if (!is_in_name) is_in_name = 1;

            *p_curr_parsed_vars = *p_curr_vars;

            if (++name_letter_cnt == MAX_VAR_NAME_LEN)
            {
                // слишком длинное читаемое в данный момент имя, значит прекращаем его читать,
                // в следующей итерации обработается конец этого слова и добавится '\0'
                // и пропускаем буквы вплоть до пробела/запятой/конца строки vars

                while (*p_curr_vars != ',' && !isspace(*p_curr_vars) && *p_curr_vars != '\0') p_curr_vars++;

                if (*p_curr_vars == '\0')
                {
                    // в таком случае следующей итерации не будет, а значит нужно здесь добавить '\0'
                    *p_curr_parsed_vars = '\0';
                }

                continue;
            }

            //все ок, передвигаем указатель по слову
            p_curr_parsed_vars++;
        }

        p_curr_vars++;
    }

    return 1;
}
