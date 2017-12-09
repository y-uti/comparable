/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Nikita Popov                                                 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_comparable.h"
#include "zend_interfaces.h"

zend_module_entry comparable_module_entry = {
	STANDARD_MODULE_HEADER,
	"comparable",
	NULL,
	PHP_MINIT(comparable),
	NULL,
	NULL,
	NULL,
	NULL,
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_COMPARABLE
ZEND_GET_MODULE(comparable)
#endif

zend_class_entry *comparable_ce;

static zend_object_handlers comparable_handlers;

static zend_object *comparable_create_object_override(zend_class_entry *ce)
{
	zend_object *object;

	object = zend_objects_new(ce);
	object_properties_init(object, ce);

	object->handlers = &comparable_handlers;

	return object;
}

static int comparable_compare_objects(zval *obj1, zval *obj2)
{
	zval retval;
	int result;

	zend_call_method_with_2_params(NULL, Z_OBJCE_P(obj1), NULL, "compare", &retval, obj1, obj2);

	if (Z_TYPE(retval) == IS_NULL) {
		return zend_get_std_object_handlers()->compare_objects(obj1, obj2);
	}

	return ZEND_NORMALIZE_BOOL(Z_LVAL(retval));
}

static int implement_comparable(zend_class_entry *interface, zend_class_entry *ce)
{
	if (ce->create_object != NULL) {
		zend_error(E_ERROR, "Comparable interface can only be used on userland classes");
	}

	ce->create_object = comparable_create_object_override;

	return SUCCESS;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_comparable, 0, 0, 2)
	ZEND_ARG_INFO(0, obj1)
	ZEND_ARG_INFO(0, obj2)
ZEND_END_ARG_INFO()

const zend_function_entry comparable_functions[] = {
	ZEND_FENTRY(compare, NULL, arginfo_comparable, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT|ZEND_ACC_STATIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(comparable)
{
	zend_class_entry tmp_ce;
	INIT_CLASS_ENTRY(tmp_ce, "Comparable", comparable_functions);
	comparable_ce = zend_register_internal_interface(&tmp_ce);

	comparable_ce->interface_gets_implemented = implement_comparable;

	memcpy(&comparable_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	comparable_handlers.compare_objects = comparable_compare_objects;

	return SUCCESS;
}
