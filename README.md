# GxFont_GFX_TFT_eSPI
Font Rendering Graphics Library

## Base class for font rendering and fonts of library TFT_eSPI

### This class requires the following methods to be implemented in the subclass:
- virtual void drawPixel(uint32_t x, uint32_t y, uint32_t color) = 0;
- virtual void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) = 0;
- virtual void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) = 0;

### This library is made for use with GxEPD and GxEPD2
- in GxEPD it is used in a subclass of Adafruit_GFX, GxFont_GFX.
- this subclass serves as a switch-bridge to the subclass of GxFont_GFX_TFT_eSPI.
- GxFont_GFX serves as base class of GxEPD (coming version).

### Initial Version 1.0.0, under construction
