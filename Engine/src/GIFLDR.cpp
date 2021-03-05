#include "GIFLDR.hpp"

#include <array>
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
enum class SegmentType {
  invalid,
  image,
  extension,
  trailer,
};

class LZWTable {
  struct Entry {
    uint8_t byte;
    bool has_prefix;
    uint16_t prefix_entry_index;
  };

  uint8_t _initial_key_size;
  uint16_t _num_entries;

  std::array<Entry, 0x1000> _entries;

public:
  explicit LZWTable(uint8_t key_size)
    : _initial_key_size(key_size),
      _num_entries(0) {
    // Key size is in bits,
    // to that, we add the CLEAR and the STOP
    clear();
  }

  uint16_t num_entries() const { return _num_entries; }

  void clear() {
    _num_entries = (1 << (_initial_key_size)) + 2;

    for (auto key = 0; key < _num_entries; key++) {
      _entries[key].has_prefix = false;
      _entries[key].prefix_entry_index = 0;
      _entries[key].byte = (uint8_t)(key & 0xFF);
    }
  }

  bool is_full() const {
    return _num_entries >= 0x1000;
  }

  bool has_index(uint16_t index) const {
    return _num_entries >= index;
  }

  /**
   * Follows the chain starting at `index` until there is no more
   * previous items and outputs that byte
   *
   * @param index where to start searching from
   * @return the top most byte of the chain
   */
  uint8_t find_root_byte(uint16_t index) const {
    if (!has_index(index)) return 0;

    while (_entries[index].has_prefix) {
      index = _entries[index].prefix_entry_index;
    }

    return _entries[index].byte;
  }

  uint16_t size_of_index(uint16_t index) const {
    if (!has_index(index)) return 0;
    // If we know the index, it's at least 1 byte long
    // Now, we count how many prefixes we can do until we reach an
    // entry that doesn't have a prefix
    uint16_t count = 1;

    while (_entries[index].has_prefix) {
      index = _entries[index].prefix_entry_index;
      count++;
    }

    return count;
  }

  /* Call @size_of_index to know how long this method will write
   * @returns the data pointer pointing to after the last byte
   */
  uint8_t *write_index_to(uint16_t index, uint8_t *data) {
    const auto length = size_of_index(index);
    if (length == 0) {
      return data;
    }

    // the entries are built up this way "backwards",
    // so it's necessary to emit them in reverse order.
    data += length - 1;

    *data = _entries[index].byte;
    data--;

    while (_entries[index].has_prefix) {
      index = _entries[index].prefix_entry_index;
      *data = _entries[index].byte;
      data--;
    }

    return data + length;
  }

  void add(uint16_t previous, uint8_t byte) {
    _entries[_num_entries].has_prefix = true;
    _entries[_num_entries].prefix_entry_index = previous;
    _entries[_num_entries].byte = byte;
    _num_entries++;
  }
};

class GifImageDataReader {
  const std::unique_ptr<e00::Stream> &_stream;

  uint8_t _chunk_length;
  uint8_t _shift;
  uint8_t _last_byte_read;

public:
  GifImageDataReader(const std::unique_ptr<e00::Stream> &stream)
    : _stream(stream),
      _chunk_length(0),
      _shift(0),
      _last_byte_read(0) {}

  ~GifImageDataReader() = default;


  uint16_t next_key(uint16_t key_size) {
    uint16_t key = 0;
    int frag_size = 0;

    for (auto bits_read = 0; bits_read < key_size; bits_read += frag_size) {
      const auto right_pad = (_shift + bits_read) % 8;
      if (right_pad == 0) {
        if (_chunk_length == 0) {
          _stream->read(_chunk_length);
        }

        _stream->read(_last_byte_read);
        _chunk_length--;
      }

      frag_size = std::min(key_size - bits_read, 8 - right_pad);
      key |= ((uint16_t)_last_byte_read >> right_pad) << bits_read;
    }

    _shift = (_shift + key_size) % 8;

    key &= (1 << key_size) - 1;
    return key;
  }
};

struct ExtensionHeader {
  enum class Type {
    unknown,
    plain_text,
    graphic_control,
    comment,
    application,
  };

  Type type;
  uint8_t length;
};

struct FrameDataHeader {
  uint16_t left_position;
  uint16_t top_position;
  uint16_t width;
  uint16_t height;

  bool has_local_color_table;
  bool is_interlaced;
  bool is_local_color_table_sorted;

  std::vector<GifColor> local_color_table;
};

void discard_sub_block(const std::unique_ptr<e00::Stream> &stream) {
  uint8_t size;
  do {
    if (!stream->read(size)) {
      return;
    }

    stream->seek(stream->current_position() + size);
  } while (size > 0);
}

SegmentType read_gif_segment_type(const std::unique_ptr<e00::Stream> &stream) {
  char seg;

  // Read the segment type
  if (!stream->read(seg)) {
    // Nope outta here if we can't
    return SegmentType::invalid;
  }

  if (seg == ',') return SegmentType::image;
  if (seg == ';') return SegmentType::trailer;
  if (seg == '!') return SegmentType::extension;

  return SegmentType::invalid;
}

ExtensionHeader read_gif_extension_header(const std::unique_ptr<e00::Stream> &stream) {
  ExtensionHeader read_header;
  uint8_t label;
  if (!stream->read(label)) {
    return { ExtensionHeader::Type::unknown, 0 };
  }

  read_header.type = ExtensionHeader::Type::unknown;
  switch (label) {
    case 0x01: read_header.type = ExtensionHeader::Type::plain_text; break;
    case 0xF9: read_header.type = ExtensionHeader::Type::graphic_control; break;
    case 0xFE: read_header.type = ExtensionHeader::Type::comment; break;
    case 0xFF: read_header.type = ExtensionHeader::Type::application; break;
  }

  if (!stream->read(read_header.length)) {
    return { ExtensionHeader::Type::unknown, 0 };
  }

  return read_header;
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

FrameDataHeader read_frame_header(const std::unique_ptr<e00::Stream> &stream) {
  // GIF docs calls this an image
  FrameDataHeader header;
  stream->read(header.left_position);
  stream->read(header.top_position);
  stream->read(header.width);
  stream->read(header.height);

  uint8_t packed;
  stream->read(packed);

  header.is_local_color_table_sorted = packed & 0x20;
  header.is_interlaced = packed & 0x40;
  header.has_local_color_table = packed & 0x80;

  if (header.has_local_color_table) {
    header.local_color_table.resize(1 << ((packed & 0x07) + 1));
    for (auto i = 0u; i < header.local_color_table.size(); i++) {
      header.local_color_table.at(i) = read_gif_color(stream);
    }
  }

  return header;
}

std::error_code decode_image(const std::unique_ptr<e00::Stream> &stream, const FrameDataHeader &header, const GifGraphicControlExtensionData &gce, const std::unique_ptr<e00::resource::Bitmap> &image) {

  uint8_t initial_key_size = 0;
  stream->read(initial_key_size);
  LZWTable table(initial_key_size);

  // Make sure we mark the end of the image stream-- just to make
  // sure we don't accumulate errors
  const auto start_pos = stream->current_position();
  discard_sub_block(stream);
  const auto end_pos = stream->current_position();
  stream->seek(start_pos);

  // Make our CLEAR and STOP codes
  const auto clear = 1 << initial_key_size;
  const auto stop = clear + 1;

  // Initialize our reader that helps us read X bits keys
  GifImageDataReader reader(stream);
  uint16_t key_size = initial_key_size + 1;

  // What was our previous code?
  uint16_t previous_key = std::numeric_limits<uint16_t>::max();

  while (1) {
    const auto code = reader.next_key(key_size);
    if (code == clear) {
      table.clear();
      continue;
    } else if (code == stop) {
      break;
    }

    // We didn't get a clear nor a break
    if (previous_key != std::numeric_limits<uint16_t>::max()
        && !table.is_full()) {

      if (code == table.num_entries()) {
        table.add(previous_key, table.find_root_byte(previous_key));
      } else {
        table.add(previous_key, table.find_root_byte(code));
      }

      // GIF89a mandates that this stops at 12 bits
      if (!table.is_full()
          && (table.num_entries() & (table.num_entries() - 1)) == 0) key_size++;
    }

    // Keep our previous code
    previous_key = code;

    // Write data to the bitmap
    const auto data_len = table.size_of_index(code);
    // debug
    std::vector<uint8_t> debug_data;
    debug_data.resize(data_len);
    table.write_index_to(code, debug_data.data());

    debug_data.resize(debug_data.size());
  }

  // Reset stream position to the end
  stream->seek(end_pos);

  return {};
}

}// namespace

namespace e00::impl {
std::error_code make_error_code(GifLoaderErrorCode e) {
  return std::error_code(static_cast<int>(e), gif_loader_err_category);
}

std::vector<std::unique_ptr<resource::Bitmap>> load_gif_from_stream(const std::unique_ptr<Stream> &stream) {
  // Make sure we're at the beginning of the stream
  stream->seek(0);

  // Make sure we have the minimum stream size
  if (stream->stream_size() < sizeof(GifHeader)) {
    // It's too small!
    return {};
  }

  // Start by confirming we have the right file type by the header
  GifHeader header = read_gif_header(stream);

  // Verify signature
  if (header.signature[0] != 'G'
      || header.signature[1] != 'I'
      || header.signature[2] != 'F') {
    // Not a GIF
    return {};
  }

  // Verify version
  if (header.version[0] != '8' || header.version[2] != 'a'
      || !(header.version[1] == '7' || header.version[1] == '9')) {
    // Not a version we understand
    return {};
  }

  const bool has_color_table = header.packed & 0x80;
  if (!has_color_table) {
    // no global color table: error out ?
    // for now, yes error out
    return {};
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

  bool has_gce = false;
  GifGraphicControlExtensionData last_gce;
  std::vector<std::unique_ptr<resource::Bitmap>> images;

  while (stream->max_read() > 0) {
    const auto segment = read_gif_segment_type(stream);
    switch (segment) {
      case SegmentType::invalid:
        return {};
      case SegmentType::image:
        {
          // In GIFs, frames can be smaller than the actual bitmap size
          // Frame can also have their local color table (the total number
          // of colors can exceed 256) but we need to make a compromise;
          const auto frame_header = read_frame_header(stream);

          // Create an image the full size
          auto frame = e00::resource::Bitmap::CreateMemoryBitmap(
            {},
            Vec2<uint16_t>(header.screen_width, header.screen_height),
            true);

          std::error_code ec;
          if (has_gce) {
            ec = decode_image(stream, frame_header, last_gce, frame);
          } else {
            ec = decode_image(stream, frame_header, {}, frame);
          }

          if (!ec) {
            images.push_back(frame);
          }
        }
        break;
      case SegmentType::extension:
        {
          const auto ext_header = read_gif_extension_header(stream);
          const auto position = stream->current_position();
          switch (ext_header.type) {
            case ExtensionHeader::Type::unknown: break;
            case ExtensionHeader::Type::plain_text: break;
            case ExtensionHeader::Type::graphic_control:
              last_gce = read_gce(stream);
              has_gce = true;
              break;
            case ExtensionHeader::Type::comment: break;
            case ExtensionHeader::Type::application: break;
          }

          // Always skip headers
          stream->seek(position + ext_header.length);
          discard_sub_block(stream);
        }
        break;
      case SegmentType::trailer:
        // END OF FILE normally
        return images;
    }
  }

  return {};
}

}// namespace e00::impl
