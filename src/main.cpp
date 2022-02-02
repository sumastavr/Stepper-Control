#include <Arduino.h>

#include <Wire.h>

#include <M5Core2.h>

#define STEPMOTOR_I2C_ADDR 0x70

void SendByte(byte addr, byte b) {
  Wire1.beginTransmission(addr);
  Wire1.write(b);
  Wire1.endTransmission();
}

void SendCommand(byte addr, char *c) {
  Wire1.beginTransmission(addr);
  while ((*c) != 0) {
    Wire1.write(*c);
    c++;
  }
  Wire1.write(0x0d);
  Wire1.write(0x0a);
  Wire1.endTransmission();
}

// Defines gestures
Gesture swipeRight("swipe right", 160, DIR_RIGHT, 30, true);
Gesture swipeDown("swipe down", 120, DIR_DOWN, 30, true);
Gesture swipeLeft("swipe left", 160, DIR_LEFT, 30, true);
Gesture swipeUp("swipe up", 120, DIR_UP, 30, true);

// Defines the buttons. Colors in format {bg, text, outline}
ButtonColors on_clrs = {RED, WHITE, WHITE};
ButtonColors off_clrs = {BLACK, WHITE, WHITE};

Button plus_X(0, 0, 0, 0, false ,"", off_clrs, on_clrs, TL_DATUM);
Button min_X(0, 0, 0, 0, false, "", off_clrs, on_clrs, TL_DATUM);

Button plus_Y(0, 0, 0, 0, false ,"", off_clrs, on_clrs, TL_DATUM);
Button min_Y(0, 0, 0, 0, false, "", off_clrs, on_clrs, TL_DATUM);

Button plus_Z(0, 0, 0, 0, false ,"", off_clrs, on_clrs, TL_DATUM);
Button min_Z(0, 0, 0, 0, false, "", off_clrs, on_clrs, TL_DATUM);

Button plus_Step(0, 0, 0, 0, false ,"", off_clrs, on_clrs, TL_DATUM);
Button min_Step(0, 0, 0, 0, false, "", off_clrs, on_clrs, TL_DATUM);

Button plus_Speed(0, 0, 0, 0, false ,"", off_clrs, on_clrs, TL_DATUM);
Button min_Speed(0, 0, 0, 0, false, "", off_clrs, on_clrs, TL_DATUM);

// For performance measurement (Single tap on bottom-right button)
uint32_t startTime;
uint32_t times = 0;

// Positions the buttons and draws them. (Only because height and width
// change. If we were only switching between normal and upside-down we
// would only need the Buttons.draw() here.)
void doButtons() {

  /*
  int16_t hw = M5.Lcd.width() / 2;
  int16_t hh = M5.Lcd.height() / 2;

  tl.set(0, 0, hw - 5, hh - 5);
  bl.set(0, hh + 5, hw - 5, hh - 5);
  tr.set(hw + 5, 0, hw - 5, hh - 5);
  br.set(hw + 5, hh + 5, hw - 5, hh - 5);
*/

  //plus_X.set(M5.Lcd.width()/2+10,10,110,50);
  min_X.set(5,0,110,40);
  min_Y.set(5,50,110,40);
  min_Z.set(5,100,110,40);
  min_Step.set(5,150,110,40);
  min_Speed.set(5,200,110,40);
 
  min_X.setTextSize(2);
  min_X.setLabel("   x-");

  min_Y.setTextSize(2);
  min_Y.setLabel("   y-");

  min_Z.setTextSize(2);
  min_Z.setLabel("   z-");

  min_Step.setTextSize(2);
  min_Step.setLabel(" Step-");

  min_Speed.setTextSize(2);
  min_Speed.setLabel(" RPM-");

  plus_X.set(M5.Lcd.width()-5-110,0,110,40);
  plus_Y.set(M5.Lcd.width()-5-110,50,110,40);
  plus_Z.set(M5.Lcd.width()-5-110,100,110,40);
  plus_Step.set(M5.Lcd.width()-5-110,150,110,40);
  plus_Speed.set(M5.Lcd.width()-5-110,200,110,40);
 
  plus_X.setTextSize(2);
  plus_X.setLabel("   x+");

  plus_Y.setTextSize(2);
  plus_Y.setLabel("   y+");

  plus_Z.setTextSize(2);
  plus_Z.setLabel("   z+");

  plus_Step.setTextSize(2);
  plus_Step.setLabel("Step+");

  plus_Speed.setTextSize(2);
  plus_Speed.setLabel("RPM+");

  M5.Buttons.draw();
}

void doRotation(Event& e) {
  // Gestures and Buttons have an instanceIndex() that starts at zero
  // so by defining the gestures in the right order I can use that as
  // the input for M5.Lcd.setRotation.
  uint8_t new_rotation = e.gesture->instanceIndex();
  if (new_rotation != M5.Lcd.rotation) {
    M5.Lcd.clearDisplay();
    M5.Lcd.setRotation(new_rotation);
    doButtons();
  }
}

void toggleColor(Event& e) {
  // Just so we can type "b." instead of "e.button->"
  Button& b = *e.button;

  if (b != M5.background) {
    // Toggles the button color between black and blue
    b.off.bg = (b.off.bg == BLACK) ? BLUE : BLACK;
    b.draw();
  }
}

void showPerformance(Event& e) {
  Serial.printf("%d in %d ms, average M5.update() took %.2f microseconds\n",
   times, millis() - startTime, (float)((millis() - startTime) * 1000) / times);
  startTime = millis();
  times = 0;
}

void eventDisplay(Event& e) {
  Serial.printf("%-12s finger%d  %-18s (%3d, %3d) --> (%3d, %3d)   ",
                e.typeName(), e.finger, e.objName(), e.from.x, e.from.y,
                e.to.x, e.to.y);
  Serial.printf("( dir %d deg, dist %d, %d ms )\n", e.direction(),
                e.distance(), e.duration);
}


float step=0.03;
int speed=5;

String strStep=String(step,2);
String strSpeed=String(speed);

float currentX,currentY,currentZ;

void setup() {

  M5.begin(true,true,true,true,kMBusModeInput);

  Wire1.begin(21,22);

  //M5.Buttons.addHandler(doRotation, E_GESTURE);
  //M5.Buttons.addHandler(toggleColor, E_DBLTAP);
  M5.Buttons.addHandler(eventDisplay, E_ALL - E_MOVE);
  //br.addHandler(showPerformance, E_TAP);
  //br.repeatDelay = 1000;
  doButtons();
  startTime = millis();
  Serial.begin(115200);

  //M5.Lcd.setFreeFont(&unicode_24px);  //Set the GFX font to use.  设置要使用的GFX字体
  M5.Lcd.setTextDatum(TC_DATUM);  //Set text alignment to center-up alignment. 设置文本对齐方式为居中向上对齐

  step=0.05;
  speed=10;

  strStep=String(step,2);
  strSpeed=String(speed);

}

void updateValues(){

  M5.Lcd.fillRect(125,0,80,250,BLACK);
  M5.Lcd.setCursor(125, 30); //Move the cursor position to (x,y).  移动光标位置到 (x,y)处
  M5.Lcd.setTextColor(WHITE); //Set the font color to white.  设置字体颜色为白色
  M5.Lcd.setTextSize(2);  //Set the font size.  设置字体大小
  M5.Lcd.print(String(currentX,1));  //Serial output format string.  输出格式化字符串
  
  M5.Lcd.setCursor(125, 80); //Move the cursor position to (x,y).  移动光标位置到 (x,y)处
  M5.Lcd.print(String(currentY,1));  //Serial output format string.  输出格式化字符串

  M5.Lcd.setCursor(125, 130); //Move the cursor position to (x,y).  移动光标位置到 (x,y)处
  M5.Lcd.print(String(currentZ,1));  //Serial output format string.  输出格式化字符串

  M5.Lcd.setCursor(125, 180); //Move the cursor position to (x,y).  移动光标位置到 (x,y)处
  M5.Lcd.print(String(step,2));  //Serial output format string.  输出格式化字符串

  M5.Lcd.setCursor(125, 230); //Move the cursor position to (x,y).  移动光标位置到 (x,y)处
  M5.Lcd.print(String(speed));  //Serial output format string.  输出格式化字符串

}

void controlMotor(int motor, bool direction){
  updateValues();
  char buf[30];
  String command;
  strStep=String(step,2);
  strSpeed=String(speed,2);

  if(direction){
    if(motor==0){
      SendCommand(STEPMOTOR_I2C_ADDR,"G91");
      command = "G0 X"+strStep+" F"+strSpeed;
      command.toCharArray(buf,command.length());
      SendCommand(STEPMOTOR_I2C_ADDR,buf);
    }
    if(motor==1){
      SendCommand(STEPMOTOR_I2C_ADDR,"G91");
      command = "G0 Y"+strStep+" F"+strSpeed;
      command.toCharArray(buf,command.length());
      SendCommand(STEPMOTOR_I2C_ADDR,buf);
    }
  }else{
    if(motor==0){
      SendCommand(STEPMOTOR_I2C_ADDR,"G91");
      command = "G0 X-"+strStep+" F"+strSpeed;
      command.toCharArray(buf,command.length());
      SendCommand(STEPMOTOR_I2C_ADDR,buf);
    }

    if(motor==1){
      SendCommand(STEPMOTOR_I2C_ADDR,"G91");
      command = "G0 Y-"+strStep+" F"+strSpeed;
      command.toCharArray(buf,command.length());
      SendCommand(STEPMOTOR_I2C_ADDR,buf);
    }
  }

  Serial.println(command);

}

long refreshCounter=millis();

void loop() {

  M5.update();

  if(millis()-refreshCounter>1000){
    updateValues();
    refreshCounter=millis();
  }

  times++;

  if (plus_X.isPressed()){
    currentX+=step;
    controlMotor(0,1);
    delay(5);
  }else if(min_X.isPressed()){
    currentX-=step;
    controlMotor(0,0);
    delay(5);
  }

  if (plus_Y.isPressed()){
    currentY+=step;
    controlMotor(1,0);
    delay(5);
  }else if(min_Y.isPressed()){
    currentY-=step;
    controlMotor(1,1);
    delay(5);
  }

  if (plus_Step.isPressed()){
    //controlMotor(0,1);
    step+=0.01;
    updateValues();
    delay(25);
  }else if(min_Step.isPressed()){
    //controlMotor(0,0);
    step-=0.01;
    updateValues();
    delay(25);
  }

  if (plus_Speed.isPressed()){
    //controlMotor(0,1);
    speed++;
    updateValues();
    delay(25);
  }else if(min_Speed.isPressed()){
    //controlMotor(0,0);
    speed--;
    updateValues();
    delay(25);
  }


}




/*
#define STEPMOTOR_I2C_ADDR 0x70
// #define STEPMOTOR_I2C_ADDR 0x71


void SendByte(byte addr, byte b) {
  Wire1.beginTransmission(addr);
  Wire1.write(b);
  Wire1.endTransmission();
}

void SendCommand(byte addr, char *c) {
  Wire1.beginTransmission(addr);
  while ((*c) != 0) {
    Wire1.write(*c);
    c++;
  }
  Wire1.write(0x0d);
  Wire1.write(0x0a);
  Wire1.endTransmission();
}

void setup() {
  M5.begin(true,true,true,true,kMBusModeInput);

  Wire1.begin(21,22);
  m5.Lcd.setTextColor(WHITE, BLACK);
  m5.Lcd.setTextSize(2);
  m5.lcd.setBrightness(100);
  M5.Lcd.setCursor(4, 10);
  M5.Lcd.println("StepMotor Test: 0x70");
  M5.Lcd.setCursor(4, 30);
  M5.Lcd.println("Press A: 0x70");
  SendCommand(STEPMOTOR_I2C_ADDR,"G91");
}


void loop() {
  M5.update(); //Read the press state of the key.  读取按键 A, B, C 的状态
  if (M5.BtnA.isPressed()) {
    M5.Lcd.print('A');
    SendCommand(STEPMOTOR_I2C_ADDR,"G91");
    SendCommand(STEPMOTOR_I2C_ADDR, "G0 X-0.03 Y0.03 Z0 F10");
    delay(25);
  } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
    M5.Lcd.print('B');
  } else if (M5.BtnC.isPressed()) {
    M5.Lcd.print('C');
    SendCommand(STEPMOTOR_I2C_ADDR,"G91");
    SendCommand(STEPMOTOR_I2C_ADDR, "G0 X0.03 Y-0.03 Z0 F10");
    delay(25);
  } else if (M5.BtnB.wasReleasefor(700)) {
    M5.Lcd.clear(BLACK);  
    M5.Lcd.setCursor(0, 0);
  }
}

*/

/*
void setup() {
  M5.begin();  //Init M5Core.  初始化 M5Core2
  M5.Lcd.setTextColor(YELLOW);  //Set the font color to yellow.  设置字体颜色为黄色
  M5.Lcd.setTextSize(2);  //Set the font size.  设置字体大小为2
  M5.Lcd.setCursor(65, 10); //Move the cursor position to (x, y).  移动光标位置到 (x, y)处
  M5.Lcd.println("Button example"); //The screen prints the formatted string and wraps the line.  输出格式化字符串并换行
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.println("Press button B for 700ms");
  M5.Lcd.println("to clear screen.");
  M5.Lcd.setTextColor(RED);
}

void loop() {
  M5.update(); //Read the press state of the key.  读取按键 A, B, C 的状态
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) {
    M5.Lcd.print('A');
  } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
    M5.Lcd.print('B');
  } else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200)) {
    M5.Lcd.print('C');
  } else if (M5.BtnB.wasReleasefor(700)) {
    M5.Lcd.clear(WHITE);  // Clear the screen and set white to the background color.  清空屏幕并将白色设置为底色
    M5.Lcd.setCursor(0, 0);
  }
}
*/

/*
  if (digitalRead(39) == LOW)  // A button
  {
    while (digitalRead(39) == LOW) delay(1);
    SendCommand(STEPMOTOR_I2C_ADDR, "G1 X20Y20Z20 F500");
    SendCommand(STEPMOTOR_I2C_ADDR, "G1 X0Y0Z0 F400");
  }
  if (digitalRead(37) == LOW)  // C button
  {
    while (1) {
      SendCommand(STEPMOTOR_I2C_ADDR, "G1 X0Y0Z0 F100");
      delay(1000);
      M5.Lcd.print(".");
      delay(3000);
      SendCommand(STEPMOTOR_I2C_ADDR, "G1 X0.01 Y0.05 Z0.5 F100");
      delay(3000);
      M5.Lcd.print(".");
      delay(1000);
    }
  }
  // Get Data from Module.  从模块获取数据。
  Wire1.requestFrom(STEPMOTOR_I2C_ADDR, 1);
  if (Wire1.available() > 0) {
    int u = Wire1.read();
    if (u != 0) Serial.write(u);
  }
  delay(1);
  // Send Data to Module.  向模块发送数据
  while (Serial.available() > 0) {
    int inByte = Serial.read();
    SendByte(STEPMOTOR_I2C_ADDR, inByte);
  }
*/