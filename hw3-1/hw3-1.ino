
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int buttonPin = 2;

int mode = 0;
int colorIndex = 0;

const int colors[7][3] = {
  {255, 0, 0},     // 紅
  {0, 255, 0},     // 綠
  {0, 0, 255},     // 藍
  {180, 0, 255},   // 紫
  {255, 30, 100},  // 粉紅
  {255, 180, 0},   // 黃
  {0, 255, 255}    // 青
};

bool buttonDown = false;
bool longHandled = false;
unsigned long pressStart = 0;
unsigned long lastDebounce = 0;
int lastReading = HIGH;
int stableButton = HIGH;

void setColor(int r, int g, int b) {
  // 共陽極 RGB：數值需反轉
  analogWrite(redPin, 255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin, 255 - b);
}

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  setColor(0, 0, 0);
}

void loop() {
  unsigned long now = millis();
  int reading = digitalRead(buttonPin);

  // 按鈕防彈跳
  if (reading != lastReading) {
    lastDebounce = now;
  }

  if (now - lastDebounce >= 30 &&
      reading != stableButton) {
    stableButton = reading;

    if (stableButton == LOW) {
      buttonDown = true;
      longHandled = false;
      pressStart = now;
    } else if (buttonDown) {
      if (!longHandled) {
        colorIndex = (colorIndex + 1) % 7;
      }
      buttonDown = false;
    }
  }

  lastReading = reading;

  // 長按一秒切換模式
  if (buttonDown && !longHandled &&
      now - pressStart >= 1000) {
    mode = (mode + 1) % 4;
    longHandled = true;
  }

  int r = colors[colorIndex][0];
  int g = colors[colorIndex][1];
  int b = colors[colorIndex][2];

  if (mode == 0) {
    // 模式 1：恆亮
    setColor(r, g, b);
  }
  else if (mode == 1) {
    // 模式 2：閃爍
    if ((now / 400) % 2 == 0) {
      setColor(r, g, b);
    } else {
      setColor(0, 0, 0);
    }
  }
  else if (mode == 2) {
    // 模式 3：呼吸燈
    int phase = (now / 5) % 510;
    int brightness =
      phase <= 255 ? phase : 510 - phase;

    setColor(
      r * brightness / 255,
      g * brightness / 255,
      b * brightness / 255
    );
  }
  else {
    // 模式 4：彩虹漸變（額外加分）
    unsigned long t = (now / 8) % 1536;
    int step = t / 256;
    int v = t % 256;

    if (step == 0) setColor(255, v, 0);
    if (step == 1) setColor(255-v, 255, 0);
    if (step == 2) setColor(0, 255, v);
    if (step == 3) setColor(0, 255-v, 255);
    if (step == 4) setColor(v, 0, 255);
    if (step == 5) setColor(255, 0, 255-v);
  }
}
