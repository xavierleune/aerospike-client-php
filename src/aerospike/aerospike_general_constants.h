/*
 *
 * Copyright (C) 2014-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef __AEROSPIKE_GENERAL_CONSTANTS_H__
#define __AEROSPIKE_GENERAL_CONSTANTS_H__

#include "aerospike/aerospike_index.h"
#include "aerospike/as_operations.h"
#define MAX_GENERAL_CONSTANT_STR_SIZE 512
#define AS_CDT_OP_LIST_APPEND_NEW AS_CDT_OP_LIST_APPEND + 1000
#define AS_CDT_OP_LIST_INSERT_NEW AS_CDT_OP_LIST_INSERT + 1000
#define AS_CDT_OP_LIST_INSERT_ITEMS_NEW AS_CDT_OP_LIST_INSERT_ITEMS + 1000
#define AS_CDT_OP_LIST_POP_NEW AS_CDT_OP_LIST_POP + 1000
#define AS_CDT_OP_LIST_POP_RANGE_NEW AS_CDT_OP_LIST_POP_RANGE + 1000
#define AS_CDT_OP_LIST_REMOVE_NEW AS_CDT_OP_LIST_REMOVE + 1000
#define AS_CDT_OP_LIST_REMOVE_RANGE_NEW AS_CDT_OP_LIST_REMOVE_RANGE + 1000
#define AS_CDT_OP_LIST_CLEAR_NEW AS_CDT_OP_LIST_CLEAR + 1000
#define AS_CDT_OP_LIST_SET_NEW AS_CDT_OP_LIST_SET + 1000
#define AS_CDT_OP_LIST_GET_NEW AS_CDT_OP_LIST_GET + 1000
#define AS_CDT_OP_LIST_GET_RANGE_NEW AS_CDT_OP_LIST_GET_RANGE + 1000
#define AS_CDT_OP_LIST_TRIM_NEW AS_CDT_OP_LIST_TRIM + 1000
#define AS_CDT_OP_LIST_SIZE_NEW AS_CDT_OP_LIST_SIZE + 1000

/*
 *******************************************************************************************************
 * Structure to map constant number to constant name string for Aerospike General Long constants.
 *******************************************************************************************************
 */
typedef struct Aerospike_General_Long_Constants {
    int constantno;
    char constant_str[MAX_GENERAL_CONSTANT_STR_SIZE];
} AerospikeGeneralLongConstants;

/*
 *******************************************************************************************************
 * Structure to map constant number to constant name string for Aerospike General String constants.
 *******************************************************************************************************
 */
typedef struct Aerospike_General_String_Constants {
    char constant_value[MAX_GENERAL_CONSTANT_STR_SIZE];
    char constant_str[MAX_GENERAL_CONSTANT_STR_SIZE];
} AerospikeGeneralStringConstants;

/*
 *******************************************************************************************************
 * Instance of Mapper of constant number to constant name string for Aerospike General Long constants.
 *******************************************************************************************************
 */
static AerospikeGeneralLongConstants aerospike_general_long_constants[] = {
    { AS_INDEX_STRING,      "INDEX_STRING"       },
    { AS_INDEX_NUMERIC,     "INDEX_NUMERIC"      },
    { AS_INDEX_TYPE_DEFAULT,"INDEX_TYPE_DEFAULT" },
    { AS_INDEX_TYPE_LIST,   "INDEX_TYPE_LIST"    },
    { AS_INDEX_TYPE_MAPKEYS,"INDEX_TYPE_MAPKEYS" },
    { AS_INDEX_TYPE_MAPVALUES,"INDEX_TYPE_MAPVALUES" },
    { AS_OPERATOR_WRITE,    "OPERATOR_WRITE"     },
    { AS_OPERATOR_READ,     "OPERATOR_READ"      },
    { AS_OPERATOR_INCR,     "OPERATOR_INCR"      },
    { AS_OPERATOR_PREPEND,  "OPERATOR_PREPEND"   },
    { AS_OPERATOR_APPEND,   "OPERATOR_APPEND"    },
    { AS_OPERATOR_TOUCH,    "OPERATOR_TOUCH"     },
    { AS_CDT_OP_LIST_APPEND_NEW,"OP_LIST_APPEND" },
    { AS_CDT_OP_LIST_INSERT_NEW,"OP_LIST_INSERT" },
    { AS_CDT_OP_LIST_INSERT_ITEMS_NEW,"OP_LIST_INSERT_ITEMS"},
    { AS_CDT_OP_LIST_POP_NEW, "OP_LIST_POP"      },
    { AS_CDT_OP_LIST_POP_RANGE_NEW, "OP_LIST_POP_RANGE"},
    { AS_CDT_OP_LIST_REMOVE_NEW, "OP_LIST_REMOVE"},
    { AS_CDT_OP_LIST_REMOVE_RANGE_NEW, "OP_LIST_REMOVE_RANGE"},
    { AS_CDT_OP_LIST_CLEAR_NEW, "OP_LIST_CLEAR"  },
    { AS_CDT_OP_LIST_SET_NEW, "OP_LIST_SET"      },
    { AS_CDT_OP_LIST_GET_NEW, "OP_LIST_GET"      },
    { AS_CDT_OP_LIST_GET_RANGE_NEW, "OP_LIST_GET_RANGE"},
    { AS_CDT_OP_LIST_TRIM_NEW, "OP_LIST_TRIM"    },
    { AS_CDT_OP_LIST_SIZE_NEW, "OP_LIST_SIZE"    }
};

/*
 *******************************************************************************************************
 * Instance of Mapper of constant number to constant name string for Aerospike General String constants.
 *******************************************************************************************************
 */
static AerospikeGeneralStringConstants aerospike_general_string_constants[] = {
    { "=",         "OP_EQ"  },
    { "BETWEEN",   "OP_BETWEEN" },
    { "CONTAINS",   "OP_CONTAINS" },
    { "RANGE",   "OP_RANGE" },
    { "GEOWITHIN",   "OP_GEOWITHINREGION" }
};

#define AEROSPIKE_GENERAL_LONG_CONSTANTS_ARR_SIZE (sizeof(aerospike_general_long_constants)/sizeof(AerospikeGeneralLongConstants))
#define AEROSPIKE_GENERAL_STRING_CONSTANTS_ARR_SIZE (sizeof(aerospike_general_string_constants)/sizeof(AerospikeGeneralStringConstants))

/*
 *******************************************************************************************************
 * MACRO to expose general long constants in Aerospike class.
 *
 * @param Aerospike_ce          The zend class ent
 * ry for Aerospike class.
 *******************************************************************************************************
 */
#define EXPOSE_GENERAL_CONSTANTS_LONG_ZEND(Aerospike_ce)                                        \
do {                                                                                            \
    int32_t i;                                                                                  \
    for (i = 0; i < AEROSPIKE_GENERAL_LONG_CONSTANTS_ARR_SIZE; i++) {                           \
        zend_declare_class_constant_long(                                                       \
                Aerospike_ce, aerospike_general_long_constants[i].constant_str,                 \
                    strlen(aerospike_general_long_constants[i].constant_str),                   \
                aerospike_general_long_constants[i].constantno TSRMLS_CC);                      \
    }                                                                                           \
} while(0);

/*
 *******************************************************************************************************
 * MACRO to expose general string constants in Aerospike class.
 *
 * @param Aerospike_ce          The zend class entry for Aerospike class.
 *******************************************************************************************************
 */
#define EXPOSE_GENERAL_CONSTANTS_STRING_ZEND(Aerospike_ce)                                      \
do {                                                                                            \
    int32_t i;                                                                                  \
    for (i = 0; i < AEROSPIKE_GENERAL_STRING_CONSTANTS_ARR_SIZE; i++) {                         \
        zend_declare_class_constant_string(                                                     \
                Aerospike_ce, aerospike_general_string_constants[i].constant_str,               \
                    strlen(aerospike_general_string_constants[i].constant_str),                 \
                        aerospike_general_string_constants[i].constant_value TSRMLS_CC);        \
    }                                                                                           \
} while(0);
#endif
