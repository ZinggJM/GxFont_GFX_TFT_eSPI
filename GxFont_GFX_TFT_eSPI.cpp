/***************************************************
  Arduino TFT graphics library targeted at ESP8266
  and ESP32 based boards.

  This is a standalone library that contains the
  hardware driver, the graphics functions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce their
  size.

  Created by Bodmer 2/12/16
  Bodmer: Added RPi 16 bit display support
 ****************************************************/

// GxFont_GFX_TFT_eSPI : font rendering graphics library
// extracted and adapted by Jean-Marc Zingg for use with GxEPD and GxEPD2
// code based on https://github.com/Bodmer/TFT_eSPI

#include "GxFont_GFX_TFT_eSPI.h"

//#define DIAG(x) x

#ifndef DIAG
#define DIAG(x)
#endif

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

/***************************************************************************************
** Function name:           GxFont_GFX_TFT_eSPI
** Description:             Constructor , we must use hardware SPI pins
***************************************************************************************/
GxFont_GFX_TFT_eSPI::GxFont_GFX_TFT_eSPI(int16_t w, int16_t h)
{

  // The control pins are deliberately set to the inactive state (CS high) as setup()
  // might call and initialise other SPI peripherals which would could cause conflicts
  // if CS is floating or undefined.
  _init_width  = _width  = w; // Set by specific xxxxx_Defines.h file or by users sketch
  _init_height = _height = h; // Set by specific xxxxx_Defines.h file or by users sketch
  rotation  = 0;
  cursor_y  = cursor_x    = 0;
  textfont  = 1;
  textsize  = 1;
  textcolor   = 0xFFFF; // White
  textbgcolor = 0x0000; // Black
  padX = 0;             // No padding
  textwrapX  = true;    // Wrap text at end of line when using print stream
  textwrapY  = false;   // Wrap text at bottom of screen when using print stream
  textdatum = TL_DATUM; // Top Left text alignment is default
  fontsloaded = 0;

#ifdef LOAD_GLCD
  fontsloaded  = 0x0002; // Bit 1 set
#endif

#ifdef LOAD_FONT2
  fontsloaded |= 0x0004; // Bit 2 set
#endif

#ifdef LOAD_FONT4
  fontsloaded |= 0x0010; // Bit 4 set
#endif

#ifdef LOAD_FONT6
  fontsloaded |= 0x0040; // Bit 6 set
#endif

#ifdef LOAD_FONT7
  fontsloaded |= 0x0080; // Bit 7 set
#endif

#ifdef LOAD_FONT8
  fontsloaded |= 0x0100; // Bit 8 set
#endif

}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}


/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position and font
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setCursor(int16_t x, int16_t y, uint8_t font)
{
  textfont = font;
  cursor_x = x;
  cursor_y = y;
}


/***************************************************************************************
** Function name:           getCursorX
** Description:             Get the text cursor x position
***************************************************************************************/
int16_t GxFont_GFX_TFT_eSPI::getCursorX(void) const
{
  return cursor_x;
}

/***************************************************************************************
** Function name:           getCursorY
** Description:             Get the text cursor y position
***************************************************************************************/
int16_t GxFont_GFX_TFT_eSPI::getCursorY(void) const
{
  return cursor_y;
}


/***************************************************************************************
** Function name:           setTextSize
** Description:             Set the text size multiplier
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextSize(uint8_t s)
{
  if (s > 7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
  textsize = (s > 0) ? s : 1; // Don't allow font size 0
}


/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground colour (background is transparent)
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}


/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground and background colour
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextColor(uint16_t c, uint16_t b)
{
  textcolor   = c;
  textbgcolor = b;
}


/***************************************************************************************
** Function name:           setTextWrap
** Description:             Define if text should wrap at end of line
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextWrap(boolean wrapX, boolean wrapY)
{
  textwrapX = wrapX;
  textwrapY = wrapY;
}


/***************************************************************************************
** Function name:           setTextDatum
** Description:             Set the text position reference datum
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextDatum(uint8_t d)
{
  textdatum = d;
}


/***************************************************************************************
** Function name:           setTextPadding
** Description:             Define padding width (aids erasing old text and numbers)
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextPadding(uint16_t x_width)
{
  padX = x_width;
}


/***************************************************************************************
** Function name:           getRotation
** Description:             Return the rotation value (as used by setRotation())
***************************************************************************************/
uint8_t GxFont_GFX_TFT_eSPI::getRotation(void)
{
  return rotation;
}

/***************************************************************************************
** Function name:           getTextDatum
** Description:             Return the text datum value (as used by setTextDatum())
***************************************************************************************/
uint8_t GxFont_GFX_TFT_eSPI::getTextDatum(void)
{
  return textdatum;
}


/***************************************************************************************
** Function name:           width
** Description:             Return the pixel width of display (per current rotation)
***************************************************************************************/
// Return the size of the display (per current rotation)
int16_t GxFont_GFX_TFT_eSPI::width(void)
{
  return _width;
}


/***************************************************************************************
** Function name:           height
** Description:             Return the pixel height of display (per current rotation)
***************************************************************************************/
int16_t GxFont_GFX_TFT_eSPI::height(void)
{
  return _height;
}


/***************************************************************************************
** Function name:           textWidth
** Description:             Return the width in pixels of a string in a given font
***************************************************************************************/
int16_t GxFont_GFX_TFT_eSPI::textWidth(const String& string)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return textWidth(buffer, textfont);
}

int16_t GxFont_GFX_TFT_eSPI::textWidth(const String& string, int font)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return textWidth(buffer, font);
}

int16_t GxFont_GFX_TFT_eSPI::textWidth(const char *string)
{
  return textWidth(string, textfont);
}

int16_t GxFont_GFX_TFT_eSPI::textWidth(const char *string, int font)
{
  int str_width  = 0;

#ifdef SMOOTH_FONT
  if (fontLoaded)
  {
    while (*string)
    {
      uint16_t unicode = decodeUTF8(*string++);
      if (unicode)
      {
        if (unicode == 0x20) str_width += gFont.spaceWidth;
        else
        {
          uint16_t gNum = 0;
          bool found = getUnicodeIndex(unicode, &gNum);
          if (found)
          {
            if (str_width == 0 && gdX[gNum] < 0) str_width -= gdX[gNum];
            if (*string) str_width += gxAdvance[gNum];
            else str_width += (gdX[gNum] + gWidth[gNum]);
          }
          else str_width += gFont.spaceWidth + 1;
        }
      }
    }
    return str_width;
  }
#endif

  unsigned char uniCode;
  char *widthtable;

  if (font > 1 && font < 9)
  {
    widthtable = (char *)pgm_read_dword( &(fontdata[font].widthtbl ) ) - 32; //subtract the 32 outside the loop

    while (*string)
    {
      uniCode = *(string++);
      if (uniCode > 31 && uniCode < 128)
        str_width += pgm_read_byte( widthtable + uniCode); // Normally we need to subract 32 from uniCode
      else str_width += pgm_read_byte( widthtable + 32); // Set illegal character = space width
    }
  }
  else
  {

#ifdef LOAD_GFXFF
    if (gfxFont) // New font
    {
      while (*string)
      {
        uniCode = *(string++);
        if ((uniCode >= (uint8_t)pgm_read_byte(&gfxFont->first)) && (uniCode <= (uint8_t)pgm_read_byte(&gfxFont->last )))
        {
          uniCode -= pgm_read_byte(&gfxFont->first);
          GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[uniCode]);
          // If this is not the  last character then use xAdvance
          if (*string) str_width += pgm_read_byte(&glyph->xAdvance);
          // Else use the offset plus width since this can be bigger than xAdvance
          else str_width += ((int8_t)pgm_read_byte(&glyph->xOffset) + pgm_read_byte(&glyph->width));
        }
      }
    }
    else
#endif
    {
#ifdef LOAD_GLCD
      while (*string++) str_width += 6;
#endif
    }
  }
  return str_width * textsize;
}


/***************************************************************************************
** Function name:           fontsLoaded
** Description:             return an encoded 16 bit value showing the fonts loaded
***************************************************************************************/
// Returns a value showing which fonts are loaded (bit N set =  Font N loaded)

uint16_t GxFont_GFX_TFT_eSPI::fontsLoaded(void)
{
  return fontsloaded;
}


/***************************************************************************************
** Function name:           fontHeight
** Description:             return the height of a font (yAdvance for free fonts)
***************************************************************************************/
int16_t GxFont_GFX_TFT_eSPI::fontHeight(int16_t font)
{
#ifdef SMOOTH_FONT
  if (fontLoaded) return gFont.yAdvance;
#endif

#ifdef LOAD_GFXFF
  if (font == 1)
  {
    if (gfxFont) // New font
    {
      return pgm_read_byte(&gfxFont->yAdvance) * textsize;
    }
  }
#endif
  return pgm_read_byte( &fontdata[font].height ) * textsize;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a single character in the Adafruit GLCD font
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg, uint8_t size)
{
  DIAG (Serial.print("GxFont_GFX_TFT_eSPI::drawChar("); Serial.print(c); Serial.println(") GLCD");)
  if ((x >= (int16_t)_width)            || // Clip right
      (y >= (int16_t)_height)           || // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

  if (c < 32) return;
#ifdef LOAD_GLCD
  //>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  if (!gfxFont) { // 'Classic' built-in font
#endif
    //>>>>>>>>>>>>>>>>>>

    DIAG (Serial.print("GxFont_GFX_TFT_eSPI::drawChar("); Serial.print(c); Serial.println(") GLCD .");)
    boolean fillbg = (bg != color);

    if ((size == 1) && fillbg)
    {
      DIAG (Serial.print("GxFont_GFX_TFT_eSPI::drawChar("); Serial.print(c); Serial.println(") GLCD 1");)
      uint8_t column[6];
      uint8_t mask = 0x1;
      //setAddrWindow(x, y, x + 5, y + 8);
      for (int8_t i = 0; i < 5; i++ ) column[i] = pgm_read_byte(font + (c * 5) + i);
      column[5] = 0;

      for (int8_t j = 0; j < 8; j++)
      {
        for (int8_t k = 0; k < 5; k++ )
        {
          if (column[k] & mask)
          {
            //tft_Write_16(color);
            drawPixel(x + k, y + j, color);
          }
          else
          {
            //tft_Write_16(bg);
            drawPixel(x + k, y + j, bg);
          }
        }
        mask <<= 1;
        //tft_Write_16(bg);
        drawPixel(x + 5, y + j, bg);
      }
    }
    else
    {
      DIAG (Serial.print("GxFont_GFX_TFT_eSPI::drawChar("); Serial.print(c); Serial.println(") GLCD 2");)
      for (int8_t i = 0; i < 6; i++ ) {
        uint8_t line;
        if (i == 5)
          line = 0x0;
        else
          line = pgm_read_byte(font + (c * 5) + i);

        if (size == 1) // default size
        {
          for (int8_t j = 0; j < 8; j++) {
            if (line & 0x1) drawPixel(x + i, y + j, color);
            line >>= 1;
          }
        }
        else {  // big size
          for (int8_t j = 0; j < 8; j++) {
            if (line & 0x1) fillRect(x + (i * size), y + (j * size), size, size, color);
            else if (fillbg) fillRect(x + i * size, y + j * size, size, size, bg);
            line >>= 1;
          }
        }
      }
    }

    //>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  } else { // Custom font
#endif
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>
#endif // LOAD_GLCD

#ifdef LOAD_GFXFF
    DIAG (Serial.print("GxFont_GFX_TFT_eSPI::drawChar("); Serial.print(c); Serial.println(") GLCD 3");)
    // Filter out bad characters not present in font
    if ((c >= (uint8_t)pgm_read_byte(&gfxFont->first)) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last )))
    {
      //>>>>>>>>>>>>>>>>>>>>>>>>>>>

      c -= pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
      uint8_t  *bitmap = (uint8_t *)pgm_read_dword(&gfxFont->bitmap);

      uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
      uint8_t  w  = pgm_read_byte(&glyph->width),
               h  = pgm_read_byte(&glyph->height),
               xa = pgm_read_byte(&glyph->xAdvance);
      int8_t   xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
      uint8_t  xx, yy, bits, bit = 0;
      int16_t  xo16 = 0, yo16 = 0;

      if (size > 1) {
        xo16 = xo;
        yo16 = yo;
      }

      // Here we have 3 versions of the same function just for evaluation purposes
      // Comment out the next two #defines to revert to the slower Adafruit implementation

      // If FAST_LINE is defined then the free fonts are rendered using horizontal lines
      // this makes rendering fonts 2-5 times faster. Particularly good for large fonts.
      // This is an elegant solution since it still uses generic functions present in the
      // stock library.

      // If FAST_SHIFT is defined then a slightly faster (at least for AVR processors)
      // shifting bit mask is used

      // Free fonts don't look good when the size multiplier is >1 so we could remove
      // code if this is not wanted and speed things up

#define FAST_HLINE
#define FAST_SHIFT
      //FIXED_SIZE is an option in User_Setup.h that only works with FAST_LINE enabled

#ifdef FIXED_SIZE
      x += xo; // Save 88 bytes of FLASH
      y += yo;
#endif

#ifdef FAST_HLINE

#ifdef FAST_SHIFT
      uint16_t hpc = 0; // Horizontal foreground pixel count
      for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
          if (bit == 0) {
            bits = pgm_read_byte(&bitmap[bo++]);
            bit  = 0x80;
          }
          if (bits & bit) hpc++;
          else {
            if (hpc) {
#ifndef FIXED_SIZE
              if (size == 1) drawFastHLine(x + xo + xx - hpc, y + yo + yy, hpc, color);
              else fillRect(x + (xo16 + xx - hpc)*size, y + (yo16 + yy)*size, size * hpc, size, color);
#else
              drawFastHLine(x + xx - hpc, y + yy, hpc, color);
#endif
              hpc = 0;
            }
          }
          bit >>= 1;
        }
        // Draw pixels for this line as we are about to increment yy
        if (hpc) {
#ifndef FIXED_SIZE
          if (size == 1) drawFastHLine(x + xo + xx - hpc, y + yo + yy, hpc, color);
          else fillRect(x + (xo16 + xx - hpc)*size, y + (yo16 + yy)*size, size * hpc, size, color);
#else
          drawFastHLine(x + xx - hpc, y + yy, hpc, color);
#endif
          hpc = 0;
        }
      }
#else
      uint16_t hpc = 0; // Horizontal foreground pixel count
      for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
          if (!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if (bits & 0x80) hpc++;
          else {
            if (hpc) {
              if (size == 1) drawFastHLine(x + xo + xx - hpc, y + yo + yy, hpc, color);
              else fillRect(x + (xo16 + xx - hpc)*size, y + (yo16 + yy)*size, size * hpc, size, color);
              hpc = 0;
            }
          }
          bits <<= 1;
        }
        // Draw pixels for this line as we are about to increment yy
        if (hpc) {
          if (size == 1) drawFastHLine(x + xo + xx - hpc, y + yo + yy, hpc, color);
          else fillRect(x + (xo16 + xx - hpc)*size, y + (yo16 + yy)*size, size * hpc, size, color);
          hpc = 0;
        }
      }
#endif

#else
      for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
          if (!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if (bits & 0x80) {
            if (size == 1) {
              drawPixel(x + xo + xx, y + yo + yy, color);
            } else {
              fillRect(x + (xo16 + xx)*size, y + (yo16 + yy)*size, size, size, color);
            }
          }
          bits <<= 1;
        }
      }
#endif
    }
#endif


#ifdef LOAD_GLCD
#ifdef LOAD_GFXFF
  } // End classic vs custom font
#endif
#endif

}



/***************************************************************************************
** Function name:           color565
** Description:             convert three 8 bit RGB levels to a 16 bit colour value
***************************************************************************************/
uint16_t GxFont_GFX_TFT_eSPI::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


/***************************************************************************************
** Function name:           color16to8
** Description:             convert 16 bit colour to an 8 bit 332 RGB colour value
***************************************************************************************/
uint8_t GxFont_GFX_TFT_eSPI::color16to8(uint16_t c)
{
  return ((c & 0xE000) >> 8) | ((c & 0x0700) >> 6) | ((c & 0x0018) >> 3);
}


/***************************************************************************************
** Function name:           color8to16
** Description:             convert 8 bit colour to a 16 bit 565 colour value
***************************************************************************************/
uint16_t GxFont_GFX_TFT_eSPI::color8to16(uint8_t color)
{
  uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table
  uint16_t color16 = 0;

  //        =====Green=====     ===============Red==============
  color16  = (color & 0x1C) << 6 | (color & 0xC0) << 5 | (color & 0xE0) << 8;
  //        =====Green=====    =======Blue======
  color16 |= (color & 0x1C) << 3 | blue[color & 0x03];

  return color16;
}

/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through serial stream
***************************************************************************************/
size_t GxFont_GFX_TFT_eSPI::write(uint8_t utf8)
{
  if (utf8 == '\r') return 1;
  DIAG (Serial.print("GxFont_GFX_TFT_eSPI::write("); Serial.print(utf8); Serial.println(")");)
#ifdef SMOOTH_FONT
  if (fontLoaded)
  {
    uint16_t unicode = decodeUTF8(utf8);
    if (!unicode) return 0;

    //fontFile = SPIFFS.open( _gFontFilename, "r" );

    if (!fontFile)
    {
      fontLoaded = false;
      return 0;
    }

    drawGlyph(unicode);

    //fontFile.close();
    return 1;
  }
#endif

  uint8_t uniCode = utf8;        // Work with a copy
  if (utf8 == '\n') uniCode += 22; // Make it a valid space character to stop errors
  else if (utf8 < 32) return 0;

  uint16_t width = 0;
  uint16_t height = 0;

  //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  //Serial.print((uint8_t) uniCode); // Debug line sends all printed TFT text to serial port
  //Serial.println(uniCode, HEX); // Debug line sends all printed TFT text to serial port
  //delay(5);                     // Debug optional wait for serial port to flush through
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  if (!gfxFont) {
#endif
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef LOAD_FONT2
    if (textfont == 2)
    {
      DIAG (Serial.print("GxFont_GFX_TFT_eSPI::write("); Serial.print(utf8); Serial.println(") 1");)
      if (utf8 > 127) return 0;
      // This is 20us faster than using the fontdata structure (0.443ms per character instead of 0.465ms)
      width = pgm_read_byte(widtbl_f16 + uniCode - 32);
      height = chr_hgt_f16;
      // Font 2 is rendered in whole byte widths so we must allow for this
      width = (width + 6) / 8;  // Width in whole bytes for font 2, should be + 7 but must allow for font width change
      width = width * 8;        // Width converted back to pixels
    }
#ifdef LOAD_RLE
    else
#endif
#endif

#ifdef LOAD_RLE
    {
      if ((textfont > 2) && (textfont < 9))
      {
        if (utf8 > 127) return 0;
        // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
        // A tad slower than above but this is not significant and is more convenient for the RLE fonts
        width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[textfont].widthtbl ) ) + uniCode - 32 );
        height = pgm_read_byte( &fontdata[textfont].height );
      }
    }
#endif

#ifdef LOAD_GLCD
    if (textfont == 1)
    {
      width =  6;
      height = 8;
    }
#else
    if (textfont == 1) return 0;
#endif

    height = height * textsize;

    if (utf8 == '\n') {
      cursor_y += height;
      cursor_x  = 0;
    }
    else
    {
      if (textwrapX && (cursor_x + width * textsize > _width))
      {
        cursor_y += height;
        cursor_x = 0;
      }
      if (textwrapY && (cursor_y >= _height)) cursor_y = 0;
      cursor_x += drawChar(uniCode, cursor_x, cursor_y, textfont);
    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  } // Custom GFX font
  else
  {
    DIAG (Serial.print("GxFont_GFX_TFT_eSPI::write("); Serial.print(utf8); Serial.println(") 2");)

    if (utf8 == '\n') {
      cursor_x  = 0;
      cursor_y += (int16_t)textsize *
                  (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else {
      if (uniCode > (uint8_t)pgm_read_byte(&gfxFont->last )) return 0;
      if (uniCode < (uint8_t)pgm_read_byte(&gfxFont->first)) return 0;

      uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
      uint8_t   w     = pgm_read_byte(&glyph->width),
                h     = pgm_read_byte(&glyph->height);
      if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
        int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
        if (textwrapX && ((cursor_x + textsize * (xo + w)) > _width)) {
          // Drawing character would go off right edge; wrap to new line
          cursor_x  = 0;
          cursor_y += (int16_t)textsize *
                      (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        if (textwrapY && (cursor_y >= _height)) cursor_y = 0;
        drawChar(cursor_x, cursor_y, uniCode, textcolor, textbgcolor, textsize);
      }
      cursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
    }
  }
#endif // LOAD_GFXFF
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  return 1;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a Unicode onto the screen
***************************************************************************************/
int16_t GxFont_GFX_TFT_eSPI::drawChar(unsigned int uniCode, int x, int y)
{
  return drawChar(uniCode, x, y, textfont);
}

int16_t GxFont_GFX_TFT_eSPI::drawChar(unsigned int uniCode, int x, int y, int font)
{
  DIAG (Serial.print("GxFont_GFX_TFT_eSPI::drawChar("); Serial.print(uniCode); Serial.println(") uniCode");)
  if (font == 1)
  {
#ifdef LOAD_GLCD
#ifndef LOAD_GFXFF
    drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
    return 6 * textsize;
#endif
#else
#ifndef LOAD_GFXFF
    return 0;
#endif
#endif

#ifdef LOAD_GFXFF
    drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
    if (!gfxFont) { // 'Classic' built-in font
#ifdef LOAD_GLCD
      return 6 * textsize;
#else
      return 0;
#endif
    }
    else
    {
      if ((uniCode >= pgm_read_byte(&gfxFont->first)) && (uniCode <= pgm_read_byte(&gfxFont->last) ))
      {
        uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
        return pgm_read_byte(&glyph->xAdvance) * textsize;
      }
      else
      {
        return 0;
      }
    }
#endif
  }

  if ((font > 1) && (font < 9) && ((uniCode < 32) || (uniCode > 127))) return 0;

  int width  = 0;
  int height = 0;
  uint32_t flash_address = 0;
  uniCode -= 32;

#ifdef LOAD_FONT2
  if (font == 2)
  {
    // This is faster than using the fontdata structure
    flash_address = pgm_read_dword(&chrtbl_f16[uniCode]);
    width = pgm_read_byte(widtbl_f16 + uniCode);
    height = chr_hgt_f16;
  }
#ifdef LOAD_RLE
  else
#endif
#endif

#ifdef LOAD_RLE
  {
    if ((font > 2) && (font < 9))
    {
      // This is slower than above but is more convenient for the RLE fonts
      flash_address = pgm_read_dword( pgm_read_dword( &(fontdata[font].chartbl ) ) + uniCode * sizeof(void *) );
      width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[font].widthtbl ) ) + uniCode );
      height = pgm_read_byte( &fontdata[font].height );
    }
  }
#endif

  int w = width;
  int pX      = 0;
  int pY      = y;
  uint8_t line = 0;

#ifdef LOAD_FONT2 // chop out code if we do not need it
  if (font == 2)
  {
    w = w + 6; // Should be + 7 but we need to compensate for width increment
    w = w / 8;
    if (x + width * textsize >= (int16_t)_width) return width * textsize ;

    if (textcolor == textbgcolor || textsize != 1)
    {
      for (int i = 0; i < height; i++)
      {
        if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);

        for (int k = 0; k < w; k++)
        {
          line = pgm_read_byte((uint8_t *)flash_address + w * i + k);
          if (line)
          {
            if (textsize == 1)
            {
              pX = x + k * 8;
              if (line & 0x80) drawPixel(pX, pY, textcolor);
              if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
              if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
              if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
              if (line & 0x08) drawPixel(pX + 4, pY, textcolor);
              if (line & 0x04) drawPixel(pX + 5, pY, textcolor);
              if (line & 0x02) drawPixel(pX + 6, pY, textcolor);
              if (line & 0x01) drawPixel(pX + 7, pY, textcolor);
            }
            else
            {
              pX = x + k * 8 * textsize;
              if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
              if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
              if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x08) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x04) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x02) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x01) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);
            }
          }
        }
        pY += textsize;
      }
    }
    else
    {
      // Faster drawing of characters and background using block write
      //setAddrWindow(x, y, (x + w * 8) - 1, y + height - 1);

      uint8_t mask;
      for (int i = 0; i < height; i++)
      {
        for (int k = 0; k < w; k++)
        {
          line = pgm_read_byte((uint8_t *)flash_address + w * i + k);
          pX = x + k * 8;
          mask = 0x80;
          while (mask)
          {
            if (line & mask)
            {
              //tft_Write_16(textcolor);
              drawPixel(x + k, y + i, textcolor);
            }
            else
            {
              //tft_Write_16(textbgcolor);
              drawPixel(x + k, y + i, textbgcolor);
            }
            mask = mask >> 1;
          }
        }
        pY += textsize;
      }
    }
  }

#ifdef LOAD_RLE
  else
#endif
#endif  //FONT2

#ifdef LOAD_RLE  //674 bytes of code
    // Font is not 2 and hence is RLE encoded
  {
    w *= height; // Now w is total number of pixels in the character
    if ((textsize != 1) || (textcolor == textbgcolor))
    {
      if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);
      int px = 0, py = pY; // To hold character block start and end column and row values
      int pc = 0; // Pixel count
      uint8_t np = textsize * textsize; // Number of pixels in a drawn pixel

      uint8_t tnp = 0; // Temporary copy of np for while loop
      uint8_t ts = textsize - 1; // Temporary copy of textsize
      // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
      // w is total number of pixels to plot to fill character block
      while (pc < w)
      {
        line = pgm_read_byte((uint8_t *)flash_address);
        flash_address++;
        if (line & 0x80)
        {
          line &= 0x7F;
          line++;
          if (ts)
          {
            px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
            py = y + textsize * (pc / width);
          }
          else
          {
            px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
            py = y + pc / width;
          }
          while (line--)
          { // In this case the while(line--) is faster
            pc++; // This is faster than putting pc+=line before while()?
            //setAddrWindow(px, py, px + ts, py + ts);

            if (ts)
            {
              tnp = np;
              while (tnp--)
              {
                //tft_Write_16(textcolor);
                fillRect(px, py, ts, ts, textcolor);
              }
            }
            else
            {
              //tft_Write_16(textcolor);
              drawPixel(px, py, textcolor);
            }
            px += textsize;

            if (px >= (x + width * textsize))
            {
              px = x;
              py += textsize;
            }
          }
        }
        else
        {
          line++;
          pc += line;
        }
      }
    }
    else // Text colour != background && textsize = 1
    {
      // so use faster drawing of characters and background using block write
      //setAddrWindow(x, y, x + width - 1, y + height - 1);
      int dx = 0;
      int dy = 0;

      uint8_t textcolorBin[] = { (uint8_t) (textcolor >> 8), (uint8_t) textcolor };
      uint8_t textbgcolorBin[] = { (uint8_t) (textbgcolor >> 8), (uint8_t) textbgcolor };

      // Maximum font size is equivalent to 180x180 pixels in area
      while (w > 0)
      {
        line = pgm_read_byte((uint8_t *)flash_address++); // 8 bytes smaller when incrementing here
        if (line & 0x80)
        {
          line &= 0x7F;
          line++;
          w -= line;
          drawFastHLine(x + dx, y + dy, line, textcolor);
          dx += line;
          if (dx >= width)
          {
            dy += dx / width;
            dx %= dx;
          }
        }
        else
        {
          line++;
          w -= line;
          drawFastHLine(x + dx, y + dy, line, textbgcolor);
          dx += line;
          if (dx >= width)
          {
            dy += dx / width;
            dx %= dx;
          }
        }
      }
    }
  }
  // End of RLE font rendering
#endif
  return width * textsize;    // x +
}


/***************************************************************************************
** Function name:           drawString (with or without user defined font)
** Description :            draw string with padding if it is defined
***************************************************************************************/
// Without font number, uses font set by setTextFont()
int16_t GxFont_GFX_TFT_eSPI::drawString(const String& string, int poX, int poY)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return drawString(buffer, poX, poY, textfont);
}
// With font number
int16_t GxFont_GFX_TFT_eSPI::drawString(const String& string, int poX, int poY, int font)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return drawString(buffer, poX, poY, font);
}

// Without font number, uses font set by setTextFont()
int16_t GxFont_GFX_TFT_eSPI::drawString(const char *string, int poX, int poY)
{
  return drawString(string, poX, poY, textfont);
}
// With font number
int16_t GxFont_GFX_TFT_eSPI::drawString(const char *string, int poX, int poY, int font)
{
  int16_t sumX = 0;
  uint8_t padding = 1, baseline = 0;
  uint16_t cwidth = textWidth(string, font); // Find the pixel width of the string in the font
  uint16_t cheight = 8 * textsize;

#ifdef LOAD_GFXFF
  if (font == 1) {
    if (gfxFont) {
      cheight = glyph_ab * textsize;
      poY += cheight; // Adjust for baseline datum of free fonts
      baseline = cheight;
      padding = 101; // Different padding method used for Free Fonts

      // We need to make an adjustment for the bottom of the string (eg 'y' character)
      if ((textdatum == BL_DATUM) || (textdatum == BC_DATUM) || (textdatum == BR_DATUM)) {
        cheight += glyph_bb * textsize;
      }
    }
  }
#endif

  if (textdatum || padX)
  {

    // If it is not font 1 (GLCD or free font) get the baseline and pixel height of the font
#ifdef SMOOTH_FONT
    if (fontLoaded) {
      baseline = gFont.maxAscent;
      cheight  = fontHeight(0);
    }

    else
#endif
      if (font != 1) {
        baseline = pgm_read_byte( &fontdata[font].baseline ) * textsize;
        cheight = fontHeight(font);
      }

    switch (textdatum) {
      case TC_DATUM:
        poX -= cwidth / 2;
        padding += 1;
        break;
      case TR_DATUM:
        poX -= cwidth;
        padding += 2;
        break;
      case ML_DATUM:
        poY -= cheight / 2;
        //padding += 0;
        break;
      case MC_DATUM:
        poX -= cwidth / 2;
        poY -= cheight / 2;
        padding += 1;
        break;
      case MR_DATUM:
        poX -= cwidth;
        poY -= cheight / 2;
        padding += 2;
        break;
      case BL_DATUM:
        poY -= cheight;
        //padding += 0;
        break;
      case BC_DATUM:
        poX -= cwidth / 2;
        poY -= cheight;
        padding += 1;
        break;
      case BR_DATUM:
        poX -= cwidth;
        poY -= cheight;
        padding += 2;
        break;
      case L_BASELINE:
        poY -= baseline;
        //padding += 0;
        break;
      case C_BASELINE:
        poX -= cwidth / 2;
        poY -= baseline;
        padding += 1;
        break;
      case R_BASELINE:
        poX -= cwidth;
        poY -= baseline;
        padding += 2;
        break;
    }
    // Check coordinates are OK, adjust if not
    if (poX < 0) poX = 0;
    if (poX + cwidth > _width)   poX = _width - cwidth;
    if (poY < 0) poY = 0;
    if (poY + cheight - baseline > _height) poY = _height - cheight;
  }


  int8_t xo = 0;
#ifdef LOAD_GFXFF
  if ((font == 1) && (gfxFont) && (textcolor != textbgcolor))
  {
    cheight = (glyph_ab + glyph_bb) * textsize;
    // Get the offset for the first character only to allow for negative offsets
    uint8_t   c2    = *string;
    if ((c2 >= pgm_read_byte(&gfxFont->first)) && (c2 <= pgm_read_byte(&gfxFont->last) ))
    {
      c2 -= pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
      xo = pgm_read_byte(&glyph->xOffset) * textsize;
      // Adjust for negative xOffset
      if (xo > 0) xo = 0;
      else cwidth -= xo;
      // Add 1 pixel of padding all round
      //cheight +=2;
      //fillRect(poX+xo-1, poY - 1 - glyph_ab * textsize, cwidth+2, cheight, textbgcolor);
      fillRect(poX + xo, poY - glyph_ab * textsize, cwidth, cheight, textbgcolor);
    }
    padding -= 100;
  }
#endif

#ifdef SMOOTH_FONT
  if (fontLoaded)
  {
    if (textcolor != textbgcolor) fillRect(poX, poY, cwidth, cheight, textbgcolor);
    //drawLine(poX - 5, poY, poX + 5, poY, TFT_GREEN);
    //drawLine(poX, poY - 5, poX, poY + 5, TFT_GREEN);
    //fontFile = SPIFFS.open( _gFontFilename, "r");
    if (!fontFile) return 0;
    uint16_t len = strlen(string);
    uint16_t n = 0;
    setCursor(poX, poY);
    while (n < len)
    {
      uint16_t unicode = decodeUTF8((uint8_t*)string, &n, len - n);
      drawGlyph(unicode);
    }
    sumX += cwidth;
    //fontFile.close();
  }
  else
#endif
    while (*string) sumX += drawChar(*(string++), poX + sumX, poY, font);

  //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  // Switch on debugging for the padding areas
  //#define PADDING_DEBUG

#ifndef PADDING_DEBUG
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  if ((padX > cwidth) && (textcolor != textbgcolor))
  {
    int16_t padXc = poX + cwidth + xo;
#ifdef LOAD_GFXFF
    if ((font == 1) && (gfxFont))
    {
      poX += xo; // Adjust for negative offset start character
      poY -= glyph_ab * textsize;
    }
#endif
    switch (padding) {
      case 1:
        fillRect(padXc, poY, padX - cwidth, cheight, textbgcolor);
        break;
      case 2:
        fillRect(padXc, poY, (padX - cwidth) >> 1, cheight, textbgcolor);
        padXc = (padX - cwidth) >> 1;
        if (padXc > poX) padXc = poX;
        fillRect(poX - padXc, poY, (padX - cwidth) >> 1, cheight, textbgcolor);
        break;
      case 3:
        if (padXc > padX) padXc = padX;
        fillRect(poX + cwidth - padXc, poY, padXc - cwidth, cheight, textbgcolor);
        break;
    }
  }


#else

  //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  // This is debug code to show text (green box) and blanked (white box) areas
  // It shows that the padding areas are being correctly sized and positioned

  if ((padX > sumX) && (textcolor != textbgcolor))
  {
    int16_t padXc = poX + sumX; // Maximum left side padding
#ifdef LOAD_GFXFF
    if ((font == 1) && (gfxFont)) poY -= glyph_ab;
#endif
    drawRect(poX, poY, sumX, cheight, TFT_GREEN);
    switch (padding) {
      case 1:
        drawRect(padXc, poY, padX - sumX, cheight, TFT_WHITE);
        break;
      case 2:
        drawRect(padXc, poY, (padX - sumX) >> 1, cheight, TFT_WHITE);
        padXc = (padX - sumX) >> 1;
        if (padXc > poX) padXc = poX;
        drawRect(poX - padXc, poY, (padX - sumX) >> 1, cheight, TFT_WHITE);
        break;
      case 3:
        if (padXc > padX) padXc = padX;
        drawRect(poX + sumX - padXc, poY, padXc - sumX, cheight, TFT_WHITE);
        break;
    }
  }
#endif
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  return sumX;
}


/***************************************************************************************
** Function name:           drawNumber
** Description:             draw a long integer
***************************************************************************************/
int16_t GxFont_GFX_TFT_eSPI::drawNumber(long long_num, int poX, int poY)
{
  char str[12];
  ltoa(long_num, str, 10);
  return drawString(str, poX, poY, textfont);
}

int16_t GxFont_GFX_TFT_eSPI::drawNumber(long long_num, int poX, int poY, int font)
{
  char str[12];
  ltoa(long_num, str, 10);
  return drawString(str, poX, poY, font);
}


/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat, prints 7 non zero digits maximum
***************************************************************************************/
// Assemble and print a string, this permits alignment relative to a datum
// looks complicated but much more compact and actually faster than using print class
int16_t GxFont_GFX_TFT_eSPI::drawFloat(float floatNumber, int dp, int poX, int poY)
{
  return drawFloat(floatNumber, dp, poX, poY, textfont);
}

int16_t GxFont_GFX_TFT_eSPI::drawFloat(float floatNumber, int dp, int poX, int poY, int font)
{
  char str[14];               // Array to contain decimal string
  uint8_t ptr = 0;            // Initialise pointer for array
  int8_t  digits = 1;         // Count the digits to avoid array overflow
  float rounding = 0.5;       // Round up down delta

  if (dp > 7) dp = 7; // Limit the size of decimal portion

  // Adjust the rounding value
  for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

  if (floatNumber < -rounding)    // add sign, avoid adding - sign to 0.0!
  {
    str[ptr++] = '-'; // Negative number
    str[ptr] = 0; // Put a null in the array as a precaution
    digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
    floatNumber = -floatNumber; // Make positive
  }

  floatNumber += rounding; // Round up or down

  // For error put ... in string and return (all GxFont_GFX_TFT_eSPI library fonts contain . character)
  if (floatNumber >= 2147483647) {
    strcpy(str, "...");
    return drawString(str, poX, poY, font);
  }
  // No chance of overflow from here on

  // Get integer part
  unsigned long temp = (unsigned long)floatNumber;

  // Put integer part into array
  ltoa(temp, str + ptr, 10);

  // Find out where the null is to get the digit count loaded
  while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
  digits += ptr;                  // Count the digits

  str[ptr++] = '.'; // Add decimal point
  str[ptr] = '0';   // Add a dummy zero
  str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

  // Get the decimal portion
  floatNumber = floatNumber - temp;

  // Get decimal digits one by one and put in array
  // Limit digit count so we don't get a false sense of resolution
  uint8_t i = 0;
  while ((i < dp) && (digits < 9)) // while (i < dp) for no limit but array size must be increased
  {
    i++;
    floatNumber *= 10;       // for the next decimal
    temp = floatNumber;      // get the decimal
    ltoa(temp, str + ptr, 10);
    ptr++; digits++;         // Increment pointer and digits count
    floatNumber -= temp;     // Remove that digit
  }

  // Finally we can plot the string and return pixel length
  return drawString(str, poX, poY, font);
}


/***************************************************************************************
** Function name:           setFreeFont
** Descriptions:            Sets the GFX free font to use
***************************************************************************************/

#ifdef LOAD_GFXFF

void GxFont_GFX_TFT_eSPI::setFreeFont(const GFXfont *f)
{
  textfont = 1;
  gfxFont = (GFXfont *)f;

  glyph_ab = 0;
  glyph_bb = 0;
  uint8_t numChars = pgm_read_byte(&gfxFont->last) - pgm_read_byte(&gfxFont->first);

  // Find the biggest above and below baseline offsets
  for (uint8_t c = 0; c < numChars; c++)
  {
    GFXglyph *glyph1  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
    int8_t ab = -pgm_read_byte(&glyph1->yOffset);
    if (ab > glyph_ab) glyph_ab = ab;
    int8_t bb = pgm_read_byte(&glyph1->height) - ab;
    if (bb > glyph_bb) glyph_bb = bb;
  }
}


/***************************************************************************************
** Function name:           setTextFont
** Description:             Set the font for the print stream
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextFont(uint8_t f)
{
  textfont = (f > 0) ? f : 1; // Don't allow font 0
  gfxFont = NULL;
}

#else


/***************************************************************************************
** Function name:           setFreeFont
** Descriptions:            Sets the GFX free font to use
***************************************************************************************/

// Alternative to setTextFont() so we don't need two different named functions
void GxFont_GFX_TFT_eSPI::setFreeFont(uint8_t font)
{
  setTextFont(font);
}


/***************************************************************************************
** Function name:           setTextFont
** Description:             Set the font for the print stream
***************************************************************************************/
void GxFont_GFX_TFT_eSPI::setTextFont(uint8_t f)
{
  textfont = (f > 0) ? f : 1; // Don't allow font 0
}

#endif


#ifdef SMOOTH_FONT
#include "Extensions/Smooth_font.cpp"
#endif


