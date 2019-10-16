/* Modified by Bodmer to be an example for TFT_HX8357 library.
   This sketch uses the GLCD font only.

   The performance for each test is reported to the serial
   port at 38400 baud.

   This test occupies the whole of the display therefore
   will take twice as long as it would on a 320 x 240
   display. Bear this in mind when making performance
   comparisons.

   Make sure all the required font is loaded by editting the
   User_Setup.h file in the TFT_HX8357 library folder.

   Original header is at the end of the sketch, some text in it is
   not applicable to the HX8357 display supported by this example.
*/

// modified by Jean-Marc Zingg to be an example for the SSD1283A library (from GxTFT library)
// original source taken from https://github.com/Bodmer/TFT_HX8357

#include <SSD1283A.h> //Hardware-specific library

// adapt the constructor parameters to your wiring for the appropriate processor conditional, 
// or add a new one or adapt the catch all other default

#if (defined(TEENSYDUINO) && (TEENSYDUINO == 147))
// for Mike's Artificial Horizon
SSD1283A screen(/*CS=*/ 10, /*DC=*/ 15, /*RST=*/ 14, /*LED=*/ -1); //hardware spi,cs,cd,reset,led

// for my wirings used for e-paper displays:
#elif defined (ESP8266)
SSD1283A screen(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*LED=D2*/ 4); //hardware spi,cs,cd,reset,led
#elif defined(ESP32)
SSD1283A screen(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*LED=*/ 4); //hardware spi,cs,cd,reset,led
#elif defined(_BOARD_GENERIC_STM32F103C_H_)
SSD1283A screen(/*CS=4*/ SS, /*DC=*/ 3, /*RST=*/ 2, /*LED=*/ 1); //hardware spi,cs,cd,reset,led
#elif defined(__AVR)
SSD1283A screen(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*LED=*/ 7); //hardware spi,cs,cd,reset,led
#else
// catch all other default
SSD1283A screen(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*LED=*/ 7); //hardware spi,cs,cd,reset,led
#endif

//GFXcanvas16 canvas(130, 130); // uses heap space

// let the linker complain if not enough ram
GFXcanvas16T<130, 130> canvas; // uses dynamic memory space

void show_canvas_on_screen()
{
  screen.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
}

void show_canvas_on_screen_timed()
{
  uint32_t start = micros();
  screen.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
  uint32_t elapsed = micros() - start;
  Serial.print(F("show_canvas_on_screen    ")); Serial.println(elapsed);
}

#if !defined(ESP8266)
#define yield()
#endif

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  //Serial.println(String(controller.name) + " Test on " + String(io.name));

  screen.init();

  Serial.println("screen.init() done");

  Serial.println(F("Benchmark                Time (microseconds)"));

  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(500);

  show_canvas_on_screen_timed();

  //return;

  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(3000);

  //return;

  Serial.print(F("Lines                    "));
  Serial.println(testLines(CYAN));
  delay(500);

  //return;

  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(RED, BLUE));
  delay(500);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(GREEN));
  delay(500);

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(YELLOW, MAGENTA));
  delay(500);

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, MAGENTA));

  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, WHITE));
  delay(500);

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  delay(500);

  //while(1) yield();

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  delay(500);

  //return;

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(500);

  //while(1) yield();

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(500);

  //while (1) yield();

  //testEllipses();
  //testCurves();

  Serial.println(F("Done!"));

}

void loop(void)
{
  for (uint8_t rotation = 0; rotation < 4; rotation++)
  {
    canvas.setRotation(rotation);
    testText();
    //show_canvas_on_screen(); // not needed, but THIS SHOWED A POTENTIAL ERROR IN THE LIBRARY
    delay(10000);
  }
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  canvas.fillScreen(BLACK);
  show_canvas_on_screen();
  yield();
  //if (controller.ID == 0x8875) delay(200); // too fast to be seen
  canvas.fillScreen(RED);
  show_canvas_on_screen();
  yield();
  //if (controller.ID == 0x8875) delay(200); // too fast to be seen
  canvas.fillScreen(GREEN);
  show_canvas_on_screen();
  yield();
  //if (controller.ID == 0x8875) delay(200); // too fast to be seen
  canvas.fillScreen(BLUE);
  show_canvas_on_screen();
  yield();
  //if (controller.ID == 0x8875) delay(200); // too fast to be seen
  canvas.fillScreen(BLACK);
  show_canvas_on_screen();
  yield();
  return micros() - start;
}

unsigned long testText() {
  canvas.fillScreen(BLACK);
  unsigned long start = micros();
  canvas.setCursor(0, 0);
  canvas.setTextColor(WHITE);  canvas.setTextSize(1);
  canvas.println("Hello World!");
  canvas.setTextColor(YELLOW); canvas.setTextSize(2);
  canvas.println(1234.56);
  canvas.setTextColor(RED);    canvas.setTextSize(3);
  canvas.println(0xDEADBEEF, HEX);
  canvas.println();
  canvas.setTextColor(GREEN);
  canvas.setTextSize(5);
  canvas.println("Groop");
  canvas.setTextSize(2);
  canvas.println("I implore thee,");
  canvas.setTextSize(1);
  canvas.setTextColor(GREEN);
  canvas.println("my foonting turlingdromes.");
  canvas.println("And hooptiously drangle me");
  canvas.println("with crinkly bindlewurdles,");
  canvas.println("Or I will rend thee");
  canvas.println("in the gobberwarts");
  canvas.println("with my blurglecruncheon,");
  canvas.println("see if I don't!");
  show_canvas_on_screen();
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = canvas.width(),
                h = canvas.height();

  canvas.fillScreen(BLACK);
  yield();

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  t     = micros() - start; // fillScreen doesn't count against timing

  canvas.fillScreen(BLACK);
  yield();

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  show_canvas_on_screen();
  t    += micros() - start;

  canvas.fillScreen(BLACK);
  yield();

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  show_canvas_on_screen();
  t    += micros() - start;

  canvas.fillScreen(BLACK);
  yield();

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) canvas.drawLine(x1, y1, x2, y2, color);
  yield();
  show_canvas_on_screen();

  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = canvas.width(), h = canvas.height();

  canvas.fillScreen(BLACK);
  yield();
  start = micros();
  for (y = 0; y < h; y += 5) canvas.drawFastHLine(0, y, w, color1);
  yield();
  for (x = 0; x < w; x += 5) canvas.drawFastVLine(x, 0, h, color2);
  yield();
  show_canvas_on_screen();

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = canvas.width()  / 2,
                cy = canvas.height() / 2;

  canvas.fillScreen(BLACK);
  n     = min(canvas.width(), canvas.height());
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    canvas.drawRect(cx - i2, cy - i2, i, i, color);
  }
  show_canvas_on_screen();

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = canvas.width()  / 2 - 1,
                cy = canvas.height() / 2 - 1;

  canvas.fillScreen(BLACK);
  n = min(canvas.width(), canvas.height());
  for (i = n; i > 0; i -= 6) {
    i2    = i / 2;
    start = micros();
    canvas.fillRect(cx - i2, cy - i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    canvas.drawRect(cx - i2, cy - i2, i, i, color2);
    yield();
  }
  show_canvas_on_screen();

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = canvas.width(), h = canvas.height(), r2 = radius * 2;

  canvas.fillScreen(BLACK);
  start = micros();
  for (x = radius; x < w; x += r2) {
    for (y = radius; y < h; y += r2) {
      canvas.fillCircle(x, y, radius, color);
    }
    yield();
  }
  show_canvas_on_screen();

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                      w = canvas.width()  + radius,
                      h = canvas.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for (x = 0; x < w; x += r2) {
    for (y = 0; y < h; y += r2) {
      canvas.drawCircle(x, y, radius, color);
    }
    yield();
  }
  show_canvas_on_screen();

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = canvas.width()  / 2 - 1,
                      cy = canvas.height() / 2 - 1;

  canvas.fillScreen(BLACK);
  n     = min(cx, cy);
  start = micros();
  for (i = 0; i < n; i += 5) {
    canvas.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      screen.color565(0, 0, i));
  }
  show_canvas_on_screen();

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = canvas.width()  / 2 - 1,
                   cy = canvas.height() / 2 - 1;

  canvas.fillScreen(BLACK);
  start = micros();
  for (i = min(cx, cy); i > 10; i -= 5) {
    start = micros();
    canvas.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                        screen.color565(0, i, i));
    t += micros() - start;
    canvas.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                        screen.color565(i, i, 0));
    yield();
  }
  show_canvas_on_screen();

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = canvas.width()  / 2 - 1,
                cy = canvas.height() / 2 - 1;

  canvas.fillScreen(BLACK);
  w     = min(canvas.width(), canvas.height());
  start = micros();
  for (i = 0; i < w; i += 6) {
    i2 = i / 2;
    canvas.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, screen.color565(i, 0, 0));
  }
  show_canvas_on_screen();

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = canvas.width()  / 2 - 1,
                cy = canvas.height() / 2 - 1;

  canvas.fillScreen(BLACK);
  start = micros();
  for (i = min(canvas.width(), canvas.height()); i > 20; i -= 6) {
    i2 = i / 2;
    canvas.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, screen.color565(0, i, 0));
    yield();
  }
  show_canvas_on_screen();

  return micros() - start;
}

#if 0
void testEllipses()
{
  canvas.fillScreen(BLACK);
  for (int i = 0; i < 40; i++)
  {
    int rx = random(60);
    int ry = random(60);
    int x = rx + random(480 - rx - rx);
    int y = ry + random(320 - ry - ry);
    canvas.fillEllipse(x, y, rx, ry, random(0xFFFF));
  }
  show_canvas_on_screen();
  delay(2000);
  canvas.fillScreen(BLACK);
  for (int i = 0; i < 40; i++)
  {
    int rx = random(60);
    int ry = random(60);
    int x = rx + random(480 - rx - rx);
    int y = ry + random(320 - ry - ry);
    canvas.drawEllipse(x, y, rx, ry, random(0xFFFF));
  }
  show_canvas_on_screen();
  delay(2000);
}

void testCurves()
{
  uint16_t x = canvas.width() / 2;
  uint16_t y = canvas.height() / 2;
  canvas.fillScreen(BLACK);

  canvas.drawEllipse(x, y, 100, 60, PURPLE);
  canvas.fillCurve(x, y, 80, 30, 0, CYAN);
  canvas.fillCurve(x, y, 80, 30, 1, MAGENTA);
  canvas.fillCurve(x, y, 80, 30, 2, YELLOW);
  canvas.fillCurve(x, y, 80, 30, 3, RED);
  delay(2000);
  show_canvas_on_screen();

  canvas.drawCurve(x, y, 90, 50, 0, CYAN);
  canvas.drawCurve(x, y, 90, 50, 1, MAGENTA);
  canvas.drawCurve(x, y, 90, 50, 2, YELLOW);
  canvas.drawCurve(x, y, 90, 50, 3, RED);
  canvas.fillCircle(x, y, 30, BLUE);
  delay(5000);
  show_canvas_on_screen();
}
#endif

// Original sketch header
/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
