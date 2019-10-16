// created by Jean-Marc Zingg to be a standalone SSD1283A library (instead of the GxCTRL_SSD1283A class for the GxTFT library)
// code extracts taken from https://github.com/lcdwiki/LCDWIKI_SPI
// code extracts taken from https://github.com/adafruit/Adafruit-GFX-Library
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// note: my only SSD1283A display is write only, read methods of this class don't work

#ifndef _SSD1283A_H_
#define _SSD1283A_H_

#include <Arduino.h>
#include <SPI.h>

#include "GFXcanvas16T.h"

#if defined(__AVR__) || defined(__arm__)
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif

// include the graphics rendering class you want to use (or both)
//#include <LCDWIKI_GUI.h>
#include <Adafruit_GFX.h>

#ifndef _ADAFRUIT_GFX_H
class SSD1283A
#else
class SSD1283A : public Adafruit_GFX
#endif
{
  public:
    SSD1283A(int8_t cs, int8_t cd, int8_t rst, int8_t led);
    // (overridden) virtual methods
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color); 
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color); 
    virtual void fillScreen(uint16_t color);  
    virtual void setRotation(uint8_t r);
    virtual void invertDisplay(bool i);
    // other public methods
    void init(void);
    void setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void pushColors(const uint16_t* data, uint16_t n); // fast one
    void setVerticalScroll(int16_t top, int16_t scrollines, int16_t offset);
    void setBackLight(bool lit);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h);
    void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
    // leftover methods from LCDWIKI_SPI or for LCDWIKI_GUI
    void pushColors(uint16_t * block, int16_t n, bool first, uint8_t flags);
    int16_t getWidth(void) const;
    int16_t getHeight(void) const;
    uint8_t getRotation(void) const;
  private:
    void _startTransaction();
    void _endTransaction();
    void _writeCommand(uint8_t cmd);
    void _writeData(uint8_t data);
    void _writeData16(uint16_t data);
    void _writeData16(uint16_t data, uint16_t n);
    void _writeData16(const uint16_t* data, uint16_t n);
    void _writeCommandData16(uint8_t cmd, uint16_t data);
    void _writeDataTransaction16(uint16_t data);
    void _writeCommandDataTransaction16(uint8_t cmd, uint16_t data);
    void _setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void _init_table16(const void *table, int16_t size);
  protected:
    uint16_t WIDTH, HEIGHT, _width, _height, _rotation;
  private:
    SPISettings _spi_settings;
    int8_t _cs, _cd, _rst, _led;
    uint16_t _inversion_bit;
};

#ifdef _LCDWIKI_GUI_H_

class SSD1283A_GUI: public SSD1283A, public LCDWIKI_GUI
{
  public:
    SSD1283A_GUI(int8_t cs, int8_t cd, int8_t rst, int8_t led) : SSD1283A(cs, cd, rst, led) {};
    uint16_t Color_To_565(uint8_t r, uint8_t g, uint8_t b)
    {
      return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
    }
    void Draw_Pixe(int16_t x, int16_t y, uint16_t color)
    {
      SSD1283A::drawPixel(x, y, color);
    }
    virtual void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
    {
      SSD1283A::fillRect(x, y, w, h, color);
    }
    virtual void Set_Addr_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
    {
      SSD1283A::setWindowAddress(x1, y1, x2, y2);
    }
    virtual void Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags)
    {
      SSD1283A::pushColors(block, n, first, flags);
    }
    virtual int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h) {};
    virtual int16_t Get_Height(void) const
    {
      return _height;
    }
    virtual int16_t Get_Width(void) const
    {
      return _width;
    }
};

#endif

#endif
