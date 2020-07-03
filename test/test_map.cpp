
#include <cstdio>
#include <ostream>
#include <fstream>
#include <memory>

#include <Engine.hpp>
#include <Engine/MapData.hpp>
#include <Engine/TileSet.hpp>

#include <Engine/ThingSystem/Actor.hpp>
#include <Engine/ThingSystem/Manager.hpp>
#include <Engine/ThingSystem/Thing.hpp>

#include <Engine/Resource/Manager.hpp>

#include <Engine/Resource/Bitmap.hpp>

class BMPSurface : public Bitmap {
  std::vector<uint8_t> _data;

  struct ColorMap {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    ColorMap& operator=(uint32_t color) {
      blue = color & 0xFFu;
      green = (color >> 8u) & 0xFFu;
      red = (color >> 16u) & 0xFFu;
      return *this;
    }
  };

  ColorMap map[0xFF];

  void write_pixel(uint8_t color, std::ostream& of) {
    const ColorMap& color_data = map[color];
    of.write((const char *) &color_data.red, sizeof(uint8_t));
    of.write((const char *) &color_data.green, sizeof(uint8_t));
    of.write((const char *) &color_data.blue, sizeof(uint8_t));
  }

public:
  BMPSurface() : Bitmap({320, 240}, BitmapBackingType::HARDWARE) {
    _data.resize(size().total(), 0);
    map[0] = 0x140c1c;
    map[1] = 0x442434;
    map[2] = 0x30346d;
    map[3] = 0x4e4a4e;
    map[4] = 0x854c30;
    map[5] = 0x346524;
    map[6] = 0xd04648;
    map[7] = 0x757161;
    map[8] = 0x597dce;
    map[9] = 0xd27d2c;
    map[10] = 0x8595a1;
    map[11] = 0x6daa2c;
    map[12] = 0xd2aa99;
    map[13] = 0x6dc2ca;
    map[14] = 0xdad45e;
    map[15] = 0xdeeed6;
  }

  ~BMPSurface() = default;

  void set_pixel(const Vec2I& point, uint8_t color) override {
    _data.at(point.y * size().x + point.x) = color;
  }

  uint8_t get_pixel(const Vec2I& point) const override {
    return _data.at(point.y * size().x + point.x);
  }

  void write(std::ostream& of) {
    // BMP Header (uint16_t)
    uint16_t header = 0x4D42;
    of.write((const char *) &header, sizeof(header));

    // File size (uint32_t)
    uint32_t file_size = (_data.size() * 3) + 54;
    of.write((const char *) &file_size, sizeof(file_size));

    // 2x Reserved (uint16_t)
    of.put(0);
    of.put(0);
    of.put(0);
    of.put(0);

    // Offset data (uint32_t)
// 16 + 32 + 16 + 16 + 32 + (10 * 32)
    uint32_t offset_data{54};
    of.write((const char *) &offset_data, sizeof(offset_data));
    // uint32_t size_of_this_header (10 * 32)
    uint32_t hdr_size{40};
    of.write((const char *) &hdr_size, sizeof(hdr_size));

    int32_t width{size().x};                      // width of bitmap in pixels
    int32_t height{size().y};                     // width of bitmap in pixels
//       (if positive, bottom-up, with origin in lower left corner)
//       (if negative, top-down, with origin in upper left corner)

    of.write((const char *) &width, sizeof(width));
    of.write((const char *) &height, sizeof(height));

    uint16_t planes{1};
    of.write((const char *) &planes, sizeof(planes));

    uint16_t bit_count{24};
    of.write((const char *) &bit_count, sizeof(bit_count));

    uint32_t compression{0};
    of.write((const char *) &compression, sizeof(compression));
    uint32_t size_image{0};
    of.write((const char *) &size_image, sizeof(size_image));
    int32_t x_pixels_per_meter{0};
    of.write((const char *) &x_pixels_per_meter, sizeof(x_pixels_per_meter));
    int32_t y_pixels_per_meter{0};
    of.write((const char *) &y_pixels_per_meter, sizeof(y_pixels_per_meter));
    uint32_t colors_used{0};
    of.write((const char *) &colors_used, sizeof(colors_used));
    uint32_t colors_important{0};
    of.write((const char *) &colors_important, sizeof(colors_important));

    for (const auto& i : _data) {
      write_pixel(i, of);
    }

  }
};

class SimpleLoader : public resource::Loader {
  class Reader : public resource::Loader::ResourceStream {
    FILE *_fp;

  public:
    explicit Reader(FILE *fp) : _fp(fp) {

    }

    ~Reader() override {
      fclose(_fp);
    }

    bool seek(uint32_t pos) override {
      fseek(_fp, pos, SEEK_SET);
      return true;
    }

    uint32_t file_size() override {
      auto current = ftell(_fp);
      fseek(_fp, 0, SEEK_END);
      auto size = ftell(_fp);
      fseek(_fp, current, SEEK_SET);
      return size;
    }

    size_t read(size_t max_size, void *data) override {
      return fread(data, max_size, 1, _fp);
    }
  };

  std::string filename_of_resource(resource::resource_id_t id) const {
    return std::to_string(id) + ".bmp";
  }

public:
  ~SimpleLoader() override = default;

  bool has_resource(resource::resource_id_t id) override {
    const auto file_name = filename_of_resource(id);
    printf("Asking for resource_id %s\n", file_name.c_str());
    FILE *tmp = std::fopen(file_name.c_str(), "rb");
    if (!tmp)
      return false;

    fclose(tmp);
    return true;
  }

  resource::resource_file_type_t resource_file_type(resource::resource_id_t id) override {
    if (has_resource(id)) {
      return resource::to_type('B', 'M', 'P');
    }

    return resource::invalid_type;
  }

  std::unique_ptr<ResourceStream> read_resource(resource::resource_id_t id) override {
    const auto file_name = filename_of_resource(id);
    return std::unique_ptr<Reader>(new Reader(std::fopen(file_name.c_str(), "rb")));
  }
};

int main() {



  printf("Testing Maps...\n");
  Tileset aTileset(4, 24, 24);
  aTileset.at(0).texture_id = resource::to_id("AAAA");
  aTileset.at(1).texture_id = resource::to_id("BBBB");
  aTileset.at(2).texture_id = resource::to_id("CCCC");
  aTileset.at(3).texture_id = resource::to_id("DDDD");

  printf("Tileset generated...\n");
  auto aMap = MapData::create_from_data(8,           // 8 width
                                    0, 1, 2, 3, 3, 2, 1, 0, //
                                    3, 2, 1, 0, 0, 1, 2, 3, //
                                    0, 1, 2, 3, 0, 0, 0, 0, //
                                    0, 0, 0, 0, 0, 1, 2, 3);

/*
  thing::Thing aThing;
  aThing.set_position(0, 0);

  thing::Actor aActor;
  aActor.set_position(2, 2);

  thing::Manager manager;
  manager.register_object(&aThing);
  manager.register_object(&aActor);
  */

}
