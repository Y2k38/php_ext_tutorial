#ifndef PHP_TEST_H
# define PHP_TEST_H

#ifdef ZTS
# include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(test)
    long counter;
    zend_bool direction;
ZEND_END_MODULE_GLOBALS(test)

#ifdef ZTS
# define TEST_G(v) TSRMG(test_globals_id, zend_test_globals *, v)
#else
# define TEST_G(v) (test_globals.v)
#endif

extern zend_module_entry test_module_entry;
# define phpext_test_ptr &test_module_entry

# define PHP_TEST_VERSION "0.1.0"

PHP_MINIT_FUNCTION(test);
PHP_MSHUTDOWN_FUNCTION(test);
PHP_RINIT_FUNCTION(test);
PHP_FUNCTION(test);

#if defined(ZTS) && defined(COMPILE_DL_TEST)
    ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif