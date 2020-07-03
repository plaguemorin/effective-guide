#pragma once

#include <string_view>

namespace e00::cfg {
struct Entry {
  std::string_view section;
  std::string_view name;
  std::string_view value;
};
}
