#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 3; 

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {34, 33, 32, 31}; 
byte colPins[COLS] = {37, 36, 35}; 

Keypad keyp = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// O_RDWR

const int rs = 22, en = 23, d4 = 24, d5 = 25, d6 = 26, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int verified = -1;

int buzz = 29;

struct voter{
  int finger;
  int voter_id;
  bool b;
};

struct candidate{
  int candidate_number;
  int votes;
};


void initialise_memory(){
  int address = 0;
  for(int i=1;i<=10;i++){
    voter v;
    v.finger = i;
    v.voter_id = (i*100)+7;
    v.b = 0;
    EEPROM.put(address,v);
    address+=sizeof(voter);
  }
  for(int i=1;i<=5;i++){
    candidate c;
    c.candidate_number = i;
    c.votes = 0;
    EEPROM.put(address,c);
    address+=sizeof(candidate);
  }
}







// lcd interface functions
void setup_lcd(int voter_id){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Voter Verified");
  delay(200);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Welcome");
  lcd.setCursor(0,1);
  lcd.print("voter_id: "+ String(voter_id));
  delay(200);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select Candidate");
}





// sd card and fingerprint interface functions

int get_fingerprint(){
   char key = keyp.getKey();
   switch(key){
      case '1':
        return 1;
      case '2':
        return 2;
      case '3':
        return 3;
      case '4':
        return 4;
      case '5':
        return 5;
      case '6':
        return 6;
      case '7':
        return 7;
      case '8':
        return 8;
      case '9':
        return 9;
      case '#':
        return 20;
      case '*':
        return 30;
      default:
        return 0;
   } 
  }


int check_identity(int finger_print){
    int address = 0;
    for(int i=1;i<=10;i++){
        voter v;
        EEPROM.get(address,v);
        if(v.finger == finger_print && v.b ==0){
          v.b = 1;
          EEPROM.put(address,v);
          return v.voter_id;                                                                       
        }
        address+=sizeof(voter);
    }
    return -1;
}


void update_memory(int candidate_number){
  int address = 0;
  for(int i=0;i<10;i++){
    address+= sizeof(voter);
  }
  for(int i=0;i<5;i++){
    candidate c;
    EEPROM.get(address,c);
    if(c.candidate_number == candidate_number){
      c.votes+=1;
      EEPROM.put(address,c);
    }
    address+=sizeof(candidate);  
  
}

}


// ballot interface functions

int fetch(){
   int pin;
   int num = 0;
   while(num!=1){
   num = 0;
   for(int i=45;i<=49;i++){
     int c = digitalRead(i);
     if(c==1){
        num+=1;
        pin = i;
     }    
   }
   }
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Hold for: ");
     for(int i=0;i<10;i++){

        lcd.setCursor(11,0);
        lcd.print(String(10-i-1));
        delay(50);
       
       if(digitalRead(pin) != HIGH){
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("Hold for 10s");
         delay(100);
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("Press again");
         delay(100);
         pin = 44;
         break;
       }
     }
     return (pin-44);  
}

// tally function
void count_votes(){
  int address = 0;
  for(int i=0;i<10;i++){
    address+= sizeof(voter);
  }
  for(int i=0;i<5;i++){
    candidate c;
    EEPROM.get(address,c);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Candidate No:"+String(c.candidate_number));
    lcd.setCursor(0,1);
    lcd.print("No of Votes: "+String(c.votes));
    delay(200);
    address+= sizeof(candidate);
    
    }
    
  }



void setup() {

pinMode(45,INPUT);
pinMode(46,INPUT);
pinMode(47,INPUT);
pinMode(48,INPUT);
pinMode(49,INPUT);
pinMode(buzz,OUTPUT);
pinMode(12, OUTPUT);

lcd.begin(16, 2);
initialise_memory();

}


void loop() {

  analogWrite(12,64);
  
  int fingerprint_identity = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press and Hold");
    lcd.setCursor(1,1);
    lcd.print("your finger");
    
    
  
  while(fingerprint_identity==0){
    fingerprint_identity = get_fingerprint();  
  }

  lcd.clear();
  lcd.print("Lift Finger");

  if(fingerprint_identity == 20){
    count_votes();
  }
  else{
  verified = check_identity(fingerprint_identity);

if(verified == -1){
  lcd.clear();
  lcd.print("INVALID VOTER");
  digitalWrite(buzz,HIGH);
  delay(30);
  digitalWrite(buzz,LOW);
  delay(10);
  digitalWrite(buzz,HIGH);
  delay(30);
  digitalWrite(buzz,LOW);
  delay(10);
  digitalWrite(buzz,HIGH);
  delay(30);
  digitalWrite(buzz,LOW);
  delay(10);
  
}


if(verified!=-1){
  setup_lcd(verified);
  int candidate_number = 0;
    while(true){
    candidate_number = fetch();
    if(candidate_number!=0){
      break;
    } 
    }   
  update_memory(candidate_number);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Vote Casted");
  digitalWrite(buzz,HIGH);
  delay(30);
  digitalWrite(buzz,LOW);
  lcd.clear();
  

  
}
  } 
  
}
