#include "LuaToBoxedConverter.hpp"
#include "RefFunction.hpp"

using namespace e00::impl::scripting;
BoxedValue lua_to_boxed_value(lua_State *L, int n, const TypeInfo &info) {
  auto guessed = lua_to_boxed_value_guess(L, n);

  // Shortcut if it's the right type
  if (guessed.get_type_info() == info) {
    return guessed;
  }

  // We need to do some conversion

  // We found nothing :(
  return BoxedValue();
}

BoxedValue lua_to_boxed_value_guess(lua_State *L, int n) {
  switch (lua_type(L, n)) {
    case LUA_TNIL:
      break;

    case LUA_TNUMBER:
      {
        int isint = 0;
        lua_Integer xi = lua_tointegerx(L, n, &isint);
        if (isint) {
          return BoxedValue(xi);
        }

        lua_Number xn = lua_tonumberx(L, n, nullptr);
        return BoxedValue(xn);
      }

    case LUA_TBOOLEAN:
      return BoxedValue(static_cast<bool>(lua_toboolean(L, n)));

    case LUA_TSTRING:
      {
        size_t len;
        const auto rawstr = lua_tolstring(L, n, &len);
        return BoxedValue(std::string(rawstr, len));
      }

    case LUA_TTABLE:
      // a lua map
      break;

    case LUA_TFUNCTION:
      // build a proxy function
      return BoxedValue(new lua::RefFunction(L, luaL_ref(L, LUA_REGISTRYINDEX)), user_type<ProxyFunction *>());

    case LUA_TUSERDATA:
      break;

    case LUA_TTHREAD:
      break;

    case LUA_TLIGHTUSERDATA:
      break;
  }

  return BoxedValue();
}
