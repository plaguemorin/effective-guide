from PIL import Image

from os import listdir
from os.path import isfile


def quantizetopalette(silf, palette, dither=False):
    """Convert an RGB or L mode image to use a given P image's palette."""
    silf.load()

    # use palette from reference image
    palette.load()
    if palette.mode != "P":
        raise ValueError("bad mode for palette image")
    if silf.mode != "RGB" and silf.mode != "L":
        raise ValueError(
            "only RGB or L mode images can be quantized to a palette"
        )
    im = silf.im.convert("P", 1 if dither else 0, palette.im)
    # the 0 above means turn OFF dithering

    # Later versions of Pillow (4.x) rename _makeself to _new
    try:
        return silf._new(im)
    except AttributeError:
        return silf._makeself(im)


palettedata = [
    0x14, 0x0c, 0x1c,
    0x44, 0x24, 0x34,
    0x30, 0x34, 0x6d,
    0x4e, 0x4a, 0x4e,
    0x85, 0x4c, 0x30,
    0x34, 0x65, 0x24,
    0xd0, 0x46, 0x48,
    0x75, 0x71, 0x61,
    0x59, 0x7d, 0xce,
    0xd2, 0x7d, 0x2c,
    0x85, 0x95, 0xa1,
    0x6d, 0xaa, 0x2c,
    0xd2, 0xaa, 0x99,
    0x6d, 0xc2, 0xca,
    0xda, 0xd4, 0x5e,
    0xde, 0xee, 0xd6
]

for f in listdir('.'):
    if isfile(f) and f.endswith('-orig.bmp'):
        palimage = Image.new('P', (24, 24))
        palimage.putpalette(palettedata * 16)
        oldimage = Image.open(f)
        newimage = quantizetopalette(oldimage, palimage, dither=False)
        newimage.save(f[:-9] + ".bmp")
