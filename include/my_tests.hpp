#ifndef __MY_TESTS_H_
#define __MY_TESTS_H_

#include <main_header.hpp>

//basic connection test;
void test_redis_connection();
void test_redis_connection_opt();

//basic Key/Value test;
void test_redis_get_and_set_opt();
void test_redis_key_type_and_del_opt();
void test_redis_expire_rename_and_unlink_opt();

//basic List test;
void test_redis_lpush_rpush_and_lpop_rpop_opt();
void test_redis_llen_lrem_lindex_lset_opt();

//basic Hashes test;
void test_redis_hget_hexists_hmset_hdel_hset_hvals_opt();
void test_redis_hgetall_hkeys_and_hlen_opt();

// full test;
void full_c_test();

#endif