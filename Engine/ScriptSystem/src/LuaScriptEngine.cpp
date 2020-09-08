#include "LuaScriptEngine.hpp"

#include <iostream>

namespace {
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
      break;

    case LUA_TUSERDATA:
      break;

    case LUA_TTHREAD:
      break;

    case LUA_TLIGHTUSERDATA:
      break;
  }

  // Param has wrong value
  std::cerr << "Param " << n << " has wrong type\n";
  return BoxedValue();
}

int lua_trampoline_handle_return(lua_State *L, const BoxedValue &boxed_rv) {
  if (boxed_rv.is_void()) {
    return 0;
  }

  (void)L;

  // Is it a number ?
  if (boxed_rv.is_arithmetic()) {

  } else if (boxed_rv.is_pointer()) {

  } else if (boxed_rv.is_reference()) {

  }


  return 0;
}

extern "C" int lua_trampoline(lua_State *L) {
//  auto ctx = static_cast<lua::LuaScriptEngine *>(lua_touserdata(L, lua_upvalueindex(1)));
  auto fn = static_cast<ProxyFunction *>(lua_touserdata(L, lua_upvalueindex(2)));
//  auto fn_name = lua_tolstring(L, lua_upvalueindex(3), nullptr);

  if (!fn) {
    // oops!
    return 0;
  }

  auto args_count = lua_gettop(L);
  //  std::cout << "Called "
  //            << fn_name << ". has "
  //            << fn->parameter_count() << " parameters. got "
  //            << args_count << " from script";

  // Count arguments
  if (args_count != fn->parameter_count()) {
    std::cerr << "Wrong number of arguments\n";
    return 0;
  }

  //
  if (fn->parameter_count() > 0) {
    std::vector<BoxedValue> values;
    values.reserve(static_cast<size_t>(fn->parameter_count()));

    const auto &params = fn->parameters();

    for (int i = 0; i < fn->parameter_count(); i++) {
      values.emplace_back(lua_to_boxed_value(L, i + 1, params.at(static_cast<size_t>(i))));
    }

    return lua_trampoline_handle_return(L, fn->operator()(FunctionParams(values)));
  }

  return lua_trampoline_handle_return(L, fn->operator()(FunctionParams()));
}
}// namespace

namespace e00::impl::scripting::lua {
LuaScriptEngine::LuaScriptEngine()
  : ScriptEngine(),
    _state(luaL_newstate()) {
  luaL_openlibs(_state);
}

LuaScriptEngine::~LuaScriptEngine() {
  lua_close(_state);
}

bool LuaScriptEngine::valid_fn_name(const std::string &fn_name) {
  (void)fn_name;
  return true;
}

void LuaScriptEngine::add_function(const std::string &fn_name, const std::shared_ptr<scripting::ProxyFunction> &fn) {
  _registered_fns.insert(std::make_pair(fn_name, fn));

  lua_pushlightuserdata(_state, this);
  lua_pushlightuserdata(_state, fn.get());
  lua_pushstring(_state, fn_name.c_str());
  lua_pushcclosure(_state, &lua_trampoline, 3);
  lua_setglobal(_state, fn_name.c_str());
}

std::error_code LuaScriptEngine::parse(const std::string &code) {
  luaL_loadstring(_state, code.c_str());
  lua_pcall(_state, 0, 0, 0);

  return {};
}


}// namespace e00::impl::scripting::lua
