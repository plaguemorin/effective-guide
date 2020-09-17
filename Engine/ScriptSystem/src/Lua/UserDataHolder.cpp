#include "UserDataHolder.hpp"

namespace e00::impl::scripting::lua {
UserDataHolder::UserDataHolder() : _magic(MAGIC) {
}

UserDataHolder::UserDataHolder(BoxedValue value)
  : _magic(MAGIC), _value(std::move(value)) {
}

UserDataHolder::UserDataHolder(BoxedValue &&value)
  : _magic(MAGIC), _value(std::move(value)) {}

int UserDataHolder::LuaGc(lua_State *L) {
  const auto data_holder = static_cast<UserDataHolder **>(luaL_checkudata(L, 1, UserDataHolder::MetaTableName));
  if (data_holder && *data_holder) {
    delete *data_holder;
    *data_holder = nullptr;
  }

  return 0;
}

}// namespace e00::impl::scripting::lua
