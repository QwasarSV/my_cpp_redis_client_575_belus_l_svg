#include <main_header.hpp>


// TEST IP AND TCP CONNECTION

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
    if (c)
    {
      redisFree(c);  
    }
}


// TEST GET AND SET

void set_value(redisContext *c, const char *key, const char *value)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "SET %s %s", key, value);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STATUS)
    {
        printf("SET operation - %s\n", reply->str);
    }
    else
    {
        printf("SET operation failed.\n");
    }
    freeReplyObject(reply);
}

void get_value(redisContext *c, const char *key)
{
    redisReply* reply;
    reply = (redisReply*)redisCommand(c, "GET %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STRING)
    {
        printf("GET operation - value: %s\n", reply->str);
    }
    else
    {
        printf("GET operation failed or key does not exist.\n");
    }
    freeReplyObject(reply);
}

void test_redis_get_and_set_opt()
{
    printf("Begin test on GET and SET\n");
    redisOptions opt = {0};
    redisContext *c;
    REDIS_OPTIONS_SET_TCP(&opt, "myredis", 6379);
    opt.options |= REDIS_OPT_PREFER_IPV4;
    c = redisConnectWithOptions(&opt);
    set_value(c, "Hiredis01", "Hiredis01_value");
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
    get_value(c, "Hiredis01");
    if (c)
    {
        redisFree(c);
    }
    printf("End of test\n\n");
}

// TEST KEYS, TYPE and DEL

void list_keys(redisContext *c, const char *pattern) {
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "KEYS %s", pattern);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        printf("Matching keys:\n");
        for (size_t i = 0; i < reply->elements; i++)
        {
            printf("%s\n", reply->element[i]->str);
        }
    }
    else
    {
        printf("No matching keys found.\n");
    }
    freeReplyObject(reply);
}

void get_key_type(redisContext *c, const char *key)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "TYPE %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STATUS)
    {
        printf("Type of key %s: %s\n", key, reply->str);
    }
    else
    {
        printf("Failed to get type for key %s.\n", key);
    }
    freeReplyObject(reply);
}

void delete_key(redisContext *c, const char *key)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "DEL %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        printf("DEL operation - %lld keys deleted.\n", reply->integer);
    }
    else
    {
        printf("DEL operation failed.\n");
    }
    freeReplyObject(reply);
}


void test_redis_key_type_and_del_opt()
{
    printf("Begin test on KEY, TYPE and DEL\n");
    redisOptions opt = {0};
    redisContext *c;
    REDIS_OPTIONS_SET_TCP(&opt, "myredis", 6379);
    opt.options |= REDIS_OPT_PREFER_IPV4;
    c = redisConnectWithOptions(&opt);
    set_value(c, "Hiredis01", "Hiredis01_value");
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
    list_keys(c, "*");        // List all keys
    get_key_type(c, "Hiredis01");   // Get the type of the value stored at key "foo"
    delete_key(c, "Hiredis01");
    if (c)
    {
        redisFree(c);
    }
    printf("End of test\n\n");
}


// TEST UNLINK, EXPIRE and RENAME

void unlink_key(redisContext *c, const char *key)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "UNLINK %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        printf("UNLINK operation - %lld keys unlinked.\n", reply->integer);
    }
    else
    {
        printf("UNLINK operation failed.\n");
    }
    freeReplyObject(reply);
}

void set_expire(redisContext *c, const char *key, int seconds)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "EXPIRE %s %d", key, seconds);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        if (reply->integer == 1)
        {
            printf("EXPIRE operation - timeout set successfully.\n");
        }
        else
        {
            printf("EXPIRE operation - key does not exist.\n");
        }
    }
    else
    {
        printf("EXPIRE operation failed.\n");
    }
    freeReplyObject(reply);
}


void rename_key(redisContext *c, const char *old_key, const char *new_key)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "RENAME %s %s", old_key, new_key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STATUS)
    {
        printf("RENAME operation - key renamed successfully.\n");
    }
    else
    {
        printf("RENAME operation failed.\n");
    }
    freeReplyObject(reply);
}


void test_redis_expire_rename_and_unlink_opt()
{
    printf("Begin test on EXPIRE, RENAME and UNLINKS\n");
    redisOptions opt = {0};
    redisContext *c;
    REDIS_OPTIONS_SET_TCP(&opt, "myredis", 6379);
    opt.options |= REDIS_OPT_PREFER_IPV4;
    c = redisConnectWithOptions(&opt);
    set_value(c, "Hiredis01", "Hiredis01_value");
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
    set_expire(c, "Hiredis01", 3);
    get_value(c, "Hiredis01");
    printf("waiting for key to expire...\n");
    sleep(4);
    get_value(c, "Hiredis01");
    set_value(c, "Hiredis01", "Hiredis01_value");
    rename_key(c, "Hiredis01", "Hiredis02");
    get_value(c, "Hiredis01");
    get_value(c, "Hiredis02");
    unlink_key(c, "Hiredis02");
    if (c)
    {
        redisFree(c);
    }
    printf("End of test\n\n");
}

// TEST LPUSH/RPUSH and LPOP/RPOP

void push_to_list_left(redisContext *c, const char *key, const char *value)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "LPUSH %s %s", key, value);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        printf("LPUSH: New length of the list: %lld\n", reply->integer);
    }
    else
    {
        printf("LPUSH: Failed to push element to the list.\n");
    }

    freeReplyObject(reply);
}

void push_to_list_right(redisContext *c, const char *key, const char *value)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "RPUSH %s %s", key, value);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        printf("RPUSH: New length of the list: %lld\n", reply->integer);
    }
    else
    {
        printf("RPUSH: Failed to push element to the list.\n");
    }
    freeReplyObject(reply);
}

void pop_from_list_left(redisContext *c, const char *key)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "LPOP %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STRING)
    {
        printf("LPOP: Popped element: %s\n", reply->str);
    }
    else
    {
        printf("LPOP: List is empty or key does not exist.\n");
    }

    freeReplyObject(reply);
}

void pop_from_list_right(redisContext *c, const char *key)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "RPOP %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STRING)
    {
        printf("RPOP: Popped element: %s\n", reply->str);
    }
    else
    {
        printf("RPOP: List is empty or key does not exist.\n");
    }
    freeReplyObject(reply);
}

void test_redis_lpush_rpush_and_lpop_rpop_opt()
{
    printf("Begin test on LPUSH, RPUSH and LPOP, RPOP\n");
    redisOptions opt = {0};
    redisContext *c;
    REDIS_OPTIONS_SET_TCP(&opt, "myredis", 6379);
    opt.options |= REDIS_OPT_PREFER_IPV4;
    c = redisConnectWithOptions(&opt);
    set_value(c, "Hiredis01", "Hiredis01_value");
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
        push_to_list_left(c, "mylist", "leftElement");
        push_to_list_right(c, "mylist", "rightElement");
        pop_from_list_left(c, "mylist");
        pop_from_list_right(c, "mylist");  
    if (c)
    {
        redisFree(c);
    }
    printf("End of test\n\n");
}

// TEST LLEN, LREM, LINDEX and LSET

void list_length(redisContext *c, const char *key)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "LLEN %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        printf("Length of list %s: %lld\n", key, reply->integer);
    }
    else
    {
        printf("Failed to get length of list %s.\n", key);
    }
    freeReplyObject(reply);
}

void remove_from_list(redisContext *c, const char *key, int count, const char *element)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "LREM %s %d %s", key, count, element);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        printf("Removed %lld occurrences of %s from list %s.\n", reply->integer, element, key);
    }
    else
    {
        printf("Failed to remove elements from list %s.\n", key);
    }

    freeReplyObject(reply);
}

void get_list_element(redisContext *c, const char *key, int index)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "LINDEX %s %d", key, index);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STRING)
    {
        printf("Element at index %d in list %s: %s\n", index, key, reply->str);
    }
    else
    {
        printf("Failed to retrieve element at index %d from list %s.\n", index, key);
    }
    freeReplyObject(reply);
}

void set_list_element(redisContext *c, const char *key, int index, const char *value)
{
    redisReply *reply;
    reply = (redisReply*)redisCommand(c, "LSET %s %d %s", key, index, value);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_STATUS)
    {
        printf("LSET: Element set successfully.\n");
    }
    else
    {
        printf("LSET: Failed to set element - %s.\n", reply->str);
    }
    freeReplyObject(reply);
}


void test_redis_llen_lrem_lindex_lset_opt()
{
    printf("Begin test on LLEN, LREM, LINDEX and LSET\n");
    redisOptions opt = {0};
    redisContext *c;
    REDIS_OPTIONS_SET_TCP(&opt, "myredis", 6379);
    opt.options |= REDIS_OPT_PREFER_IPV4;
    c = redisConnectWithOptions(&opt);
    set_value(c, "Hiredis01", "Hiredis01_value");
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
        push_to_list_left(c, "mylist", "leftElement01");
        push_to_list_right(c, "mylist", "rightElement01");
        push_to_list_left(c, "mylist", "leftElement02");
        push_to_list_right(c, "mylist", "rightElement02");
        list_length(c, "mylist");
        remove_from_list(c, "mylist", 1, "rightElement02");
        list_length(c, "mylist");
        set_list_element(c, "mylist", 0, "newValue");
        get_list_element(c, "mylist", 0);
        pop_from_list_left(c, "mylist");
        pop_from_list_right(c, "mylist");
        pop_from_list_left(c, "mylist");  

    if (c)
    {
        redisFree(c);
    }
    printf("End of test\n\n");
}


// TEST HGET, HEXISTS, HMSET, HDEL, HSET and HVALS

void set_hash_field(redisContext *c, const char *key, const char *field, const char *value)
{
    redisReply *reply = (redisReply *)redisCommand(c, "HSET %s %s %s", key, field, value);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        if (reply->integer == 1)
        {
            printf("Field '%s' set for the first time.\n", field);
        }
        else
        {
            printf("Field '%s' updated.\n", field);
        }
    }
    else
    {
        printf("Failed to set field '%s'.\n", field);
    }
    freeReplyObject(reply);
}

void get_hash_values(redisContext *c, const char *key)
{
    redisReply *reply = (redisReply *)redisCommand(c, "HVALS %s", key);
    if (reply == NULL)
    {
        printf("Error: %s\n", c->errstr);
        redisFree(c);
        exit(1);
    }
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        printf("Values in hash '%s':\n", key);
        for (size_t i = 0; i < reply->elements; i++)
        {
            printf("%s\n", reply->element[i]->str);
        }
    }
    else
    {
        printf("Failed to retrieve values for hash '%s'.\n", key);
    }
    freeReplyObject(reply);
}

void test_redis_hget_hexists_hmset_hdel_hset_hvals_opt()
{
    printf("Begin test on HGET, HEXISTS, HMSET, HDEL,HSET and HVALS\n");
    redisOptions opt = {0};
    redisContext *c;
    REDIS_OPTIONS_SET_TCP(&opt, "myredis", 6379);
    opt.options |= REDIS_OPT_PREFER_IPV4;
    c = redisConnectWithOptions(&opt);
    set_value(c, "Hiredis01", "Hiredis01_value");
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
    const char *fields[] = {"field1", "field2"};
    const char *values[] = {"value1", "value2"};
    set_hash_field(c, "myhash", "field3", "value3");
    get_hash_values(c, "myhash");
    check_hash_field_exists(c, "myhash", "field1");
    delete_hash_fields(c, "myhash", "field1");
    if (c)
    {
        redisFree(c);
    }
    printf("End of test\n\n");
}