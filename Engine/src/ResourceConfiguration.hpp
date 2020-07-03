#pragma once

#include <Engine/CommonTypes.hpp>

namespace e00::impl {
struct ResourceConfiguration {
  struct MapPosition {
    std::string map_name;
    uint16_t position_x;
    uint16_t position_y;
  };

  struct Map {
    enum class DataType {
      unknown,
      text
    };

    std::string tileset;
    std::string data;
    DataType data_type;
    uint16_t width;
    uint16_t height;
  };

  struct Actor {
    enum class EssentialStatus {
      none,
      player_killable,// "Protected"
      essential,
    };

    enum class RelationshipToPlayer {
      neutral,
      ally,
      enemy,
    };

    enum class Assistance {
      none,
      player,
      player_and_ally,
    };

    std::string sprite;
    MapPosition position;
    EssentialStatus essential_status;
    RelationshipToPlayer relationship_to_player;
    uint16_t aggression_level;
    Assistance assistance;
    std::string ai;
  };

  struct Portal {
    MapPosition from_position;
    MapPosition to_position;

    uint16_t parent_marker_radius;
    bool parent_discoverable;
    std::string from_name_string_id;
  };

  enum class ConfigurationType {
    unknown,
    map,
    actor,
    portal
  };

  union ConfigurationData {
    char dummy;
    Map map;
    Actor actor;
    Portal portal;

    ConfigurationData() : dummy(0) {}
    ~ConfigurationData() {}
  };

  const std::string& id() const { return _id; }
  ConfigurationType type() const { return _type; }

  Map &map() { return data.map; }
  Actor& actor() { return data.actor; }
  Portal& portal() { return data.portal; }

  const Map &map() const { return data.map; }
  const Actor& actor() const { return data.actor; }
  const Portal& portal() const { return data.portal; }

private:
  std::string _id;
  ConfigurationType _type;
  ConfigurationData data;

  void destroy_value() {
    switch (_type) {
      case ConfigurationType::unknown: data.dummy = 0; break;
      case ConfigurationType::map: (&data.map)->Map::~Map(); break;
      case ConfigurationType::actor: (&data.actor)->Actor::~Actor(); break;
      case ConfigurationType::portal: (&data.portal)->Portal::~Portal(); break;
    }
    _type = ConfigurationType::unknown;
  }

public:
  ResourceConfiguration(std::string id_, ConfigurationType type_) : _id(std::move(id_)), _type(type_) {
    switch (_type) {
      case ConfigurationType::unknown: data.dummy = 0; break;
      case ConfigurationType::map: new (&data.map) Map{}; break;
      case ConfigurationType::actor: new (&data.actor) Actor{}; break;
      case ConfigurationType::portal: new (&data.portal) Portal{}; break;
    }
  }

  ResourceConfiguration(ResourceConfiguration &&rhs) : _id(std::move(rhs._id)), _type(rhs._type) {
    rhs._type = ConfigurationType::unknown;
    switch (_type) {
      case ConfigurationType::unknown: data.dummy = 0; break;
      case ConfigurationType::map: new (&data.map) Map{std::move(rhs.data.map)}; break;
      case ConfigurationType::actor: new (&data.actor) Actor{std::move(rhs.data.actor)}; break;
      case ConfigurationType::portal: new (&data.portal) Portal{std::move(rhs.data.portal)}; break;
    }
  }

  ~ResourceConfiguration() {
    destroy_value();
  }
};
}// namespace e00::impl
