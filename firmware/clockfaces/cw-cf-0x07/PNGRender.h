#pragma once

#include <Locator.h>
#include "mbedtls/base64.h"
#include <PNGdec.h>

#define IMAGE_BUFFER_SIZE 1024

static PNG png;
static unsigned char decodedArray[IMAGE_BUFFER_SIZE];
static size_t decodedArrayLength;

typedef struct png_position
{
  uint8_t xoff, yoff;
} PNG_POSITION;


static void PNGDraw(PNGDRAW *pDraw)
{
  uint16_t usPixels[64];
  
  PNG_POSITION *pPos = (PNG_POSITION *)pDraw->pUser;

  png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0xffffffff);

  Locator::getDisplay()->drawRGBBitmap(pPos->xoff, pPos->yoff + pDraw->y, usPixels, pDraw->iWidth, 1); 
}


static bool openImage(const char *base64Image)
{
  mbedtls_base64_decode(decodedArray, IMAGE_BUFFER_SIZE, &decodedArrayLength, (const unsigned char *)base64Image, strlen(base64Image));

  int rc = png.openRAM(decodedArray, decodedArrayLength, PNGDraw); 
  
  return (rc == PNG_SUCCESS);
}


static void renderImage(const char *base64Image, const uint8_t x, const uint8_t y)
{
  PNG_POSITION pos;
    
  if (openImage(base64Image)) 
  {
    pos.xoff = x;
    pos.yoff = y;
    int rc = png.decode((void *)&pos, 0);

    png.close();
  }
}


static void getImageDimensions(const char *base64Image, uint8_t &width, uint8_t &height)
{
  if (openImage(base64Image)) {
    width = png.getWidth();
    height = png.getHeight();

    png.close();
  }
}

