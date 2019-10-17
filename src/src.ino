// original code taken from https://github.com/lcdwiki/LCDWIKI_SPI/tree/master/Example/Example_03_display_string/display_string

#include <LCDWIKI_GUI.h> //Core graphics library
#include "SSD1283A.h" //Hardware-specific library

#if (defined(TEENSYDUINO) && (TEENSYDUINO == 147))
// for Mike's Artificial Horizon
SSD1283A_GUI mylcd(/*CS=*/ 10, /*DC=*/ 15, /*RST=*/ 14, /*LED=*/ -1); //hardware spi,cs,cd,reset,led

// for my wirings used for e-paper displays:
#elif defined (ESP8266)
SSD1283A_GUI mylcd(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*LED=D2*/ 4); //hardware spi,cs,cd,reset,led
#elif defined(ESP32)
SSD1283A_GUI mylcd(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*LED=*/ 4); //hardware spi,cs,cd,reset,led
#elif defined(_BOARD_GENERIC_STM32F103C_H_)
SSD1283A_GUI mylcd(/*CS=4*/ SS, /*DC=*/ 3, /*RST=*/ 2, /*LED=*/ 1); //hardware spi,cs,cd,reset,led
#elif defined(__AVR)
SSD1283A_GUI mylcd(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*LED=*/ 7); //hardware spi,cs,cd,reset,led
#else
// catch all other default
SSD1283A_GUI mylcd(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*LED=*/ 7); //hardware spi,cs,cd,reset,led
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
  mylcd.init();
  Serial.println("init() done");
  mylcd.Fill_Screen(BLACK);
  Serial.println("setup done");
  //testTextOnCanvas();
}

void loop()
{
  mylcd.Set_Text_Mode(0);

  for (uint8_t r = 0; r < 4; r++)
  {
    mylcd.setRotation(r);
    mylcd.Fill_Screen(0x0000);
    mylcd.Set_Text_colour(RED);
    mylcd.Set_Text_Back_colour(BLACK);
    mylcd.Set_Text_Size(1);
    mylcd.Print_String("Hello World!", 0, 0);
    mylcd.Print_Number_Float(01234.56789, 2, 0, 8, '.', 0, ' ');
    mylcd.Print_Number_Int(0xDEADBEF, 0, 16, 0, ' ', 16);

    mylcd.Set_Text_colour(GREEN);
    mylcd.Set_Text_Size(2);
    mylcd.Print_String("Hello", 0, 32);
    mylcd.Print_Number_Float(01234.56789, 2, 0, 48, '.', 0, ' ');
    mylcd.Print_Number_Int(0xDEADBEF, 0, 64, 0, ' ', 16);

    mylcd.Set_Text_colour(BLUE);
    mylcd.Set_Text_Size(3);
    mylcd.Print_String("Hello", 0, 86);
    mylcd.Print_Number_Float(01234.56789, 2, 0, 110, '.', 0, ' ');
    mylcd.Print_Number_Int(0xDEADBEF, 0, 134, 0, ' ', 16);

    delay(5000);
  }
  testTextOnCanvas();
}

#if !(defined (ESP8266) || defined (ESP32)) || false

#include "test_canvas.h"

void testTextOnCanvas()
{
  for (uint8_t r = 0; r < 4; r++)
  {
    mylcd.setRotation(r);
    mylcd.Fill_Screen(BLACK);
    uint32_t start = micros();
    uint16_t h = sizeof(test_canvas) / 130 / 2; // reduced size on AVR
    mylcd.drawRGBBitmap(0, 0, test_canvas, 130, h);
    uint32_t elapsed = micros() - start;
    Serial.print(F("drawRGBBitmap(0, 0, test_canvas, 130, h) took ")); Serial.println(elapsed);
    delay(5000);
  }
}

#else

//GFXcanvas16 canvas(130, 130); // uses heap space

// let the linker complain if not enough ram
GFXcanvas16T<130, 130> canvas; // uses dynamic memory space

void show_canvas_on_screen_timed()
{
  uint32_t start = micros();
  //mylcd.setWindowAddress(0, 0, canvas.width() - 1, canvas.height() - 1);
  //mylcd.pushColors(canvas.getBuffer(), canvas.width() * canvas.height()); // 15ms on ESP8266, 13ms on ESP32
  mylcd.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
  uint32_t elapsed = micros() - start;
  Serial.print(F("show_canvas_on_screen    ")); Serial.println(elapsed);
}

void testTextOnCanvas()
{
  canvas.fillScreen(BLACK);
  canvas.setCursor(0, 0);
  canvas.setTextColor(WHITE);  canvas.setTextSize(1);
  canvas.println("Hello World!");
  canvas.setTextColor(YELLOW); canvas.setTextSize(2);
  canvas.println(1234.56);
  canvas.setTextColor(RED);    canvas.setTextSize(3);
  canvas.println(0xDEADBEF, HEX);
  //canvas.println();
  canvas.setTextColor(GREEN);
  canvas.setTextSize(4);
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

  static bool first = true;
  //if (first) canvas.print(Serial, "test_canvas", true);
  first = false;

  for (uint8_t r = 0; r < 4; r++)
  {
    mylcd.setRotation(r);
    //mylcd.Fill_Screen(0x0000);
    show_canvas_on_screen_timed();
    delay(5000);
  }
}
#endif
