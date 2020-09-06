#include "LoaderMap.hpp"

#include <Engine/string_view_utils.hpp>
#include <Logger/Logger.hpp>

namespace {
class TextMapParser {
  e00::resource::map_position_t x, y;
  e00::resource::Map *map;

public:
  explicit TextMapParser(e00::resource::Map *map_) : x(0), y(0), map(map_) {}

  void operator()(const std::string_view &item) {
    if (item.empty()) {
      return;
    }

    uint8_t tile_id = 0;
    e00::stoi_fast(item, tile_id);

    map->set_tile(x, y, tile_id);

    x++;
    if (x >= map->width()) {
      y++;
      x = 0;
    }
  }
};
}// namespace

namespace e00::resource::loader {
std::unique_ptr<Map> load_map(const std::unique_ptr<sys::StreamFactory> &stream_factory, const Info &info, std::error_code &load_error) {
  impl::Logger logger("LoaderMap");

  logger.debug(SourceLocation(), "Loading map {}", info.id());

  // Try to open the data source of the map
  if (auto map_data_stream = stream_factory->open_stream(info.map().data)) {
    std::unique_ptr<Map> theMap(new Map(info.id(), info.map().width, info.map().height));

    // Read tile data
    switch (info.map().data_type) {
      case info::Map::DataType::unknown:
        // we don't know that type
        logger.error(SourceLocation(), "Map as unknown data type");
        return nullptr;

      case info::Map::DataType::text:
        {
          TextMapParser parser(theMap.get());
          while (!map_data_stream->end_of_stream()) {
            const auto line = map_data_stream->read_line();
            execute_comma_separated(line, parser);
          }
        }
        break;
    }

    return theMap;
  } else {
    load_error = std::make_error_code(std::errc::no_such_file_or_directory);
  }

  return nullptr;
}
}// namespace e00::resource::loader
