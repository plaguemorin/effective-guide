#include <Engine/string_view_utils.hpp>

namespace {
struct ConversionErrCategory : std::error_category {
  const char *name() const noexcept override;
  std::string message(int ev) const override;
};

const char *ConversionErrCategory::name() const noexcept {
  return "conversion";
}

std::string ConversionErrCategory::message(int ev) const {
  switch (static_cast<e00::detail::ConversionErrc>(ev)) {
    case e00::detail::ConversionErrc::NegativeNotAllowed: return "negative number not allowed";
    case e00::detail::ConversionErrc::BadCharacter: return "bad character";
    case e00::detail::ConversionErrc::StringTooBig: return "number too big";
  }
  return {};
}

const ConversionErrCategory conversionErrcCategory{};
}// namespace

namespace e00::detail {
std::error_code make_error_code(ConversionErrc e) {
  return { static_cast<int>(e), conversionErrcCategory };
}
}// namespace detail
