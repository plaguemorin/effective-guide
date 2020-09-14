#include "LuaToNativeTrampoline.hpp"
#include "LuaToBoxedConverter.hpp"
#include "BoxedToLuaConverter.hpp"
#include "TrampolineData.hpp"

using namespace e00::impl::scripting;

extern "C" int lua_trampoline(lua_State *L) {
  auto ctx = static_cast<lua::TrampolineData *>(lua_touserdata(L, lua_upvalueindex(1)));

  if (!ctx) {
    // oops!
    return 0;
  }

  auto args_count = lua_gettop(L);
  //  std::cout << "Called "
  //            << fn_name << ". has "
  //            << fn->parameter_count() << " parameters. got "
  //            << args_count << " from script";

  // Count arguments
  if (!ctx->fn()->is_varargs() && args_count != ctx->fn()->parameter_count()) {
    return 0;
  }

  //
  if (args_count > 0) {
    std::vector<BoxedValue> values;
    values.reserve(static_cast<size_t>(args_count));

    for (int i = 0; i < args_count; i++) {
      values.emplace_back(lua_to_boxed_value(L, i + 1, ctx->fn()->parameter(static_cast<size_t>(i))));
    }

    return boxed_to_lua(L, ctx->fn()->call(FunctionParams(values)));
  }

  return boxed_to_lua(L, ctx->fn()->call(FunctionParams()));
}
