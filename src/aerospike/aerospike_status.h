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
 *
 * This file should mimic constants from as_status.h
 * Any addition/deletion to the status codes should be
 * mapped and changed in the static array "aerospike_status".
 *
 */

#ifndef __AEROSPIKE_STATUS_H__
#define __AEROSPIKE_STATUS_H__

#define MAX_STATUS_MSG_SIZE 512

/*
 *******************************************************************************************************
 * Structure to map constant number to constant name string for Aerospike status constants.
 *******************************************************************************************************
 */
typedef struct Aerospike_Status {
	int statusno;
	char status_msg[MAX_STATUS_MSG_SIZE];
} AerospikeStatus;

/*
 *******************************************************************************************************
 * Instance of Mapper of constant number to constant name string for Aerospike status constants.
 *******************************************************************************************************
 */
AerospikeStatus aerospike_status[] = {
	{ AEROSPIKE_ERR_INVALID_HOST              ,   "ERR_INVALID_HOST"                   },
	{ AEROSPIKE_ERR_PARAM                     ,   "ERR_PARAM"                          },
	{ AEROSPIKE_ERR_CLIENT                    ,   "ERR_CLIENT"                         },
	{ AEROSPIKE_OK                            ,   "OK"                                 },
	{ AEROSPIKE_ERR_SERVER                    ,   "ERR_SERVER"                         },
	{ AEROSPIKE_ERR_RECORD_NOT_FOUND          ,   "ERR_RECORD_NOT_FOUND"               },
	{ AEROSPIKE_ERR_RECORD_GENERATION         ,   "ERR_RECORD_GENERATION"              },
	{ AEROSPIKE_ERR_REQUEST_INVALID           ,   "ERR_REQUEST_INVALID"                },
	{ AEROSPIKE_ERR_RECORD_EXISTS             ,   "ERR_RECORD_EXISTS"                  },
	{ AEROSPIKE_ERR_BIN_EXISTS                ,   "ERR_BIN_EXISTS"                     },
	{ AEROSPIKE_ERR_CLUSTER_CHANGE            ,   "ERR_CLUSTER_CHANGE"                 },
	{ AEROSPIKE_ERR_SERVER_FULL               ,   "ERR_SERVER_FULL"                    },
	{ AEROSPIKE_ERR_TIMEOUT                   ,   "ERR_TIMEOUT"                        },
	{ AEROSPIKE_ERR_NO_XDR                    ,   "ERR_NO_XDR"                         },
	{ AEROSPIKE_ERR_CLUSTER                   ,   "ERR_CLUSTER"                        },
	{ AEROSPIKE_ERR_BIN_INCOMPATIBLE_TYPE     ,   "ERR_BIN_INCOMPATIBLE_TYPE"          },
	{ AEROSPIKE_ERR_RECORD_TOO_BIG            ,   "ERR_RECORD_TOO_BIG"                 },
	{ AEROSPIKE_ERR_RECORD_BUSY               ,   "ERR_RECORD_BUSY"                    },
	{ AEROSPIKE_ERR_SCAN_ABORTED              ,   "ERR_SCAN_ABORTED"                   },
	{ AEROSPIKE_ERR_UNSUPPORTED_FEATURE       ,   "ERR_UNSUPPORTED_FEATURE"            },
	{ AEROSPIKE_ERR_BIN_NOT_FOUND             ,   "ERR_BIN_NOT_FOUND"                  },
	{ AEROSPIKE_ERR_DEVICE_OVERLOAD           ,   "ERR_DEVICE_OVERLOAD"                },
	{ AEROSPIKE_ERR_RECORD_KEY_MISMATCH       ,   "ERR_RECORD_KEY_MISMATCH"            },
	{ AEROSPIKE_ERR_NAMESPACE_NOT_FOUND       ,   "ERR_NAMESPACE_NOT_FOUND"            },
	{ AEROSPIKE_ERR_BIN_NAME                  ,   "ERR_BIN_NAME"                       },
	{ AEROSPIKE_QUERY_END                     ,   "ERR_QUERY_END"                      },
	{ AEROSPIKE_ERR_UDF                       ,   "ERR_UDF"                            },
	{ AEROSPIKE_ERR_LARGE_ITEM_NOT_FOUND      ,   "ERR_LARGE_ITEM_NOT_FOUND"           },
	{ AEROSPIKE_ERR_INDEX_FOUND               ,   "ERR_INDEX_FOUND"                    },
	{ AEROSPIKE_ERR_INDEX_NOT_FOUND           ,   "ERR_INDEX_NOT_FOUND"                },
	{ AEROSPIKE_ERR_INDEX_OOM                 ,   "ERR_INDEX_OOM"                      },
	{ AEROSPIKE_ERR_INDEX_NOT_READABLE        ,   "ERR_INDEX_NOT_READABLE"             },
	{ AEROSPIKE_ERR_INDEX                     ,   "ERR_INDEX"                          },
	{ AEROSPIKE_ERR_INDEX_NAME_MAXLEN         ,   "ERR_INDEX_NAME_MAXLEN"              },
	{ AEROSPIKE_ERR_INDEX_MAXCOUNT            ,   "ERR_INDEX_MAXCOUNT"                 },
	{ AEROSPIKE_ERR_QUERY_ABORTED             ,   "ERR_QUERY_ABORTED"                  },
	{ AEROSPIKE_ERR_QUERY_QUEUE_FULL          ,   "ERR_QUERY_QUEUE_FULL"               },
	{ AEROSPIKE_ERR_QUERY_TIMEOUT             ,   "ERR_QUERY_TIMEOUT"                  },
	{ AEROSPIKE_ERR_QUERY                     ,   "ERR_QUERY"                          },
	{ AEROSPIKE_ERR_UDF_NOT_FOUND             ,   "ERR_UDF_NOT_FOUND"                  },
	{ AEROSPIKE_ERR_LUA_FILE_NOT_FOUND        ,   "ERR_LUA_FILE_NOT_FOUND"             },
	{ AEROSPIKE_SECURITY_NOT_SUPPORTED        ,   "ERR_SECURITY_NOT_SUPPORTED"         },
	{ AEROSPIKE_SECURITY_NOT_ENABLED          ,   "ERR_SECURITY_NOT_ENABLED"           },
	{ AEROSPIKE_SECURITY_SCHEME_NOT_SUPPORTED ,   "ERR_SECURITY_SCHEME_NOT_SUPPORTED"  },
	{ AEROSPIKE_INVALID_USER                  ,   "ERR_INVALID_USER"                   },
	{ AEROSPIKE_USER_ALREADY_EXISTS           ,   "ERR_USER_ALREADY_EXISTS"            },
	{ AEROSPIKE_INVALID_PASSWORD              ,   "ERR_INVALID_PASSWORD"               },
	{ AEROSPIKE_EXPIRED_PASSWORD              ,   "ERR_EXPIRED_PASSWORD"               },
	{ AEROSPIKE_FORBIDDEN_PASSWORD            ,   "ERR_FORBIDDEN_PASSWORD"             },
	{ AEROSPIKE_INVALID_CREDENTIAL            ,   "ERR_INVALID_CREDENTIAL"             },
	{ AEROSPIKE_INVALID_ROLE                  ,   "ERR_INVALID_ROLE"                   },
	{ AEROSPIKE_INVALID_PRIVILEGE             ,   "ERR_INVALID_PRIVILEGE"              },
	{ AEROSPIKE_INVALID_COMMAND               ,   "ERR_INVALID_COMMAND"                },
	{ AEROSPIKE_INVALID_FIELD                 ,   "ERR_INVALID_FIELD"                  },
	{ AEROSPIKE_ILLEGAL_STATE                 ,   "ERR_ILLEGAL_STATE"                  },
	{ AEROSPIKE_NOT_AUTHENTICATED             ,   "ERR_NOT_AUTHENTICATED"              },
	{ AEROSPIKE_ROLE_VIOLATION                ,   "ERR_ROLE_VIOLATION"                 },
	{ AEROSPIKE_ROLE_ALREADY_EXISTS           ,   "ERR_ROLE_ALREADY_EXISTS"            },
	{ AEROSPIKE_ERR_GEO_INVALID_GEOJSON       ,   "ERR_GEO_INVALID_GEOJSON"             },
};

#define AEROSPIKE_STATUS_ARR_SIZE (sizeof(aerospike_status)/sizeof(AerospikeStatus))

/*
 *******************************************************************************************************
 * MACRO to expose status constants in Aerospike class.
 *
 * @param Aerospike_ce          The zend class entry for Aerospike class.
 *******************************************************************************************************
 */
#define EXPOSE_STATUS_CODE_ZEND(Aerospike_ce)                       \
do {                                                                \
    int32_t i;                                                      \
    for (i = 0; i <= AEROSPIKE_STATUS_ARR_SIZE; i++) {              \
        zend_declare_class_constant_long(                           \
                Aerospike_ce, aerospike_status[i].status_msg,       \
                    strlen(aerospike_status[i].status_msg),         \
                        aerospike_status[i].statusno TSRMLS_CC);    \
    }                                                               \
} while(0);

#endif /* end of __AEROSPIKE_STATUS_H__*/
