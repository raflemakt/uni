#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA_PIN       A4
#define OLED_SCL_PIN       A5
#define OLED_RESET         4
#define SCREEN_WIDTH       128
#define SCREEN_HEIGHT      32
#define SCREEN_ADDRESS     0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define BUTTON_1_PIN       7
#define BUTTON_2_PIN       8
#define BUZZER_PIN         3
#define RANDOM_SEED_PIN    5
#define LED_R              10
#define LED_G              11
#define LED_B              12

/*
 * RESPONSE - a two player game for Arduino.
 *   made with:   - two buttons
 *                - an SSD1306 128x32 OLED screen
 *                - piezoelectric buzzer
 *                - RGB led
 *
 *   Press your button when the LED turns green, the faster you
 *   press, the more points you get.
 *   But be aware, the game might trick you into press too early
 *   and give your opponent 500 points!
 *   First player to 1500 points wins the game.
 *
 *   Gunnar Myhre 2022 - no copyright, no license
 */

// Constants
const unsigned int win_condition = 1500;
const unsigned int max_point_gain = 1000;
const unsigned int eager_click_penality = 500;
const unsigned int trick_chance_percentage = 33;
const unsigned int trick_time_min = 1000;
const unsigned int trick_time_max = 2000;
const unsigned int secret_time_min = 2500;
const unsigned int secret_time_max = 7000;


class Button {
    /* Button state machine class with debouncing
        on both push and release transients.
        Pull down switch: vcc--[R=220]--sample--[SWITCH]--gnd

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
    bool button_reading;
    uint8_t pin;
    unsigned long t;
    unsigned long t_0;
    const uint8_t bounce_delay = 5;

  public:
    bool state_has_changed;
    bool pressed_this_cycle;

    enum State : uint8_t {
      RESET = 0,
      START,
      FIRST_LOW,
      WAIT,
      PRESSED,
      RELEASED
    };
    State state = State::RESET;
    State state_prev;

    Button(uint8_t pin) {
      this->pin = pin;
      Init();
    }

    void Init() {
      pinMode(pin, INPUT);
    }

    void Update() {
      state_prev = state;

      switch (state) {
        case State::RESET:
          state = State::START;
          break;

        case State::START:
          button_reading = digitalRead(pin);
          if (button_reading == LOW) {
            state = State::FIRST_LOW;
          }
          break;

        case State::FIRST_LOW:
          t_0 = millis();
          state = State::WAIT;
          break;

        case State::WAIT:
          button_reading = digitalRead(pin);
          t = millis();
          if (button_reading == LOW) {
            state = State::RESET;
          }
          if (t - t_0 > bounce_delay) {
            state = State::PRESSED;
          }
          break;

        case State::PRESSED:
          button_reading = digitalRead(pin);
          t_0 = millis();
          if (button_reading == HIGH) {
            state = State::RELEASED;
          }
          break;

        case State::RELEASED:
          t = millis();
          if (t - t_0 > bounce_delay) {
            state = State::RESET;
          }
          break;
      }
      state_has_changed = (state_prev != state);
      pressed_this_cycle = state_has_changed && (state == State::PRESSED);
    }
};

class Buzzer {
  private:
    uint8_t pin;
    int freq_1 = 440;
    int freq_2;
    int iterator_1;
    int iterator_2;
    int lifetime;
    unsigned long prev_timer;

  public:
    enum State : uint8_t {
      STOPPED = 0,
      STOPPING,
      MENU_MUSIC,
      TENSE_MUSIC,
      WIN_SOUND,
      FAIL_SOUND,
    };
    State state = State::STOPPED;
    State state_prev;

    Buzzer(uint8_t pin) {
      this->pin = pin;
      Init();
    }

    void Init() {
      pinMode(pin, INPUT);
    }

    void Stop() {
      state = State::STOPPING;
    }

    void Update() {
      state_prev = state;

      switch (state) {
        case State::STOPPED: {
            //OK
          } break;

        case State::STOPPING: {
            noTone(pin);
            freq_1 = 0;
            freq_2 = 0;
            iterator_1 = 0;
            state = State::STOPPED;
          } break;

        case State::MENU_MUSIC: {
            freq_1 = freq_2 * iterator_1;
            if ((millis() / 700) % 2 == 0) {
              iterator_1 ++;
            }
            tone(pin, freq_1 << freq_2);
            freq_2 ++;
            if (freq_2 > 4) {
              freq_2 = 0;
            }
            if (iterator_1 > 12) {
              iterator_1 = 0;
            }
          } break;

        case State::TENSE_MUSIC: {
            // HAXX, bit shifting and crazyness
            freq_1 = freq_2 * iterator_1;
            if ((millis() / 700) % 2 == 0) {
              iterator_1 ++;
            }
            tone(pin, freq_1 << freq_2);
            freq_2 ++;
            if (freq_2 > 3) {
              freq_2 = 0;
            }
            if (iterator_1 > 3) {
              iterator_1 = 0;
            }
        } break;

        case State::WIN_SOUND: {
            if (state_prev != state) {
              lifetime = 10000;
              iterator_1 = 0;
            }
            iterator_1 ++;
            tone(pin, 100 * (iterator_1 % 43));
            lifetime --;

            if (lifetime < 0) {
              noTone(pin);
              state = State::STOPPING;
            }
          } break;

        case State::FAIL_SOUND: {
            if (((millis() / 100) % 2) == 0) {
              tone(pin, 200);
            } else {
              tone(pin, 500);
            }
          } break;
      }
    }

    void WinSound() {
      state = State::WIN_SOUND;
    }

    void FailSound() {
      state = State::FAIL_SOUND;
    }

    void MenuMusic() {
      state = State::MENU_MUSIC;
    }

    void TenseMusic() {
      state = State::TENSE_MUSIC;
    }
};


struct Player {
  unsigned int points;
};


enum GameState {
  INTRO_SCREEN = 0,
  FULL_RESET,
  NEW_ROUND,
  GAME_START,
  UNARMED,
  TRICK,
  ARMED,
  LOST,
  WON,
  FORCED_QUIT_SCREEN,
  ANNOUNCE,
  GAME_OVER,
};

void ledOff() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}


struct GameRound {
  unsigned long starting_time;
  unsigned long response_time;
  unsigned long elapsed_time;
  unsigned long secret_random_elapse_time;
  unsigned long secret_random_trick_time;
  unsigned long last_state_change;

  unsigned int point_gain;

  uint8_t winning_player;
};


// Global object instantiations
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Button button_1(BUTTON_1_PIN);
Button button_2(BUTTON_2_PIN);
GameState game_state;
GameState game_state_prev;
GameRound game_round;
Player player_1;
Player player_2;
Buzzer buzzer(BUZZER_PIN);

// Global variables
bool PRINT_TO_SERIAL = true;
bool ACCEPT_SERIAL_COMMANDS = true;
bool any_button_pressed = false;
bool game_state_has_changed;
byte received_byte;




void setup() {
  randomSeed(analogRead(RANDOM_SEED_PIN));

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  if (PRINT_TO_SERIAL) {
    Serial.begin(9600);
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;); // Don't proceed, loop forever
  }
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  button_1.Update();
  button_2.Update();
  buzzer.Update();

  any_button_pressed = (button_1.pressed_this_cycle || button_2.pressed_this_cycle);

  display.clearDisplay();

  game_state_prev = game_state;

  switch (game_state) {
    case GameState::INTRO_SCREEN: {
        buzzer.MenuMusic();
        //buzzer.WinSound();
        // Intro screen only shows on first startup
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.println("~REACTION~");
        display.setTextSize(1);
        display.setCursor(0, 15);

        // Blinking LED and alternating text every 700ms
        if (((millis() / 700) % 2) == 0) {
          display.println("Two player game.");

          digitalWrite(LED_G, HIGH);
          digitalWrite(LED_B, LOW);
        } else {
          display.println(win_condition);
          display.setCursor(30, 15);
          display.println("points to win.");

          digitalWrite(LED_G, LOW);
          digitalWrite(LED_B, HIGH);
        }

        display.setTextSize(1);
        display.setCursor(21, 24);
        display.println("press any key");
        if (any_button_pressed) {
          game_state = GameState::FULL_RESET;
        }
      } break;

    case GameState::FULL_RESET: {
        // Reset the two player structs, removing their points gained
        memset(&player_1, 0, sizeof(player_1));
        memset(&player_2, 0, sizeof(player_2));
        game_state = GameState::NEW_ROUND;
      } break;

    case GameState::NEW_ROUND: {

        // Reset the game_round struct
        memset(&game_round, 0, sizeof(game_round));

        // Ensure LED is off
        ledOff();
        buzzer.Stop();

        game_round.starting_time = millis();
        game_round.secret_random_elapse_time = random(secret_time_min, secret_time_max);

        // Decide if a trick will be done this round
        if (random(0,100) < trick_chance_percentage){
          game_round.secret_random_trick_time = random(trick_time_min, trick_time_max);
        }
        game_state = GameState::GAME_START;
      } break;

    case GameState::GAME_START: {
        game_round.elapsed_time = millis() - game_round.starting_time;
        buzzer.TenseMusic();

        display.setCursor(4, 10);
        display.setTextSize(2);
        display.println("Ready...");

        // Wait in the GAME_START-state for 1500 ms
        if (1500 < game_round.elapsed_time) {
          game_state = GameState::UNARMED;
        }
      } break;

    case GameState::UNARMED: {
        ledOff();

        display.setCursor(4, 10);
        display.setTextSize(2);
        display.println("Set...");

        if (any_button_pressed) {
          game_state = GameState::LOST;
          game_round.point_gain = eager_click_penality;

          if (button_1.pressed_this_cycle) {
            player_2.points += eager_click_penality;
            game_round.winning_player = 2;
            break;
          }
          if (button_2.pressed_this_cycle) {
            player_1.points += eager_click_penality;
            game_round.winning_player = 1;
            break;
          }
        }

        game_round.elapsed_time = millis() - game_round.starting_time;


        // After the secret elapse, go to next state
        if ((game_round.secret_random_elapse_time) < (game_round.elapsed_time)) {
          
          // If there is a secret trick time this round, go to the TRICK state
          if (game_round.secret_random_trick_time > trick_time_min) {
            game_state = GameState::TRICK;
          } else {
            // Else go to the ARMED state
            game_state = GameState::ARMED;
          }
        }
      } break;

    case GameState::TRICK: {
        digitalWrite(LED_R, HIGH);

        display.setCursor(4, 10);
        display.setTextSize(1);
        display.println("...hehe...");

        if (any_button_pressed) {
          game_state = GameState::LOST;
          game_round.point_gain = eager_click_penality;

          if (button_1.pressed_this_cycle) {
            player_2.points += eager_click_penality;
            game_round.winning_player = 2;
            break;
          }
          if (button_2.pressed_this_cycle) {
            player_1.points += eager_click_penality;
            game_round.winning_player = 1;
            break;
          }
        }

        game_round.elapsed_time = millis() - game_round.starting_time;

        // After the secret trick time, proceed to ARMED state
        if (game_round.secret_random_trick_time < (millis() - game_round.last_state_change)) {
          digitalWrite(LED_R, LOW);
          game_state = GameState::ARMED;
        }
      
      } break;

    case GameState::ARMED: {
        game_round.response_time = (millis()
                      - game_round.secret_random_elapse_time
                      - game_round.starting_time
                      - game_round.secret_random_trick_time);
        display.setCursor(44, 9);
        display.setTextSize(3);
        display.println("GO!");

        // Available points is 1000ms - response time, (or 0 if it it subceeded 0)
        game_round.point_gain = (max_point_gain - game_round.response_time) * (game_round.response_time < max_point_gain);
        display.setTextSize(1);
        display.setCursor(50, 0);
        display.println(game_round.point_gain);

        digitalWrite(LED_B, HIGH);

        if (any_button_pressed) {
          if (button_1.pressed_this_cycle) {
            game_round.winning_player = 1;
            player_1.points += game_round.point_gain;
            game_state = GameState::WON;
            break;
          }
          if (button_2.pressed_this_cycle) {
            game_round.winning_player = 2;
            player_2.points += game_round.point_gain;
            game_state = GameState::WON;
            break;
          }
        }
      } break;

    case GameState::WON: {
        ledOff();
        buzzer.WinSound();
        digitalWrite(LED_G, HIGH);
        game_state = GameState::ANNOUNCE;
      } break;

    case GameState::LOST: {
        display.setCursor(0, 10);
        display.setTextSize(2);
        display.println("TOO EARLY!");

        buzzer.FailSound();
        // Blink LED every 100 ms
        if (((millis() / 100) % 2) == 0) {
          digitalWrite(LED_R, HIGH);
        } else {
          digitalWrite(LED_R, LOW);
        }

        // Wait 1000 ms before proceeding to next state
        if ((millis() - game_round.last_state_change) > 1000) {
          buzzer.Stop();
          game_state = GameState::ANNOUNCE;
          ledOff();
        }
      } break;

    case GameState::ANNOUNCE: {
        if (buzzer.state == Buzzer::State::STOPPED) {
          buzzer.TenseMusic();
        }
        display.setCursor(0, 7);
        display.setTextSize(1);
        if (game_round.winning_player == 1) {
          display.print("Player 1 :+");
          display.setCursor(68, 7);
          display.print(game_round.point_gain);
        } else {
          display.print("Player 2 :+");
          display.setCursor(68, 7);
          display.print(game_round.point_gain);
        }
        display.setCursor(92, 7);
        display.println("points");

        if (game_round.response_time != 0) {
          // Print feedback on the winning player's response time
          display.setCursor(0, 15);
          display.setTextSize(1);
          display.println("time (ms):");
          display.setCursor(68, 15);
          display.println(game_round.response_time);

          display.setCursor(90, 15);
          switch (game_round.response_time) {
            case 0 ... 300:
              display.println("WOW!");
              break;

            case 301 ... 500:
              display.println("Great!");
              break;

            case 501 ... 700:
              display.println("Good");
              break;

            case 701 ... 1000:
              display.setCursor(105, 15);
              display.println("OK");
              break;
          }
        }

        display.setCursor(15, 24);
        display.println("press any button");

        // Draw each player's points on top of the screen
        display.setCursor(15, 0);
        display.println(player_1.points);
        display.setCursor(100, 0);
        display.println(player_2.points);
        if ((player_1.points >= win_condition) or (player_2.points >= win_condition)) {
          buzzer.Stop();
          game_state = GameState::GAME_OVER;
        }

        // Enforce 500ms wait to avoid accidentally double pressing away
        if (any_button_pressed && (millis() - game_round.last_state_change) > 500) {
          game_state = GameState::NEW_ROUND;
        }
      } break;

    case GameState::FORCED_QUIT_SCREEN: {
        // This state is only reached by a serial communication command

        // Draw each player's points on top of the screen
        display.setCursor(15, 0);
        display.println(player_1.points);
        display.setCursor(100, 0);
        display.println(player_2.points);
        if ((player_1.points >= win_condition) or (player_2.points >= win_condition)) {
          buzzer.Stop();
          game_state = GameState::GAME_OVER;
        }

        // Enforce 500ms wait to avoid accidentally double pressing away
        if (any_button_pressed && (millis() - game_round.last_state_change) > 500) {
          game_state = GameState::NEW_ROUND;
        }
      } break;

    case GameState::GAME_OVER: {
        buzzer.MenuMusic();
        display.setTextSize(1);
        display.setCursor(15, 0);
        display.println(player_1.points);
        display.setCursor(100, 0);
        display.println(player_2.points);

        display.setCursor(20, 15);
        display.setTextSize(1);

        // Blinking text every 300 ms
        if (((millis() / 300) % 2) == 0) {
          if (player_1.points > player_2.points) {
            display.println("Player 1 WINS!");
          } else {
            display.println("Player 2 WINS!");
          }
        }

        digitalWrite(LED_G, HIGH);
        // Blinking LED every 200 ms
        if (((millis() / 200) % 2) == 0) {
          digitalWrite(LED_B, HIGH);
        } else {
          digitalWrite(LED_B, LOW);
        }

        display.setTextSize(1);
        display.setCursor(15, 24);
        display.println("press any button");
        if (any_button_pressed) {
          game_state = GameState::FULL_RESET;
        }
      } break;
  }

  game_state_has_changed = (game_state != game_state_prev);

  if (game_state_has_changed) {
    game_round.last_state_change = millis();
  }

  // Finally display the screen buffer
  display.display();


  if (PRINT_TO_SERIAL) {
    Serial.print("game_state: ");
    Serial.print(game_state);
    Serial.print("   button_1: ");
    Serial.print(button_1.state);
    Serial.print("   button_2: ");
    Serial.print(button_2.state);
    Serial.print("   elapse: ");
    Serial.print(game_round.elapsed_time);
    Serial.print("   random: ");
    Serial.print(game_round.secret_random_elapse_time);
    Serial.print("   trick: ");
    Serial.print(game_round.secret_random_trick_time);
    Serial.println();
  }

  if (ACCEPT_SERIAL_COMMANDS) {
    if (Serial.available()) {
      received_byte = Serial.read();

      // r resets the game
      if (received_byte == 114) {game_state = GameState::INTRO_SCREEN;}

      // q quit (FORCED_QUIT_SCREEN
      if (received_byte == 113) {game_state = GameState::FORCED_QUIT_SCREEN;}
    }
    
  }
}
