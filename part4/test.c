#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_test.h"
#include "zend_interfaces.h"
#include <zend_exceptions.h>

zend_class_entry *test_ce_exception;
zend_class_entry *test_ce_myclass;

static void test_init_exception() {
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "MyException", NULL);
	test_ce_exception = zend_register_internal_class_ex(&ce, (zend_class_entry*)zend_exception_get_default());
}

typedef struct _test_struct {
    zend_object std;
    int unknown_id;
    char *unknown_str;
} test_struct;

static zend_object *create_test_struct(zend_class_entry *class_type) {
	test_struct *intern;

	intern = ecalloc(1, sizeof(test_struct) + zend_object_properties_size(class_type));

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->std.handlers = zend_get_std_object_handlers();

	return &intern->std;
}

static void free_test_struct(void *object) {
	test_struct *secrets = (test_struct*)object;
	if (secrets->unknown_str) {
		efree(secrets->unknown_str);
	}
	efree(secrets);
}

PHP_METHOD(MyClass, __construct)
{
	char *msg;
	size_t msg_len;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_STRING(msg, msg_len)
	ZEND_PARSE_PARAMETERS_END();

	zend_update_property_string(test_ce_myclass, getThis(), "msg", sizeof("msg")-1, msg);
}

PHP_METHOD(MyClass, hello)
{
    RETURN_STRING("hello");
}

PHP_METHOD(MyClass, factory)
{
	zval *myzval;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_ZVAL(myzval)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, test_ce_myclass);

	zend_call_method(return_value, test_ce_myclass, NULL, "__construct", sizeof("__construct")-1, NULL, 1, myzval, NULL);
}

PHP_METHOD(MyClass, attachStruct) {
	test_struct *secrets;

	secrets = (test_struct*)getThis();

	RETURN_LONG(secrets->unknown_id);
}

PHP_METHOD(MyClass, throwExcept) {
	zend_throw_exception(test_ce_exception, "custom exception throw", 1024);
}

static const zend_function_entry test_methods[] = {
	PHP_ME(MyClass, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(MyClass, hello, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(MyClass, factory, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(MyClass, attachStruct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(MyClass, throwExcept, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static void test_init_myclass()
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "MyClass", test_methods);
	ce.create_object = create_test_struct;
	test_ce_myclass = zend_register_internal_class(&ce);

	zend_declare_property_bool(test_ce_myclass, "success", sizeof("success")-1, 1, ZEND_ACC_PUBLIC);
}

PHP_MINIT_FUNCTION(test)
{
	test_init_myclass();
	test_init_exception();
	return SUCCESS;
}

/* 模块注册 */
zend_module_entry test_module_entry = {
    STANDARD_MODULE_HEADER,
    "test",                    /* 扩展名称 */
    NULL,                      /* 自定义函数注册 */
    PHP_MINIT(test),           /* PHP_MINIT */
    NULL,                      /* PHP_MSHUTDOWN */
    NULL,                      /* PHP_RINIT */
    NULL,                      /* PHP_RSHUTDOWN */
    NULL,                      /* PHP_MINFO */
    PHP_TEST_VERSION,          /* version */
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TEST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(test)
#endif