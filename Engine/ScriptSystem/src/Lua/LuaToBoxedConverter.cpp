#include "LuaToBoxedConverter.hpp"
#include "RefFunction.hpp"

using namespace e00::impl::scripting;
BoxedValue lua_to_boxed_value(lua_State *L, int n, const TypeInfo &info) {
  switch (lua_type(L, n)) {
    case LUA_TNIL:
      {
      }
      break;

    case LUA_TNUMBER:
      if (info.is_arithmetic()) {
        // in lua this could be float, double, int, long...
        if (info.is_compatible<float>()) {
          return BoxedValue(lua_tonumberx(L, n, nullptr));
        } else if (info.is_compatible<double>()) {
          return BoxedValue((double)lua_tonumberx(L, n, nullptr));
        } else if (info.is_compatible<int>()) {
          return BoxedValue(lua_tointegerx(L, n, nullptr));
        } else if (info.is_compatible<unsigned int>()) {
          return BoxedValue((unsigned int)lua_tointegerx(L, n, nullptr));
        } else if (info.is_compatible<long>()) {
          return BoxedValue((long)lua_tointegerx(L, n, nullptr));
        } else if (info.is_compatible<unsigned long>()) {
          return BoxedValue((unsigned long)lua_tointegerx(L, n, nullptr));
        } else if (info.is_compatible<char>()) {
          return BoxedValue((char)lua_tointegerx(L, n, nullptr));
        } else if (info.is_compatible<unsigned char>()) {
          return BoxedValue((unsigned char)lua_tointegerx(L, n, nullptr));
        } else if (info.is_compatible<bool>()) {
          return BoxedValue((bool)lua_toboolean(L, n));
        }
      }

      // Wrong type
      break;

    case LUA_TBOOLEAN:
      if (info.is_compatible<bool>()) {
        return BoxedValue((bool)lua_toboolean(L, n));
      }
      break;

    case LUA_TSTRING:
      {
        size_t len;
        const auto rawstr = lua_tolstring(L, n, &len);

        if (info.is_const()) {
          if (info.is_compatible<const std::string &>()) {
            return BoxedValue(std::string(rawstr, len));
          } else if (info.is_compatible<const std::string_view &>()) {
            return BoxedValue(std::string_view(rawstr, len));
          }
        }

        if (info.is_compatible<std::string>()) {
          return BoxedValue(std::string(rawstr, len));
        } else if (info.is_compatible<std::string_view>()) {
          return BoxedValue(std::string_view(rawstr, len));
        }
        break;
      }

    case LUA_TTABLE:
      // a lua map
      break;

    case LUA_TFUNCTION:
      // build a proxy function
      return BoxedValue(new lua::RefFunction(L, luaL_ref(L, LUA_REGISTRYINDEX)), user_type<ProxyFunction*>());

    case LUA_TUSERDATA:
      break;

    case LUA_TTHREAD:
      break;

    case LUA_TLIGHTUSERDATA:
      break;
  }

  // Param has wrong value
  return BoxedValue();
}
