#ifndef MY_REDIS_CLIENT_
#define MY_REDIS_CLIENT_

#include <string>
#include <hiredis/hiredis.h>


namespace my 
{
    #define INVALID     0 
    #define TCP_ADDR    1
    #define IP_ADDR     2
    #define TCP_STR =   "tcp://"

    class RedisCli
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

        public:
            redisCli(const std::string& host.cstr)
            {

            }
            ~redisCLient(context)
            {
                if (context)
                {   
                    redisFree(context);
                }
            }
    };
};


#endif