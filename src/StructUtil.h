#ifndef __BASE_STRUCT_H_H__
#define __BASE_STRUCT_H_H__

#ifndef WIN32
#include <ext/hash_map>
using namespace __gnu_cxx;
namespace __gnu_cxx
{
        template<>
        struct hash<long long>
        {
                size_t operator()(long long __x) const
                {
                        return __x;
                }
        } ;

        template<>
        struct hash<std::string>
        {
                size_t operator()(std::string __s) const
                {
                        return __stl_hash_string(__s.c_str());
                }
        } ;
}
#else
#include <hash_map>
using namespace stdext;
#endif

#endif