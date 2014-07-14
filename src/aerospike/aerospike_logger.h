

enum Aerospike_logger_constants {
    LOG_LEVEL_TRACE = 1, 
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_OFF
};

#define MAX_LOGGER_CONSTANT_STR_SIZE 512

typedef struct Aerospike_logger_Constants {
    int constantno;
    char constant_str[MAX_LOGGER_CONSTANT_STR_SIZE];
} AerospikeLoggerConstants;

AerospikeLoggerConstants aerospike_logger_constants[] = {
    { LOG_LEVEL_TRACE         ,   "LOG_LEVEL_TRACE"               },
    { LOG_LEVEL_DEBUG         ,   "LOG_LEVEL_DEBUG"               },
    { LOG_LEVEL_INFO          ,   "LOG_LEVEL_INFO"                },
    { LOG_LEVEL_WARN          ,   "LOG_LEVEL_WARN"                },
    { LOG_LEVEL_ERROR         ,   "LOG_LEVEL_ERROR"               },
    { LOG_LEVEL_OFF           ,   "LOG_LEVEL_OFF"                 }
};

#define AEROSPIKE_LOGGER_CONSTANTS_ARR_SIZE (sizeof(aerospike_logger_constants)/sizeof(AerospikeLoggerConstants))

#define EXPOSE_LOGGER_CONSTANTS_STR_ZEND(Aerospike_ce)                                          \
do {                                                                        					\
    int32_t i;                                                           						\
    for (i = 0; i < AEROSPIKE_LOGGER_CONSTANTS_ARR_SIZE; i++) {          						\
        zend_declare_class_constant_long(                                						\
                Aerospike_ce, aerospike_logger_constants[i].constant_str,      					\
                    strlen(aerospike_logger_constants[i].constant_str),          				\
                        aerospike_logger_constants[i].constantno TSRMLS_CC);   					\
    }                                                                   					    \
} while(0);

