#pragma once

#include <string>
#include <list>

namespace e00 {
struct Menu {

  struct Item {
    enum class Type {
      action,
      text,
      toggle,
      range
    };

    std::string display_text;
    Type type;
  };

  std::string name;
  std::list<Item> items;
};
}
