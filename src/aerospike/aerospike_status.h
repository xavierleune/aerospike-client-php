/*
 *
 * This file should be mimic with as_status.h
 * Any addition/deletion to the status codes should be
 * mapped and changed in the static array "aerospike_status".
 *
 */

#ifndef __AEROSPIKE_STATUS_H__
#define __AEROSPIKE_STATUS_H__

#define MAX_STATUS_MSG_SIZE 512

typedef struct Aerospike_Status {
    int statusno;
    char status_msg[MAX_STATUS_MSG_SIZE];
} AerospikeStatus;

AerospikeStatus aerospike_status[] = {
  { AEROSPIKE_OK                      ,   "OK"                     }, 
  { AEROSPIKE_ERR                     ,   "ERR"                    },
  { AEROSPIKE_ERR_CLIENT              ,   "ERR_CLIENT"             },
  { AEROSPIKE_ERR_PARAM               ,   "ERR_PARAM"              },
  { AEROSPIKE_ERR_CLUSTER             ,   "ERR_CLUSTER"            },
  { AEROSPIKE_ERR_TIMEOUT             ,   "ERR_TIMEOUT"            },
  { AEROSPIKE_ERR_THROTTLED           ,   "ERR_THROTTLED"          },
  { AEROSPIKE_ERR_SERVER              ,   "ERR_SERVER"             },
  { AEROSPIKE_ERR_REQUEST_INVALID     ,   "ERR_REQUEST_INVALID"    },
  { AEROSPIKE_ERR_SERVER_FULL         ,   "ERR_SERVER_FULL"        },
  { AEROSPIKE_ERR_CLUSTER_CHANGE      ,   "ERR_CLUSTER_CHANGE"     },
  { AEROSPIKE_ERR_UNSUPPORTED_FEATURE ,   "ERR_UNSUPPORTED_FEATURE"},
  { AEROSPIKE_ERR_DEVICE_OVERLOAD     ,   "ERR_DEVICE_OVERLOAD"    },
  { AEROSPIKE_ERR_RECORD              ,   "ERR_RECORD"             },
  { AEROSPIKE_ERR_RECORD_BUSY         ,   "ERR_RECORD_BUSY"        },
  { AEROSPIKE_ERR_RECORD_NOT_FOUND    ,   "ERR_RECORD_NOT_FOUND"   },
  { AEROSPIKE_ERR_RECORD_EXISTS       ,   "ERR_RECORD_EXISTS"      },
  { AEROSPIKE_ERR_RECORD_GENERATION   ,   "ERR_RECORD_GENERATION"  },
  { AEROSPIKE_ERR_RECORD_TOO_BIG      ,   "ERR_RECORD_TOO_BIG"     },
//  { AEROSPIKE_ERR_BIN_TYPE            ,   "ERR_BIN_TYPE"           },
  { AEROSPIKE_ERR_RECORD_KEY_MISMATCH ,   "ERR_RECORD_KEY_MISMATCH"},
  { AEROSPIKE_ERR_SCAN                ,   "ERR_SCAN"               },
  { AEROSPIKE_ERR_SCAN_ABORTED        ,   "ERR_SCAN_ABORTED"       },
  { AEROSPIKE_ERR_QUERY               ,   "ERR_QUERY"              },
  { AEROSPIKE_ERR_QUERY_ABORTED       ,   "ERR_QUERY_ABORTED"      },
  { AEROSPIKE_ERR_QUERY_QUEUE_FULL    ,   "ERR_QUERY_QUEUE_FULL"   },
  { AEROSPIKE_ERR_INDEX               ,   "ERR_INDEX"              },
  { AEROSPIKE_ERR_INDEX_OOM           ,   "ERR_INDEX_OOM"          },
  { AEROSPIKE_ERR_INDEX_NOT_FOUND     ,   "ERR_INDEX_NOT_FOUND"    },
  { AEROSPIKE_ERR_INDEX_FOUND         ,   "ERR_INDEX_FOUND"        },
  { AEROSPIKE_ERR_INDEX_NOT_READABLE  ,   "ERR_INDEX_NOT_READABLE" },
  { AEROSPIKE_ERR_INDEX_NAME_MAXLEN   ,   "ERR_INDEX_NAME_MAXLEN"  },
  { AEROSPIKE_ERR_INDEX_MAXCOUNT      ,   "ERR_INDEX_MAXCOUNT"     },
  { AEROSPIKE_ERR_UDF                 ,   "ERR_UDF"                },
  { AEROSPIKE_ERR_UDF_NOT_FOUND       ,   "ERR_UDF_NOT_FOUND"      }
};

#define AEROSPIKE_STATUS_ARR_SIZE (sizeof(aerospike_status)/sizeof(AerospikeStatus))

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
