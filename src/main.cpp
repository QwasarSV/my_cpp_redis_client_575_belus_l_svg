#include <main_header.hpp>

int main(int argc, char** argv)
{

    test_redis_connection_opt();
    test_redis_get_and_set_opt();
    test_redis_key_type_and_del_opt();
    test_redis_expire_rename_and_unlink_opt();
    test_redis_lpush_rpush_and_lpop_rpop_opt();
    test_redis_llen_lrem_lindex_lset_opt();
    test_redis_hget_hexists_hmset_hdel_hset_hvals_opt();
    test_redis_hgetall_hkeys_and_hlen_opt();
    
    return EXIT_SUCCESS;
}