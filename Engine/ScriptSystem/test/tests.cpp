#include <catch2/catch.hpp>

#include <iostream>
#include <Engine/ScriptSystem/ScriptEngine.hpp>

namespace {
bool returns_true() {
  return true;
}

bool accepts_an_int(int i) {
  return i == 3;
}
}// namespace

TEST_CASE("Script System initializes") {
  auto script = e00::impl::ScriptEngine::Create();


  std::cout << "Script engine: " << script->engine_name() << "\n";
}

TEST_CASE("Can log from script") {
  auto script = e00::impl::ScriptEngine::Create();
  script->parse("\nprint(\"Hello World\")\n");
}

TEST_CASE("Can call free functions") {
  auto script = e00::impl::ScriptEngine::Create();

  script->register_function("returns_true", &returns_true);
  script->parse("\nreturns_true()\n");
}

TEST_CASE("Passing an int to a function") {
  auto script = e00::impl::ScriptEngine::Create();

  script->register_function("accepts_an_int", &accepts_an_int);
  script->parse("\naccepts_an_int(3)\n");
}
