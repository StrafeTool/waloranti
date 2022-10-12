#include <Mouse.h>
#include <hiduniversal.h>
#include <SPI.h>
#include "hidmouserptparser.h"

USB Usb;
HIDUniversal Hid(&Usb);
HIDMouseEvents MouEvents;
HIDMouseReportParser Mou(&MouEvents);

void setup() {
  Mouse.begin();
  Serial.begin(115200);
  Serial.setTimeout(0);
  
  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");
    delay(200);

  if (!Hid.SetReportParser(0, &Mou))
    ErrorMessage<uint8_t> (PSTR("SetReportParser"), 1);
}

void mouseMove(long x, long y) {
  long max = max(abs(x), abs(y));
  int count = (int) (max / 127);
  signed char stepX = x / (count + 1);
  signed char stepY = y / (count + 1);
  for (int i = 0; i < count; i++) {
    Mouse.move(stepX, stepY);
  }
  signed char resX = x - (stepX * count);
  signed char resY = y - (stepY * count);
  if (resX != 0 || resY != 0) {
    Mouse.move(resX, resY);
  }
}

void loop() {
  if(Serial.available() > 0) {
    String rawdata = Serial.readString();
    String _x = split(rawdata, ',', 0);
    String _y = split(rawdata, ',', 1);
    
    if((_x != "") && (_y != "")) {
      int x = _x.toInt();
      int y = _y.toInt();
      if (x == 0 & y == 0)
      {
        Mouse.press(MOUSE_LEFT);
        delay(random(5, 10));
        Mouse.release(MOUSE_LEFT);
        delay(1);
      }
      else
      {
        mouseMove(x, y);
      }
    }
   }
  Usb.Task();
}

String split(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
