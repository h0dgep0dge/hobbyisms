/*
  An Arduino Mega based alarm clock, written by Robbie Mckennie, 2.0
  What happened to 1.0 you ask? It was a terrible thing that shall never see the light of day.
*/

volatile int vals[2][7] = {{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
volatile int render = 0;
int field = 0;
int mode = 0;
int backlight = 1;
char days[7][5] = {"MON","TUE","WED","THU","FRI","SAT","SUN"};
int months[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

int pin_up = 42;
int pin_down = 41;
int pin_left = 43;
int pin_right = 40;
int pin_snooze = 36;
int pin_mode = 39;
int pin_backlight = 38;
int pin_alarm = 49;
// Maybe do alarm mode? On/off vs tone?

Print *p;
int print_mode = 1; // 0 = LCD,1 = Plain serial

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  p = &Serial;
  pinMode(pin_up,INPUT_PULLUP);
  pinMode(pin_down,INPUT_PULLUP);
  pinMode(pin_left,INPUT_PULLUP);
  pinMode(pin_right,INPUT_PULLUP);
  pinMode(pin_snooze,INPUT_PULLUP);
  pinMode(pin_mode,INPUT_PULLUP);
  pinMode(pin_backlight,INPUT_PULLUP);
  pinMode(pin_alarm,OUTPUT);
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void loop() {
  if(render == 1) {
    write_time(vals[mode][0],vals[mode][1],vals[mode][2],vals[mode][3],vals[mode][4],vals[mode][5],vals[mode][6],field);
    render = 0;
  }
  if(digitalRead(pin_up) == 0) {
    unsigned long s = millis();
    vals[mode][field]++;
    vals[mode][field] = vals[mode][field]%get_ceil(field);
    render = 1;
    while(millis() < s+500) if(digitalRead(pin_up) == 1) break;
  }
  if(digitalRead(pin_down) == 0) {
    unsigned long s = millis();
    vals[mode][field]--;
    if(vals[mode][field] < 0) vals[mode][field] = (vals[mode][field]+get_ceil(field))%get_ceil(field);
    render = 1;
    while(millis() < s+500) if(digitalRead(pin_down) == 1) break;
  }
  if(digitalRead(pin_right) == 0) {
    field++;
    field = field%7;
    while(digitalRead(pin_right) == 0);
    render = 1;
  }
  if(digitalRead(pin_left) == 0) {
    field--;
    while(field < 0) field = (field+7)%7;
    while(digitalRead(pin_left) == 0);
    render = 1;
  }
  if(mode == digitalRead(pin_mode)) {
    mode = 1-mode;
    render = 1;
  }
  if(backlight != digitalRead(pin_backlight)) {
    backlight = digitalRead(pin_backlight);
    switch(backlight) {
      case 0:
        Serial3.write(124);
        delay(50);
        Serial3.write(157);
        delay(50);
        break;
      case 1:
        Serial3.write(0x7C);
        delay(50);
        Serial3.write(128);
        delay(50);
        break;
    }
  }
  if(vals[0][0] == vals[1][0] && vals[0][1] == vals[1][1]) tone(pin_alarm,2000);
  if(digitalRead(pin_snooze) == 0) noTone(pin_alarm);
  if(print_mode == 1) while(Serial.available() > 0) {
    int c;
    c = Serial.read();
    if(c == 'r' || c == 'R') {
      int i;
      for(i = 0;i < 7;i++) vals[0][i] = 0;
    }
  }
}

void write_time(int hour,int minute,int second,int day_w,int day_i,int month,int year,int field) {
  if(print_mode == 0) p->write(254);
  if(print_mode == 0) p->write(1);
  if(print_mode == 0) p->write(254);
  if(print_mode == 0) p->write(132);
  if(hour < 10) p->print("0");
  p->print(hour);
  p->print(":");
  if(minute < 10) p->print("0");
  p->print(minute);
  p->print(":");
  if(second < 10) p->print("0");
  p->print(second);
  if(print_mode == 1) p->println("");
  if(mode == 1) {
    if(print_mode == 0) p->write(254);
    if(print_mode == 0) p->write(195);
    p->print("Set Alarm");
  }
  else {
    if(print_mode == 0) p->write(254);
    if(print_mode == 0) p->write(193);
    p->print(days[day_w]);
    p->print(" ");
    if(day_i+1 < 10) p->print("0");
    p->print(day_i+1);
    p->print("/");
    if(month+1 < 10) p->print("0");
    p->print(month+1);
    p->print("/");
    p->print(year+2000);
  }
  if(print_mode == 1) p->println("");
}

ISR(TIMER1_COMPA_vect) {
  vals[0][2]++;
  while(vals[0][2] >= 60) vals[0][2] -= 60,vals[0][1]++;
  while(vals[0][1] >= 60) vals[0][1] -= 60,vals[0][0]++;
  while(vals[0][0] >= 24) vals[0][0] -= 24,vals[0][3] = (vals[0][3]+1)%7,vals[0][4]++;
  while(vals[0][4] >= get_ceil(4)) vals[0][4] = 0,vals[0][5]++;
  while(vals[0][5] >= 12) vals[0][5] -= 12,vals[0][6]++;
  vals[0][6] = vals[0][6]%1000;
  render = 1;
}

int get_ceil(int field) {
  switch(field) {
    case 0:
      return 24;
    case 1:
      return 60;
    case 2:
      return 60;
    case 3:
      return 7;
    case 4:
      return get_days_in_month();
    case 5:
      return 12;
    case 6:
      return 1000;    
  }
}

int get_days_in_month() {
  if(vals[0][5] == 1) {
    if(vals[0][6]%400 == 0) return 29;
    else if(vals[0][6]%100 == 0) return 28;
    else if(vals[0][6]%4 == 0) return 29;
    return 28;
  }
  else return months[vals[0][5]];
}
