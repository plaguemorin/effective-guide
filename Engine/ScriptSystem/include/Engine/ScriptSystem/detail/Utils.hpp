#pragma once

#include <memory>
#include <tuple>

namespace e00::impl::scripting::detail {
template<typename B, typename D, typename... Arg>
std::unique_ptr<B> make_unique_base(Arg &&... arg) {
  return std::unique_ptr<B>(static_cast<B *>(new D(std::forward<Arg>(arg)...)));
}
}// namespace e00::impl::scripting::detail
