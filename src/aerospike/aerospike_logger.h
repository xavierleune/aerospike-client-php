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

/*
 *******************************************************************************************************
 * Enum for C client's Logger constants.
 *******************************************************************************************************
 */

enum Aerospike_logger_constants {
	LOG_LEVEL_TRACE = 1,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_OFF
};

#define MAX_LOGGER_CONSTANT_STR_SIZE 512

/*
 *******************************************************************************************************
 * Structure to map constant number to constant name string for Aerospike Logger constants.
 *******************************************************************************************************
 */
typedef struct Aerospike_logger_Constants {
	int constantno;
	char constant_str[MAX_LOGGER_CONSTANT_STR_SIZE];
} AerospikeLoggerConstants;

/*
 *******************************************************************************************************
 * Instance of Mapper of constant number to constant name string for Aerospike Logger constants.
 *******************************************************************************************************
 */
AerospikeLoggerConstants aerospike_logger_constants[] = {
	{ LOG_LEVEL_TRACE, "LOG_LEVEL_TRACE" },
	{ LOG_LEVEL_DEBUG, "LOG_LEVEL_DEBUG" },
	{ LOG_LEVEL_INFO,  "LOG_LEVEL_INFO"  },
	{ LOG_LEVEL_WARN,  "LOG_LEVEL_WARN"  },
	{ LOG_LEVEL_ERROR, "LOG_LEVEL_ERROR" },
	{ LOG_LEVEL_OFF,   "LOG_LEVEL_OFF"   }
};

#define AEROSPIKE_LOGGER_CONSTANTS_ARR_SIZE (sizeof(aerospike_logger_constants)/sizeof(AerospikeLoggerConstants))

/*
 *******************************************************************************************************
 * MACRO to expose logger constants in Aerospike class.
 *
 * @param Aerospike_ce          The zend class entry for Aerospike class.
 *******************************************************************************************************
 */
#define EXPOSE_LOGGER_CONSTANTS_STR_ZEND(Aerospike_ce)                         \
	do {                                                                       \
    	int32_t i;                                                             \
    	for (i = 0; i < AEROSPIKE_LOGGER_CONSTANTS_ARR_SIZE; i++) {            \
        	zend_declare_class_constant_long(                                  \
				Aerospike_ce, aerospike_logger_constants[i].constant_str,      \
				strlen(aerospike_logger_constants[i].constant_str),            \
				aerospike_logger_constants[i].constantno TSRMLS_CC);           \
    	}                                                                      \
	} while(0);
