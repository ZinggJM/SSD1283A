// created by Jean-Marc Zingg to be a standalone SSD1283A library (instead of the GxCTRL_SSD1283A class for the GxTFT library)
// code extracts taken from https://github.com/lcdwiki/LCDWIKI_SPI
// code extracts taken from https://github.com/adafruit/Adafruit-GFX-Library
//
// License: GNU GENERAL PUBLIC LICENSE V3, see LICENSE
//
// note: my only SSD1283A display is write only, read methods of this class don't work

#include <SPI.h>
#include "SSD1283A.h"

#define SPI_SPEED 27000000
//#define SPI_SPEED 4000000

#define TFTLCD_DELAY16  0xFFFF

#ifndef _ADAFRUIT_GFX_H
SSD1283A::SSD1283A(int8_t cs, int8_t cd, int8_t rst, int8_t led) : _spi_settings(SPI_SPEED, MSBFIRST, SPI_MODE0)
#else
SSD1283A::SSD1283A(int8_t cs, int8_t cd, int8_t rst, int8_t led) : Adafruit_GFX(130, 130), _spi_settings(SPI_SPEED, MSBFIRST, SPI_MODE0)
#endif
{
  _cs = cs;
  _cd = cd;
  _rst = rst;
  _led = led;
  _inversion_bit = 0;
  digitalWrite(_cs, HIGH);
  digitalWrite(_cd, HIGH);
  pinMode(cs, OUTPUT);	  // Enable outputs
  pinMode(cd, OUTPUT);

  if (rst >= 0)
  {
    digitalWrite(rst, HIGH);
    pinMode(rst, OUTPUT);
  }
  if (led >= 0)
  {
    pinMode(led, OUTPUT);
  }
  _rotation = 0;
  WIDTH = 130;
  HEIGHT = 130;
  _width = WIDTH;
  _height = HEIGHT;
}

// *** (overridden) virtual methods ***

void SSD1283A::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
  {
    return;
  }
  _startTransaction();
  _setWindowAddress(x, y, x, y);
  _writeData16(color);
  _endTransaction();
}

void SSD1283A::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  //  if ((x < 0) || (y < 0) || (w < 1) || (h < 1) || (x + w > _width) || (y + h > _height))
  //  {
  //    Serial.print("fillRect("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(", "); Serial.print(w); Serial.print(", "); Serial.print(h); Serial.println(") oops? "); delay(1);
  //  }
  // a correct clipping is the goal. try to achieve this
  if (x < 0) w += x, x = 0;
  if (y < 0) h += y, y = 0;
  if (x + w > _width) w = _width - x;
  if (y + h > _height) h = _height - y;
  if ((w < 1) || (h < 1)) return;
  _startTransaction();
  _setWindowAddress(x, y, x + w - 1, y + h - 1);
  _writeCommand(0x22);
  _writeData16(color, w * h);
  _endTransaction();
}

void SSD1283A::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  fillRect(x, y, 1, h, color);
}

void SSD1283A::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  fillRect(x, y, w, 1, color);
}

void SSD1283A::fillScreen(uint16_t color)
{
  fillRect(0, 0, WIDTH, HEIGHT, color);
}

void SSD1283A::setRotation(uint8_t r)
{
#ifdef _ADAFRUIT_GFX_H
  Adafruit_GFX::setRotation(r);
#endif
  _rotation = r & 3;
  _width = (_rotation & 1) ? HEIGHT : WIDTH;
  _height = (_rotation & 1) ? WIDTH : HEIGHT;
  _startTransaction();
  switch (_rotation)
  {
    // reg(0x01) bit RL 0x0100 doesn't work
    case 0:
      _writeCommandData16(0x01, _inversion_bit | 0x2183);
      _writeCommandData16(0x03, 0x6830);
      break;
    case 1:
      _writeCommandData16(0x01, _inversion_bit | 0x2283);
      _writeCommandData16(0x03, 0x6808);
      break;
    case 2:
      _writeCommandData16(0x01, _inversion_bit | 0x2183);
      _writeCommandData16(0x03, 0x6800);
      break;
    case 3:
      _writeCommandData16(0x01, _inversion_bit | 0x2283);
      _writeCommandData16(0x03, 0x6838);
      break;
  }
  _endTransaction();
  setWindowAddress(0, 0, _width - 1, _height - 1);
  setVerticalScroll(0, HEIGHT, 0);
}

void SSD1283A::invertDisplay(bool i)
{
  _inversion_bit = i ? 0x0800 : 0x0000;
  setRotation(_rotation);
}

// *** other public methods ***

void SSD1283A::init(void)
{
  digitalWrite(_cs, HIGH);
  SPI.begin();
  //SPI.beginTransaction( { SPI_SPEED, MSBFIRST, SPI_MODE0 } );
  if (_rst >= 0)
  {
    digitalWrite(_rst, LOW);
    delay(2);
    digitalWrite(_rst, HIGH);
  }
  setBackLight(true);
  delay(200);
  static const uint16_t SSD1283A_regValues[] PROGMEM =
  {
    0x10, 0x2F8E,
    0x11, 0x000C,
    0x07, 0x0021,
    0x28, 0x0006,
    0x28, 0x0005,
    0x27, 0x057F,
    0x29, 0x89A1,
    0x00, 0x0001,
    TFTLCD_DELAY16, 100,
    0x29, 0x80B0,
    TFTLCD_DELAY16, 30,
    0x29, 0xFFFE,
    0x07, 0x0223,
    TFTLCD_DELAY16, 30,
    0x07, 0x0233,
    0x01, 0x2183,
    0x03, 0x6830,
    0x2F, 0xFFFF,
    0x2C, 0x8000,
    0x27, 0x0570,
    0x02, 0x0300,
    0x0B, 0x580C,
    0x12, 0x0609,
    0x13, 0x3100,
  };
  _init_table16(SSD1283A_regValues, sizeof(SSD1283A_regValues));
  setRotation(_rotation);
  invertDisplay(false);
}

void SSD1283A::setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  _startTransaction();
  _setWindowAddress(x1, y1, x2, y2);
  _endTransaction();
}

void SSD1283A::pushColors(const uint16_t* data, uint16_t n)
{
  _startTransaction();
  _writeData16(data, n);
  _endTransaction();
}

void SSD1283A::setVerticalScroll(int16_t top, int16_t scrollines, int16_t offset)
{
  int16_t bfa = HEIGHT - top - scrollines;
  int16_t vsp;
  int16_t sea = top;
  if (offset <= -scrollines || offset >= scrollines)
  {
    offset = 0; //valid scroll
  }
  vsp = top + offset; // vertical start position
  if (offset < 0)
  {
    vsp += scrollines;          //keep in unsigned range
  }
  sea = top + scrollines - 1;
  _writeCommandDataTransaction16(0x41, vsp);
}

void SSD1283A::setBackLight(bool lit)
{
  if (_led >= 0) digitalWrite(_led, lit ? HIGH : LOW);
}

uint16_t SSD1283A::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// code extract taken from Adafruit_SPITFT::drawRGBBitmap https://github.com/adafruit/Adafruit-GFX-Library
void SSD1283A::drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h)
{
  if (!pcolors) return; // canvas might be empty
  int16_t x2, y2; // Lower-right coord
  if (( x             >= _width ) ||     // Off-edge right
      ( y             >= _height) ||      // " top
      ((x2 = (x + w - 1)) <  0      ) ||  // " left
      ((y2 = (y + h - 1)) <  0)     ) return; // " bottom

  int16_t bx1 = 0, by1 = 0; // Clipped top-left within bitmap
  int16_t saveW = w;        // Save original bitmap width value
  if (x < 0) // Clip left
  {
    w  +=  x;
    bx1 = -x;
    x   =  0;
  }
  if (y < 0) // Clip top
  {
    h  +=  y;
    by1 = -y;
    y   =  0;
  }
  if (x2 >= _width ) w = _width  - x; // Clip right
  if (y2 >= _height) h = _height - y; // Clip bottom

  pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
  _startTransaction();
  _setWindowAddress(x, y, x + w - 1, y + h - 1); // Clipped area
  while (h--) // For each (clipped) scanline...
  {
    _writeData16(pcolors, w); // Push one (clipped) row
    pcolors += saveW; // Advance pointer by one full (unclipped) line
  }
  _endTransaction();
}

// code extract taken from Adafruit_SPITFT::drawRGBBitmap https://github.com/adafruit/Adafruit-GFX-Library
void SSD1283A::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h)
{
  const uint16_t* pcolors = bitmap;
  int16_t x2, y2; // Lower-right coord
  if (( x             >= _width ) ||     // Off-edge right
      ( y             >= _height) ||      // " top
      ((x2 = (x + w - 1)) <  0      ) ||  // " left
      ((y2 = (y + h - 1)) <  0)     ) return; // " bottom

  int16_t bx1 = 0, by1 = 0; // Clipped top-left within bitmap
  int16_t saveW = w;        // Save original bitmap width value
  if (x < 0) // Clip left
  {
    w  +=  x;
    bx1 = -x;
    x   =  0;
  }
  if (y < 0) // Clip top
  {
    h  +=  y;
    by1 = -y;
    y   =  0;
  }
  if (x2 >= _width ) w = _width  - x; // Clip right
  if (y2 >= _height) h = _height - y; // Clip bottom

  pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
  _startTransaction();
  _setWindowAddress(x, y, x + w - 1, y + h - 1); // Clipped area
  while (h--) // For each (clipped) scanline...
  {
    for (int16_t i = 0; i < w; i++) // Push one (clipped) row
    {
      _writeData16(pgm_read_word(pcolors + i));
    }
    pcolors += saveW; // Advance pointer by one full (unclipped) line
  }
  _endTransaction();
}

// *** leftover methods from LCDWIKI_SPI or for LCDWIKI_GUI ***

void SSD1283A::pushColors(uint16_t * block, int16_t n, bool first, uint8_t flags)
{
  uint16_t color;
  uint8_t h, l;
  bool isconst = flags & 1;
  //  bool isbigend = (flags & 2) != 0;
  _startTransaction();
  if (first)
  {
    _writeCommand(0x22);
  }
  while (n-- > 0)
  {
    if (isconst)
    {
      color = pgm_read_word(block++);
    }
    else
    {
      color = (*block++);

    }
    _writeData16(color);
  }
  _endTransaction();
}

int16_t SSD1283A::getWidth(void) const
{
  return _width;
}

int16_t SSD1283A::getHeight(void) const
{
  return _height;
}

uint8_t SSD1283A::getRotation(void) const
{
  return _rotation;
}

// *** private methods

void SSD1283A::_startTransaction()
{
  SPI.beginTransaction(_spi_settings);
  if (_cs >= 0) digitalWrite(_cs, LOW);
}

void SSD1283A::_endTransaction()
{
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void SSD1283A::_writeCommand(uint8_t cmd)
{
  digitalWrite(_cd, LOW);
  SPI.transfer(cmd);
  digitalWrite(_cd, HIGH);
}

void SSD1283A::_writeData(uint8_t data)
{
  digitalWrite(_cd, HIGH);
  SPI.transfer(data);
}

void SSD1283A::_writeData16(uint16_t data)
{
  digitalWrite(_cd, HIGH);
#if (defined (ESP8266) || defined(ESP32)) && true // faster
  SPI.write16(data);
#else
  SPI.transfer(data >> 8);
  SPI.transfer(data);
#endif
}

void SSD1283A::_writeData16(uint16_t data, uint16_t n)
{
  digitalWrite(_cd, HIGH);
#if (defined (ESP8266) || defined(ESP32)) && true // fastest
  uint16_t swapped = ((data << 8) | (data >> 8));
  SPI.writePattern((uint8_t*)&swapped, 2, n);
#elif (defined (ESP8266) || defined(ESP32)) && true // faster
  uint16_t swapped = ((data << 8) | (data >> 8));
  while (n-- > 0)
  {
    SPI.write16(swapped);
  }
#else
  while (n-- > 0)
  {
    SPI.transfer(data >> 8);
    SPI.transfer(data);
  }
#endif
}

void SSD1283A::_writeData16(const uint16_t* data, uint16_t n)
{
  digitalWrite(_cd, HIGH);
#if (defined (ESP8266) || defined(ESP32) || (TEENSYDUINO == 147)) && true // fastest
  static const uint16_t swap_buffer_size = 64; // optimal for ESP8266 SPI
  static const uint16_t max_chunk = swap_buffer_size / 2; // uint16_t's
  uint8_t swap_buffer[swap_buffer_size];
  const uint8_t* p1 = reinterpret_cast<const uint8_t*> (data);
  const uint8_t* p2 = p1 + 1;
  while (n > 0)
  {
    uint16_t chunk = min(max_chunk, n);
    n -= chunk;
    uint8_t* p3 = swap_buffer;
    uint8_t* p4 = p3 + 1;
    uint16_t ncopy = chunk;
    while (ncopy-- > 0)
    {
      *p3 = *p2; p3 += 2; p2 += 2;
      *p4 = *p1; p4 += 2; p1 += 2;
    }
#if (defined (ESP8266) || defined(ESP32))
    SPI.transferBytes(swap_buffer, 0, 2 * chunk);
#else
    SPI.transfer(swap_buffer, 0, 2 * chunk);
#endif
  }
#else
  while (n-- > 0)
  {
    uint16_t color = (*data++);
#if (defined (ESP8266) || defined(ESP32)) && true // faster
    SPI.write16(color);
#else
    SPI.transfer(color >> 8);
    SPI.transfer(color);
#endif
  }
#endif
}

void SSD1283A::_writeCommandData16(uint8_t cmd, uint16_t data)
{
  digitalWrite(_cd, LOW);
  SPI.transfer(cmd);
  digitalWrite(_cd, HIGH);
  SPI.transfer(data >> 8);
  SPI.transfer(data);
}

void SSD1283A::_writeDataTransaction16(uint16_t data)
{
  _startTransaction();
  _writeData16(data);
  _endTransaction();
}

void SSD1283A::_writeCommandDataTransaction16(uint8_t cmd, uint16_t data)
{
  _startTransaction();
  _writeCommandData16(cmd, data);
  _endTransaction();
}

void SSD1283A::_setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  switch (_rotation)
  {
    case 0:
      _writeCommand(0x44);
      _writeData(x2 + 2);
      _writeData(x1 + 2);
      _writeCommand(0x45);
      _writeData(y2 + 2);
      _writeData(y1 + 2);
      _writeCommand(0x21);
      _writeData(y1 + 2);
      _writeData(x1 + 2);
      //Serial.print("_setWindowAddress "); Serial.print(y1 + 2); Serial.print(", "); Serial.println(x1 + 2);
      break;
    case 1:
      _writeCommand(0x44);
      _writeData(HEIGHT - y1 + 1);
      _writeData(HEIGHT - y2 + 1);
      _writeCommand(0x45);
      _writeData(WIDTH - x1 - 1);
      _writeData(WIDTH - x2 - 1);
      _writeCommand(0x21);
      _writeData(WIDTH - x1 - 1);
      _writeData(HEIGHT - y1 + 1);
      //Serial.print("_setWindowAddress "); Serial.print(WIDTH - x1 - 1); Serial.print(", "); Serial.println(HEIGHT - y1 + 1);
      break;
    case 2:
      _writeCommand(0x44);
      _writeData(WIDTH - x1 + 1);
      _writeData(WIDTH - x2 + 1);
      _writeCommand(0x45);
      _writeData(HEIGHT - y1 + 1);
      _writeData(HEIGHT - y2 + 1);
      _writeCommand(0x21);
      _writeData(HEIGHT - y1 + 1);
      _writeData(WIDTH - x1 + 1);
      //Serial.print("_setWindowAddress "); Serial.print(HEIGHT - y1 + 1); Serial.print(", "); Serial.println(WIDTH - x1 + 1);
      break;
    case 3:
      _writeCommand(0x44);
      _writeData(y2 + 2);
      _writeData(y1 + 2);
      _writeCommand(0x45);
      _writeData(x2);
      _writeData(x1);
      _writeCommand(0x21);
      _writeData(x1);
      _writeData(y1 + 2);
      //Serial.print("_setWindowAddress "); Serial.print(x1); Serial.print(", "); Serial.println(y1 + 2);
      break;
  }
  _writeCommand(0x22);
}

void SSD1283A::_init_table16(const void *table, int16_t size)
{
  uint16_t *p = (uint16_t *) table;
  while (size > 0)
  {
    uint16_t cmd = pgm_read_word(p++);
    uint16_t d = pgm_read_word(p++);
    if (cmd == TFTLCD_DELAY16)
    {
      delay(d);
    }
    else
    {
      _writeCommandDataTransaction16(cmd, d);
    }
    size -= 2 * sizeof(int16_t);
  }
}
