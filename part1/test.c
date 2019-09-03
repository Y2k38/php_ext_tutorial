#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_test.h"

ZEND_DECLARE_MODULE_GLOBALS(test)

static const zend_function_entry test_functions[] = {
    PHP_FE(test, NULL)
    PHP_FE_END
};

zend_module_entry test_module_entry = {
    STANDARD_MODULE_HEADER,
    "test",
    test_functions,
    PHP_MINIT(test),
    PHP_MSHUTDOWN(test),
    PHP_RINIT(test),
    NULL,
    NULL,
    PHP_TEST_VERSION,
    STANDARD_MODULE_PROPERTIES
};

PHP_INI_BEGIN()
PHP_INI_ENTRY("test.foo", "bar", PHP_INI_ALL, NULL)
STD_PHP_INI_ENTRY("test.direction", "1", PHP_INI_ALL, OnUpdateBool, direction, zend_test_globals, test_globals)
PHP_INI_END();

static void php_test_init_globals(zend_test_globals *test_globals) {
    test_globals->direction = 1;
}

PHP_RINIT_FUNCTION(test)
{
    TEST_G(counter) = 0;

    return SUCCESS;
}

PHP_MINIT_FUNCTION(test)
{
    ZEND_INIT_MODULE_GLOBALS(test, php_test_init_globals, NULL);

    REGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(test)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_FUNCTION(test)
{
    if (TEST_G(direction)) {
        TEST_G(counter)++;
    } else {
        TEST_G(counter)--;
    }

    RETURN_LONG(TEST_G(counter));
}

#ifdef COMPILE_DL_TEST
# ifdef ZTS
    ZEND_TSRMLS_CACHE_DEFINE()
# endif
    ZEND_GET_MODULE(test)
#endif
