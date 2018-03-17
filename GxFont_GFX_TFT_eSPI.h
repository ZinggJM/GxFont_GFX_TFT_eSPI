/***************************************************
  Arduino TFT graphics library targeted at ESP8266
  and ESP32 based boards.

  This is a standalone library that contains the
  hardware driver, the graphics functions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce
  their FLASH footprint.

 ****************************************************/

// GxFont_GFX_TFT_eSPI : font rendering graphics library
// extracted and adapted by Jean-Marc Zingg for use with GxEPD and GxEPD2
// code based on https://github.com/Bodmer/TFT_eSPI

// Stop fonts etc being loaded multiple times
#ifndef _GxFont_GFX_TFT_eSPI_H_
#define _GxFont_GFX_TFT_eSPI_H_

// Include header file that defines the fonts loaded
// available and the pins to be used
#include <User_Setup.h>

// Only load the fonts defined in User_Setup.h (to save space)
// Set flag so RLE rendering code is optionally compiled
#ifdef LOAD_GLCD
#include <Fonts/glcdfont.c>
#endif

#ifdef LOAD_FONT2
#include <Fonts/Font16.h>
#endif

#ifdef LOAD_FONT4
#include <Fonts/Font32rle.h>
#define LOAD_RLE
#endif

#ifdef LOAD_FONT6
#include <Fonts/Font64rle.h>
#ifndef LOAD_RLE
#define LOAD_RLE
#endif
#endif

#ifdef LOAD_FONT7
#include <Fonts/Font7srle.h>
#ifndef LOAD_RLE
#define LOAD_RLE
#endif
#endif

#ifdef LOAD_FONT8
#include <Fonts/Font72rle.h>
#ifndef LOAD_RLE
#define LOAD_RLE
#endif
#elif defined LOAD_FONT8N
#define LOAD_FONT8
#include <Fonts/Font72x53rle.h>
#ifndef LOAD_RLE
#define LOAD_RLE
#endif
#endif

#include <Arduino.h>
#include <Print.h>

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

#ifdef SMOOTH_FONT
// Call up the SPIFFS FLASH filing system for the anti-aliased fonts
#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
#include "SPIFFS.h"
#endif
#endif


#ifdef LOAD_GFXFF
// We can include all the free fonts and they will only be built into
// the sketch if they are used

#include <Fonts/GFXFF/gfxfont.h>

// Call up any user custom fonts
#include <User_Setups/User_Custom_Fonts.h>

// Original Adafruit_GFX "Free Fonts"
#include <Fonts/GFXFF/TomThumb.h>  // TT1

#include <Fonts/GFXFF/FreeMono9pt7b.h>  // FF1 or FM9
#include <Fonts/GFXFF/FreeMono12pt7b.h> // FF2 or FM12
#include <Fonts/GFXFF/FreeMono18pt7b.h> // FF3 or FM18
#include <Fonts/GFXFF/FreeMono24pt7b.h> // FF4 or FM24

#include <Fonts/GFXFF/FreeMonoOblique9pt7b.h>  // FF5 or FMO9
#include <Fonts/GFXFF/FreeMonoOblique12pt7b.h> // FF6 or FMO12
#include <Fonts/GFXFF/FreeMonoOblique18pt7b.h> // FF7 or FMO18
#include <Fonts/GFXFF/FreeMonoOblique24pt7b.h> // FF8 or FMO24

#include <Fonts/GFXFF/FreeMonoBold9pt7b.h>  // FF9  or FMB9
#include <Fonts/GFXFF/FreeMonoBold12pt7b.h> // FF10 or FMB12
#include <Fonts/GFXFF/FreeMonoBold18pt7b.h> // FF11 or FMB18
#include <Fonts/GFXFF/FreeMonoBold24pt7b.h> // FF12 or FMB24

#include <Fonts/GFXFF/FreeMonoBoldOblique9pt7b.h>  // FF13 or FMBO9
#include <Fonts/GFXFF/FreeMonoBoldOblique12pt7b.h> // FF14 or FMBO12
#include <Fonts/GFXFF/FreeMonoBoldOblique18pt7b.h> // FF15 or FMBO18
#include <Fonts/GFXFF/FreeMonoBoldOblique24pt7b.h> // FF16 or FMBO24

// Sans serif fonts
#include <Fonts/GFXFF/FreeSans9pt7b.h>  // FF17 or FSS9
#include <Fonts/GFXFF/FreeSans12pt7b.h> // FF18 or FSS12
#include <Fonts/GFXFF/FreeSans18pt7b.h> // FF19 or FSS18
#include <Fonts/GFXFF/FreeSans24pt7b.h> // FF20 or FSS24

#include <Fonts/GFXFF/FreeSansOblique9pt7b.h>  // FF21 or FSSO9
#include <Fonts/GFXFF/FreeSansOblique12pt7b.h> // FF22 or FSSO12
#include <Fonts/GFXFF/FreeSansOblique18pt7b.h> // FF23 or FSSO18
#include <Fonts/GFXFF/FreeSansOblique24pt7b.h> // FF24 or FSSO24

#include <Fonts/GFXFF/FreeSansBold9pt7b.h>  // FF25 or FSSB9
#include <Fonts/GFXFF/FreeSansBold12pt7b.h> // FF26 or FSSB12
#include <Fonts/GFXFF/FreeSansBold18pt7b.h> // FF27 or FSSB18
#include <Fonts/GFXFF/FreeSansBold24pt7b.h> // FF28 or FSSB24

#include <Fonts/GFXFF/FreeSansBoldOblique9pt7b.h>  // FF29 or FSSBO9
#include <Fonts/GFXFF/FreeSansBoldOblique12pt7b.h> // FF30 or FSSBO12
#include <Fonts/GFXFF/FreeSansBoldOblique18pt7b.h> // FF31 or FSSBO18
#include <Fonts/GFXFF/FreeSansBoldOblique24pt7b.h> // FF32 or FSSBO24

// Serif fonts
#include <Fonts/GFXFF/FreeSerif9pt7b.h>  // FF33 or FS9
#include <Fonts/GFXFF/FreeSerif12pt7b.h> // FF34 or FS12
#include <Fonts/GFXFF/FreeSerif18pt7b.h> // FF35 or FS18
#include <Fonts/GFXFF/FreeSerif24pt7b.h> // FF36 or FS24

#include <Fonts/GFXFF/FreeSerifItalic9pt7b.h>  // FF37 or FSI9
#include <Fonts/GFXFF/FreeSerifItalic12pt7b.h> // FF38 or FSI12
#include <Fonts/GFXFF/FreeSerifItalic18pt7b.h> // FF39 or FSI18
#include <Fonts/GFXFF/FreeSerifItalic24pt7b.h> // FF40 or FSI24

#include <Fonts/GFXFF/FreeSerifBold9pt7b.h>  // FF41 or FSB9
#include <Fonts/GFXFF/FreeSerifBold12pt7b.h> // FF42 or FSB12
#include <Fonts/GFXFF/FreeSerifBold18pt7b.h> // FF43 or FSB18
#include <Fonts/GFXFF/FreeSerifBold24pt7b.h> // FF44 or FSB24

#include <Fonts/GFXFF/FreeSerifBoldItalic9pt7b.h>  // FF45 or FSBI9
#include <Fonts/GFXFF/FreeSerifBoldItalic12pt7b.h> // FF46 or FSBI12
#include <Fonts/GFXFF/FreeSerifBoldItalic18pt7b.h> // FF47 or FSBI18
#include <Fonts/GFXFF/FreeSerifBoldItalic24pt7b.h> // FF48 or FSBI24

#endif // #ifdef LOAD_GFXFF

//These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right
#define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
#define C_BASELINE 10 // Centre character baseline
#define R_BASELINE 11 // Right character baseline


// New color definitions use for all my libraries
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F

// Next is a special 16 bit colour value that encodes to 8 bits
// and will then decode back to the same 16 bit value.
// Convenient for 8 bit and 16 bit transparent sprites.
#define TFT_TRANSPARENT 0x0120

// This is a structure to conveniently hold information on the default fonts
// Stores pointer to font character image address table, width table and height

// Create a null set in case some fonts not used (to prevent crash)
const  uint8_t widtbl_null[1] = {0};
PROGMEM const uint8_t chr_null[1] = {0};
PROGMEM const uint8_t* const chrtbl_null[1] = {chr_null};

typedef struct {
  const uint8_t *chartbl;
  const uint8_t *widthtbl;
  uint8_t height;
  uint8_t baseline;
} fontinfo;

// Now fill the structure
const PROGMEM fontinfo fontdata [] = {
#ifdef LOAD_GLCD
  { (const uint8_t *)font, widtbl_null, 0, 0 },
#else
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
#endif
  // GLCD font (Font 1) does not have all parameters
  { (const uint8_t *)chrtbl_null, widtbl_null, 8, 7 },

#ifdef LOAD_FONT2
  { (const uint8_t *)chrtbl_f16, widtbl_f16, chr_hgt_f16, baseline_f16},
#else
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
#endif

  // Font 3 current unused
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

#ifdef LOAD_FONT4
  { (const uint8_t *)chrtbl_f32, widtbl_f32, chr_hgt_f32, baseline_f32},
#else
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
#endif

  // Font 5 current unused
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

#ifdef LOAD_FONT6
  { (const uint8_t *)chrtbl_f64, widtbl_f64, chr_hgt_f64, baseline_f64},
#else
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
#endif

#ifdef LOAD_FONT7
  { (const uint8_t *)chrtbl_f7s, widtbl_f7s, chr_hgt_f7s, baseline_f7s},
#else
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
#endif

#ifdef LOAD_FONT8
  { (const uint8_t *)chrtbl_f72, widtbl_f72, chr_hgt_f72, baseline_f72}
#else
  { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 }
#endif
};


// Class functions and variables
class GxFont_GFX_TFT_eSPI : public Print 
{
  public:
    GxFont_GFX_TFT_eSPI(int16_t _W, int16_t _H);

    virtual void drawPixel(uint32_t x, uint32_t y, uint32_t color) = 0;
    virtual void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) = 0;
    virtual void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) = 0;

    void drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg, uint8_t size);

    int16_t drawChar(unsigned int uniCode, int x, int y, int font);
    int16_t drawChar(unsigned int uniCode, int x, int y);

    void setCursor(int16_t x, int16_t y);
    void setCursor(int16_t x, int16_t y, uint8_t font);
    void setTextColor(uint16_t color);
    void setTextColor(uint16_t fgcolor, uint16_t bgcolor);
    void setTextSize(uint8_t size);

    void setTextWrap(boolean wrapX, boolean wrapY = false);
    void setTextDatum(uint8_t datum);
    void setTextPadding(uint16_t x_width);

#ifdef LOAD_GFXFF
    void setFreeFont(const GFXfont *f = NULL);
    void setTextFont(uint8_t font);
#else
    void setFreeFont(uint8_t font);
    void setTextFont(uint8_t font);
#endif

    uint8_t getRotation(void);
    uint8_t getTextDatum(void);
    uint8_t color16to8(uint16_t color565); // Convert 16 bit colour to 8 bits

    int16_t getCursorX(void) const;
    int16_t getCursorY(void) const;

    uint16_t fontsLoaded(void);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    uint16_t color8to16(uint8_t color332);  // Convert 8 bit colour to 16 bits

    int16_t drawNumber(long long_num, int poX, int poY, int font);
    int16_t drawNumber(long long_num, int poX, int poY);
    int16_t drawFloat(float floatNumber, int decimal, int poX, int poY, int font);
    int16_t drawFloat(float floatNumber, int decimal, int poX, int poY);

    // Handle char arrays
    int16_t drawString(const char *string, int poX, int poY, int font);
    int16_t drawString(const char *string, int poX, int poY);

    // Handle String type
    int16_t drawString(const String& string, int poX, int poY, int font);
    int16_t drawString(const String& string, int poX, int poY);

    int16_t height(void);
    int16_t width(void);
    int16_t textWidth(const char *string, int font);
    int16_t textWidth(const char *string);
    int16_t textWidth(const String& string, int font);
    int16_t textWidth(const String& string);
    int16_t fontHeight(int16_t font);

    size_t write(uint8_t);

    int32_t  cursor_x, cursor_y;
    uint32_t textcolor, textbgcolor;

  protected:

    int32_t  padX;

    uint32_t _init_width, _init_height; // Display w/h as input, used by setRotation()
    uint32_t _width, _height; // Display w/h as modified by current rotation
    uint32_t fontsloaded;

    uint8_t  glyph_ab,  // glyph height above baseline
             glyph_bb,  // glyph height below baseline
             textfont,  // Current selected font
             textsize,  // Current font size multiplier
             textdatum, // Text reference datum
             rotation;  // Display rotation (0-3)

    bool     textwrapX, textwrapY;   // If set, 'wrap' text at right and optionally bottom edge of display

#ifdef LOAD_GFXFF
    GFXfont  *gfxFont;
#endif

    // Load the Anti-aliased font extension
#ifdef SMOOTH_FONT
#include "Extensions/Smooth_font.h"
#endif

}; // End of class GxFont_GFX_TFT_eSPI

#endif
