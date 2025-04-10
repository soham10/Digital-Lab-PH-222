#include <Adafruit_NeoPixel.h>

// Pin Definitions
#define LED_PIN 2  
#define NUM_LEDS 64
#define JOY_X A1
#define JOY_Y A0
#define JOY_SW 12
#define BRIGHTNESS 40

Adafruit_NeoPixel matrix(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800); // Initialising the matrix

int cursorX = 0, cursorY = 7;
int grid[8][8];
bool revealed[8][8] = {false};
bool buttonPressed = false;

void setup() {
    Serial.begin(9600);
    matrix.begin();
    matrix.clear();
    matrix.setBrightness(BRIGHTNESS);
    pinMode(JOY_SW, INPUT_PULLUP);
    randomSeed(analogRead(0));
    generateMinesweeperGrid();
    updateLEDs();}

void loop() {
    handleJoystick();
    if (digitalRead(JOY_SW) == LOW) {
        if (!buttonPressed) {
            revealTile(cursorX, cursorY);
            updateLEDs();
            buttonPressed = true;}}
    else {
        buttonPressed = false;}
    delay(100);}

void generateMinesweeperGrid() {
    // First clear the grid
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            grid[i][j] = -1; // no mine
        }}
        
    // Place mines
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (random(10) < 2) {
                grid[i][j] = 0;} // mine
        }}

    // Calculate adjacent mines for all non-mine tiles
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (grid[i][j] != 0) {
                int mines = countAdjacentMines(i, j);
                grid[i][j] = mines;
                if (mines == 0) {
                    grid[i][j] = 5;}
            }}}}

int countAdjacentMines(int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && grid[nx][ny] == 0) {
                count++;}
        }}
    return count;}

// Once clicked, reveal the tile
void revealTile(int x, int y) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8 || revealed[x][y]) return; 
    revealed[x][y] = true;
    if (grid[x][y] == 5) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && grid[nx][ny] != 0) {
                    revealTile(nx, ny);}
            }}}}

void updateLEDs() {
    matrix.clear();
    // Draw revealed tiles with proper colors
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (revealed[x][y]) {
                setLEDColor(x, y, getColor(grid[x][y]));}}}
    // Draw cursor (white)
    setLEDColor(cursorX, cursorY, matrix.Color(255, 255, 255));
    matrix.show();}

// Game rules and logic behind the tile colours
uint32_t getColor(int value) {
    switch (value) {
        case 0:  return matrix.Color(255, 0, 0);     // Mine - Red
        case 1:  return matrix.Color(255, 200, 0);   // 1 adjacent mine - Yellow
        case 2:  return matrix.Color(0, 0, 255);     // 2 adjacent mines - Blue
        case 3:  return matrix.Color(150, 0, 150);   // 3 adjacent mines - Violet
        case 4:  return matrix.Color(0, 120, 200);  // 4 adjacent mines - Light Blue
        case 5:  return matrix.Color(0, 255, 0);     // Safe (no adjacent mines) - Green
        default: return matrix.Color(0, 0, 0);       // Shouldn't happen
    }}

void setLEDColor(int x, int y, uint32_t color) {
    int index = (7 - y) * 8 + x;
    matrix.setPixelColor(index, color);}

// Joystick function to control cursor movement
void handleJoystick() {
    int xValue = analogRead(JOY_X);
    int yValue = analogRead(JOY_Y);
    if (xValue < 400) cursorX = max(0, cursorX - 1);
    else if (xValue > 600) cursorX = min(7, cursorX + 1);
    if (yValue < 400) cursorY = min(7, cursorY + 1);
    else if (yValue > 600) cursorY = max(0, cursorY - 1);
    updateLEDs();
    delay(100);
}