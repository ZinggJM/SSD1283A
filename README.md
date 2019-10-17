# SSD1283A for Arduino

This library supports SSD1283A to use with Adafruit_GFX and/or LCDWIKI_GUI.

The code is extracted from LCDWIKI_SPI and adapted for single controller use and Adafruit_GFX.

Only HW SPI is supported.

### Version 1.0.3
- fixed rotation (rendering canvas ok for all 4 rotations)
#### Version 1.0.2
- added template class GFXcanvas16T, uses dynamic memory space instead of heap space
- added method drawRGBBitmap() for PROGMEM bitmaps
#### Version 1.0.1
- added method drawRGBBitmap(), e.g. for fast canvas rendering
- use rotation for drawing to the canvas, but use only rotation 0 or 3 for rendering the canvas to screen.
- the "quick test" example src/src.ino shows the issue
#### Version 1.0.0
- improved rotation somewhat, see general remarks
- fixed clipping (drawPixel, fillRect)
- optimizations and cleanup
#### Version 0.0.6
- fixed rotation
#### Version 0.0.5
- fast pushColors() optimized also for Teensy 3.2
- cleaner implementation of fast pushColors()
#### Version 0.0.4
- fix some copy/past or replace errors
#### Version 0.0.3
- added example graphicstest_through_canvas.ino
- added faster pushColors() method, optimized for ESP32, ESP8266
#### Version 0.0.2
- fixed SPI transaction nesting issue, causing hang on ESP32 and other platforms
- still a preliminary version
#### Version 0.0.1
- initial preliminary version for initial check-in
- works with my "Transflective Display Module LCD Display DIY SPI Serial Port 130*130 Communicate for Arduino"
- https://www.aliexpress.com/item/4000116854157.html
- rotation does not work correctly