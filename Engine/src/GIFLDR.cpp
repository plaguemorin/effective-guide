#include "GIFLDR.hpp"
#include <chrono>

namespace {
struct GifLoaderErrCategory : std::error_category {
  const char *name() const noexcept override { return "engine"; }
  std::string message(int ev) const override {
    switch (static_cast<e00::impl::GifLoaderErrorCode>(ev)) {
      case e00::impl::GifLoaderErrorCode::not_valid_gif: return "not a valid gif";
    }
    return {};
  }
};

const GifLoaderErrCategory gif_loader_err_category{};

struct GifHeader {
  // Header
  uint8_t signature[3]; /* Header Signature (always "GIF") */
  uint8_t version[3]; /* GIF format version("87a" or "89a") */
  // Logical Screen Descriptor
  uint16_t screen_width; /* Width of Display Screen in Pixels */
  uint16_t screen_height; /* Height of Display Screen in Pixels */
  uint8_t packed; /* GCT follows for 256 colors with resolution 3×8 bits/primary;
                      the lowest 3 bits represent the bit depth minus 1,
                      the highest true bit means that the GCT is present */
  uint8_t background_color; /* Background Color Index */
  uint8_t aspect_ratio; /* Pixel Aspect Ratio */
};

struct GifGraphicControlExtensionData {
  /*
    <Packed Fields>	=	Reserved	3 Bits
      Disposal Method	3 Bits
      User Input Flag	1 Bit
      Transparent Color Flag	1 Bit

    Delay Time                    Unsigned
    Transparent Color Index       Byte
---
      Dispose Values :
        0 -	No disposal specified. The decoder is not required to take any action.
        1 -	Do not dispose. The graphic is to be left in place.
        2 -	Restore to background color. The area used by the graphic must be restored to the background color.
        3 -	Restore to previous. The decoder is required to restore the area overwritten by the graphic with what was there prior to rendering the graphic.
        4-7 -	To be defined.
   */
  enum class DisposeMethod {
    not_specified = 0,
    do_not_dispose = 1,
    restore_to_background_color = 2,
    restore_to_previous = 3,
    reserved_4 = 4,
    reserved_5 = 5,
    reserved_6 = 6,
    reserved_7 = 7,
  };


  std::chrono::milliseconds delay;
  uint8_t transparent_color_index;
  DisposeMethod disposal_method;
  bool wait_on_user_input;
  bool has_transparent_color;
};

struct GifColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

/*
the file is divided into segments, each introduced by a 1-byte sentinel:
  An image (introduced by 0x2C, an ASCII comma ',')
  An extension block (introduced by 0x21, an ASCII exclamation point '!')
  The trailer (a single byte of value 0x3B, an ASCII semicolon ';'), which should be the last byte of the file.
*/
struct GifSegment {
  enum class Type {
    invalid,
    image,
    extension_unknown,
    extension_plain_text,
    extension_graphic_control,
    extension_comment,
    extension_application,
    trailer,
  };

  Type type;
  uint8_t size;
};

GifSegment::Type read_gif_segment_type(const std::unique_ptr<e00::Stream> &stream) {
  char seg;

  // Read the segment type
  if (!stream->read(seg)) {
    // Nope outta here if we can't
    return GifSegment::Type::invalid;
  }

  // Do the simple first
  if (seg == ',') return GifSegment::Type::image;
  if (seg == ';') return GifSegment::Type::trailer;

  // If it's an extension, read more to figure out the real type
  if (seg == '!') {
    // Read the real extension type
    uint8_t label;
    if (!stream->read(label)) {
      return GifSegment::Type::invalid;
    }

    switch (label) {
      case 0x01: return GifSegment::Type::extension_plain_text;
      case 0xF9: return GifSegment::Type::extension_graphic_control;
      case 0xFE: return GifSegment::Type::extension_comment;
      case 0xFF: return GifSegment::Type::extension_application;
    }

    return GifSegment::Type::extension_unknown;
  }

  return GifSegment::Type::invalid;
}

GifSegment read_next_segment_header(const std::unique_ptr<e00::Stream> &stream) {
  // Make sure we have something to read
  if (stream->max_read() <= 0)
    return GifSegment{ GifSegment::Type::invalid, 0 };

  // Read the type
  GifSegment::Type segment_type = read_gif_segment_type(stream);
  if (segment_type == GifSegment::Type::invalid) {
    // If it's not a segment, abort right away
    return GifSegment{ GifSegment::Type::invalid, 0 };
  }

  // If it's the trailer, there's no length, return immediately
  if (segment_type == GifSegment::Type::trailer) {
    return GifSegment{ GifSegment::Type::trailer, 0 };
  }

  // Since we have a valid type, the next byte is always the length
  uint8_t len;
  if (!stream->read(len)) {
    return GifSegment{ GifSegment::Type::invalid, 0 };
  }

  // Return what we have
  return GifSegment{ segment_type, len };
}

GifHeader read_gif_header(const std::unique_ptr<e00::Stream> &stream) {
  GifHeader header;

  stream->read(3, header.signature);
  stream->read(3, header.version);
  stream->read(header.screen_width);
  stream->read(header.screen_height);
  stream->read(header.packed);
  stream->read(header.background_color);
  stream->read(header.aspect_ratio);

  return header;
}

GifColor read_gif_color(const std::unique_ptr<e00::Stream> &stream) {
  GifColor color;

  stream->read(color.r);
  stream->read(color.g);
  stream->read(color.b);

  return color;
}

GifGraphicControlExtensionData read_gce(const std::unique_ptr<e00::Stream> &stream) {
  uint8_t r;
  if (!stream->read(r)) {
    return {};
  }

  GifGraphicControlExtensionData value;

  const auto raw_dispose_method = (r >> 2) & 3;
  switch (raw_dispose_method) {
    case 0: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::not_specified; break;
    case 1: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::do_not_dispose; break;
    case 2: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::restore_to_background_color; break;
    case 3: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::restore_to_previous; break;
    case 4: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::reserved_4; break;
    case 5: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::reserved_5; break;
    case 6: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::reserved_6; break;
    case 7: value.disposal_method = GifGraphicControlExtensionData::DisposeMethod::reserved_7; break;
  }

  value.wait_on_user_input = r & 2;

  /**
   * If not 0, this field specifies the number of hundredths (1/100) of a second to
   * wait before continuing with the processing of the Data Stream. The clock starts
   * ticking immediately after the graphic is rendered. This field may be used in
   * conjunction with the User Input Flag field.
   */
  uint16_t raw_delay;
  if (!stream->read(raw_delay)) {
    return {};
  }
  value.delay = std::chrono::duration_cast<decltype(value.delay)>(
    std::chrono::duration<long long, std::centi>(raw_delay));

  value.has_transparent_color = r & 1;
  if (value.has_transparent_color) {
    if (!stream->read(value.transparent_color_index)) {
      return {};
    }
  }

  return value;
}

void discard_sub_block(const std::unique_ptr<e00::Stream> &stream) {
  uint8_t size;
  do {
    if (!stream->read(size)) {
      return;
    }

    stream->seek(stream->current_position() + size);
  } while (size > 0);
}

}// namespace

namespace e00::impl {
std::error_code make_error_code(GifLoaderErrorCode e) {
  return std::error_code(static_cast<int>(e), gif_loader_err_category);
}

std::unique_ptr<resource::Bitmap> load_gif_from_stream(const std::unique_ptr<Stream> &stream) {
  // Make sure we're at the beginning of the stream
  stream->seek(0);

  // Make sure we have the minimum stream size
  if (stream->stream_size() < sizeof(GifHeader)) {
    // It's too small!
    return nullptr;
  }

  // Start by confirming we have the right file type by the header
  GifHeader header = read_gif_header(stream);

  // Verify signature
  if (header.signature[0] != 'G'
      || header.signature[1] != 'I'
      || header.signature[2] != 'F') {
    // Not a GIF
    return nullptr;
  }

  // Verify version
  if (header.version[0] != '8' || header.version[2] != 'a'
      || !(header.version[1] == '7' || header.version[1] == '9')) {
    // Not a version we understand
    return nullptr;
  }

  const bool has_color_table = header.packed & 0x80;
  if (!has_color_table) {
    // no global color table: error out ?
    // for now, yes error out
    return nullptr;
  }

  // Color Space Depth
  auto depth = ((header.packed >> 4) & 7) + 1;
  // Ignore Sort Flag
  // Global Color Table Size
  auto gct_size = 1 << ((header.packed & 0x07) + 1);

  // Read the color table
  std::vector<GifColor> color_table;
  if (has_color_table) {
    color_table.resize(gct_size);
    for (auto i = 0u; i < color_table.size(); i++) {
      color_table[i] = read_gif_color(stream);
    }
  }

  while (stream->max_read() > 0) {
    const auto segment = read_next_segment_header(stream);
    switch (segment.type) {
      case GifSegment::Type::invalid:
        return nullptr;
      case GifSegment::Type::image:
        break;
      case GifSegment::Type::extension_plain_text:
      case GifSegment::Type::extension_graphic_control:
        {
          auto gce = read_gce(stream);
        }
      case GifSegment::Type::extension_comment:
      case GifSegment::Type::extension_application:
      case GifSegment::Type::extension_unknown:
        // Skip !
        stream->seek(stream->current_position() + segment.size);
        discard_sub_block(stream);
        break;
      case GifSegment::Type::trailer:
        // END OF FILE normally
        return nullptr;
    }
  }

  return nullptr;
}

}// namespace e00::impl
