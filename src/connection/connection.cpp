#include <main_header.hpp>

void test_redis_connection()
{
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err)
    {
        if (c)
        {
            printf("Error: %s\n", c->errstr);
            // handle error
        }
        else
        {
            printf("Can't allocate redis context\n");
        }
    }
}

void test_redis_connection_opt()
{
    redisOptions opt = {0};
    redisContext *c;
    REDIS_OPTIONS_SET_TCP(&opt, "myredis", 6379);
    opt.options |= REDIS_OPT_PREFER_IPV4;
    c = redisConnectWithOptions(&opt);
    if (c == NULL || c->err)
    {
        if (c)
        {
            printf("Error: %s\n", c->errstr);
        }
        else
        {
            printf("Can't allocate redis context\n");
        }
    }
    else
    {
        printf("Successfully connected to Redis\n");
    }
    if (c) redisFree(c);
}