
#ifndef BITPIXELS
#define BITPIXELS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// a BitPixel is defined by two bits
// 2^0 is the pixel bit where 1 = pixel is on and 0 = pixel is off
// 2^1 is the alpha bit where 1 = opaque and 0 = transparent
// 2^7 is an error bit for the pixel
#define BITPIXEL_MASK 0x01
#define BITALPHA_MASK 0x02
#define BITERROR_MASK 0x80

typedef uint8_t BitPixel;

// image structure
typedef struct {
  uint32_t width;
  uint32_t height;
  uint8_t channels; // 1 or 2, 1 channel = data only, 2 channels = data + alpha
  uint8_t *data;   // pixel bytes
  uint8_t *alpha;  // pixel opacity bytes
} BitImage;

int placeBIPixel (uint32_t x, uint32_t y, BitPixel pixel, BitImage *image);
void placeBIImage (uint32_t x, uint32_t y, BitImage *src_image, BitImage *dst_image);

int drawBIPixel (uint32_t x, uint32_t y, BitPixel pixel, BitImage *image);
void drawBILine (uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, BitPixel pixel, BitImage *image);
void drawBIRect (uint32_t x, uint32_t y, uint32_t w, uint32_t h, BitPixel pixel, BitImage *image);
void drawBIFillRect (uint32_t x, uint32_t y, uint32_t w, uint32_t h, BitPixel pixel, BitImage *image);
void drawBIImage (uint32_t x, uint32_t y, BitImage *src_image, BitImage *dst_image);

int getBILayerSize (BitImage *image);
int getBIRowSize (BitImage *image);
int getBILayerOffset (uint32_t x, uint32_t y, BitImage *image);
int validateBICoords (uint32_t x, uint32_t y, BitImage *image);
BitPixel getBIPixel (uint32_t x, uint32_t y, BitImage *image);

int fwriteBIUint32 (FILE * dst_fp, uint32_t val);
int freadBIUint32 (FILE * src_fp, uint32_t *data);
int fwriteBI (FILE * dst_fp, BitImage *image);
BitImage * freadBI (FILE *src_fp);

BitImage * newBI (uint32_t width, uint32_t height, uint8_t channels);
void freeBI (BitImage *image);

#endif
