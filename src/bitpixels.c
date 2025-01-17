
#include "bitpixels.h"
#include <math.h>



//
// place methods - overwrites existing pixel with the provided pixel //
//


// place the BitPixel as is into the BitImage, returns 0 on success, 1 on failure
int placeBIPixel (uint32_t x, uint32_t y, BitPixel pixel, BitImage *image) {
  if (validateBICoords(x, y, image)) return 1;
  // get current image data byte
  int layerOffset = getBILayerOffset(x, y, image);
  uint8_t imageByte = image->data[layerOffset];
  // create a byte with the bit set for the given x coordinate
  uint8_t pixelByte = 0x01 << 7 - (x % 8);
  // merge the pixel data byte into layer
  if (!(pixel & BITPIXEL_MASK)) imageByte &= ~pixelByte;
  else imageByte |= pixelByte;
  image->data[layerOffset] = imageByte;
  // if image has no alpha layer then return
  if (image->channels == 1) return 0;
  // get current image alpha byte
  imageByte = image->alpha[layerOffset];
  // merge the pixel alpha byte into layer
  if (!(pixel & BITALPHA_MASK)) imageByte &= ~pixelByte;
  else imageByte |= pixelByte;
  image->alpha[layerOffset] = imageByte;
  return 0;
}

// place a source BitImage into a destination BitImage as is
void placeBIImage (uint32_t x, uint32_t y, BitImage *src_image, BitImage *dst_image) {
  for (int iy = 0; iy < src_image->height; iy++) {
    for (int ix = 0; ix < src_image->width; ix++) {
      // uint8_t pixel = getBIDataPixel(ix, iy, src_image);
      BitPixel pixel = getBIPixel(ix, iy, src_image);
      placeBIPixel(x + ix, y + iy, pixel, dst_image);
    }
  }
}



//
// draw methods - draws pixels with alpha considered //
//


// draw a BitPixel into the BitImage, returns 0 on success, 1 on failure
int drawBIPixel (uint32_t x, uint32_t y, BitPixel pixel, BitImage *image) {
  if (validateBICoords(x, y, image)) return 1;
  // if transparent then do nothing
  if (!(pixel & BITALPHA_MASK)) return 0;
  // place the pixel in the image
  return placeBIPixel(x, y, pixel, image);
}

// draw a line of BitPixels into the BitImage
void drawBILine (uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, BitPixel pixel, BitImage *image) {
  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;
  int err = (dx > dy ? dx: -dy) / 2;
  while (1) {
    drawBIPixel(x0, y0, pixel, image);
    if (x0 == x1 && y0 == y1) break;
    int e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

// draw a rectangle of BitPixels into the BitImage
void drawBIRect (uint32_t x, uint32_t y, uint32_t w, uint32_t h, BitPixel pixel, BitImage *image) {
  drawBILine(x, y, x, y + h - 1, pixel, image);
  drawBILine(x, y + h - 1, x + w - 1, y + h - 1, pixel, image);
  drawBILine(x + w - 1, y + h - 1, x + w - 1, y, pixel, image);
  drawBILine(x + w - 1, y, x, y, pixel, image);
}

// draw a filled rectangle of BitPixels into the BitImage
void drawBIFillRect (uint32_t x, uint32_t y, uint32_t w, uint32_t h, BitPixel pixel, BitImage *image) {
  for (int iy = y; iy < y + h; iy++) {
    drawBIRect(x, iy, w, h - (iy - y) * 2, pixel, image);
  }
}

// draw a source BitImage into a destination BitImage
void drawBIImage (uint32_t x, uint32_t y, BitImage *src_image, BitImage *dst_image) {
  for (int iy = 0; iy < src_image->height; iy++) {
    for (int ix = 0; ix < src_image->width; ix++) {
      BitPixel pixel = getBIPixel(ix, iy, src_image);
      drawBIPixel(x + ix, y + iy, pixel, dst_image);
    }
  }
}




//
// information methods //
//

// get the data/alpha layer size in bytes for a BitImage
int getBILayerSize (BitImage *image) {
  return getBIRowSize(image) * image->height;
}

// get the data/alpha layer row size in bytes for a BitImage
int getBIRowSize (BitImage *image) {
  return image->width / 8 + !!(image->width % 8);
}

// get offset into BitImage layer based on x, y coordinates
int getBILayerOffset (uint32_t x, uint32_t y, BitImage *image) {
  return y * getBIRowSize(image) + x / 8;
}

// validate the x, y coordinates for the given BitImage
int validateBICoords (uint32_t x, uint32_t y, BitImage *image) {
  if (x < 0 || y < 0 || x >= image->width || y >= image->width) return 1;
  return 0;
}

// get the BitPixel at the x, y coordinates from the given BitImage
BitPixel getBIPixel (uint32_t x, uint32_t y, BitImage *image) {
  if (validateBICoords(x, y, image)) return BITERROR_MASK;
  int layerOffset = getBILayerOffset(x, y, image);
  BitPixel pixel = (image->data[layerOffset] >> (7 - x % 8)) & BITPIXEL_MASK;
  if (image->channels == 2) pixel |= ((image->alpha[layerOffset] >> (7 - x % 8)) << 1) & BITALPHA_MASK;
  else pixel |= BITALPHA_MASK;
  return pixel;
}




//
// file operations //
//

// write BitImage unit32 value
int fwriteBIUint32 (FILE *dst_fp, uint32_t val) {
  size_t n;
  uint8_t data_byte;
  data_byte = (val & 0xff000000) >> 24;
  n = fwrite(&data_byte, 1, 1, dst_fp);
  if (!n) return 1;
  data_byte = (val & 0x00ff0000) >> 16;
  n = fwrite(&data_byte, 1, 1, dst_fp);
  if (!n) return 1;
  data_byte = (val & 0x0000ff00) >> 8;
  n = fwrite(&data_byte, 1, 1, dst_fp);
  if (!n) return 1;
  data_byte = val & 0xff;
  n = fwrite(&data_byte, 1, 1, dst_fp);
  if (!n) return 1;
  return 0;
}

// read BitImage uint32 value
int freadBIUint32 (FILE *src_fp, uint32_t *data) {
  size_t n;
  uint8_t buffer[4];
  n = fread(buffer, 1, 4, src_fp);
  if (n != 4) return 1;
  (*data) = (uint32_t)((buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3]);
  return 0;
}

// write BitImage to file
int fwriteBI (FILE *dst_fp, BitImage *image) {
  size_t n;
  if (fwriteBIUint32(dst_fp, image->width)) return 1;
  if (fwriteBIUint32(dst_fp, image->height)) return 1;
  n = fwrite(&image->channels, 1, 1, dst_fp);
  if (!n) return 1;
  n = fwrite(image->data, 1, getBILayerSize(image), dst_fp);
  if (!n) return 1;
  if (image->channels == 2) {
    n = fwrite(image->alpha, 1, getBILayerSize(image), dst_fp);
    if (!n) return 1;
  }
  return 0;
}

// read BitImage from file
BitImage * freadBI (FILE *src_fp) {
  BitImage *image = calloc(sizeof(BitImage), 1);
  if (!image) return NULL;
  size_t n;
  if (freadBIUint32(src_fp, &image->width)) {
    freeBI(image);
    return NULL;
  }
  if (freadBIUint32(src_fp, &image->height)) {
    freeBI(image);
    return NULL;
  }
  if (!fread(&image->channels, 1, 1, src_fp)) {
    freeBI(image);
    return NULL;
  }
  image->data = calloc(getBILayerSize(image), 1);
  if (!image->data) {
    freeBI(image);
    return NULL;
  }
  if (!fread(image->data, 1, getBILayerSize(image), src_fp)) {
    freeBI(image);
    return NULL;
  }
  if (image->channels == 2) {
    image->alpha = calloc(getBILayerSize(image), 1);
    if (!image->alpha) {
      freeBI(image);
      return NULL;
    }
    if (!fread(image->alpha, 1, getBILayerSize(image), src_fp)) {
      freeBI(image);
      return NULL;
    }
  }
  return image;
}



//
// memory operations //
//

// allocate new BitImage
BitImage * newBI (uint32_t width, uint32_t height, uint8_t channels) {
  BitImage *image = calloc(sizeof(BitImage), 1);
  image->width = width;
  image->height = height;
  image->channels = channels;
  image->data = calloc(getBILayerSize(image), 1);
  if (!image->data) {
    freeBI(image);
    return NULL;
  }
  if (image->channels == 2) {
    image->alpha = calloc(getBILayerSize(image), 1);
    if (!image->alpha) {
      freeBI(image);
      return NULL;
    }
  }
  return image;
}

// free BitImage memory
void freeBI (BitImage *image) {
  if (image->alpha) free(image->alpha);
  if (image->data) free(image->data);
  free(image);
}
