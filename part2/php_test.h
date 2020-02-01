#ifndef PHP_TEST_H
# define PHP_TEST_H

#ifdef ZTS
# include "TSRM.h"
#endif

extern zend_module_entry test_module_entry;
# define phpext_test_ptr &test_module_entry

# define PHP_TEST_VERSION "0.1.0"

PHP_FUNCTION(test);
PHP_FUNCTION(test_type);
PHP_FUNCTION(test_again);
PHP_FUNCTION(test_array);
PHP_FUNCTION(test_array_strings);
PHP_FUNCTION(test_array_walk);
PHP_FUNCTION(test_array_value);
PHP_FUNCTION(test_get_global_var);
PHP_FUNCTION(test_set_local_var);

#if defined(ZTS) && defined(COMPILE_DL_TEST)
    ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif