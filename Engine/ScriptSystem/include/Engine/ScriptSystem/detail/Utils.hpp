#pragma once

#include <memory>
#include <tuple>

namespace e00::impl::scripting::detail {
template<typename B, typename D, typename... Arg>
std::shared_ptr<B> make_shared_base(Arg &&... arg) {
  return std::shared_ptr<B>(static_cast<B *>(new D(std::forward<Arg>(arg)...)));
}
}// namespace e00::impl::scripting::detail
