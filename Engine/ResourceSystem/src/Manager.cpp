#include <Engine/ResourceSystem/Manager.hpp>
#include <Engine/ResourceSystem/Error.hpp>

#include <Engine/Configuration/Parser.hpp>

#include "LoaderMap.hpp"
#include "PackIndexMaker.hpp"
#include "ResourcePackConfiguration.hpp"

namespace {
constexpr auto PACK_CONFIGURATION_FN = "pack.ini";

e00::resource::Info load_info_from_pack(const std::unique_ptr<e00::sys::StreamFactory> &pack, const e00::resource::PackIndex::Locator &locator) {
  // PORTING: IF YOUR PLATFORM NEED FULL PATH (XBOX) PLEASE IMPLEMENT IN THE StreamFactory SUBCLASS
  if (auto cfg_stream = pack->open_stream(PACK_CONFIGURATION_FN)) {
    // We got the pack, parse it and find out about the resource
    e00::resource::impl::ResourcePackConfiguration listener(locator.name, locator.type);
    if (auto parse_ec = e00::cfg::parse(cfg_stream, &listener)) {
      // We have an error :(
      return { locator.name, parse_ec };
    }

    return listener.make_info();
  }

  return e00::resource::Info(locator.name);
}

std::unique_ptr<e00::resource::Resource> try_load_from_loaders(const std::unique_ptr<e00::sys::StreamFactory> &stream_factory, const e00::resource::Info &info, std::error_code &load_error) {

  switch (info.type()) {
    case e00::resource::info::Type::unknown:
      load_error = e00::resource::make_error_code(e00::resource::ResourceError::invalid_resource_type);
      return nullptr;
    case e00::resource::info::Type::map: return e00::resource::loader::load_map(stream_factory, info, load_error);
    case e00::resource::info::Type::actor: break;
    case e00::resource::info::Type::portal: break;
    case e00::resource::info::Type::sprite: break;
    case e00::resource::info::Type::tileset: break;
    case e00::resource::info::Type::bitmap: break;
  }

  // We didn't find any loaders that could load this resource :(
  return nullptr;
}
}// namespace

namespace e00::resource {

Manager::Manager()
  : _logger("ResourceManager") {
}

Manager::~Manager() {}

Manager::PackLoadResult Manager::add_pack(std::unique_ptr<sys::StreamFactory> &&pack_loader) {
  // Make sure we don't try to access NULL, that would be a shame...
  if (!pack_loader) {
    // Err, wat?
    _logger.error(SourceLocation(), "Asked to add a null pack, aborting");
    return { {}, make_error_code(ResourceError::invalid_pack) };
  }

  // PORTING: IF YOUR PLATFORM NEED FULL PATH (XBOX) PLEASE IMPLEMENT IN THE StreamFactory SUBCLASS
  if (auto cfg_stream = pack_loader->open_stream(PACK_CONFIGURATION_FN)) {
    PackIndex index;
    impl::PackIndexMaker listener(index);

    // Try and parse the INI
    if (auto ec = cfg::parse(cfg_stream, &listener)) {
      _logger.error(SourceLocation(), "Failed to load pack configuration: {}", ec.message());
      return { index.id, ec };
    }

    index.pack = std::move(pack_loader);

    // Show some info
    _logger.info(SourceLocation(), "Pack \"{}\" ({}) loaded {} resources...", index.name, index.id, index.resource_index.size());

    // Add it the the knowledge base
    _resources_index.emplace_back(std::move(index));
    return { index.id, {} };
  }

  return { {}, make_error_code(ResourceError::invalid_pack) };
}

std::unique_ptr<Resource> Manager::load_resource(const std::string &resource_name, info::Type type) {
  // Find the pack that would contain this resource
  for (const auto &pack : _resources_index) {
    if (const auto locator = pack.find(resource_name, type)) {
      _logger.debug(SourceLocation(), "Resource {} ({}) was found in pack {}", resource_name, type, pack.name);
      // If this pack contains this resource, try to load the info from it (it should succeed as we already parsed this file)
      if (const auto info = load_info_from_pack(pack.pack, locator)) {
        // We now need to find a loader that will accept this info an load it
        // TODO: Do something with this error
        std::error_code load_error;

        if (auto resource = try_load_from_loaders(pack.pack, info, load_error)) {
          return resource;
        }

        _logger.error(SourceLocation(),
          "Failed to load resource {} from pack {}: {}",
          resource_name,
          pack.name,
          load_error.message());
      }
    }
  }

  _logger.error(SourceLocation(), "Unable to find resource {} ({})", resource_name, type);

  // Nothing was found :(
  return nullptr;
}


std::unique_ptr<Stream> Manager::get_script_for_pack_id(const std::string &pack_id) {
  for (const auto &pack : _resources_index) {
    if (pack.id == pack_id && !pack.script.empty()) {
      return pack.pack->open_stream(pack.script);
    }
  }
  return {};
}

void Manager::add_reference(const ResourcePtrData *data) {
  (void)data;
}

void Manager::remove_reference(const ResourcePtrData *data) {
  (void)data;
}


}// namespace e00::resource
