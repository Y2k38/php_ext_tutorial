#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_test.h"

static const zend_function_entry test_functions[] = {
    PHP_FE(test, NULL)
    PHP_FE(test_type, NULL)
    PHP_FE(test_again, NULL)
    PHP_FE(test_array, NULL)
    PHP_FE(test_array_strings, NULL)
    PHP_FE(test_array_walk, NULL)
    PHP_FE(test_array_value, NULL)
    PHP_FE(test_get_global_var, NULL)
    PHP_FE(test_set_local_var, NULL)
    PHP_FE_END
};

zend_module_entry test_module_entry = {
    STANDARD_MODULE_HEADER,
    "test",
    test_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_TEST_VERSION,
    STANDARD_MODULE_PROPERTIES
};

PHP_FUNCTION(test)
{
    zend_string *name;
    zend_long t_param = 0;
    int times = 1;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(name)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(t_param)
    ZEND_PARSE_PARAMETERS_END();

    if (ZEND_NUM_ARGS() == 2) {
        times = (int) (t_param < 1 ? 1 : t_param);
    }

    for (int i = 0; i < times; i++) {
        php_printf("Hello %s", ZSTR_VAL(name));
    }
    RETURN_TRUE;
}

PHP_FUNCTION(test_type)
{
    zval *uservar;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(uservar);
    ZEND_PARSE_PARAMETERS_END();

    switch (Z_TYPE_P(uservar)) {
        case IS_NULL:
            php_printf("NULL");
            break;
        case IS_TRUE:
            php_printf("Boolean: TRUE");
            break;
        case IS_FALSE:
            php_printf("Boolean: FALSE");
            break;
        case IS_LONG:
            php_printf("Long: %ld", Z_LVAL_P(uservar));
            break;
        case IS_DOUBLE:
            php_printf("Double: %f", Z_DVAL_P(uservar));
            break;
        case IS_STRING:
            php_printf("String: ");
            PHPWRITE(Z_STRVAL_P(uservar), Z_STRLEN_P (uservar));
            break;
        case IS_RESOURCE:
            php_printf("Resource");
            break;
        case IS_ARRAY:
            php_printf("Array");
            break;
        case IS_OBJECT:
            php_printf("Object");
            break;
        default:
            php_printf("Unknown");
    }
}

PHP_FUNCTION(test_again)
{
    zval *zname;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(zname);
    ZEND_PARSE_PARAMETERS_END();

    convert_to_string(zname);
    php_printf("Hello ");
    PHPWRITE(Z_STRVAL_P(zname), Z_STRLEN_P(zname));
    RETURN_TRUE;
}

PHP_FUNCTION(test_array)
{
    array_init(return_value);
    add_index_long(return_value, 3, 123);
    add_next_index_string(return_value, "example");

    char *mystr = estrdup("five");
    add_next_index_string(return_value, mystr);
    efree(mystr);

    add_assoc_double(return_value, "pi", 3.1415926);
    
    zval subarr;
    array_init(&subarr);
    add_next_index_string(&subarr, "hello");
    add_assoc_zval(return_value, "subarr", &subarr);
}

PHP_FUNCTION(test_array_strings)
{
    zval *arr, *data;
    HashTable *arr_hash;
    HashPosition pointer;
    int array_count;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_ARRAY(arr);
    ZEND_PARSE_PARAMETERS_END();

    arr_hash = Z_ARRVAL_P(arr);
    array_count = zend_hash_num_elements(arr_hash);

    php_printf("The array passed contains %d elements\n", array_count);

    for (
        zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
        data = zend_hash_get_current_data_ex(arr_hash, &pointer);
        zend_hash_move_forward_ex(arr_hash, &pointer)
    ) {
        // example 1
        // if (Z_TYPE_P(data) == IS_STRING) {
        //     PHPWRITE(Z_STRVAL_P(data), Z_STRLEN_P(data));
        //     php_printf("\n");
        // }

        // example 2
        // zval tmp;
        // tmp = *data;
        // zval_copy_ctor(&tmp);
        // convert_to_string(&tmp);
        // PHPWRITE(Z_STRVAL(tmp), Z_STRLEN(tmp));
        // php_printf("\n");
        // zval_dtor(&tmp);

        // example 3
        int keytype;
        zend_string * str_index;
        zend_ulong num_index;

        keytype = zend_hash_get_current_key_ex(arr_hash, &str_index, &num_index, &pointer);
        if (HASH_KEY_IS_STRING == keytype) {
            PHPWRITE(ZSTR_VAL(str_index), ZSTR_LEN(str_index));
        } else if (HASH_KEY_IS_LONG == keytype) {
            php_printf("%ld", num_index);
        }

        php_printf(" => ");

        zval tmp;
        tmp = *data;
        zval_copy_ctor(&tmp);
        convert_to_string(&tmp);
        PHPWRITE(Z_STRVAL(tmp), Z_STRLEN(tmp));
        php_printf("\n");
        zval_dtor(&tmp);
    }

    RETURN_TRUE;
}

static int test_array_walk(zval *pDest)
{
    zval tmp;

    tmp = *pDest;
    zval_copy_ctor(&tmp);
    convert_to_string(&tmp);
    PHPWRITE(Z_STRVAL(tmp), Z_STRLEN(tmp));
    php_printf("\n");
    zval_dtor(&tmp);

    return ZEND_HASH_APPLY_KEEP;
}

static int test_array_walk_arg(zval *pDest, void *argument)
{
    php_printf("%s", (char *)argument);
    test_array_walk(pDest);

    return ZEND_HASH_APPLY_KEEP;
}

static int test_array_walk_args(zval *pDest, int num_args, va_list args, zend_hash_key *hash_key)
{
    php_printf("%s", va_arg(args, char *));
    test_array_walk(pDest);
    php_printf("%s\n", va_arg(args, char *));

    return ZEND_HASH_APPLY_KEEP;
}

PHP_FUNCTION(test_array_walk)
{
    zval *arr;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_ARRAY(arr);
    ZEND_PARSE_PARAMETERS_END();

    zend_hash_apply(Z_ARRVAL_P(arr), (apply_func_t) test_array_walk);
    zend_hash_apply_with_argument(Z_ARRVAL_P(arr), (apply_func_arg_t) test_array_walk_arg, "Hello");
    zend_hash_apply_with_arguments(Z_ARRVAL_P(arr), (apply_func_args_t) test_array_walk_args, 2, "Hello ", "Welcome to my extension!");
}

PHP_FUNCTION(test_array_value)
{
    zval *arr, *offset, *val;
    char * tmp;
    zend_string *str_index = NULL;
    zend_ulong num_index;

    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_ARRAY(arr);
        Z_PARAM_ZVAL(offset);
    ZEND_PARSE_PARAMETERS_END();

    switch (Z_TYPE_P(offset)) {
        case IS_NULL:
        case IS_FALSE:
            num_index = 0;
            break;
        case IS_TRUE:
            num_index = 1;
            break;
        case IS_DOUBLE:
            num_index = (long) Z_DVAL_P(offset);
            break;
        case IS_LONG:
        case IS_RESOURCE:
            num_index = Z_LVAL_P(offset);
            break;
        case IS_STRING:
            str_index = zval_get_string(offset);
            break;
        case IS_ARRAY:
            tmp = "Array";
            str_index = zend_string_init(tmp, sizeof(tmp) - 1, 0);
            break;
        case IS_OBJECT:
            tmp = "Object";
            str_index = zend_string_init(tmp, sizeof(tmp) - 1, 0);
            break;
        default:
            tmp = "Unknown";
            str_index = zend_string_init(tmp, sizeof(tmp) - 1, 0);
    }

    if (str_index && (val = zend_hash_find(Z_ARRVAL_P(arr), str_index)) == NULL) {
        RETURN_NULL();
    } else if (!str_index && (val = zend_hash_index_find(Z_ARRVAL_P(arr), num_index)) == NULL) {
        RETURN_NULL();
    }

    *return_value = *val;
    zval_copy_ctor(return_value);
}

PHP_FUNCTION(test_get_global_var)
{
    zval *val;
    zend_string *varname;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_STR(varname);
    ZEND_PARSE_PARAMETERS_END();

    if ((val = zend_hash_find(&EG(symbol_table), varname)) == NULL) {
        php_error_docref(NULL, E_NOTICE, "Undefined variable: %s", ZSTR_VAL(varname));
        RETURN_NULL();
    }

    *return_value = *val;
    zval_copy_ctor(return_value);
}

PHP_FUNCTION(test_set_local_var)
{
    zval *val;
    zend_string *key;
    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_STR(key);
        Z_PARAM_ZVAL(val);
    ZEND_PARSE_PARAMETERS_END();

    zval newval;
    newval = *val;
    zval_copy_ctor(&newval);
    // something wrong, segmentation fault
    zend_hash_add(EG(current_execute_data)->symbol_table, key, &newval);
    zval_dtor(&newval);

    RETURN_TRUE;
}

#ifdef COMPILE_DL_TEST
# ifdef ZTS
    ZEND_TSRMLS_CACHE_DEFINE()
# endif
    ZEND_GET_MODULE(test)
#endif