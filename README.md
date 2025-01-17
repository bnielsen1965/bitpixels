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
second bit is the alpha opacity level.


## pixel bit masks

Constants are defined to assist in definging and utilizing a BitPixel.

```c
#define BITPIXEL_MASK 0x01
#define BITALPHA_MASK 0x02
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


