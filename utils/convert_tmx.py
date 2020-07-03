import os
import sys
import xml.etree.ElementTree as ET
from itertools import zip_longest


def grouper(n, iterable, padvalue=None):
    "grouper(3, 'abcdefg', 'x') --> ('a','b','c'), ('d','e','f'), ('g','x','x')"
    return zip_longest(*[iter(iterable)] * n, fillvalue=padvalue)


def convert_properties_of(parent_element):
    def prop_convert(prop_element):
        name = prop_element.attrib.get('name')
        value = prop_element.attrib.get('value')

        type = prop_element.attrib.get('type')
        if type == 'int':
            return {'name': name, 'value': int(value)}

        return {'name': name, 'value': value}

    property_element = parent_element.find('properties')
    if property_element:
        properties = property_element.findall('property')
        return list(prop_convert(x) for x in properties)

    return []


def convert_tileset_inner(firstgid, element):
    image = element.find('image')

    tile_options = {}

    # Find all tiles, most of them might not have anything interesting
    tiles = element.findall('tile')
    for tile in tiles:
        tile_id = int(tile.attrib.get('id', 0))
        props = convert_properties_of(tile)
        if len(props) > 0:
            tile_options[tile_id] = {'properties': props}

    return {
        'firstgid': firstgid,
        'name': element.attrib.get("name"),
        'image': image.attrib.get('source'),
        'width': int(image.attrib.get('width', 0)),
        'height': int(image.attrib.get('height', 0)),
        'tilewidth': int(element.attrib.get('tilewidth', 0)),
        'tileheight': int(element.attrib.get('tileheight', 0)),
        'properties': convert_properties_of(element),
        'tiles': tile_options
    }


def parse_sourced_tileset(firstgid, source_filename):
    return convert_tileset_inner(firstgid, ET.parse(source_filename).getroot())


def convert_tileset(tileset_element):
    firstgid = int(tileset_element.attrib.get('firstgid', 0))

    # is it a source= or not ?
    source = tileset_element.attrib.get("source", "")
    if source != '':
        return parse_sourced_tileset(firstgid, source)

    return convert_tileset_inner(firstgid, tileset_element)


def convert_layer_data(data_txt, encoding):
    if encoding == "csv":
        return [int(i) for i in data_txt.strip().split(",")]

    return []


def convert_layer(layer):
    data = layer.find('data')

    return {
        'name': layer.attrib.get('name'),
        'items': convert_layer_data(data.text, data.attrib.get('encoding')),
        'width': int(layer.attrib.get('width', 0)),
        'height': int(layer.attrib.get('height', 0))
    }


def load_tmx(filename):
    tree = ET.parse(filename)
    root = tree.getroot()

    return {
        'orientation': root.attrib.get("orientation"),
        'renderorder': root.attrib.get("renderorder"),
        'width': int(root.attrib.get("width", 1)),
        'height': int(root.attrib.get("height", 1)),
        'backgroundcolor': root.attrib.get("backgroundcolor"),
        'tileset': list(convert_tileset(x) for x in root.findall('tileset')),
        'layers': list(convert_layer(x) for x in root.findall('layer'))
    }


def write_map(map, file):
    file.write("width=%d\n" % map['width'])
    file.write("height=%d\n" % map['height'])
    file.write("layers=%d\n" % len(map['layers']))
    file.write("tileset=%s\n" % map['tileset'][0]['name'])

    for layer in map['layers']:
        file.write("\n[layer]\n")
        file.write("name=%s\n" % layer['name'])
        for i in range(0, len(layer['items']), map['width']):
            lines = layer['items'][i:i + map['width']]
            file.write("line=%s\n" % (','.join(str(x) for x in lines)))


def write_tileset(tileset, file):
    file.write("source=%s\n" % os.path.basename(tileset['image']))
    file.write("tilewidth=%s\n" % tileset['tilewidth'])
    file.write("tileheight=%s\n" % tileset['tileheight'])

    for prop in tileset['properties']:
        file.write("%s=%s\n" % (prop['name'], prop['value']))


if __name__ == "__main__":
    map_name = sys.argv[1]
    output_directory = sys.argv[2]

    map_dir = os.path.dirname(map_name)
    map_name = os.path.basename(map_name)
    os.chdir(map_dir)

    map = load_tmx(map_name)

    with open("%s/%s.ini" % (output_directory, os.path.splitext(map_name)[0]), 'w') as f:
        write_map(map, f)

    # Each tileset has its own INI
    for tileset in map['tileset']:
        with open("%s/%s.ini" % (output_directory, tileset['name']), 'w') as f:
            write_tileset(tileset, f)

    sys.exit(0)
