PHP_ARG_ENABLE(test, whether to enable test support,
[  --enable-test          Enable test support], no)

if test "$PHP_TEST" != "no"; then
  AC_DEFINE(HAVE_TEST, 1, [ Have test support ])
  PHP_NEW_EXTENSION(test, test.c, $ext_shared)
fi