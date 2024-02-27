/*
 *  Needed:
 *  LCD shield
 *  Arduino
 *  Select key - change mode
 *  Rotery encoder, up and down keys - gain(mode 2) and offset(mode 3)
 *  Potentiometer - Vpot
 *  
 *  Result:
 *  print the measured on the lcd where Measured = Gain ∗ (Vpot + Offset)
 */
#include <LiquidCrystal.h>
#define Key_Right 1
#define Key_Left 2
#define Key_Up 3
#define Key_Down 4
#define Key_Select 5
#define No_Key_Pressed -1
#define Gain_Step 2
#define Offset_Step 0.25

int Mode = 1, last_key_pressed = No_Key_Pressed;
int Max = -10000, Min = 10000;
double Offset = 0.1, Gain = 49.9;
unsigned int last_A = 0, last_B = 0;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // 16 columns * 2 rows
  lcd.begin(16, 2);
}

// Convert ADC value to key number
int get_key(unsigned int pot_analog){
  int key_number = No_Key_Pressed;
  if (pot_analog < 60)
    key_number = Key_Right;
  else if (pot_analog < 200)
    key_number = Key_Up;
  else if (pot_analog < 400)
    key_number = Key_Down;
  else if (pot_analog < 600)
    key_number = Key_Left;
  else if (pot_analog < 800)
    key_number = Key_Select;
    
  return key_number;
}

void show_gain(){
  lcd.setCursor(0, 1);
  lcd.print("Gain: ");
  lcd.print(Gain);
}

void show_offset(){
  lcd.setCursor(0, 1);
  lcd.print("Offset: ");
  lcd.print(Offset);
}

//Measured = Gain * (pot_analog + Offset)
void show_measured(unsigned int pot_analog){
  double measured = Gain * (pot_analog + Offset);
  lcd.setCursor(0, 0);
  lcd.print("Measured: ");
  lcd.print(measured);
  if (measured > Max)
    Max = measured;
  if (measured < Min)
    Min = measured;
}

void mode_1(int key, unsigned int pot_analog){
  // There is a new key pressed
  if (key != last_key_pressed){
    delay(50);
    if (key == Key_Select){
      Mode += 1;
      return;
    }
  }
  show_measured(pot_analog);
  show_gain();
}

/*
 * Return:
 * -1 -> not change 
 * 0 -> down
 * 1 -> up
 */
int read_rotery(){
  unsigned int A = analogRead(A1), B = analogRead(A2);

  // Not change
  if (A == last_A or B == last_B)
    return -1;

  //Update last_A and last_B
  last_A = A;
  last_B = B;

  if (A > B)
    return 0;
  return 1;
}

void mode_2(int key, unsigned int pot_analog){
  int rotery_state = read_rotery();
  if (rotery_state == 0 and Gain > 1.9)
    Gain -= Gain_Step;
  else if (rotery_state == 1 and Gain < 99.9)
    Gain += Gain_Step;
    
  // There is a new key pressed
  if (key != last_key_pressed){
    delay(50);
    if (key == Key_Select){
      Mode += 1;
      return;
    }
    else if (key == Key_Up and Gain < 99.9)
      Gain += Gain_Step;
    else if (key == Key_Down and Gain > 1.9)
      Gain -= Gain_Step;
  }
  show_measured(pot_analog);
  show_gain();
}

void mode_3(int key, unsigned int pot_analog){
  int rotery_state = read_rotery();
  if (rotery_state == 0 and Offset > -2.5)
    Offset -= Offset_Step;
  else if (rotery_state == 1 and Offset < 2.5)
    Offset += Offset_Step;
    
  // There is a new key pressed
  if (key != last_key_pressed){
    delay(50);
    if (key == Key_Select){
      Mode += 1;
      return;
    }
    else if (key == Key_Up and Offset < 2.5)
      Offset += Offset_Step;
    else if (key == Key_Down and Offset > -2.5)
      Offset -= Offset_Step;
  }
  show_measured(pot_analog);
  show_offset();
}

void mode_4(int key, unsigned int pot_analog){
  if (key != last_key_pressed){
    delay(50);
    if (key == Key_Select){
      Mode = 1;
      return;
    }
    if (key == Key_Down){
      Min = 0;
      Max = 0;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("Max: ");
  lcd.print(Max);
  lcd.setCursor(0, 1);
  lcd.print("Min: ");
  lcd.print(Min);
}

void loop() {
  unsigned int pot_analog = analogRead(A5); // Potentiometer
  unsigned int key_analog = analogRead(A0); // LCD shield
  int key = get_key(key_analog);

  if (Mode == 1)
    mode_1(key, pot_analog);
  else if (Mode == 2)
    mode_2(key, pot_analog);
  else if (Mode == 3)
    mode_3(key, pot_analog);
  else if (Mode == 4)
    mode_4(key, pot_analog);
    
  last_key_pressed = key;
  delay(100);
}
