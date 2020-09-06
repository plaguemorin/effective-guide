#include <Engine/ResourceSystem/Error.hpp>

namespace {
struct ResourceErrorCategory : std::error_category {
  const char *name() const noexcept override {
    return "resources";
  }
  std::string message(int i) const override {
    switch (static_cast<e00::resource::ResourceError>(i)) {
      case e00::resource::ResourceError::invalid_pack: return "invalid resource pack";
      case e00::resource::ResourceError::invalid_resource_type: return "invalid resource type";
    }
    return {};
  }
};

const ResourceErrorCategory resErrCategory{};
}

namespace e00::resource {
std::error_code make_error_code(ResourceError error) {
  return std::error_code(static_cast<int>(error), resErrCategory);
}
}
