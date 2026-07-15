// stb_image implementation TU. PNG (and other) decoding used by midp_image.c.
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG          // the game ships PNGs; keep the binary small
#define STBI_NO_STDIO          // we decode from memory buffers only
#include "stb_image.h"
