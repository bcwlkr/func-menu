#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>

// Define the display's control pins
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define BLACK 0x0000
#define WHITE 0xFFFF

// Declare the display object
MCUFRIEND_kbv tft;

// Button pins
#define UP_BUTTON 49
#define DOWN_BUTTON 51
#define SELECT_BUTTON 53

int menuOption = 0;         // Variable to track the selected menu option
const int totalOptions = 3; // Number of menu options

// DnD character stats
int strength = 10;
int dexterity = 14;
int constitution = 13;
int intelligence = 12;
int wisdom = 10;
int charisma = 15;

// Dice roller variables
int diceType = 6; // Default die type (D6)
int diceCount = 1;

// Function to calculate modifiers
int calculateModifier(int stat) {
  return (stat - 10) / 2;
}

void setup() {
  Serial.begin(9600);

  // Initialize the display
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  // Initialize button pins as inputs with pull-up resistors
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);

  // Draw the initial menu
  drawMenu();
}

void loop() {
  // Handle up button press
  if (digitalRead(UP_BUTTON) == LOW) {
    menuOption--;
    if (menuOption < 0) menuOption = totalOptions - 1; // Loop back to last option
    drawMenu();
    delay(200); // Debounce delay
  }

  // Handle down button press
  if (digitalRead(DOWN_BUTTON) == LOW) {
    menuOption++;
    if (menuOption >= totalOptions) menuOption = 0; // Loop back to first option
    drawMenu();
    delay(200); // Debounce delay
  }

  // Handle select button press
  if (digitalRead(SELECT_BUTTON) == LOW) {
    executeOption(menuOption); // Execute the selected option
    delay(200); // Debounce delay
  }
}

void drawMenu() {
  // Clear the screen
  tft.fillScreen(BLACK);

  // Draw the menu options
  for (int i = 0; i < totalOptions; i++) {
    tft.setCursor(40, 50 + (i * 50));
    if (menuOption == i) {
      tft.setTextColor(GREEN); // Highlight selected option
    } else {
      tft.setTextColor(WHITE);
    }
    if (i == 0) {
      tft.print("Dice"); // Option 1: Dice
    } else if (i == 1) {
      tft.print("Stats"); // Option 2: Stats
    } else if (i==2) {
      tft.print("Skills"); // Option 3: Skills
    }
  }
}

void executeOption(int option) {
  switch (option) {
    case 0:
      diceMenu(); // Open the Dice Roller
      break;
    case 1:
      displayStats(); // Display character stats
      break;
    case 2:
      skillsMenu(); // Display character skills
    default:
      displayOption(option); // Default behavior for other options
      break;
  }
}

void diceMenu() {
  int diceMenuOption = 0;
  int prevDiceMenuOption = -1;  // Track previous selection
  const int totalDiceOptions = 6; // D4, D6, D8, D10, D12, D20
  const int diceTypes[] = {4, 6, 8, 10, 12, 20};

  bool menuActive = true;
  while (menuActive) {
    // Only redraw if selection changes
    if (diceMenuOption != prevDiceMenuOption) {
      tft.fillScreen(BLACK);
      tft.setCursor(40, 20);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.print("Select Die Type:");

      for (int i = 0; i < totalDiceOptions; i++) {
        tft.setCursor(40, 60 + (i * 30));
        tft.setTextColor(diceMenuOption == i ? GREEN : WHITE);
        tft.print("D");
        tft.print(diceTypes[i]);
      }
      prevDiceMenuOption = diceMenuOption; // Update previous selection
    }

    if (digitalRead(UP_BUTTON) == LOW) {
      diceMenuOption = (diceMenuOption - 1 + totalDiceOptions) % totalDiceOptions;
      delay(200);
    }

    if (digitalRead(DOWN_BUTTON) == LOW) {
      diceMenuOption = (diceMenuOption + 1) % totalDiceOptions;
      delay(200);
    }

    if (digitalRead(SELECT_BUTTON) == LOW) {
      diceType = diceTypes[diceMenuOption];
      delay(200);
      selectDiceCount();
      menuActive = false;
    }
  }
}

void selectDiceCount() {
  diceCount = 1;
  int prevDiceCount = -1;
  bool countMenuActive = true;

  while (countMenuActive) {
    if (diceCount != prevDiceCount) {
      tft.fillScreen(BLACK);
      tft.setCursor(40, 20);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.print("How many dice?");

      tft.setCursor(100, 100);
      tft.setTextSize(3);
      tft.setTextColor(GREEN);
      tft.print(diceCount);

      prevDiceCount = diceCount;
    }

    if (digitalRead(UP_BUTTON) == LOW) {
      diceCount++;
      delay(200);
    }

    if (digitalRead(DOWN_BUTTON) == LOW) {
      if (diceCount > 1) diceCount--;
      delay(200);
    }

    if (digitalRead(SELECT_BUTTON) == LOW) {
      rollDice();
      countMenuActive = false;
    }
  }
}

void rollDice() {
  tft.fillScreen(BLACK);
  tft.setCursor(20, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Results:");

  int total = 0;
  for (int i = 0; i < diceCount; i++) {
    int roll = random(1, diceType + 1);
    total += roll;

    tft.setCursor(20, 60 + (i * 30));
    tft.print("Roll ");
    tft.print(i + 1);
    tft.print(": ");
    tft.print(roll);
  }

  tft.setCursor(20, 200);
  tft.print("Total: ");
  tft.print(total);

  while (true) {
    if (digitalRead(SELECT_BUTTON) == LOW) {
      drawMenu();
      break;
    }
  }
}

void displayStats() {
  tft.fillScreen(BLACK);
  tft.setCursor(40, 20);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  tft.print("Strength: "); tft.print(strength); tft.print(" ("); tft.print(calculateModifier(strength)); tft.print(")");

  tft.setCursor(20, 50);
  tft.print("Dexterity: "); tft.print(dexterity); tft.print(" ("); tft.print(calculateModifier(dexterity)); tft.print(")");

  tft.setCursor(20, 80);
  tft.print("Constitution: "); tft.print(constitution); tft.print(" ("); tft.print(calculateModifier(constitution)); tft.print(")");

  tft.setCursor(20, 110);
  tft.print("Charisma: "); tft.print(charisma); tft.print(" ("); tft.print(calculateModifier(charisma)); tft.print(")");

  tft.setCursor(20, 140);
  tft.print("Intelligence: "); tft.print(intelligence); tft.print(" ("); tft.print(calculateModifier(intelligence)); tft.print(")");

  tft.setCursor(20, 170);
  tft.print("Wisdom: "); tft.print(wisdom); tft.print(" ("); tft.print(calculateModifier(wisdom)); tft.print(")");

  while (true) {
    if (digitalRead(UP_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW || digitalRead(SELECT_BUTTON) == LOW) {
      delay(200);
      drawMenu();
      break;
    }
  }
}

void skillsMenu() {
  tft.fillScreen(BLACK);
  tft.setCursor(40, 10);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  tft.print("Acrobatics"); tft.print("  ("); tft.print(calculateModifier(dexterity)); tft.print(")\n");

  tft.print("Animal Handling"); tft.print("  ("); tft.print(calculateModifier(wisdom));  tft.print(")\n"); 
  tft.setCursor(40, 20);

  tft.print("Arcana"); tft.print("  ("); tft.print(calculateModifier(intelligence));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Athletics"); tft.print(" ("); tft.print(calculateModifier(strength));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Deception"); tft.print("  ("); tft.print(calculateModifier(charisma));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("History"); tft.print("  ("); tft.print(calculateModifier(intelligence));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Insight"); tft.print("  ("); tft.print(calculateModifier(wisdom));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Intimidation"); tft.print("  ("); tft.print(calculateModifier(charisma));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Investigation"); tft.print("  ("); tft.print(calculateModifier(intelligence));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Medicine"); tft.print("  ("); tft.print(calculateModifier(wisdom));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Nature"); tft.print("  ("); tft.print(calculateModifier(intelligence));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Perception"); tft.print("  ("); tft.print(calculateModifier(wisdom));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Performance"); tft.print("  ("); tft.print(calculateModifier(charisma));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Persuasion"); tft.print("  ("); tft.print(calculateModifier(charisma));  tft.print(")\n");
  tft.setCursor(40, 20);
  
  tft.print("Religion"); tft.print("  ("); tft.print(calculateModifier(intelligence));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Sleight of Hand"); tft.print("  ("); tft.print(calculateModifier(dexterity));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Stealth"); tft.print("  ("); tft.print(calculateModifier(dexterity));  tft.print(")\n");
  tft.setCursor(40, 20);

  tft.print("Survival"); tft.print("  ("); tft.print(calculateModifier(wisdom));  tft.print(")\n");
  tft.setCursor(40, 20);

  while (true) {
    if (digitalRead(UP_BUTTON) == LOW || digitalRead(DOWN_BUTTON) == LOW || digitalRead(SELECT_BUTTON) == LOW) {
      delay(200);
      drawMenu();
      break;
    }
  }
}
void displayOption(int option) {
  // Display the selected option on the screen
}
