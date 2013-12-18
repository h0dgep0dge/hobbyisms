/*
  An Arduino Mega based alarm clock, written by Robbie Mckennie, 2.0
  What happened to 1.0 you ask? It was a terrible thing that shall never see the light of day.
  Pin mappings; Probably should be stored in variables, i'm just an idiot
    Up: 42
    Down: 41
    Left: 43
    Right: 40
    Snooze: 36
    Set alarm: 39
    Backlight: 38
*/

volatile int vals[2][7] = {{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
volatile int render = 0;
int field = 0;
int mode = 0;
int backlight = 1;
char days[7][5] = {"MON","TUE","WED","THU","FRI","SAT","SUN"};

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(36,INPUT_PULLUP);
  pinMode(38,INPUT_PULLUP);
  pinMode(39,INPUT_PULLUP);
  pinMode(40,INPUT_PULLUP);
  pinMode(41,INPUT_PULLUP);
  pinMode(42,INPUT_PULLUP);
  pinMode(43,INPUT_PULLUP);
  pinMode(49,OUTPUT);
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
  while(vals[0][2] >= 60) vals[0][2] -= 60,vals[0][1]++;
  while(vals[0][1] >= 60) vals[0][1] -= 60,vals[0][0]++;
  while(vals[0][0] >= 24) vals[0][0] -= 24,vals[0][3]++;
  if(render == 1) {
    write_time(mode,vals[mode][0],vals[mode][1],vals[mode][2],vals[mode][3],vals[mode][4],vals[mode][5],vals[mode][6]);
    render = 0;
  }
  if(digitalRead(42) == 0) {
    unsigned long s = millis();
    vals[mode][field]++;
    vals[mode][field] = vals[mode][field]%get_ceil(field);
    render = 1;
    while(millis() < s+500) if(digitalRead(42) == 1) break;
  }
  if(digitalRead(41) == 0) {
    unsigned long s = millis();
    vals[mode][field]--;
    if(vals[mode][field] < 0) vals[mode][field] = (vals[mode][field]+get_ceil(field))%get_ceil(field);
    render = 1;
    while(millis() < s+500) if(digitalRead(41) == 1) break;
  }
  if(digitalRead(40) == 0) {
    field++;
    field = field%7;
    while(digitalRead(40) == 0);
  }
  if(digitalRead(43) == 0) {
    field--;
    while(field < 0) field = (field+7)%7;
    while(digitalRead(43) == 0);
  }
  if(mode == digitalRead(39)) {
    mode = 1-mode;
    render = 1;
  }
  if(backlight != digitalRead(38)) {
    backlight = digitalRead(38);
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
  if(vals[0][0] == vals[1][0] && vals[0][1] == vals[1][1]) tone(49,2000);
  if(digitalRead(36) == 0) noTone(49);
}

void write_time(int alarm,int hour,int minute,int second,int day_w,int day_i,int month,int year) {
  Serial3.write(254);
  Serial3.write(1);
  Serial3.write(254);
  Serial3.write(132);
  if(hour < 10) Serial3.print("0");
  Serial3.print(hour);
  if(minute < 10) Serial3.print("0");
  Serial3.print(minute);
  Serial3.print(":");
  if(second < 10) Serial3.print("0");
  Serial3.print(second);
  if(mode == 1) {
    Serial3.write(254);
    Serial3.write(195);
    Serial3.print("Set Alarm");
  }
  else {
    Serial3.write(254);
    Serial3.write(193);
    Serial3.print(days[day_w]);
    Serial3.print(" ");
    if(day_i+1 < 10) Serial3.print("0");
    Serial3.print(day_i+1);
    Serial3.print("/");
    if(month+1 < 10) Serial3.print("0");
    Serial3.print(month+1);
    Serial3.print("/");
   //if(month < 10) Serial3.print("0");
    Serial3.print(year+2000);
  }
}

ISR(TIMER1_COMPA_vect) {
  vals[0][2]++;
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
    // THERE ARE MORE! i'll do them later...     
  }
}
