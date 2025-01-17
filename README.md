# bitpixels

A small graphics library for monchrome displays written in C.

The library is intended for embedded C applications that rely on 
a small monochrome display such as the many small OLED displays 
with an I2C or SPI interface.

# data types

The library defines the BitPixel and BitImage data types.


## BitPixel

BitPixel is an unsigned 8 bit integer type used to store the value 
of a single pixel. The first bit is the pixel data bit and the 
second bit is the alpha opacity level. The last bit in the pixel is 
used to designate an error in a pixel operation.


## pixel bit masks

Constants are defined to assist in defining and utilizing a BitPixel.

```c
#define BITPIXEL_MASK 0x01
#define BITALPHA_MASK 0x02
#define BITERROR_MASK 0x80
```


## BitImage

BitImage is a struct used to hold a monochrome image with meta 
data to describe the image and pointers to the data bytes and 
a alpha layer bytes for images with 2 channels for pixel opacity.

```c
typedef struct {
  uint32_t width;
  uint32_t height;
  uint8_t channels; // 1 or 2, 1 channel = data only, 2 channels = data + alpha
  uint8_t *data;   // pixel bytes
  uint8_t *alpha;  // pixel opacity bytes
} BitImage;
```


# methods

The library includes a range of methods to operate on images and pixels.


## int placeBIPixel (uint32_t x, uint32_t y, BitPixel pixel, BitImage *image);

Place the BitPixel as is into the BitImage, returns 0 on success, 1 on failure.
The effect of the place method is to overwrite pixel data in an image including 
the transparency alpha layer.


## void placeBIImage (uint32_t x, uint32_t y, BitImage *src_image, BitImage *dst_image);

Place a source BitImage into a destination BitImage as is. This has the effect of 
overwriting pixel data in the destination image including the transparency alpha layer.


## int drawBIPixel (uint32_t x, uint32_t y, BitPixel pixel, BitImage *image);

Draw a BitPixel into the BitImage, returns 0 on success, 1 on failure. If the pixel 
is transparent then it will not overwrite the existing pixel in the image.


## void drawBILine (uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, BitPixel pixel, BitImage *image);

Draw a line of BitPixels into the BitImage.


## void drawBIRect (uint32_t x, uint32_t y, uint32_t w, uint32_t h, BitPixel pixel, BitImage *image);

Draw a rectangle of BitPixels into the BitImage.


## void drawBIFillRect (uint32_t x, uint32_t y, uint32_t w, uint32_t h, BitPixel pixel, BitImage *image);

Draw a filled rectangle of BitPixels into the BitImage.


## void drawBIImage (uint32_t x, uint32_t y, BitImage *src_image, BitImage *dst_image);

Draw a source BitImage into a destination BitImage.


## int getBILayerSize (BitImage *image);

Get the data/alpha layer size in bytes for a BitImage.


## int getBIRowSize (BitImage *image);

Get the data/alpha layer row size in bytes for a BitImage.


## int getBILayerOffset (uint32_t x, uint32_t y, BitImage *image);

Get offset into BitImage layer based on x, y coordinates.


## int validateBICoords (uint32_t x, uint32_t y, BitImage *image);

Validate the x, y coordinates for the given BitImage. Returns 0 if the 
coordinates are valid and 1 if invalid.


## BitPixel getBIPixel (uint32_t x, uint32_t y, BitImage *image);

Get the BitPixel at the x, y coordinates from the given BitImage.


## int fwriteBIUint32 (FILE * dst_fp, uint32_t val);

Write BitImage unit32 value to a file stream.


## int freadBIUint32 (FILE * src_fp, uint32_t *data);

Read BitImage uint32 value from a file stream.


## int fwriteBI (FILE * dst_fp, BitImage *image);

Write BitImage to file stream.


## BitImage * freadBI (FILE *src_fp);

Read BitImage from file stream.


## BitImage * newBI (uint32_t width, uint32_t height, uint8_t channels);

Allocate memory for new BitImage.


## void freeBI (BitImage *image);

Free BitImage memory allocated by the newBI() method.
