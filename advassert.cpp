#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "advassert.h"

struct AdvAssertType
{
    const char* type;
    const char* specf;
};

//! @brief Represents any pointer, which can be used in printf with spec "%p".
const char* AdvAssertPointer = "void *";

//! @brief Contains all supported types.
const AdvAssertType adv_assert_types[] = {
                                            {"int",             "%d" },
                                            {"char",            "%c" },
                                            {"double",          "%f" },
                                            {"float",           "%f" },
                                            {"long int",        "%ld"},
                                            {AdvAssertPointer,  "%p" }
                                         };
enum Type {
    INT=0,
    CHAR=1,
    DOUBLE=2,
    FLOAT=3,
    LONG_INT=4,
    POINTER=5,
};

//! @brief Maximum variable's name's length (including '\0').
const unsigned int MAX_VAR_NAME_LEN = 101;

//! @brief Maximum type's name's length (including '\0').
const unsigned int MAX_TYPE_NAME_LEN = 51;

//! @brief Maximum format's length (including '\0').
const unsigned int MAX_FORMAT_LEN = MAX_TYPE_NAME_LEN + 10;

//! @brief Number of supported types, aka len(adv_assert_types)
const unsigned int N_SUPPORTED_TYPES = sizeof(adv_assert_types)/sizeof(adv_assert_types[0]);

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
                        unsigned int parsed_types[]);

int main() // временно для тестов
{
    char x1 = 'F';
    double eji = 23.42;

    ADV_ASSERT(x1 < 0, "Explanation", 3, (char) x1, ( double)   eji, ( double *) &eji );

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
                const unsigned int nVars,
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
    unsigned int *parsed_types = (unsigned int*) calloc(nVars, sizeof(AdvAssertType));

    int is_parse_fine = parse_vars(  vars_as_string,
                                    nVars,
                                    parsed_vars,
                                    parsed_types);

    if (is_parse_fine)
    {
        for (unsigned int i = 0; i < nVars; ++i)
        {
            char format[MAX_FORMAT_LEN] = "%s = ";
            strcat(format, adv_assert_types[ parsed_types[i] ].specf);

            switch( (Type) parsed_types[i] )
            {
                case Type::INT:
                    fprintf(stderr, format, parsed_vars[i], va_arg(vars, int) );
                    break;
                case Type::CHAR:
                    fprintf(stderr, format, parsed_vars[i], (char) va_arg(vars, int)); // 'char' is promoted to 'int' when passed through '...'
                    break;
                case Type::DOUBLE:
                case Type::FLOAT:      // 'float' is promoted to 'double' when passed through '...'
                    fprintf(stderr, format, parsed_vars[i], va_arg(vars, double));
                    break;
                case Type::LONG_INT:
                    fprintf(stderr, format, parsed_vars[i], va_arg(vars, long int));
                    break;
                case Type::POINTER:
                    fprintf(stderr, format, parsed_vars[i], va_arg(vars, void*));
                    break;
                default:
                    printf("ADV_ASSERT: %s has unknown type. "
                    "Can't read arguments...\n",  parsed_vars[i]);
                    break;
            }
            printf("\n");
        }
    }
    else
    {
        printf("Some error occurred during parsing variables of the assert. "
        "Here are the passed variables: %s\n", vars_as_string);
    }

    free(parsed_vars);
    free(parsed_types);

    va_end(vars);
}

static int parse_vars(  const char* vars_as_string,
                        const unsigned int nVars,
                        char parsed_vars[][MAX_VAR_NAME_LEN],
                        unsigned int parsed_types[])
{
    const char* p_curr_vars = vars_as_string; // указатель на обрабатываемый в данный момент символ vars
    int is_in_name = 0;
    char *p_curr_parsed_vars = parsed_vars[0];
    unsigned int ind_curr_parsed_vars = 0; // совпадает с количеством прочитанных на данный момент аргументов
    unsigned int name_letter_cnt = 0; // счетчик кол-ва букв в одном имени

    while (*p_curr_vars != '\0' && ind_curr_parsed_vars < nVars)
    {
        // пояснение к условию: ind_curr_parsed_vars в том числе содержит в себе кол-во прочитанных аргументов

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
            }
        }
        else if ( *p_curr_vars == '(' )
        {
            // начинается "(тип)", нужно все это обработать до выхода из данного блока
            char buf_type[MAX_TYPE_NAME_LEN];
            unsigned int ind_buf = 0;

            // пропускаем возможные пробелы между '(' и первой буквой
            p_curr_vars++;
            while( isspace( *(p_curr_vars) ) ) p_curr_vars++;

            // теперь p_curr_vars указывает на первую букву типа
            // тип может иметь пробелы; нужно читать вплоть до ')'
            // а затем отрезать последние пробелы (если они есть)

            // читаем все символы до закрывающей скобки
            while ( *p_curr_vars != ')' )
            {
                buf_type[ind_buf++] = *(p_curr_vars++);

                if (ind_buf == MAX_TYPE_NAME_LEN - 1)
                {
                    //если ind_buf индекс последнего доступного элемента,
                    //то уже прочитать тип не выйдет, выдаем ошибку

                    return 0;
                }
            }

            //ind_buf - индекс элемента за последним помещенным
            buf_type[ind_buf] = '\0';

            //чистка задних пробелов
            while( buf_type[--ind_buf] == ' ') buf_type[ind_buf] = '\0';

            //теперь в buf_type нормальное имя типа
            //ищем тип в массиве типов
            unsigned int type_ind;

            //ЧАСТНЫЙ СЛУЧАЙ: если последний непробельный символ это *, значит это указатель
            if ( buf_type[ind_buf] == '*' ) strcpy(buf_type, AdvAssertPointer);

            //ОБЩИЙ СЛУЧАЙ
            for (type_ind = 0; type_ind < N_SUPPORTED_TYPES; ++type_ind)
            {
                if (strcmp(buf_type, adv_assert_types[type_ind].type) == 0) break;
            }

            if (type_ind == N_SUPPORTED_TYPES)
            {
                //тип не был найден, кидаем ошибку

                return 0;
            }

            *(parsed_types++) = type_ind;
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
