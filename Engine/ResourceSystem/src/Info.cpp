#include <new>
#include <cstring>
#include <utility>

#include "Info.hpp"

namespace e00::resource {
void Info::destroy_value() {
  switch (_type) {
    case info::Type::unknown: break;
    case info::Type::map: (&_data.map)->Map::~Map(); break;
    case info::Type::actor: (&_data.actor)->Actor::~Actor(); break;
    case info::Type::portal: (&_data.portal)->Portal::~Portal(); break;
    case info::Type::sprite: (&_data.sprite)->Sprite::~Sprite(); break;
    case info::Type::tileset: (&_data.tileset)->Tileset::~Tileset(); break;
    case info::Type::bitmap: (&_data.bitmap)->Bitmap::~Bitmap(); break;
  }
  _type = info::Type::unknown;
  _data.dummy = 0;
}

Info::Info(const Info &other) : _type(info::Type::unknown) {
  copy(other);
}

Info::Info(Info &&other) noexcept : _type(info::Type::unknown) {
  move(std::move(other));
}

Info::Info(std::string id_, info::Type type_) : _id(std::move(id_)), _type(type_) {
  switch (_type) {
    case info::Type::unknown: _data.dummy = 0; break;
    case info::Type::map: new (&_data.map) info::Map(); break;
    case info::Type::actor: new (&_data.actor) info::Actor(); break;
    case info::Type::portal: new (&_data.portal) info::Portal(); break;
    case info::Type::sprite: new (&_data.sprite) info::Sprite(); break;
    case info::Type::tileset: new (&_data.tileset) info::Tileset(); break;
    case info::Type::bitmap: new (&_data.bitmap) info::Bitmap(); break;
  }
}

Info &Info::operator=(const Info &rhs) {
  if (&rhs != this) {
    copy(rhs);
  }

  return *this;
}

void Info::move(Info &&other) noexcept {
  destroy_value();

  _id = std::move(other._id);
  _type = other._type;

  switch (_type) {
    case info::Type::unknown: break;
    case info::Type::map: new (&_data.map) info::Map(std::move(other._data.map)); break;
    case info::Type::actor: new (&_data.actor) info::Actor(std::move(other._data.actor)); break;
    case info::Type::portal: new (&_data.portal) info::Portal(std::move(other._data.portal)); break;
    case info::Type::sprite: new (&_data.sprite) info::Sprite(std::move(other._data.sprite)); break;
    case info::Type::tileset: new (&_data.tileset) info::Tileset(std::move(other._data.tileset)); break;
    case info::Type::bitmap: new (&_data.bitmap) info::Bitmap(std::move(other._data.bitmap)); break;
  }

  other.destroy_value();
}

void Info::copy(const Info &rhs) {
  destroy_value();

  _id = rhs._id;
  _type = rhs._type;

  switch (_type) {
    case info::Type::unknown: break;
    case info::Type::map: new (&_data.map) info::Map(rhs._data.map); break;
    case info::Type::actor: new (&_data.actor) info::Actor(rhs._data.actor); break;
    case info::Type::portal: new (&_data.portal) info::Portal(rhs._data.portal); break;
    case info::Type::sprite: new (&_data.sprite) info::Sprite(rhs._data.sprite); break;
    case info::Type::tileset: new (&_data.tileset) info::Tileset(rhs._data.tileset); break;
    case info::Type::bitmap: new (&_data.bitmap) info::Bitmap(rhs._data.bitmap); break;
  }
}
}// namespace e00::resource
