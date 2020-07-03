#include <catch2/catch.hpp>

#include <iostream>

#include <Engine.hpp>
#include <Engine/Math.hpp>
#include <Engine/System/LoggerSink.hpp>
#include "Logger.hpp"

double ColorCompare_REF(int r1, int g1, int b1, int r2, int g2, int b2) {
  double luma1 = (r1 * 299 + g1 * 587 + b1 * 114) / (255.0 * 1000);
  double luma2 = (r2 * 299 + g2 * 587 + b2 * 114) / (255.0 * 1000);
  double lumadiff = luma1 - luma2;
  double diffR = (r1 - r2) / 255.0, diffG = (g1 - g2) / 255.0, diffB = (b1 - b2) / 255.0;
  return (diffR * diffR * 0.299 + diffG * diffG * 0.587 + diffB * diffB * 0.114) * 0.75
         + lumadiff * lumadiff;
}


class MemorySink : public e00::sys::LoggerSink {
public:
  void info(const std::string_view &string) override {
    std::cout << string;
  }

  void error(const std::string_view &string) override {
    std::cerr << string;
  }
};


TEST_CASE("Color compare matches reference") {
  e00::Color red(255, 0, 0);

}

TEST_CASE("Engine Starts In Correct State") {
  e00::Engine engine;
  
}


TEST_CASE("Logger format") {
  MemorySink sink;

  e00::impl::Logger logger("Fred");
  logger.add_sink(&sink);

  logger.log(source_location(), Logger::lvl::info, "Fred {}", 45, 34, "allo");

}
