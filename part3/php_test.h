#ifndef PHP_TEST_H
# define PHP_TEST_H

#ifdef ZTS
# include "TSRM.h"
#endif

extern zend_module_entry test_module_entry;
# define phpext_test_ptr &test_module_entry

# define PHP_TEST_VERSION "0.1.0"

typedef struct {
    zend_string *name;
    zend_long age;
} php_test_person;

#define PHP_TEST_PERSON_RES_NAME "Person Data"

PHP_MINIT_FUNCTION(test);
PHP_FUNCTION(test_person_new);
PHP_FUNCTION(test_person_greet);
PHP_FUNCTION(test_person_delete);
PHP_FUNCTION(test_person_pnew);
PHP_FUNCTION(test_person_pnew2);

#if defined(ZTS) && defined(COMPILE_DL_TEST)
    ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif