#include <main_header.hpp>

void test_pp_get_and_set_opt()
{
    printf("Begin c++ test on GET and SET\n");
    my_redis::RedisClient rc("tcp://myredis:6379");
    std::cout << "SET operation - " << rc.set("Hiredis01", "Hiredis01_value") << std::endl;
    std::cout << "GET operation - value: " << rc.get("Hiredis01") << std::endl;
    printf("End of c++ test...\n\n\n");
}

void test_pp_keys_type_and_del_opt()
{
    printf("Begin c++ test on KEYS, TYPE and DEL\n");
    my_redis::RedisClient rc("tcp://myredis:6379");
    std::cout << "SET operation - " << rc.set("Hiredis01", "Hiredis01_value") << std::endl;
    std::vector<std::string> vector = rc.keys("*");
    for (std::vector<std::string>::iterator it = std::begin(vector); it != std::end(vector); ++it)
    {
        std::cout << *it << "\n";
    }
    std::cout << "Type of key " << "Hiredis01" << " :" << rc.type("Hiredis01")  << std::endl;
    std::cout << "DEL operation - " << rc.del("Hiredis01") << " keys deleted." << std::endl;
    printf("End of c++ test...\n\n\n");
}

void test_pp_expire_rename_and_unlink_opt()
{
    printf("Begin c++ test on EXPIRE, RENAME and UNLINK\n");
    my_redis::RedisClient rc("tcp://myredis:6379");
    rc.expire("Hiredis01", 3);
    rc.get("Hiredis01");
    printf("Waiting for key to expire...\n");
    sleep(4);
    rc.get("Hiredis01");
    rc.set("Hiredis01", "Hiredis01_value");
    rc.rename("Hiredis01", "Hiredis02");
    rc.get("Hiredis01");
    rc.get("Hiredis02");
    rc.unlink("Hiredis02");
    printf("End of c++ test...\n\n\n");
}

void test_pp_lpush_rpush_and_lpop_rpop_opt()
{
    printf("Begin c++ test on LPUSH, RPUSH and LPOP, RPOP\n");
    my_redis::RedisClient rc("tcp://myredis:6379");
    rc.lpush("mylist", "leftElement");
    rc.rpush("mylist", "rightElement");
    rc.lpop("mylist");
    rc.rpop("mylist");
    printf("End of c++ test...\n\n\n");
}

void test_pp_redis_llen_lrem_lindex_lset_opt()
{
    printf("Begin c++ test on LLEN, LREM, LINDEX and LSET\n");
    my_redis::RedisClient rc("tcp://myredis:6379");
    rc.lpush("mylist", "leftElement01");
    rc.rpush("mylist", "rightElement01");
    rc.lpush("mylist", "leftElement02");
    rc.rpush("mylist", "rightElement02");
    rc.llen("mylist");
    rc.lrem("mylist", 1, "rightElement02");
    rc.llen("mylist");
    rc.lset("mylist", 0, "newValue");
    rc.lindex("mylist", 0);
    rc.lpop("mylist");
    rc.rpop("mylist");
    rc.lpop("mylist");
    printf("End of c++ test...\n\n\n");
}

void test_pp_redis_hget_hexists_hmset_hdel_hset_hvals_opt()
{
    printf("Begin c++ test on HGET, HEXISTS, HMSET, HDEL,HSET and HVALS\n"); 
    my_redis::RedisClient rc("tcp://myredis:6379");
    const char* fields[] = {"field1", "field2"};
    const char* values[] = {"value1", "value2"};
    rc.hmset("myhash", fields, values, 2);
    rc.hset("myhash", "field3", "value3");
    rc.hget("myhash", "field1");
    rc.hvals("myhash");
    rc.hexists("myhash", "field3");
    rc.hdel("myhash", "field3");
    rc.hdel("myhash", "field2");
    rc.hdel("myhash", "field1");
    printf("End of c++ test...\n\n\n");
}

void test_pp_redis_hgetall_hkeys_and_hlen_opt()
{
    printf("Begin c++ test on HGETALL, HKEYS, and HLENS\n");
    const char* fields[] = {"field1", "field2"};
    const char* values[] = {"value1", "value2"};
    my_redis::RedisClient rc("tcp://myredis:6379");
    rc.hmset("myhash", fields, values, 2);
    rc.hset("myhash", "field3", "value3");
    rc.hgetall("myhash");
    rc.hkeys("myhash");
    rc.hlen("myhash"); 
    rc.hdel("myhash", "field3");
    rc.hdel("myhash", "field2");
    rc.hdel("myhash", "field1");
    printf("End of c++ test...\n\n\n");
}

int full_cpp_test()
{
    
    test_pp_get_and_set_opt();
    test_pp_keys_type_and_del_opt();
    test_pp_expire_rename_and_unlink_opt();
    test_pp_lpush_rpush_and_lpop_rpop_opt();
    test_pp_redis_llen_lrem_lindex_lset_opt();
    test_pp_redis_hget_hexists_hmset_hdel_hset_hvals_opt();
    test_pp_redis_hgetall_hkeys_and_hlen_opt();
    return EXIT_SUCCESS;  
}