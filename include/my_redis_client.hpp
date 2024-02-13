#ifndef MY_REDIS_CLIENT_
#define MY_REDIS_CLIENT_

#include <string>
#include <hiredis/hiredis.h>


namespace my_redis
{
    #define INVALID     0
    #define TCP_ADDR    1
    #define IP_ADDR     2
    #define TCP_STR     "tcp://"

    class RedisClient
    {
        private:
            redisContext* context;
            std::string host;
            int port;

            int getAdressType(const std::string& address)
            {
                if (address.rfind(TCP_STR, 0) == 0)
                {
                    return TCP_ADDR;
                }
                else if (!address.empty() && std::isdigit(address[0]))
                {
                    return IP_ADDR;
                }
                return INVALID;
            }

            void connection(const std::string& address)
            {
                splitHostAndPort(address, host, port);
                context = redisConnect(host.c_str(), port);
                if (context == NULL || context->err)
                {
                    if (context)
                    {
                        printf("Error: %s\n", context->errstr);
                    }
                    else
                    {
                        printf("Can't allocate redis context\n");
                    }
                }
            }

            void connection_opt(const std::string& address)
            {
                splitHostAndPort(address, host, port);
                redisOptions opt = {0};
                REDIS_OPTIONS_SET_TCP(&opt, host.c_str(), port);
                opt.options |= REDIS_OPT_PREFER_IPV4;
                context = redisConnectWithOptions(&opt);
                if (context == NULL || context->err)
                {
                    if (context)
                    {
                        printf("Error: %s\n", context->errstr);
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
            }

            void splitHostAndPort(const std::string& input, std::string& host, int& port)
            {
                size_t protocolPos = input.find("://");
                size_t startPos = 0;
                if (protocolPos != std::string::npos)
                {
                    startPos = protocolPos + 3;
                }
                size_t colonPos = input.find(':', startPos);
                if (colonPos == std::string::npos)
                {
                    throw std::invalid_argument("Input string does not contain a port separator ':'");
                }
                host = input.substr(startPos, colonPos - startPos);
                std::string portStr = input.substr(colonPos + 1);
                if (portStr.empty() || portStr.find_first_not_of("0123456789") != std::string::npos)
                {
                    throw std::invalid_argument("Port must be a number");
                }
                port = std::stoi(portStr);
                if (port < 0 || port > 65535)
                {
                    throw std::invalid_argument("Port number is out of valid range");
                }
            }
            
            std::string createErrMsg(const std::string& msg, const std::string& value)
            {
                std::string errMsg = msg + value;
                return errMsg;
            }

        public:
            RedisClient(const std::string& address)
            {
                if (getAdressType(address) == TCP_ADDR)
                {
                    connection_opt(address);
                }
                else if (getAdressType(address) == IP_ADDR)
                {
                    connection(address);
                }
            }


// KEY/VALUES
            std::string set(const char* key, const char* value)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "SET %s %s", key, value);
                if (reply == NULL)
                {
                    throw std::runtime_error(context->errstr);
                }
                std::unique_ptr<redisReply, decltype(&freeReplyObject)> autoReply(reply, freeReplyObject);
                if (reply->type == REDIS_REPLY_STATUS)
                {
                    return std::string(reply->str, reply->len);
                }
                else
                {
                    throw std::runtime_error("SET operation failed.");
                }
            }

            std::string get(const char* key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "GET %s", key);
                if (reply == NULL)
                {
                    throw std::runtime_error(context->errstr);
                }
                std::unique_ptr<redisReply, decltype(&freeReplyObject)> autoReply(reply, freeReplyObject);
                if (reply->type == REDIS_REPLY_STRING)
                {
                    return std::string(reply->str, reply->len);
                }
                else
                {
                    throw std::runtime_error("GET operation failed or key does not exist.");
                }
            }

            std::vector<std::string> keys(const char *pattern)
            {
                std::vector<std::string> keys; 
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "KEYS %s", pattern);
                if (reply == NULL)
                {
                    throw std::runtime_error(context->errstr);
                }
                std::unique_ptr<redisReply, decltype(&freeReplyObject)> autoReply(reply, freeReplyObject);

                if (reply->type == REDIS_REPLY_ARRAY)
                {
                    keys.reserve(reply->elements); // same as std::unique_ptr<std::string[]> keys(new std::string[reply->elements]); ?
                    printf("Matching keys:\n");
                    for (size_t index = 0; index < reply->elements; index++)
                    {
                        auto* element = reply->element[index];
                        if (element->type == REDIS_REPLY_STRING)
                        {
                            keys.emplace_back(element->str, element->len);
                        }
                        else
                        {
                            std::cout << "NOT A STRING" << std::endl;
                        } // does element type change in middle of response ?? 
                    }
                }
                else
                {
                    printf("No matching keys found.\n");
                }
                return keys;
            }

            std::string type(const char* key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "TYPE %s", key);
                if (reply == NULL)
                {
                    throw std::runtime_error(context->errstr);
                }
                std::unique_ptr<redisReply, decltype(&freeReplyObject)> autoReply(reply, freeReplyObject);
                if (reply->type == REDIS_REPLY_STATUS)
                {
                    return std::string(reply->str, reply->len);
                }
                else
                {
                    std::string errMsg = createErrMsg("Failed to get type for key ", key);
                    throw std::runtime_error(errMsg);
                }
            }

            std::string del(const char* key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "DEL %s", key);
                if (reply == NULL)
                {
                    throw std::runtime_error(context->errstr);
                }
                std::unique_ptr<redisReply, decltype(&freeReplyObject)> autoReply(reply, freeReplyObject);
                if (reply->type == REDIS_REPLY_INTEGER)
                {
                    return std::to_string(reply->integer);
                }
                else
                {
                    throw std::runtime_error("DEL operation failed.");
                }
            }

            void unlink(const char* key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "UNLINK %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void expire(const char* key, int seconds)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "EXPIRE %s %d", key, seconds);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void rename(const char *old_key, const char *new_key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "RENAME %s %s", old_key, new_key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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
//LISTS
            void lpush(const char* key, const char* value)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "LPUSH %s %s", key, value);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void rpush(const char* key, const char* value)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "RPUSH %s %s", key, value);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void lpop(const char* key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "LPOP %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void rpop(const char* key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "RPOP %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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


            void llen(const char* key)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "LLEN %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void lrem(const char* key, int count, const char *element)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "LREM %s %d %s", key, count, element);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void lindex(const char* key, int index)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "LINDEX %s %d", key, index);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void lset(const char* key, int index, const char* value)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "LSET %s %d %s", key, index, value);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

//HASHES

            void hget(const char* key, const char* field)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "HGET %s %s", key, field);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }
                if (reply->type == REDIS_REPLY_STRING)
                {
                    printf("Value of '%s': %s\n", field, reply->str);
                }
                else if (reply->type == REDIS_REPLY_NIL)
                {
                    printf("Field '%s' does not exist.\n", field);
                }
                else
                {
                    printf("Failed to retrieve value for field '%s'.\n", field);
                }
                freeReplyObject(reply);
            }

            void hexists(const char* key, const char* field)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "HEXISTS %s %s", key, field);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }
                if (reply->type == REDIS_REPLY_INTEGER)
                {
                    if (reply->integer == 1)
                    {
                        printf("Field '%s' exists.\n", field);
                    }
                    else
                    {
                        printf("Field '%s' does not exist.\n", field);
                    }
                }
                else
                {
                    printf("Failed to check existence for field '%s'.\n", field);
                }
                freeReplyObject(reply);
            }

            void hmset(const char* key, const char** fields, const char** values, size_t fieldCount)
            {
                const char* argv[2 + fieldCount * 2];
                size_t argvlen[2 + fieldCount * 2];
                argv[0] = "HMSET";
                argvlen[0] = 5;
                argv[1] = key;
                argvlen[1] = strlen(key);
                int index = 0;
                while (index < fieldCount)
                {
                    argv[2 + index * 2] = fields[index];
                    argvlen[2 + index * 2] = strlen(fields[index]);
                    argv[2 + index * 2 + 1] = values[index];
                    argvlen[2 + index * 2 + 1] = strlen(values[index]);
                    index += 1;
                }
                redisReply* reply = (redisReply*)redisCommandArgv(context, 2 + fieldCount * 2, argv, argvlen);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }
                if (reply->type == REDIS_REPLY_STATUS)
                {
                    printf("Fields set successfully.\n");
                }
                else
                {
                    printf("Failed to set fields.\n");
                }
                freeReplyObject(reply);
            }

            void hdel(const char* key, const char* field)
            {
                redisReply* reply;
                reply = (redisReply*)redisCommand(context, "HDEL %s %s", key, field);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }
                if (reply->type == REDIS_REPLY_INTEGER)
                {
                    printf("Number of fields deleted: %lld\n", reply->integer);
                }
                else
                {
                    printf("Failed to delete field '%s' from hash.\n", field);
                }
                freeReplyObject(reply);
            }

            void hset(const char* key, const char* field, const char* value)
            {
                redisReply* reply = (redisReply*)redisCommand(context, "HSET %s %s %s", key, field, value);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
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

            void hvals(const char* key)
            {
                redisReply* reply = (redisReply*)redisCommand(context, "HVALS %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }
                if (reply->type == REDIS_REPLY_ARRAY)
                {
                    printf("Values in hash '%s':\n", key);
                    for (size_t index = 0; index < reply->elements; index++) //iso C++ cannot compare integer and ptr but this works...
                    {
                        printf("%s\n", reply->element[index]->str);
                    }
                }
                else
                {
                    printf("Failed to retrieve values for hash '%s'.\n", key);
                }
                freeReplyObject(reply);
            }

            void hgetall(const char* key)
            {
                redisReply* reply = (redisReply*)redisCommand(context, "HGETALL %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }
                if (reply->type == REDIS_REPLY_ARRAY)
                {
                    for (size_t index = 0; index < reply->elements; index += 2) //iso C++ cannot compare integer and ptr but this works...
                    {
                        printf("Field: %s, Value: %s\n", reply->element[index]->str, reply->element[index + 1]->str);
                    }
                }
                else
                {
                    printf("Failed to retrieve hash fields for '%s'.\n", key);
                }
                freeReplyObject(reply);
            }

            void hkeys(const char* key)
            {
                redisReply* reply = (redisReply*)redisCommand(context, "HKEYS %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }

                if (reply->type == REDIS_REPLY_ARRAY)
                {
                    printf("Fields in hash '%s':\n", key);
                    int index = 0;
                    for (size_t index = 0; index < reply->elements; index++) //iso C++ cannot compare integer and ptr but this works...
                    {
                        printf("%s\n", reply->element[index]->str);
                    }
                }
                else
                {
                    printf("Failed to retrieve keys for hash '%s'.\n", key);
                }

                freeReplyObject(reply);
            }

            void hlen(const char* key)
            {
                redisReply *reply = (redisReply *)redisCommand(context, "HLEN %s", key);
                if (reply == NULL)
                {
                    printf("Error: %s\n", context->errstr);
                    redisFree(context);
                    exit(1);
                }
                if (reply->type == REDIS_REPLY_INTEGER)
                {
                    printf("Number of fields in hash '%s': %lld\n", key, reply->integer);
                }
                else
                {
                    printf("Failed to get the length of hash '%s'.\n", key);
                }
                freeReplyObject(reply);
            }

            ~RedisClient()
            {
                if (context)
                {   
                    redisFree(context);
                }
            }
    };
};

#endif