#include "ResourceManager.hpp"

#include "INIReader/StreamIniReader.hpp"
#include "Loaders/ResourcePackConfiguration.hpp"

namespace e00::impl {
ResourceManager::ResourceManager(sys::ResourceStreamLoader *stream_loader)
  : _logger("ResourceManager"),
    _loader(stream_loader) {
}

ResourceManager::~ResourceManager() = default;

std::error_code ResourceManager::parse_configuration(const std::string &file_name, cfg::ParserListener *listener) {
  if (_loader == nullptr) {
    // Error: no loader
    _logger.error(SourceLocation(), "No loader to open configuration file \"{}\"", file_name);

    // TODO: replace with proper error
    return make_error_code(EngineErrorCode::not_configured);
  }

  // PORTING: IF YOUR PLATFORM NEED FULL PATH (XBOX) PLEASE IMPLEMENT IN THE ResourceStreamLoader SUBCLASS
  if (auto cfg_stream = _loader->load_file(file_name)) {
    _logger.info(SourceLocation(), "Parsing configuration file \"{}\"", file_name);
    return impl::parse_ini(cfg_stream, listener);
  }

  _logger.error(SourceLocation(), "Stream loader was unable to open configuration file \"{}\"", file_name);
  return std::make_error_code(std::errc::no_such_file_or_directory);
}

std::unique_ptr<resource::Resource> ResourceManager::load_resource(const std::string &resource_name, type_t type) {
  auto it = _known_resources.find(resource_name);
  if (it == _known_resources.end()) {
    _logger.error(SourceLocation(), "Resource with name {} is not known", resource_name);
    return nullptr;
  }

  (void)type;

  return nullptr;
}

std::error_code ResourceManager::add_pack(sys::ResourceStreamLoader *pack_loader) {
  // Make sure we don't try to access NULL, that would be a shame...
  if (pack_loader == nullptr) {
    // Err, wat?
    _logger.error(SourceLocation(), "Asked to add a null pack, aborting");
    return make_error_code(EngineErrorCode::invalid_argument);
  }

  // PORTING: IF YOUR PLATFORM NEED FULL PATH (XBOX) PLEASE IMPLEMENT IN THE ResourceStreamLoader SUBCLASS
  if (auto cfg_stream = pack_loader->load_file("pack.ini")) {
    ResourcePackConfiguration listener;

    // Try and parse the INI
    if (auto ec = impl::parse_ini(cfg_stream, &listener)) {
      _logger.error(SourceLocation(), "Failed to load pack configuration (pack.ini): {}", ec.message());
      return ec;
    }

    // Print some info about the pack
    _logger.info(SourceLocation(), "Loaded Pack {} ({})", listener._name, listener._id);

    // Dump info about configs
    for (const auto &config : listener._configurations) {
      _logger.info(SourceLocation(), "Known configuration about {} ({})", config.id(), config.type());
      switch (config.type()) {
        case ResourceConfiguration::ConfigurationType::unknown: _logger.info(SourceLocation(), "Unknown resource type: Config is invalid"); break;
        case ResourceConfiguration::ConfigurationType::map:
          _logger.info(SourceLocation(), "Map: {}x{}, Data: {} ({}), Tileset: {}", (int)config.map().width, (int)config.map().height, config.map().data, config.map().data_type, config.map().tileset);
          break;
        case ResourceConfiguration::ConfigurationType::actor:
          _logger.info(SourceLocation(), "Actor on {} at {}, {}", config.actor().position.map_name, config.actor().position.position_x, config.actor().position.position_y);
          break;
        case ResourceConfiguration::ConfigurationType::portal:
          break;
      }
    }
  }

  return {};
}
}// namespace e00::impl
