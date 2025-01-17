
#include "bitpixels.h"
#include <math.h>


//
// pixel operations //
//


BitPixel getBIPixel (uint32_t x, uint32_t y, BitImage *image) {
  int layerOffset = getBILayerOffset(x, y, image);
  BitPixel pixel = (image->data[layerOffset] >> (7 - x % 8)) & BITPIXEL_MASK;
  if (image->channels == 2) pixel |= ((image->alpha[layerOffset] >> (7 - x % 8)) << 1) & BITALPHA_MASK;
  else pixel |= BITALPHA_MASK;
  return pixel;
}



//
// place methods //
//


// place the pixel as is into the image
void placeBIPixel (uint32_t x, uint32_t y, BitPixel pixel, BitImage *image) {
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
  if (image->channels == 1) return;
  // get current image alpha byte
  imageByte = image->alpha[layerOffset];
  // merge the pixel alpha byte into layer
  if (!(pixel & BITALPHA_MASK)) imageByte &= ~pixelByte;
  else imageByte |= pixelByte;
  image->alpha[layerOffset] = imageByte;
}

void placeBIImage (int x, int y, BitImage *src_image, BitImage *dst_image) {
  for (int iy = 0; iy < src_image->height; iy++) {
    for (int ix = 0; ix < src_image->width; ix++) {
      // uint8_t pixel = getBIDataPixel(ix, iy, src_image);
      BitPixel pixel = getBIPixel(ix, iy, src_image);
      placeBIPixel(x + ix, y + iy, pixel, dst_image);
    }
  }
}



//
// draw methods //
//


// draw a pixel into the image
void drawBIPixel (int x, int y, BitPixel pixel, BitImage *image) {
  // if transparent then do nothing
  if (!(pixel & BITALPHA_MASK)) return;
  // place the pixel in the image
  placeBIPixel(x, y, pixel, image);
}

void drawBILine (int x0, int y0, int x1, int y1, BitPixel pixel, BitImage *image) {
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

void drawBIRect (int x, int y, int w, int h, BitPixel pixel, BitImage *image) {
  drawBILine(x, y, x, y + h - 1, pixel, image);
  drawBILine(x, y + h - 1, x + w - 1, y + h - 1, pixel, image);
  drawBILine(x + w - 1, y + h - 1, x + w - 1, y, pixel, image);
  drawBILine(x + w - 1, y, x, y, pixel, image);
}


void drawBIFillRect (int x, int y, int w, int h, BitPixel pixel, BitImage *image) {
  for (int iy = y; iy < y + h; iy++) {
    drawBIRect(x, iy, w, h - (iy - y) * 2, pixel, image);
  }
}

void drawBIImage (int x, int y, BitImage *src_image, BitImage *dst_image) {
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
int getBILayerOffset (int x, int y, BitImage *image) {
  return y * getBIRowSize(image) + x / 8;
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

// new BitImage
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
