#define BUTTON_1_PIN 6
#define BUTTON_2_PIN 5
#define BUTTON_3_PIN 4

/* Example code using my ButtonStateMachine class
 *  state machine pros:
 *    - makes multitasking easy
 *    - finite states-->great for debugging embedded systems
 *      -->profiling timing spent in states is available
 *    - no delay() calls
 *    - object oriented-->reuse of code
 *    - weak computational model-->easier to grasp
 *    - robust
 *    - low cost abstraction
 *  state machine cons:
 *    - hard to understand without flowchart
 *    - component changes may require full rewrite of flow
    Gunnar Myhre 2022. No copyright. */

const bool WRITE_TO_SERIAL = true;

class ButtonStateMachine {
  /* Button state machine class with debouncing
   *  on both push and release transients.
   *  Pull down switch: vcc--[R=220]--sample--[SWITCH]--gnd
   *  
          ┌────────┐ 1==1┌───────────┐
          │0: RESET├────►│1: START   │ val==0
          └────────┘     │ * read pin├──┐
          ▲   ▲          └───────────┘  │
          │   └───────┐                 ▼
          │elap>5     │     ┌──────────────────┐
    ┌─────┴─────────┐ │     │2: FIRST_LOW      │
    │5: RELEASED    │ │     │ * start timer t_0│
    │ * sample timer│ │     └───────────────┬──┘
    └─▲─────────────┘ │val==0           1==1│
      │val==1         │   ┌───────────────┐ │
      ├───────────┐   └───┤3: WAIT        │◄┘
      │4: PRESSED │       │ * read pin    │
      │ * read pin│ elap>5│ * sample timer│
      └───────────┘◄──────┴───────────────┘
      flowchart made with https://asciiflow.com */
  private:
    bool val;
    uint8_t pin;
    unsigned long t;
    unsigned long t_0;
    const uint8_t bounce_delay = 5;
    
  public:
    uint8_t state_prev = State::RESET;
    bool state_has_changed;
    
    enum State : uint8_t {
      RESET = 0,
      START,
      FIRST_LOW,
      WAIT,
      PRESSED,
      RELEASED
    };
    State state = State::RESET;

    ButtonStateMachine(uint8_t pin) {
      this->pin = pin;
      Init();
    }

    void Init(){
      pinMode(pin, INPUT);
    }
    
    void NextState(){
      state_prev = state;
      
      switch (state){
        case State::RESET:
          state = State::START;
        break;
    
        case State::START:
          val = digitalRead(pin);
          if (val == LOW) {state = State::FIRST_LOW;}
        break;
    
        case State::FIRST_LOW:
          t_0 = millis();
          state = State::WAIT;
        break;
    
        case State::WAIT:
          val = digitalRead(pin);
          t = millis();
          if (val == LOW) {state = State::RESET;}
          if (t - t_0 > bounce_delay){state = State::PRESSED;}
        break;
    
        case State::PRESSED:
          val = digitalRead(pin);
          t_0 = millis();
          if (val == HIGH) {state = State::RELEASED;}
        break;
    
        case State::RELEASED:
          t = millis();
          if (t - t_0 > bounce_delay){state = State::RESET;}
        break;
      }
      state_has_changed = (state_prev != state);
    }
};


// Global object instantiations
ButtonStateMachine button_1(BUTTON_1_PIN);
ButtonStateMachine button_2(BUTTON_2_PIN);
ButtonStateMachine button_3(BUTTON_3_PIN);


void setup(){
  if (WRITE_TO_SERIAL) {Serial.begin(9600);}
}


void loop(){
  button_1.NextState();
  button_2.NextState();
  button_3.NextState();

  // Debug
  if (WRITE_TO_SERIAL) {
    if (button_1.state_has_changed){
      if (button_1.state == ButtonStateMachine::State::PRESSED) {Serial.println("BUTTON ONE");}
      Serial.print(" state_s1: ");
      Serial.println(button_1.state);
    }
    if (button_2.state_has_changed){
      if (button_2.state == ButtonStateMachine::State::PRESSED) {Serial.println("BUTTON TWO");}
      Serial.print(" state_s2: ");
      Serial.println(button_2.state);
    }
    if (button_3.state_has_changed){
      if (button_3.state == ButtonStateMachine::State::PRESSED) {Serial.println("BUTTON THREE");}
      Serial.print(" state_s3: ");
      Serial.println(button_3.state);
    }
  }
}
