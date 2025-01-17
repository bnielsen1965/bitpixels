
#include "bitpixels.h"
#include <stdio.h>

#define FILENAME_TEST "test.bim"

const uint32_t width = 3;
const uint32_t height = 3;
const uint8_t channels = 2;
uint8_t data[] = { 0xA0, 0x40, 0xA0 };
uint8_t alpha[] = { 0xFF, 0x40, 0xFF };
BitImage image = {
  .width = width,
  .height = height,
  .channels = channels,
  .data = data,
  .alpha = alpha
};
const char * testFilename = "test.oled";

void showImage (BitImage *image);
int saveImage (BitImage *image);
BitImage * loadImage ();

int main (int argc, char * argv[]) {
  BitPixel pixelOnTransparent = BITPIXEL_MASK;
  BitPixel pixeloffOpaque = BITALPHA_MASK;
  BitPixel pixelOnOpaque = BITALPHA_MASK | BITPIXEL_MASK;

  printf("2 channel 3x3 X image with middle row having transparent end pixels.\n");
  showImage(&image);

  if (saveImage(&image)) return 1;

  BitImage *image2 = loadImage();
  showImage(image2);

  // place a transparent on pixel
  placeBIPixel(0, 0, pixelOnTransparent, image2);
  printf("place transparent on pixel at 0, 0\n");
  showImage(image2);

  // place a opaque off pixel
  placeBIPixel(2, 0, pixelOnOpaque, image2);
  printf("place opaque off pixel at 2, 0\n");
  showImage(image2);

  // draw a transparent on pixel
  drawBIPixel(1, 1, pixelOnTransparent, image2);
  printf("draw transparent on pixel at 1, 1\n");
  showImage(image2);

  // draw opaque off pixel
  drawBIPixel(1, 1, pixelOnOpaque, image2);
  printf("draw opaque off pixel at 1, 1\n");
  showImage(image2);

  BitImage *image3 = newBI(16, 16, 2);
  if (!image3) {
    printf("Failed to allocate memory for image 3.\n");
    return 1;
  }

  drawBIPixel(0, 0, pixelOnOpaque, image3);
  drawBIPixel(8, 0, pixelOnOpaque, image3);
  drawBIPixel(15, 0, pixelOnOpaque, image3);
  drawBIPixel(8, 15, pixelOnOpaque, image3);
  printf("\n");
  showImage(image3);

  drawBILine(0, 15, 15, 0, pixelOnOpaque, image3);
  printf("\n");
  showImage(image3);

  drawBIRect (1, 1, 14, 14, pixelOnOpaque, image3);
  printf("\n");
  showImage(image3);

  drawBIFillRect(4, 4, 8, 8, pixelOnOpaque, image3);
  printf("\n");
  showImage(image3);

  drawBIImage(3, 13, &image, image3);
  printf("\n");
  showImage(image3);

  placeBIImage(3, 13, &image, image3);
  printf("\n");
  showImage(image3);

  free(image3);
  free(image2);


return 0;
}

// show character representation of image
void showImage (BitImage *image) {
  printf("Image width %d, height %d, channels %d\n", image->width, image->height, image->channels);
  printf("Layer size: %d bytes\n", getBILayerSize(image));

  printf("\nLegend: - = transparent, * = on, space = off\n");
  for (int y = 0; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      BitPixel pixel = getBIPixel(x, y, image);
      // if pixel is transparent then show background dash
      if (!(pixel & BITALPHA_MASK)) {
        printf("-");
        continue;
      }
      // if pixel is on then show asterisk
      if (pixel & BITPIXEL_MASK) {
        printf("*");
        continue;
      }
      // show space for pixel off
      printf(" ");
    }
    printf("\n");
  }
  printf("\n");
}

// save image to file
int saveImage (BitImage *image) {
  printf("Saving image to %s...\n", FILENAME_TEST);
  remove(FILENAME_TEST);
  FILE * fp = fopen(FILENAME_TEST, "wb");
  if (!fp) {
    fprintf(stderr, "Error opening %s for write.", FILENAME_TEST);
    return 1;
  }
  if (fwriteBI(fp, image)) {
    fprintf(stderr, "Error writing image to %s.", FILENAME_TEST);
    fclose(fp);
    return 1;
  }
  fclose(fp);
  printf("\n");
  return 0;
}

// load image from file
BitImage * loadImage () {
  printf("Loading image from %s...\n", FILENAME_TEST);
  FILE * fp = fopen(FILENAME_TEST, "rb");
  if (!fp) {
    fprintf(stderr, "Error opening %s for read.", FILENAME_TEST);
    return NULL;
  }
  BitImage *image = freadBI(fp);
  if (!image) {
    fprintf(stderr, "Error reading image to %s.", FILENAME_TEST);
    fclose(fp);
    return NULL;
  }
  fclose(fp);
  printf("\n");
  return image;
}
