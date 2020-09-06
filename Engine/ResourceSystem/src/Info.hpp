#pragma once

#include <system_error>

#include <Engine/ResourceSystem/Info/Type.hpp>
#include <Engine/ResourceSystem/Info/Map.hpp>
#include <Engine/ResourceSystem/Info/Actor.hpp>
#include <Engine/ResourceSystem/Info/Portal.hpp>
#include <Engine/ResourceSystem/Info/Sprite.hpp>
#include <Engine/ResourceSystem/Info/Tileset.hpp>
#include <Engine/ResourceSystem/Info/Bitmap.hpp>

namespace e00::resource {
class Info {
  union Data {
    char dummy;
    info::Map map;
    info::Actor actor;
    info::Portal portal;
    info::Sprite sprite;
    info::Tileset tileset;
    info::Bitmap bitmap;

    Data() : dummy(0) {}
    ~Data() {}
  };
/*
  template<info::Type T>
  struct info_to_type {};
  template<>
  struct info_to_type<info::Type::map> {
    typedef info::Map type;
    static type &get(Data &d) { return d.map; }
    static const type &get(const Data &d) { return d.map; }
  };
  template<>
  struct info_to_type<info::Type::actor> {
    typedef info::Actor type;
    static type &get(Data &d) { return d.actor; }
    static const type &get(const Data &d) { return d.actor; }
  };
  template<>
  struct info_to_type<info::Type::portal> {
    typedef info::Portal type;
    static type &get(Data &d) { return d.portal; }
    static const type &get(const Data &d) { return d.portal; }
  };
  template<>
  struct info_to_type<info::Type::sprite> {
    typedef info::Sprite type;
    static type &get(Data &d) { return d.sprite; }
    static const type &get(const Data &d) { return d.sprite; }
  };
  template<>
  struct info_to_type<info::Type::tileset> {
    typedef info::Tileset type;
    static type &get(Data &d) { return d.tileset; }
    static const type &get(const Data &d) { return d.tileset; }
  };*/

  std::string _id;
  info::Type _type;
  Data _data;
  std::error_code _error;

  void destroy_value();

  void move(Info &&other) noexcept;

  void copy(const Info &rhs);

public:
  ~Info() { destroy_value(); }

  explicit Info(std::string id_)
    : _id(std::move(id_)),
      _type(info::Type::unknown) {
    _data.dummy = 0;
  }

  Info(std::string id_, std::error_code error_)
    : _id(std::move(id_)),
      _type(info::Type::unknown),
      _error(error_) {
    _data.dummy = 0;
  }

  Info(const Info &other);

  Info(Info &&other) noexcept;

  Info(std::string id_, info::Type type_);

  Info &operator=(Info &&rhs) noexcept {
    if (&rhs != this) {
      move(std::move(rhs));
    }

    return *this;
  }

  Info &operator=(const Info &);

  [[nodiscard]] const std::string &id() const { return _id; }

  [[nodiscard]] info::Type type() const { return _type; }

  explicit operator bool() const { return !_error && _type != info::Type::unknown; }

  [[nodiscard]] const std::error_code &error() const { return _error; }
/*
  template<info::Type T>
  inline const typename info_to_type<T>::type &get() const { return info_to_type<T>::get(_data); }
  template<info::Type T>
  inline typename info_to_type<T>::type &get() { return info_to_type<T>::get(_data); }*/

  info::Map &map() { return _data.map; }
  info::Actor &actor() { return _data.actor; }
  info::Portal &portal() { return _data.portal; }
  info::Sprite &sprite() { return _data.sprite; }
  info::Tileset &tileset() { return _data.tileset; }
  info::Bitmap &bitmap() { return _data.bitmap; }

  [[nodiscard]] const info::Map &map() const { return _data.map; }
  [[nodiscard]] const info::Actor &actor() const { return _data.actor; }
  [[nodiscard]] const info::Portal &portal() const { return _data.portal; }
  [[nodiscard]] const info::Sprite &sprite() const { return _data.sprite; }
  [[nodiscard]] const info::Tileset &tileset() const { return _data.tileset; }
  [[nodiscard]] const info::Bitmap &bitmap() const { return _data.bitmap; }
};


}// namespace e00::resource
