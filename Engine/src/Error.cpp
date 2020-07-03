#include "Error.hpp"

namespace {
struct EngineErrCategory : std::error_category {
  const char *name() const noexcept override;
  std::string message(int ev) const override;
};

const char *EngineErrCategory::name() const noexcept {
  return "engine";
}

std::string EngineErrCategory::message(int ev) const {
  switch (static_cast<e00::EngineErrorCode>(ev)) {
    case e00::EngineErrorCode::not_configured: return "not configured correctly";
    case e00::EngineErrorCode::duplicate_resource: return "duplicate resource";
    case e00::EngineErrorCode::failed_to_initialize_input: return "failed to initialize input system";
    case e00::EngineErrorCode::too_many_input_systems: return "too many input systems";
    case e00::EngineErrorCode::invalid_argument: return "invalid argument";
    case e00::EngineErrorCode::dependent_resource_not_found: return "dependent resource not found";
  }
  return {};
}

const EngineErrCategory engineErrCategory{};
}// namespace

namespace e00 {
std::error_code make_error_code(EngineErrorCode e) {
  return std::error_code(static_cast<int>(e), engineErrCategory);
}
}// namespace e00
