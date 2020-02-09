#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_test.h"

int le_test_person;
int le_test_person_persist;

static const zend_function_entry test_functions[] = {
    PHP_FE(test_person_new, NULL)
    PHP_FE(test_person_greet, NULL)
    PHP_FE(test_person_delete, NULL)
    PHP_FE(test_person_pnew, NULL)
    PHP_FE(test_person_pnew2, NULL)
    PHP_FE_END
};

zend_module_entry test_module_entry = {
    STANDARD_MODULE_HEADER,
    "test",
    test_functions,
    PHP_MINIT(test),
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_TEST_VERSION,
    STANDARD_MODULE_PROPERTIES
};

static void php_test_person_dtor(zend_resource *res)
{
    php_test_person *person = (php_test_person *) res->ptr;

    if (person) {
        if (person->name) {
            zend_string_release(person->name); // efree zend_string => zend_string_release
        }
        efree(person);
    }
}

static void php_test_person_persist_dtor(zend_resource *res)
{
    php_test_person *person = (php_test_person *) res->ptr;

    if (person) {
        if (person->name) {
            zend_string_release(person->name);
        }
        pefree(person, 1);
    }
}

PHP_MINIT_FUNCTION(test)
{
    le_test_person = zend_register_list_destructors_ex(php_test_person_dtor, NULL, PHP_TEST_PERSON_RES_NAME, module_number);
    le_test_person_persist = zend_register_list_destructors_ex(NULL, php_test_person_persist_dtor, PHP_TEST_PERSON_RES_NAME, module_number);
}

PHP_FUNCTION(test_person_new)
{
    php_test_person *person;
    zend_string *name;
    zend_long age;

    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_STR(name);
        Z_PARAM_LONG(age);
    ZEND_PARSE_PARAMETERS_END();

    if (age < 0 || age > 255) {
        php_error_docref(NULL, E_WARNING, "Nonsense age (%ld) given, person resource not created.", age);
        RETURN_FALSE;
    }

    person = emalloc(sizeof(php_test_person));
    person->name = zend_string_copy(name); // estrndup + zend_string => zend_string_copy
    person->age = age;

    RETURN_RES(zend_register_resource(person, le_test_person));
}

PHP_FUNCTION(test_person_greet)
{
    php_test_person *person;
    zval *zperson;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_RESOURCE(zperson);
    ZEND_PARSE_PARAMETERS_END();

    person = (php_test_person *) zend_fetch_resource_ex(zperson, PHP_TEST_PERSON_RES_NAME, le_test_person);

    php_printf("Hello ");
    PHPWRITE(ZSTR_VAL(person->name), ZSTR_LEN(person->name));
    php_printf("!\nAccording to my records, you are %ld years old.\n", person->age);

    RETURN_TRUE;
}

PHP_FUNCTION(test_person_delete)
{
    zval * zperson;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_RESOURCE(zperson);
    ZEND_PARSE_PARAMETERS_END();

    zend_list_delete(Z_RES_P(zperson));

    RETURN_TRUE;
}

PHP_FUNCTION(test_person_pnew)
{
    php_test_person * person;
    zend_string * name;
    zend_long age;

    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_STR(name);
        Z_PARAM_LONG(age);
    ZEND_PARSE_PARAMETERS_END();

    if (age < 0 || age > 255) {
        php_error_docref(NULL, E_WARNING, "Nonsense age (%ld) given, person resource not created.", age);
        RETURN_FALSE;
    }

    person = pemalloc(sizeof(php_test_person), 1);
    person->name = zend_string_dup(name, 1);
    person->age = age;

    RETURN_RES(zend_register_resource(person, le_test_person_persist));
}

PHP_FUNCTION(test_person_pnew2)
{
    php_test_person * person;
    char * key_raw;
    size_t key_len;
    zend_string * name, * key;
    zend_long age;
    zval * zperson = NULL;

    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_STR(name);
        Z_PARAM_LONG(age);
    ZEND_PARSE_PARAMETERS_END();

    if (age < 0 || age > 255) {
        php_error_docref(NULL, E_WARNING, "Nonsense age (%ld) given, person resource not created.", age);
        RETURN_FALSE;
    }

    key_len = spprintf(&key_raw, 0, "test_person_%s_%ld\n", ZSTR_VAL(name), age);
    key = zend_string_init(key_raw, key_len, 1);
    efree(key_raw);

    if ((zperson = zend_hash_find(&EG(persistent_list), key)) != NULL) {
        person = (php_test_person *) zend_fetch_resource_ex(zperson, PHP_TEST_PERSON_RES_NAME, le_test_person_persist);

        ZVAL_RES(return_value, zend_register_persistent_resource_ex(key, person, le_test_person_persist));
        zend_string_release(key);
        return ;
    }

    person = pemalloc(sizeof(php_test_person), 1);
    person->name = zend_string_copy(name);
    person->age = age;

    ZVAL_RES(return_value, zend_register_persistent_resource_ex(key, person, le_test_person_persist));
    zend_string_release(key);
}

#ifdef COMPILE_DL_TEST
# ifdef ZTS
    ZEND_TSRMLS_CACHE_DEFINE()
# endif
    ZEND_GET_MODULE(test)
#endif