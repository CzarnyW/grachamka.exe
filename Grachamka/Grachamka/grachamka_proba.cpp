#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <allegro5/allegro.h>  
#include <allegro5/allegro_primitives.h>  
#include <allegro5/allegro_font.h>  
#include <allegro5/allegro_ttf.h>  
#include <allegro5/allegro_image.h>
#include <atomic>
bool showDeleteMessage = false;
float deleteMessageTimer = 0;
int deletedSlotNumber = 0;
int slot;
bool showSaveMessage = false;
float saveMessageTimer = 0;
using namespace std;
const int SCREEN_W = 1500;
const int SCREEN_H = 1000;

atomic<bool> allegro_running(true);

enum GameState {
    TITLE_SCREEN,
    MENU,
    SHOP,
    FIGHT,
    TRAINING,
    TRAINING_SILA,
    TRAINING_OBRONA,
    TRAINING_MAGIA,
    TRAINING_KONDYCJA,
    TRAINING_CIALO,
    ARENA,
    STATYSTYKI,
    ZARZAD,
    NOWA_GRA,
    ARENA_TRENINGOWA,
    MINIGRA_SILA,
    MINIGRA_MAGIA,
    MINIGRA_AGILITY,
    MINIGRA_WITALNOSC,
    LEVEL_UP_SCREEN,
    MINIGAME_RESULTS
};

struct Ball {
    float x, y, speed;
    bool isGolden;
    bool active;
};

struct Stats {
    int strength;
    int agility;
    int vitality;
    int magic;
    double strengthMult;
    double defenseMult;
    double healthMult;
    double magicMult;
};

struct Equipment {
    string name;
    int value;
    int defenseValue;
    int healthBonus;
    int price;
    string type;
    int level;
};

struct Enemy {
    string name;
    int hp;
    int maxHp;
    int damage;
    int defense;
    int exp;
    int gold;
    int level;
    bool firstTimeReward;
};

struct Player {
    string name;
    int level;
    int exp;
    int expToNext;
    int gold;
    Stats stats;
    int maxHp;
    int currentHp;
    int energy;
    int maxEnergy;
    Equipment weapon;
    Equipment armor;
    Equipment shield;
    int skillPoints;
    int wins;
    int losses;
    int saveSlot;
    int arenaLevel;
    int arenaMaxLevel;
    vector<bool> defeatedBosses;
};

struct SaveFile {
    string playerName;
    int level;
    int exp;
    int expToNext;
    int gold;
    int strength, agility, vitality, magic;
    double strengthMult, defenseMult, healthMult, magicMult;
    int maxHp;
    int currentHp;
    int energy;
    int maxEnergy;
    string weaponName;
    int weaponLevel;
    int weaponPrice;
    string armorName;
    int armorLevel;
    int armorPrice;
    string shieldName;
    int shieldLevel;
    int shieldPrice;
    int skillPoints;
    int wins;
    int losses;
    int arenaLevel;
};

struct Button {
    float x, y;
    float width, height;
    const char* text;
    ALLEGRO_COLOR color;
    ALLEGRO_COLOR textColor;
    ALLEGRO_COLOR hoverColor;
    bool isHovered;
};

struct Explosion {
    float x, y;
    float radius;
    int lifetime;
    int currentFrame;
    ALLEGRO_COLOR color;
    bool active;
};

struct MinigameResult {
    string minigameName;
    int scoreAchieved;
    int statGained;
    string statName;
    int expGained;
    int levelBefore;
    int levelAfter;
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\nNacisnij Enter aby kontynuowac...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void displayHeader(const string& title) {
    cout << "\n--------------------------------------------------------------\n";
    cout << "!  " << setw(56) << left << title << "  !\n";
    cout << "--------------------------------------------------------------\n\n";
}

double calculateDodgeChance(const Player& player) {
    double dodgeChance = player.stats.agility * 0.01;
    if (dodgeChance > 30.0) dodgeChance = 30.0;
    return dodgeChance;
}

void displayPlayerStats(const Player& player) {
    cout << "-----------------------------------------------\n";
    cout << "! " << setw(15) << left << player.name
        << " ! Poziom: " << setw(2) << player.level
        << " ! Zloto: " << setw(6) << player.gold << " !\n";
    cout << "! Gniazdo zapisu: " << player.saveSlot << "     "
        << "              !\n";
    cout << "-----------------------------------------------\n";
    cout << "! HP: " << setw(4) << player.currentHp << "/" << setw(4) << player.maxHp
        << " ! Energia: " << setw(3) << player.energy << "/" << setw(3) << player.maxEnergy << "     !\n";
    cout << "! EXP: " << setw(5) << player.exp << "/" << setw(5) << player.expToNext
        << " ! Punkty: " << setw(2) << player.skillPoints << "           !\n";
    cout << "-----------------------------------------------\n";
    cout << "! Sila: " << setw(3) << player.stats.strength
        << " ! Zrecznosc: " << setw(3) << player.stats.agility << "               !\n";
    cout << "! Witalnosc: " << setw(3) << player.stats.vitality
        << " ! Magia: " << setw(3) << player.stats.magic << "                  !\n";
    cout << "-----------------------------------------------\n";
    cout << "! Zwyciestwa: " << setw(3) << player.wins
        << " ! Porazki: " << setw(3) << player.losses << "              !\n";
    cout << "-----------------------------------------------\n";
    double dodgeChance = calculateDodgeChance(player);
    cout << "! Szansa na unik: " << fixed << setprecision(2) << dodgeChance << "%"
        << "                       !\n";
    cout << "-----------------------------------------------\n";
}

void displayEquipment(const Player& player) {
    cout << "\n--------------- EKWIPUNEK ---------------\n";
    cout << "! Bron: " << setw(30) << left
        << (player.weapon.name.empty() ? "Brak" : player.weapon.name + " (+" + to_string(player.weapon.value) + ")") << "!\n";
    cout << "! Zbroja: " << setw(28) << left
        << (player.armor.name.empty() ? "Brak" : player.armor.name + " (+" + to_string(player.armor.defenseValue) + ")") << "!\n";
    cout << "! Tarcza: " << setw(30) << left
        << (player.shield.name.empty() ? "Brak" : player.shield.name + " (+" + to_string(player.shield.defenseValue) + ")") << "!\n";
    cout << "-------------------------------------------\n";
}

string getSaveFileName(int slot) {
    return "save_" + to_string(slot) + ".txt";
}

bool saveGameExists(int slot) {
    ifstream file(getSaveFileName(slot));
    return file.good();
}

void saveGame(const Player& player) {
    string filename = getSaveFileName(player.saveSlot);
    ofstream file(filename);

    if (!file.is_open()) {
        cout << "Blad: Nie mozna utworzyc pliku zapisu!\n";
        return;
    }

    file << player.name << "\n";
    file << player.level << "\n";
    file << player.exp << "\n";
    file << player.expToNext << "\n";
    file << player.gold << "\n";
    file << player.stats.strength << "\n";
    file << player.stats.agility << "\n";
    file << player.stats.vitality << "\n";
    file << player.stats.magic << "\n";
    file << fixed << setprecision(2) << player.stats.strengthMult << "\n";
    file << player.stats.defenseMult << "\n";
    file << player.stats.healthMult << "\n";
    file << player.stats.magicMult << "\n";
    file << player.maxHp << "\n";
    file << player.currentHp << "\n";
    file << player.energy << "\n";
    file << player.maxEnergy << "\n";
    file << player.weapon.name << "\n";
    file << player.weapon.level << "\n";
    file << player.weapon.price << "\n";
    file << player.armor.name << "\n";
    file << player.armor.level << "\n";
    file << player.armor.price << "\n";
    file << player.shield.name << "\n";
    file << player.shield.level << "\n";
    file << player.shield.price << "\n";
    file << player.skillPoints << "\n";
    file << player.wins << "\n";
    file << player.losses << "\n";
    file << player.arenaLevel << "\n";

    file.close();
}

vector<Equipment> initWeapons();
vector<Equipment> initArmors();
vector<Equipment> initShields();

bool loadGame(Player& player) {
    string filename = getSaveFileName(player.saveSlot);
    ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    SaveFile save;
    getline(file, save.playerName);
    file >> save.level >> save.exp >> save.expToNext >> save.gold;
    file >> save.strength >> save.agility >> save.vitality >> save.magic;
    file >> save.strengthMult >> save.defenseMult >> save.healthMult >> save.magicMult;
    file >> save.maxHp >> save.currentHp >> save.energy >> save.maxEnergy;

    file.ignore(); // Usuń \n po maxEnergy
    getline(file, save.weaponName);
    file >> save.weaponLevel >> save.weaponPrice;

    file.ignore(); // Usuń \n po weaponPrice
    getline(file, save.armorName);
    file >> save.armorLevel >> save.armorPrice;

    file.ignore(); // Usuń \n po armorPrice
    getline(file, save.shieldName);
    file >> save.shieldLevel >> save.shieldPrice;

    file.ignore(); // ⭐ TO BRAKUJE! Usuń \n po shieldPrice
    file >> save.skillPoints >> save.wins >> save.losses >> save.arenaLevel;

    file.close();
    vector<Equipment> weapons = initWeapons();
    vector<Equipment> armors = initArmors();
    vector<Equipment> shields = initShields();
    // Przypisanie do gracza
    player.name = save.playerName;
    player.level = save.level;
    player.exp = save.exp;
    player.expToNext = save.expToNext;
    player.gold = save.gold;
    player.stats.strength = save.strength;
    player.stats.agility = save.agility;
    player.stats.vitality = save.vitality;
    player.stats.magic = save.magic;
    player.stats.strengthMult = save.strengthMult;
    player.stats.defenseMult = save.defenseMult;
    player.stats.healthMult = save.healthMult;
    player.stats.magicMult = save.magicMult;
    player.maxHp = save.maxHp;
    player.currentHp = save.currentHp;
    player.energy = save.energy;
    player.maxEnergy = save.maxEnergy;
    player.skillPoints = save.skillPoints;
    player.wins = save.wins;
    player.losses = save.losses;
    player.arenaLevel = save.arenaLevel;

    if (!save.weaponName.empty() && save.weaponName != "BRAK") {
        if (save.weaponLevel >= 0 && save.weaponLevel < (int)weapons.size()) {
            player.weapon = weapons[save.weaponLevel]; // Przypisz CAŁY obiekt
        }
    }

    if (!save.armorName.empty() && save.armorName != "BRAK") {
        if (save.armorLevel >= 0 && save.armorLevel < (int)armors.size()) {
            player.armor = armors[save.armorLevel]; // Przypisz CAŁY obiekt
        }
    }

    if (!save.shieldName.empty() && save.shieldName != "BRAK") {
        if (save.shieldLevel >= 0 && save.shieldLevel < (int)shields.size()) {
            player.shield = shields[save.shieldLevel]; // Przypisz CAŁY obiekt
        }
    }
    return true;
}

void deleteSave(int slot) {
    string filename = getSaveFileName(slot);
    if (remove(filename.c_str()) == 0) {
        cout << "\nZapis z gniazda " << slot << " zostal usuniety!\n";
    }
}

void manageSaves() {
    while (true) {
        clearScreen();
        displayHeader("ZARZADZANIE ZAPISAMI");

        cout << "\nDostepne gniazda zapisu:\n\n";
        for (int i = 1; i <= 3; i++) {
            if (saveGameExists(i)) {
                cout << i << ". [ZAJETNE] ";
                ifstream file(getSaveFileName(i));
                string playerName;
                int level;
                getline(file, playerName);
                file >> level;
                file.close();
                cout << playerName << " - Poziom " << level << "\n";
            }
            else {
                cout << i << ". [PUSTE]\n";
            }
        }

        cout << "\n1. Usun zapis z gniazda 1\n";
        cout << "2. Usun zapis z gniazda 2\n";
        cout << "3. Usun zapis z gniazda 3\n";
        cout << "4. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice >= 1 && choice <= 3) {
            if (saveGameExists(choice)) {
                cout << "\nCzy na pewno chcesz usunac zapis? (T/N): ";
                char confirm;
                cin >> confirm;
                if (confirm == 'T' || confirm == 't') {
                    deleteSave(choice);
                    pause();
                }
            }
            else {
                cout << "\nGniazdo zapisu jest puste!\n";
                pause();
            }
        }
        else if (choice == 4) {
            return;
        }
        else {
            cout << "\nZly wybor!\n";
            pause();
        }
    }
}

Button createButton(float x, float y, float width, float height, const char* text,
    ALLEGRO_COLOR color, ALLEGRO_COLOR textColor, ALLEGRO_COLOR hoverColor) {
    Button btn;
    btn.x = x;
    btn.y = y;
    btn.width = width;
    btn.height = height;
    btn.text = text;
    btn.color = color;
    btn.textColor = textColor;
    btn.hoverColor = hoverColor;
    btn.isHovered = false;
    return btn;
}

bool isMouseOverButton(const Button& btn, int mouseX, int mouseY) {
    return mouseX >= btn.x && mouseX <= btn.x + btn.width &&
        mouseY >= btn.y && mouseY <= btn.y + btn.height;
}

void drawButton(const Button& btn, ALLEGRO_FONT* font) {
    ALLEGRO_COLOR currentColor = btn.isHovered ? btn.hoverColor : btn.color;
    al_draw_filled_rectangle(btn.x, btn.y, btn.x + btn.width, btn.y + btn.height, currentColor);
    al_draw_rectangle(btn.x, btn.y, btn.x + btn.width, btn.y + btn.height, al_map_rgb(255, 255, 255), 2);
    int textWidth = al_get_text_width(font, btn.text);
    int textHeight = al_get_font_line_height(font);
    float textX = btn.x + (btn.width - textWidth) / 2;
    float textY = btn.y + (btn.height - textHeight) / 2;
    al_draw_text(font, btn.textColor, textX, textY, 0, btn.text);
}

void drawTitleScreen(ALLEGRO_FONT* titleFont, ALLEGRO_FONT* buttonFont, Button& startButton) {
    al_clear_to_color(al_map_rgb(20, 30, 50));
    al_draw_text(titleFont, al_map_rgb(255, 215, 0), SCREEN_W / 2, SCREEN_H / 4, ALLEGRO_ALIGN_CENTRE, "MISTRZOWIE OSTRZA");
    drawButton(startButton, buttonFont);
}

int calculateMaxHp(const Player& player) {
    int base = 100 + (int)(player.stats.vitality * 10 * player.stats.healthMult);
    return base;
}

int calculateDamage(const Player& player) {
    int dmg = (int)(player.stats.strength * 2 * player.stats.strengthMult);
    if (!player.weapon.name.empty()) dmg += player.weapon.value;
    return dmg;
}

int calculateDefense(const Player& player) {
    int def = (int)(player.stats.vitality * player.stats.defenseMult);
    if (!player.armor.name.empty()) def += player.armor.defenseValue;
    if (!player.shield.name.empty()) def += player.shield.defenseValue;
    return def;
}

void levelUp(Player& player)
{
    player.level++;
    player.skillPoints += 20; // Teraz 20 punktów za poziom
    player.gold += 100 * player.level; // Nagroda złota: 100 * poziom
    player.expToNext = static_cast<int>(player.expToNext * 1.5);
    player.maxHp = calculateMaxHp(player);
    player.currentHp = player.maxHp;
    player.energy = player.maxEnergy;
    // Nie wyświetlamy nic w konsoli - będzie obsłużone graficznie
}

void gainExp(Player& player, int exp) {
    int arenaBonus = (int)pow(2, player.arenaLevel);
    exp *= arenaBonus;
    player.exp += exp;
    while (player.exp >= player.expToNext) {
        player.exp -= player.expToNext;
        levelUp(player);
    }
}

void strengthTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING SILY");

    cout << "Naciskaj ENTER tak szybko jak potrafisz!\n";
    cout << "Wpisz 'koniec' aby zakonczyc trening.\n\n";

    int score = 0;

    cout << "Naciskaj ENTER! (wpisz 'koniec' aby zakonczyc):\n\n";

    string input;
    while (true) {
        getline(cin, input);
        if (input == "koniec") break;
        if (input.empty()) {
            score++;
            cout << "Klikniecia: " << score << "\n";
        }
    }

    int statGain = max(1, score / 30);
    int expGain = score * 2;

    player.stats.strength += statGain;
    gainExp(player, expGain);
    player.energy = min(player.energy + 10, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU SILY ===\n\n";
    cout << "Klikniecia: " << score << "\n";
    cout << "Poziomy: " << statGain << "\n";
    cout << "Sila +" << statGain << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void defenseTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING OBRONY");

    cout << "Przepisz 10 kombinacji losowych znakow bez bledow!\n";
    cout << "Kazda prawidlowa kombinacja daje punkty!\n\n";
    pause();

    int score = 0;
    int rounds = 10;

    for (int i = 0; i < rounds; i++) {
        string target;
        for (int j = 0; j < 10; j++) {
            target += (char)('a' + rand() % 26);
        }

        cout << "\nRunda " << (i + 1) << "/" << rounds << "\n";
        cout << "Przepisz: " << target << "\n";
        cout << "Twoja odpowiedz: ";

        string input;
        getline(cin, input);

        if (input == target) {
            score += 10;
            cout << "Prawidlowo! (+10 pkt)\n";
        }
        else {
            cout << "Zle! Oczekiwano: " << target << "\n";
        }
    }

    int statGain = max(1, score / 15);
    int expGain = score * 2;

    player.stats.vitality += statGain;
    player.maxHp = calculateMaxHp(player);
    player.currentHp = player.maxHp;
    gainExp(player, expGain);
    player.energy = min(player.energy + 10, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU OBRONY ===\n\n";
    cout << "Punkty: " << score << "\n";
    cout << "Poziomy: " << statGain << "\n";
    cout << "Witalnosc +" << statGain << "\n";
    cout << "Max HP: " << player.maxHp << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void magicTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING MAGII");

    cout << "Rozwiazuj dzialania matematyczne!\n";
    cout << "Wpisz 'koniec' aby zakonczyc trening.\n\n";

    int score = 0;

    while (true) {
        int a = rand() % 50 + 1;
        int b = rand() % 50 + 1;
        int operation = rand() % 3;
        int answer;
        string op;

        if (operation == 0) {
            answer = a + b;
            op = "+";
        }
        else if (operation == 1) {
            answer = a - b;
            op = "-";
        }
        else {
            answer = a * b;
            op = "*";
        }

        cout << "\n" << a << " " << op << " " << b << " = ";
        string input;
        getline(cin, input);

        if (input == "koniec") break;

        try {
            int userAnswer = stoi(input);
            if (userAnswer == answer) {
                score += 10;
                cout << "Prawidlowo! (+10 pkt)\n";
            }
            else {
                cout << "Zle! Prawidlowa odpowiedz: " << answer << "\n";
            }
        }
        catch (...) {
            cout << "Zla wartosc!\n";
        }
    }

    int statGain = max(1, score / 15);
    int expGain = score * 2;

    player.stats.magic += statGain;
    gainExp(player, expGain);
    player.energy = min(player.energy + 10, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU MAGII ===\n\n";
    cout << "Punkty: " << score << "\n";
    cout << "Poziomy: " << statGain << "\n";
    cout << "Magia +" << statGain << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void agilityTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING KONDYCJI");

    cout << "Wciskaj prawidlowe klawisze!\n";
    cout << "S - w dol, W - w gore, A - w lewo, D - w prawo\n";
    cout << "Wpisz 'koniec' aby zakonczyc trening.\n\n";

    int score = 0;
    char keys[] = { 'W', 'S', 'A', 'D' };
    string keyNames[] = { "GORE", "DOL", "LEWO", "PRAWO" };

    while (true) {
        int randomKey = rand() % 4;
        cout << "Wcisnij: " << keyNames[randomKey] << " (" << keys[randomKey] << ") lub 'koniec': ";

        string input;
        getline(cin, input);

        if (input == "koniec") break;

        if (!input.empty() && (char)toupper(input[0]) == keys[randomKey]) {
            score++;
            double dodgeChance = calculateDodgeChance(player);
            cout << "Prawidlowo! Szansa unika: " << fixed << setprecision(2) << dodgeChance << "%\n";
        }
        else {
            cout << "Zle!\n";
        }
    }

    int statGain = max(1, score / 20);
    int expGain = score * 2;
    player.stats.agility += statGain;
    gainExp(player, expGain);
    player.energy = min(player.energy + 10, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU KONDYCJI ===\n\n";
    cout << "Prawidlowe wcisniecia: " << score << "\n";
    cout << "Poziomy: " << statGain << "\n";
    cout << "Zrecznosc +" << statGain << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void bodyTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING CIALA");

    cout << "Podaj liczbe 2 razy wieksza od wylosowanej!\n";
    cout << "Wpisz 'koniec' aby zakonczyc trening.\n\n";

    int score = 0;

    while (true) {
        int target = rand() % 100 + 1;
        int correct = target * 2;

        cout << "\nLiczba: " << target << " | Podaj liczbe 2x wieksza: ";

        string input;
        getline(cin, input);

        if (input == "koniec") break;

        try {
            int userAnswer = stoi(input);
            if (userAnswer == correct) {
                score += 10;
                cout << "Prawidlowo! (+10 pkt)\n";
            }
            else {
                cout << "Zle! Prawidlowa odpowiedz: " << correct << "\n";
            }
        }
        catch (...) {
            cout << "Zla wartosc!\n";
        }
    }

    int statGain = max(1, score / 15);
    int expGain = score * 2;

    player.stats.vitality += statGain;
    player.maxHp = calculateMaxHp(player);
    player.currentHp = player.maxHp;
    gainExp(player, expGain);
    player.energy = min(player.energy + 10, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU CIALA ===\n\n";
    cout << "Punkty: " << score << "\n";
    cout << "Poziomy: " << statGain << "\n";
    cout << "Witalnosc +" << statGain << "\n";
    cout << "Max HP: " << player.maxHp << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void trainingMenu(Player& player) {
    while (true) {
        clearScreen();
        displayHeader("CENTRUM TRENINGOWE");
        displayPlayerStats(player);

        cout << "Arena Level: " << player.arenaLevel << "/10\n";
        cout << "Mnoznik EXP: " << (int)pow(2, player.arenaLevel) << "x\n\n";

        cout << "1. Trening Sily (Zwieksza obrazenia)\n";
        cout << "2. Trening Obrony (Zwieksza obrone)\n";
        cout << "3. Trening Magii (Zwieksza mage)\n";
        cout << "4. Trening Kondycji (Zwieksza szanse na unik)\n";
        cout << "5. Trening Ciala (Zwieksza HP)\n";
        cout << "6. Ulepsz arenę treningowa (Obecny level: " << player.arenaLevel << "/10)\n";
        cout << "7. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: strengthTraining(player); break;
        case 2: defenseTraining(player); break;
        case 3: magicTraining(player); break;
        case 4: agilityTraining(player); break;
        case 5: bodyTraining(player); break;
        case 6:
            if (player.arenaLevel < 10) {
                player.arenaLevel++;
                cout << "\nArenę ulepszona! Nowy poziom: " << player.arenaLevel << "/10\n";
                cout << "Nowy mnoznik EXP: " << (int)pow(2, player.arenaLevel) << "x\n";
                pause();
            }
            else {
                cout << "\nArena jest juz na maksymalnym poziomie!\n";
                pause();
            }
            break;
        case 7: return;
        default: cout << "Zly wybor!\n"; pause();
        }
    }
}

void combat(Player& player, Enemy& enemy) {
    int playerHp = player.currentHp;
    int enemyHp = enemy.hp;
    int combo = 0;

    clearScreen();
    displayHeader("WALKA NA ARENIE");

    cout << "Przeciwnik: " << enemy.name << " (Poziom " << enemy.level << ")\n";
    cout << "HP: " << enemy.hp << " | Atak: " << enemy.damage << " | Obrona: " << enemy.defense << "\n\n";
    pause();

    while (playerHp > 0 && enemyHp > 0) {
        clearScreen();
        cout << "\n------------------------------------------\n";
        cout << "!          ARENA WALKI                   !\n";
        cout << "------------------------------------------\n\n";

        cout << player.name << " HP: " << playerHp << "/" << player.maxHp << "\n";
        cout << enemy.name << " HP: " << enemyHp << "/" << enemy.maxHp << "\n";
        cout << "Combo: " << combo << "x\n\n";

        cout << "1. Atak Normalny (100% obrazen, 85% trafienie)\n";
        cout << "2. Ciezki Atak (180% obrazen, 65% trafienie)\n";
        cout << "3. Szybki Atak (60% obrazen, 95% trafienie)\n";
        cout << "4. Magiczny Atak (120% + magia, 75% trafienie)\n";
        cout << "5. Obrona (Zmniejsza obrazenia o 50%)\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        bool playerDefending = false;
        int playerDamage = calculateDamage(player);
        double hitChance = 0.85;

        if (choice == 1) {
            hitChance = 0.85;
        }
        else if (choice == 2) {
            playerDamage = static_cast<int>(playerDamage * 1.8);
            hitChance = 0.65;
        }
        else if (choice == 3) {
            playerDamage = static_cast<int>(playerDamage * 0.6);
            hitChance = 0.95;
        }
        else if (choice == 4) {
            playerDamage = static_cast<int>(playerDamage * 1.2 + player.stats.magic * 3);
            hitChance = 0.75;
        }
        else if (choice == 5) {
            playerDefending = true;
            cout << "\nPrzyjalem postawe obronna!\n";
        }

        if (!playerDefending) {
            double roll = static_cast<double>(rand()) / RAND_MAX;
            if (roll < hitChance) {
                int finalDamage = max(1, playerDamage - enemy.defense / 2);
                enemyHp -= finalDamage;
                combo++;
                cout << "\nTrafiles! Obrazenia: " << finalDamage;
                if (combo > 1) cout << " (Combo x" << combo << ")";
                cout << "\n";
            }
            else {
                cout << "\nPudlo!\n";
                combo = 0;
            }
        }

        if (enemyHp <= 0) break;

        this_thread::sleep_for(chrono::milliseconds(800));

        int enemyDamage = enemy.damage + rand() % 40;  // Losowy bonus 0-39
        enemyDamage = (int)(enemyDamage * 1.3);  // Mnożnik x1.3
        double enemyHitChance = 0.75 - (player.stats.agility * 0.01);
        if (enemyHitChance < 0.3) enemyHitChance = 0.3;
        double roll = static_cast<double>(rand()) / RAND_MAX;

        if (roll < enemyHitChance) {
            int defense = calculateDefense(player);
            if (playerDefending) defense *= 2;
            int finalDamage = max(1, enemyDamage - defense);
            playerHp -= finalDamage;
            cout << enemy.name << " atakuje! Otrzymujesz " << finalDamage << " obrazen!\n";
        }
        else {
            cout << enemy.name << " pudluje!\n";
        }

        pause();
    }

    clearScreen();
    if (playerHp > 0) {
        cout << "\n\n";
        cout << "    -----------------------------------------\n";
        cout << "    !                                       !\n";
        cout << "    !           ★ ZWYCIESTWO! ★            !\n";
        cout << "    !                                       !\n";
        cout << "    !     Pokonales: " << setw(20) << left << enemy.name << "!\n";
        cout << "    !                                       !\n";
        cout << "    !     Nagrody:                          !\n";
        cout << "    !     + " << setw(4) << enemy.exp << " EXP                         !\n";
        cout << "    !     + " << setw(4) << enemy.gold << " Zlota                       !\n";

        int bonus = 0;
        if (enemy.firstTimeReward) {
            bonus = enemy.gold * 2;
            cout << "    !     + " << setw(4) << bonus << " BONUS (1x pokonanie)          !\n";
            enemy.firstTimeReward = false;
        }

        cout << "    !                                       !\n";
        cout << "    -----------------------------------------\n\n";

        player.wins++;
        player.gold += enemy.gold + bonus;
        gainExp(player, enemy.exp);
        player.currentHp = playerHp;
    }
    else {
        cout << "\n\n";
        cout << "    -----------------------------------------\n";
        cout << "    !                                       !\n";
        cout << "    !           ✗ PORAZKA ✗                !\n";
        cout << "    !                                       !\n";
        cout << "    !     Zostales pokonany...              !\n";
        cout << "    !     Tracisz polowe zlota              !\n";
        cout << "    !                                       !\n";
        cout << "    -----------------------------------------\n\n";

        player.losses++;
        player.gold = player.gold / 2;
        player.currentHp = player.maxHp / 2;
    }

    player.energy -= 20;
    pause();
}

void arenaMenu(Player& player) {
    vector<Enemy> enemies = {
        // Wzór: HP = 20*lvl² + 50*lvl + 100, DMG = lvl² + 3*lvl + 5

        // Poziom 1-5 (Bardzo łatwy start)
        {"Treningowy Manekn", 170, 170, 9, 5, 20, 10, 1, true},               // HP: 170, DMG: 9
        {"Poczatkujacy Wojownik", 280, 280, 15, 8, 35, 20, 2, true},          // HP: 280, DMG: 15
        {"Mlody Rycerz", 430, 430, 23, 12, 50, 30, 3, true},                  // HP: 430, DMG: 23
        {"Lasowiec", 620, 620, 33, 16, 65, 40, 4, true},                      // HP: 620, DMG: 33
        {"Zbir z Karczmy", 850, 850, 45, 20, 80, 50, 5, true},                // HP: 850, DMG: 45

        // Poziom 6-10
        {"Weteran Miejski", 1120, 1120, 59, 25, 100, 65, 6, true},            // HP: 1,120, DMG: 59
        {"Elitarny Straznik", 1430, 1430, 75, 30, 120, 80, 7, true},          // HP: 1,430, DMG: 75
        {"Szermierz Krolewski", 1780, 1780, 93, 35, 140, 95, 8, true},        // HP: 1,780, DMG: 93
        {"Lowca Nagrod", 2170, 2170, 113, 40, 160, 110, 9, true},             // HP: 2,170, DMG: 113
        {"Mistrz Areny", 2600, 2600, 135, 45, 180, 125, 10, true},            // HP: 2,600, DMG: 135

        // Poziom 11-15
        {"Alchemik Zla", 3070, 3070, 159, 50, 210, 145, 11, true},            // HP: 3,070, DMG: 159
        {"Nieumarły Arcymag", 3580, 3580, 185, 55, 240, 165, 12, true},       // HP: 3,580, DMG: 185
        {"Smok Rycerz", 4130, 4130, 213, 60, 270, 185, 13, true},             // HP: 4,130, DMG: 213
        {"Szal Szamana", 4720, 4720, 243, 65, 300, 205, 14, true},            // HP: 4,720, DMG: 243
        {"Cieniowy Zabojca", 5350, 5350, 275, 70, 330, 225, 15, true},        // HP: 5,350, DMG: 275

        // Poziom 16-20
        {"Bestia Mroku", 6020, 6020, 309, 75, 370, 250, 16, true},            // HP: 6,020, DMG: 309
        {"Krol Podziemi", 6730, 6730, 345, 80, 410, 275, 17, true},           // HP: 6,730, DMG: 345
        {"Wampir Wielowiekowy", 7480, 7480, 383, 85, 450, 300, 18, true},     // HP: 7,480, DMG: 383
        {"Strażnik Bram", 8270, 8270, 423, 90, 490, 325, 19, true},           // HP: 8,270, DMG: 423
        {"Smierc Wcielona", 9100, 9100, 465, 95, 530, 350, 20, true},         // HP: 9,100, DMG: 465

        // Poziom 21-25
        {"Smok Czerni", 9970, 9970, 509, 100, 580, 380, 21, true},            // HP: 9,970, DMG: 509
        {"Tytanicka Bestia", 10880, 10880, 555, 105, 630, 410, 22, true},     // HP: 10,880, DMG: 555
        {"Anioł Zagłady", 11830, 11830, 603, 110, 680, 440, 23, true},        // HP: 11,830, DMG: 603
        {"Władca Czasu", 12820, 12820, 653, 115, 730, 470, 24, true},         // HP: 12,820, DMG: 653
        {"Bestia Behemota", 13850, 13850, 705, 120, 780, 500, 25, true},      // HP: 13,850, DMG: 705

        // Poziom 26-30
        {"Destroyer Wszechświata", 14920, 14920, 759, 125, 840, 535, 26, true},   // HP: 14,920, DMG: 759
        {"Lodowy Feniks", 16030, 16030, 815, 130, 900, 570, 27, true},            // HP: 16,030, DMG: 815
        {"Wladca Demonow", 17180, 17180, 873, 135, 960, 605, 28, true},           // HP: 17,180, DMG: 873
        {"Pierwotny Chaos", 18370, 18370, 933, 140, 1020, 640, 29, true},         // HP: 18,370, DMG: 933
        {"Nieznany Byt z Mroku", 19600, 19600, 995, 150, 1100, 700, 30, true}     // HP: 19,600, DMG: 995
    };
    while (true) {
        clearScreen();
        displayHeader("ARENA WALKI - PRZYGODA");
        displayPlayerStats(player);

        if (player.energy < 15) {
            cout << "\n!!! Za malo energii! (Potrzebujesz 15) !!!\n\n";
        }

        cout << "\nWybierz poziom (1-30):\n\n";

        for (int i = 0; i < 30 && i < (int)enemies.size(); i++) {
            string status = enemies[i].firstTimeReward ? "[Nowy]" : "[*]";
            cout << setw(2) << (i + 1) << ". " << setw(25) << left << enemies[i].name
                << " [Lvl " << setw(2) << enemies[i].level
                << " | HP: " << setw(4) << enemies[i].hp
                << " | Nagroda: " << enemies[i].gold << " zlota] " << status << "\n";
        }
        cout << "0. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 0) return;

        if (choice > 0 && choice <= 30) {
            if (player.energy < 15) {
                cout << "\nZa malo energii!\n";
                pause();
                continue;
            }

            Enemy enemy = enemies[choice - 1];
            enemy.hp += (player.level - enemy.level) * 15;
            enemy.maxHp = enemy.hp;
            enemy.damage += (player.level - enemy.level) * 2;

            combat(player, enemy);

            if (choice == 30 && !enemies[29].firstTimeReward && player.currentHp > 0) {
                clearScreen();
                cout << "\n\n";
                cout << "    ===================================\n";
                cout << "    !                               !\n";
                cout << "    !   GRATULACJE! WYGRAL EŚ! ★   !\n";
                cout << "    !                               !\n";
                cout << "    !   Pokonaleś wszystkie poziomy! !\n";
                cout << "    !                               !\n";
                cout << "    ===================================\n\n";

                cout << "Czy chcesz kontynuowac?\n";
                cout << "1. TAK - Powrot do menu\n";
                cout << "2. NIE - Zamknij program\n";
                cout << "\nWybor: ";

                int endChoice;
                cin >> endChoice;

                if (endChoice == 2) {
                    clearScreen();
                    cout << "\nDziekujemy za gre!\n\n";
                    exit(0);
                }
                return;
            }
        }
        else {
            cout << "Zly wybor!\n";
            pause();
        }
    }
}

vector<Equipment> initWeapons() {
    return {
        {"Patyk", 1, 0, 0, 0, "weapon", 0},
        {"Noz do masla", 5, 0, 0, 50, "weapon", 1},
        {"Kamienny miecz", 25, 0, 0, 150, "weapon", 2},
        {"Zelazny miecz", 75, 0, 0, 500, "weapon", 3},
        {"Zelazny topor", 150, 0, 0, 1200, "weapon", 4},
        {"Kilof z Fortnita", 300, 0, 0, 2500, "weapon", 5},
        {"Zloty kamien", 500, 0, 0, 4000, "weapon", 6},
        {"Zloty miecz", 1000, 0, 0, 7500, "weapon", 7},
        {"Kostur ognia", 2000, 0, 0, 15000, "weapon", 8},
        {"MG 42", 5000, 0, 0, 35000, "weapon", 9},
        {"Piesci legendarnego zuka Biadronia", 10000, 0, 0, 75000, "weapon", 10}
    };
}

vector<Equipment> initArmors() {
    return {
        {"Sweterek cioci Helgi", 0, 1, 3, 0, "armor", 0},
        {"Kamizelka wujka Marjana", 0, 5, 10, 100, "armor", 1},
        {"Lekka drewniana zbroja", 0, 10, 25, 300, "armor", 2},
        {"Lekka kamienna zbroja", 0, 20, 75, 800, "armor", 3},
        {"Ciezka drewniana zbroja", 0, 25, 150, 1500, "armor", 4},
        {"Ciezka kamienna zbroja", 0, 50, 200, 3000, "armor", 5},
        {"Zelazna zbroja", 0, 100, 750, 6000, "armor", 6},
        {"Ciezka zelazna zbroja", 0, 150, 1100, 12000, "armor", 7},
        {"Zbroja z silver_tape", 0, 200, 1500, 20000, "armor", 8},
        {"Zbroja z Noki_3310", 0, 500, 5000, 50000, "armor", 9}
    };
}

vector<Equipment> initShields() {
    return {
        {"Pusta dlon", 0, 0, 0, 0, "shield", 0},
        {"Drewniana tarcza", 0, 10, 0, 100, "shield", 1},
        {"Duza drewniana tarcza", 0, 25, 0, 300, "shield", 2},
        {"Kamienna tarcza", 0, 60, 0, 800, "shield", 3},
        {"Zelazna tarcza", 0, 120, 0, 2000, "shield", 4},
        {"Kaczka", 0, 200, 0, 5000, "shield", 5},
        {"Diamentowa tarcza", 0, 500, 0, 12000, "shield", 6},
        {"Diamentowa tarcza sarkofagowa", 0, 1000, 0, 30000, "shield", 7},
        {"Czarna tarcza", 0, 1500, 0, 50000, "shield", 8},
        {"Biala tarcza", 0, 2000, 0, 80000, "shield", 9},
        {"Kotka", 0, 5000, 0, 200000, "shield", 10}
    };
}

void shopWeapons(Player& player) {
    vector<Equipment> weapons = initWeapons();

    while (true) {
        clearScreen();
        displayHeader("SKLEP - BRONIE");
        displayPlayerStats(player);

        cout << "\nTwoja bron:\n";
        if (player.weapon.name.empty()) {
            cout << "Brak\n\n";
        }
        else {
            cout << player.weapon.name << " (Poziom " << player.weapon.level << ")\n\n";
        }

        cout << "Dostepne ulepszenia:\n\n";

        int startLevel = player.weapon.name.empty() ? 0 : player.weapon.level + 1;

        for (int i = startLevel; i < (int)weapons.size() && i <= startLevel; i++) {
            cout << (i - startLevel + 1) << ". " << weapons[i].name << " (+" << weapons[i].value << " DMG, "
                << weapons[i].price << " zlota)\n";
        }
        cout << "0. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 0) return;

        if (choice == 1 && startLevel < (int)weapons.size()) {
            Equipment newWeapon = weapons[startLevel];

            if (player.gold >= newWeapon.price) {
                player.weapon = newWeapon;
                player.gold -= newWeapon.price;
                cout << "\nUlepszono do: " << newWeapon.name << "!\n";
                cout << "Zapłacono: " << newWeapon.price << " zlota\n";
                cout << "Pozostalo zlota: " << player.gold << "\n";
                pause();
            }
            else {
                cout << "\nZa malo zlota! Potrzebujesz: " << newWeapon.price << " zlota\n";
                cout << "Masz: " << player.gold << " zlota\n";
                pause();
            }
        }
        else {
            cout << "\nZly wybor!\n";
            pause();
        }
    }
}

void shopArmors(Player& player) {
    vector<Equipment> armors = initArmors();

    while (true) {
        clearScreen();
        displayHeader("SKLEP - ZBROJE");
        displayPlayerStats(player);

        cout << "\nTwoja zbroja:\n";
        if (player.armor.name.empty()) {
            cout << "Brak\n\n";
        }
        else {
            cout << player.armor.name << " (Poziom " << player.armor.level << ")\n\n";
        }

        cout << "Dostepne ulepszenia:\n\n";

        int startLevel = player.armor.name.empty() ? 0 : player.armor.level + 1;

        for (int i = startLevel; i < (int)armors.size() && i <= startLevel; i++) {
            cout << (i - startLevel + 1) << ". " << armors[i].name << " (+" << armors[i].defenseValue
                << " DEF, +" << armors[i].healthBonus << " HP, " << armors[i].price << " zlota)\n";
        }
        cout << "0. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 0) return;

        if (choice == 1 && startLevel < (int)armors.size()) {
            Equipment newArmor = armors[startLevel];

            if (player.gold >= newArmor.price) {
                player.armor = newArmor;
                player.gold -= newArmor.price;
                player.maxHp = calculateMaxHp(player);
                player.currentHp = player.maxHp;
                cout << "\nUlepszono do: " << newArmor.name << "!\n";
                cout << "Zapłacono: " << newArmor.price << " zlota\n";
                cout << "Pozostalo zlota: " << player.gold << "\n";
                pause();
            }
            else {
                cout << "\nZa malo zlota! Potrzebujesz: " << newArmor.price << " zlota\n";
                cout << "Masz: " << player.gold << " zlota\n";
                pause();
            }
        }
        else {
            cout << "\nZly wybor!\n";
            pause();
        }
    }
}

void shopShields(Player& player) {
    vector<Equipment> shields = initShields();

    while (true) {
        clearScreen();
        displayHeader("SKLEP - TARCZE");
        displayPlayerStats(player);

        cout << "\nTwoja tarcza:\n";
        if (player.shield.name.empty()) {
            cout << "Brak\n\n";
        }
        else {
            cout << player.shield.name << " (Poziom " << player.shield.level << ")\n\n";
        }

        cout << "Dostepne ulepszenia:\n\n";

        int startLevel = player.shield.name.empty() ? 0 : player.shield.level + 1;

        for (int i = startLevel; i < (int)shields.size() && i <= startLevel; i++) {
            cout << (i - startLevel + 1) << ". " << shields[i].name << " (+" << shields[i].defenseValue
                << " DEF, " << shields[i].price << " zlota)\n";
        }
        cout << "0. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 0) return;

        if (choice == 1 && startLevel < (int)shields.size()) {
            Equipment newShield = shields[startLevel];

            if (player.gold >= newShield.price) {
                player.shield = newShield;
                player.gold -= newShield.price;
                cout << "\nUlepszono do: " << newShield.name << "!\n";
                cout << "Zapłacono: " << newShield.price << " zlota\n";
                cout << "Pozostalo zlota: " << player.gold << "\n";
                pause();
            }
            else {
                cout << "\nZa malo zlota! Potrzebujesz: " << newShield.price << " zlota\n";
                cout << "Masz: " << player.gold << " zlota\n";
                pause();
            }
        }
        else {
            cout << "\nZly wybor!\n";
            pause();
        }
    }
}

void shop(Player& player) {
    while (true) {
        clearScreen();
        displayHeader("SKLEP");
        displayPlayerStats(player);
        displayEquipment(player);

        cout << "\n1. Bronie\n";
        cout << "2. Zbroje\n";
        cout << "3. Tarcze\n";
        cout << "4. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 4) return;

        if (choice == 1) {
            shopWeapons(player);
        }
        else if (choice == 2) {
            shopArmors(player);
        }
        else if (choice == 3) {
            shopShields(player);
        }
        else {
            cout << "Zly wybor!\n";
            pause();
        }
    }
}



void upgradeStats(Player& player) {
    while (player.skillPoints > 0) {
        clearScreen();
        displayHeader("ULEPSZENIA STATYSTYK");
        displayPlayerStats(player);

        cout << "\nMnozniki statystyk:\n";
        cout << "Obrazenia: x" << fixed << setprecision(2) << player.stats.strengthMult << "\n";
        cout << "Obrona: x" << player.stats.defenseMult << "\n";
        cout << "Zdrowotka: x" << player.stats.healthMult << "\n";
        cout << "Magia: x" << player.stats.magicMult << "\n\n";

        cout << "Wybierz statystyke do ulepszenia:\n\n";

        cout << "1. Obrazenia (Mnoznik *2)\n";
        cout << "2. Obrona (Mnoznik *2)\n";
        cout << "3. Zdrowotka (Mnoznik *2)\n";
        cout << "4. Magia (Mnoznik *2)\n";
        cout << "5. Zapisz i wyjdz\n\n";

        cout << "Pozostale punkty: " << player.skillPoints << "\n\n";

        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 5) return;

        if (choice >= 1 && choice <= 4) {
            player.skillPoints--;

            if (choice == 1) {
                player.stats.strengthMult *= 2.0;
                cout << "\nObrazen ulepszone! Nowy mnoznik: x" << fixed << setprecision(2) << player.stats.strengthMult << "\n";
            }
            else if (choice == 2) {
                player.stats.defenseMult *= 2.0;
                cout << "\nObrona ulepszona! Nowy mnoznik: x" << fixed << setprecision(2) << player.stats.defenseMult << "\n";
            }
            else if (choice == 3) {
                player.stats.healthMult *= 2.0;
                player.maxHp = calculateMaxHp(player);
                player.currentHp = player.maxHp;
                cout << "\nZdrowotka ulepszona! Nowy mnoznik: x" << fixed << setprecision(2) << player.stats.healthMult << "\n";
                cout << "Nowe max HP: " << player.maxHp << "\n";
            }
            else if (choice == 4) {
                player.stats.magicMult *= 2.0;
                cout << "\nMagia ulepszona! Nowy mnoznik: x" << fixed << setprecision(2) << player.stats.magicMult << "\n";
            }

            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
}
void drawPlayer(float x, float y, float size) {
    al_draw_filled_rectangle(x, y, x + size, y + size, al_map_rgb(50, 200, 50));
    al_draw_rectangle(x, y, x + size, y + size, al_map_rgb(255, 255, 255), 2);
}

void drawEnemy(float x, float y, float size) {
    al_draw_filled_rectangle(x, y, x + size, y + size, al_map_rgb(200, 50, 50));
    al_draw_rectangle(x, y, x + size, y + size, al_map_rgb(255, 255, 255), 2);
}

void drawHealthBar(float x, float y, float width, float height, int current, int max, ALLEGRO_COLOR color) {
    al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(50, 50, 50));
    float fillWidth = (width * current) / max;
    al_draw_filled_rectangle(x, y, x + fillWidth, y + height, color);
    al_draw_rectangle(x, y, x + width, y + height, al_map_rgb(255, 255, 255), 2);
}

// Nowa funkcja do rysowania pola tekstowego
void drawTextInput(float x, float y, float width, float height, const string& text, bool active, ALLEGRO_FONT* font) {
    ALLEGRO_COLOR bgColor = active ? al_map_rgb(70, 70, 100) : al_map_rgb(50, 50, 70);
    al_draw_filled_rectangle(x, y, x + width, y + height, bgColor);
    al_draw_rectangle(x, y, x + width, y + height, al_map_rgb(255, 255, 255), 2);

    string displayText = text;
    if (active && (int)(al_get_time() * 2) % 2 == 0) {
        displayText += "_";
    }

    al_draw_text(font, al_map_rgb(255, 255, 255), x + 10, y + height / 2 - al_get_font_line_height(font) / 2, 0, displayText.c_str());
}

// Funkcje pomocnicze dla minigier
void resetStrengthGame(float& timer, float& timeLimit, int& stage, int& clicked, int& total,
    float& btnX, float& btnY, bool& active, int screen_w, int screen_h, float scale_x, float scale_y) {
    timer = 0;
    stage = 1;
    clicked = 0;
    total = 10;
    timeLimit = (5.0f * 5.0f) / stage;
    btnX = (rand() % (int)(screen_w - 200 * scale_x)) + 100 * scale_x;
    btnY = (rand() % (int)(screen_h - 400 * scale_y)) + 200 * scale_y;
    active = true;
}

void generateNewMathProblem(int& a, int& b, int& operation, int& answer, int stage, float& timeLimit) {
    operation = rand() % 3;
    int maxNum = 10 + stage * 5;
    a = rand() % maxNum + 1;
    b = rand() % maxNum + 1;

    if (operation == 0) answer = a + b;
    else if (operation == 1) {
        if (a < b) {
            int temp = a;
            a = b;
            b = temp;
        }
        answer = a - b;
    }
    else answer = a * b;

    timeLimit = (5.0f * 5.0f) / stage;
}

void spawnBall(vector<Ball>& balls, int stage, int screen_w, float scale_x) {
    Ball b;
    b.x = rand() % (int)(screen_w - 60 * scale_x) + 30 * scale_x;
    b.y = -30;
    b.speed = 3.0f + stage * 0.5f;
    b.isGolden = (rand() % 20 == 0); // 5% szans na złotą kulkę
    b.active = true;
    balls.push_back(b);
}

void generateNewLetter(char& letter, int stage, float& timeLimit) {
    letter = 'A' + (rand() % 26);
    timeLimit = (5.0f * 5.0f) / stage;
}


int main() {
    srand(static_cast<unsigned int>(time(0)));

    if (!al_init()) {
        cerr << "Blad inicjalizacji Allegro!" << endl;
        return -1;
    }

    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    // Pobierz rozdzielczość monitora
    ALLEGRO_MONITOR_INFO monitor_info;
    al_get_monitor_info(0, &monitor_info);

    int screen_w = monitor_info.x2 - monitor_info.x1;
    int screen_h = monitor_info.y2 - monitor_info.y1;

    // Użyj 90% rozmiaru ekranu dla trybu okienkowego
    int windowed_w = (int)(screen_w * 0.9);
    int windowed_h = (int)(screen_h * 0.9);

    bool fullscreen = false;

    ALLEGRO_DISPLAY* display = al_create_display(windowed_w, windowed_h);
    if (!display) {
        cerr << "Blad tworzenia okna!" << endl;
        return -1;
    }

    al_set_window_title(display, "Mistrzowie Ostrza");

    // Oblicz współczynniki skalowania
    float scale_x = (float)windowed_w / SCREEN_W;
    float scale_y = (float)windowed_h / SCREEN_H;

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);

    // Skaluj rozmiar czcionek proporcjonalnie
    int base_font_size = (int)(20 * scale_y);
    int title_font_size = (int)(48 * scale_y);
    int small_font_size = (int)(16 * scale_y);

    ALLEGRO_FONT* font = al_load_ttf_font("arial.ttf", base_font_size, 0);
    ALLEGRO_FONT* titleFont = al_load_ttf_font("arial.ttf", title_font_size, 0);
    ALLEGRO_FONT* smallFont = al_load_ttf_font("arial.ttf", small_font_size, 0);

    if (!font || !titleFont || !smallFont) {
        font = al_create_builtin_font();
        titleFont = al_create_builtin_font();
        smallFont = al_create_builtin_font();
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // Inicjalizacja gracza
    Player player;
    player.name = "";
    player.level = 1;
    player.exp = 0;
    player.expToNext = 100;
    player.gold = 100;
    player.stats.strength = 10;
    player.stats.agility = 10;
    player.stats.vitality = 10;
    player.stats.magic = 10;
    player.stats.strengthMult = 1.0;
    player.stats.defenseMult = 1.0;
    player.stats.healthMult = 1.0;
    player.stats.magicMult = 1.0;
    player.maxHp = 100;
    player.currentHp = 100;
    player.energy = 100;
    player.maxEnergy = 100;
    player.skillPoints = 0;
    player.wins = 0;
    player.losses = 0;
    player.saveSlot = 1;
    player.arenaLevel = 0;

    vector<Equipment> weapons = initWeapons();
    vector<Equipment> armors = initArmors();
    vector<Equipment> shields = initShields();

    // Lista przeciwników
    vector<Enemy> enemies = {
        // Wzór: HP = 20*lvl² + 50*lvl + 100, DMG = lvl² + 3*lvl + 5

        // Poziom 1-5 (Bardzo łatwy start)
        {"Treningowy Manekn", 170, 170, 9, 5, 20, 10, 1, true},               // HP: 170, DMG: 9
        {"Poczatkujacy Wojownik", 280, 280, 15, 8, 35, 20, 2, true},          // HP: 280, DMG: 15
        {"Mlody Rycerz", 430, 430, 23, 12, 50, 30, 3, true},                  // HP: 430, DMG: 23
        {"Lasowiec", 620, 620, 33, 16, 65, 40, 4, true},                      // HP: 620, DMG: 33
        {"Zbir z Karczmy", 850, 850, 45, 20, 80, 50, 5, true},                // HP: 850, DMG: 45

        // Poziom 6-10
        {"Weteran Miejski", 1120, 1120, 59, 25, 100, 65, 6, true},            // HP: 1,120, DMG: 59
        {"Elitarny Straznik", 1430, 1430, 75, 30, 120, 80, 7, true},          // HP: 1,430, DMG: 75
        {"Szermierz Krolewski", 1780, 1780, 93, 35, 140, 95, 8, true},        // HP: 1,780, DMG: 93
        {"Lowca Nagrod", 2170, 2170, 113, 40, 160, 110, 9, true},             // HP: 2,170, DMG: 113
        {"Mistrz Areny", 2600, 2600, 135, 45, 180, 125, 10, true},            // HP: 2,600, DMG: 135

        // Poziom 11-15
        {"Alchemik Zla", 3070, 3070, 159, 50, 210, 145, 11, true},            // HP: 3,070, DMG: 159
        {"Nieumarły Arcymag", 3580, 3580, 185, 55, 240, 165, 12, true},       // HP: 3,580, DMG: 185
        {"Smok Rycerz", 4130, 4130, 213, 60, 270, 185, 13, true},             // HP: 4,130, DMG: 213
        {"Szal Szamana", 4720, 4720, 243, 65, 300, 205, 14, true},            // HP: 4,720, DMG: 243
        {"Cieniowy Zabojca", 5350, 5350, 275, 70, 330, 225, 15, true},        // HP: 5,350, DMG: 275

        // Poziom 16-20
        {"Bestia Mroku", 6020, 6020, 309, 75, 370, 250, 16, true},            // HP: 6,020, DMG: 309
        {"Krol Podziemi", 6730, 6730, 345, 80, 410, 275, 17, true},           // HP: 6,730, DMG: 345
        {"Wampir Wielowiekowy", 7480, 7480, 383, 85, 450, 300, 18, true},     // HP: 7,480, DMG: 383
        {"Strażnik Bram", 8270, 8270, 423, 90, 490, 325, 19, true},           // HP: 8,270, DMG: 423
        {"Smierc Wcielona", 9100, 9100, 465, 95, 530, 350, 20, true},         // HP: 9,100, DMG: 465

        // Poziom 21-25
        {"Smok Czerni", 9970, 9970, 509, 100, 580, 380, 21, true},            // HP: 9,970, DMG: 509
        {"Tytanicka Bestia", 10880, 10880, 555, 105, 630, 410, 22, true},     // HP: 10,880, DMG: 555
        {"Anioł Zagłady", 11830, 11830, 603, 110, 680, 440, 23, true},        // HP: 11,830, DMG: 603
        {"Władca Czasu", 12820, 12820, 653, 115, 730, 470, 24, true},         // HP: 12,820, DMG: 653
        {"Bestia Behemota", 13850, 13850, 705, 120, 780, 500, 25, true},      // HP: 13,850, DMG: 705

        // Poziom 26-30
        {"Destroyer Wszechświata", 14920, 14920, 759, 125, 840, 535, 26, true},   // HP: 14,920, DMG: 759
        {"Lodowy Feniks", 16030, 16030, 815, 130, 900, 570, 27, true},            // HP: 16,030, DMG: 815
        {"Wladca Demonow", 17180, 17180, 873, 135, 960, 605, 28, true},           // HP: 17,180, DMG: 873
        {"Pierwotny Chaos", 18370, 18370, 933, 140, 1020, 640, 29, true},         // HP: 18,370, DMG: 933
        {"Nieznany Byt z Mroku", 19600, 19600, 995, 150, 1100, 700, 30, true}     // HP: 19,600, DMG: 995
    };

    GameState state = TITLE_SCREEN;
    int selectedEnemy = -1;
    int combatChoice = -1;
    bool playerDefending = false;
    int combo = 0;
    string playerNameInput = "";
    bool inputActive = false;
    int selectedSaveSlot = 0;
    int previousLevel = player.level;
    MinigameResult lastMinigameResult;

    // Zmienne dla minigier
    float miniGameTimer = 0;
    float miniGameTimeLimit = 5.0f;
    int miniGameStage = 1;
    int miniGameScore = 0;
    float baseTime = 5.0f;

    // Minigra Siła - klikanie przycisków
    int strengthButtonsClicked = 0;
    int strengthButtonsTotal = 10;
    float strengthButtonX = 0;
    float strengthButtonY = 0;
    bool strengthButtonActive = false;

    // Minigra Magia - matematyka
    int mathA = 0, mathB = 0, mathAnswer = 0;
    int mathOperation = 0; // 0=+, 1=-, 2=*
    string mathInput = "";
    bool mathInputActive = false;

    // Minigra Agility - unikanie kulek
    float playerX = 0;
    float playerSpeed = 5.0f; // Wolniejszy gracz
    vector<Ball> balls;
    float ballSpawnTimer = 0;
    float ballSpawnInterval = 0.5f; // Więcej kulek
    bool leftPressed = false, rightPressed = false;
    float survivalTimer = 0; // Timer do punktów za przetrwanie

    // Minigra Witalność - wciskanie liter
    char targetLetter = 'A';
    bool letterShown = false;

    //komunikat o zapisie
    bool savePointsMessage = false;
    float saveMessageTimer = 0;

    // Komunikat o braku punktów
    bool showNoPointsMessage = false;
    float noPointsMessageTimer = 0;

    // Stan poprzedni do powrotu po level up
    GameState stateBeforeLevelUp = MENU;

    al_start_timer(timer);

    bool running = true;
    bool redraw = true;
    int mouseX = 0, mouseY = 0;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

            if (showSaveMessage) {
                saveMessageTimer += 1.0 / 60.0;
                if (saveMessageTimer > 2.0) {
                    showSaveMessage = false;
                    saveMessageTimer = 0;
                }
            }

            if (showDeleteMessage) {
                deleteMessageTimer += 1.0 / 60.0;
                if (deleteMessageTimer > 2.0) {
                    showDeleteMessage = false;
                    deleteMessageTimer = 0;
                }
            }

            // Timer dla komunikatu o braku punktów (2 sekundy)
            if (showNoPointsMessage) {
                noPointsMessageTimer += 1.0 / 60.0;
                if (noPointsMessageTimer > 2.0) {
                    showNoPointsMessage = false;
                    noPointsMessageTimer = 0;
                }
            }

            // Sprawdź czy był awans - wyświetl ekran level up
            if (player.level > previousLevel && state != LEVEL_UP_SCREEN) {
                stateBeforeLevelUp = state;
                state = LEVEL_UP_SCREEN;
                previousLevel = player.level;
            }

            // Timery dla minigier
            if (state == MINIGRA_SILA && strengthButtonActive) {
                miniGameTimer += 1.0 / 60.0;
                if (miniGameTimer >= miniGameTimeLimit) {
                    // ⭐ ZAPISZ WYNIKI
                    lastMinigameResult.minigameName = "TRENING SILY";
                    lastMinigameResult.scoreAchieved = miniGameScore;
                    lastMinigameResult.statGained = miniGameScore;
                    lastMinigameResult.statName = "Sila";
                    lastMinigameResult.expGained = miniGameScore * 2;
                    lastMinigameResult.levelBefore = player.level;

                    player.stats.strength += miniGameScore;
                    gainExp(player, miniGameScore * 2);

                    lastMinigameResult.levelAfter = player.level;
                    state = MINIGAME_RESULTS;
                }
            }
            else if (state == MINIGRA_MAGIA) {
                miniGameTimer += 1.0 / 60.0;
                if (miniGameTimer >= miniGameTimeLimit) {
                    // ⭐ ZAPISZ WYNIKI
                    lastMinigameResult.minigameName = "TRENING MAGII";
                    lastMinigameResult.scoreAchieved = miniGameScore;
                    lastMinigameResult.statGained = miniGameScore;
                    lastMinigameResult.statName = "Magia";
                    lastMinigameResult.expGained = miniGameScore * 2;
                    lastMinigameResult.levelBefore = player.level;

                    player.stats.magic += miniGameScore;
                    gainExp(player, miniGameScore * 2);

                    lastMinigameResult.levelAfter = player.level;
                    state = MINIGAME_RESULTS;
                }
            }
            else if (state == MINIGRA_AGILITY) {
                int current_w = fullscreen ? screen_w : windowed_w;
                int current_h = fullscreen ? screen_h : windowed_h;

                // Timer przetrwania - co 5 sekund +5 punktów
                survivalTimer += 1.0 / 60.0;
                if (survivalTimer >= 5.0) {
                    miniGameScore += 5;
                    survivalTimer = 0;
                }

                // Ruch gracza
                if (leftPressed && playerX > 0) playerX -= playerSpeed * scale_x;
                if (rightPressed && playerX < current_w - 60 * scale_x) playerX += playerSpeed * scale_x;

                // Spawn kulek - częściej
                ballSpawnTimer += 1.0 / 60.0;
                if (ballSpawnTimer >= ballSpawnInterval) {
                    spawnBall(balls, miniGameStage, current_w, scale_x);
                    ballSpawnTimer = 0;
                    ballSpawnInterval = max(0.2f, 0.5f - miniGameStage * 0.02f);
                }

                // Update kulek
                for (auto& ball : balls) {
                    if (ball.active) {
                        ball.y += ball.speed * scale_y;

                        // Sprawdź kolizję z graczem
                        if (ball.y + 30 * scale_y > current_h - 150 * scale_y &&
                            ball.y < current_h - 100 * scale_y &&
                            ball.x > playerX - 30 * scale_x &&
                            ball.x < playerX + 60 * scale_x) {

                            if (ball.isGolden) {
                                miniGameScore += 5;
                                ball.active = false;
                            }
                            else {
                                // ⭐ ZAPISZ WYNIKI
                                lastMinigameResult.minigameName = "TRENING ZRECZNOSCI";
                                lastMinigameResult.scoreAchieved = miniGameScore;
                                lastMinigameResult.statGained = miniGameScore;
                                lastMinigameResult.statName = "Zrecznosc";
                                lastMinigameResult.expGained = miniGameScore * 2;
                                lastMinigameResult.levelBefore = player.level;

                                player.stats.agility += miniGameScore;
                                gainExp(player, miniGameScore * 2);

                                lastMinigameResult.levelAfter = player.level;
                                state = MINIGAME_RESULTS;
                                balls.clear();
                            }
                        }

                        // Usuń kulki poza ekranem
                        if (ball.y > current_h) ball.active = false;
                    }
                }

                // Zwiększ trudność co 10 punktów
                if (miniGameScore / 10 > miniGameStage - 1) {
                    miniGameStage++;
                }
            }
            else if (state == MINIGRA_WITALNOSC && letterShown) {
                miniGameTimer += 1.0 / 60.0;
                if (miniGameTimer >= miniGameTimeLimit) {
                    // ⭐ ZAPISZ WYNIKI
                    lastMinigameResult.minigameName = "TRENING WITALNOSCI";
                    lastMinigameResult.scoreAchieved = miniGameScore;
                    lastMinigameResult.statGained = miniGameScore;
                    lastMinigameResult.statName = "Witalnosc";
                    lastMinigameResult.expGained = miniGameScore * 2;
                    lastMinigameResult.levelBefore = player.level;

                    player.stats.vitality += miniGameScore;
                    gainExp(player, miniGameScore * 2);
                    player.maxHp = calculateMaxHp(player);
                    player.currentHp = player.maxHp;

                    lastMinigameResult.levelAfter = player.level;
                    state = MINIGAME_RESULTS;
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouseX = event.mouse.x;
            mouseY = event.mouse.y;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // F11 - przełączanie fullscreen
            if (event.keyboard.keycode == ALLEGRO_KEY_F11) {
                fullscreen = !fullscreen;
                al_destroy_display(display);

                if (fullscreen) {
                    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
                    display = al_create_display(screen_w, screen_h);
                    scale_x = (float)screen_w / SCREEN_W;
                    scale_y = (float)screen_h / SCREEN_H;
                }
                else {
                    al_set_new_display_flags(0);
                    display = al_create_display(windowed_w, windowed_h);
                    scale_x = (float)windowed_w / SCREEN_W;
                    scale_y = (float)windowed_h / SCREEN_H;
                }

                al_register_event_source(queue, al_get_display_event_source(display));
            }

            // Minigra Agility - sterowanie strzałkami
            if (state == MINIGRA_AGILITY) {
                if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) leftPressed = true;
                if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) rightPressed = true;
            }

            // Minigra Witalność - wciskanie liter
            if (state == MINIGRA_WITALNOSC && letterShown) {
                if (event.keyboard.keycode >= ALLEGRO_KEY_A && event.keyboard.keycode <= ALLEGRO_KEY_Z) {
                    char pressed = 'A' + (event.keyboard.keycode - ALLEGRO_KEY_A);
                    if (pressed == targetLetter) {
                        miniGameScore++;
                        miniGameStage++;
                        generateNewLetter(targetLetter, miniGameStage, miniGameTimeLimit);
                        miniGameTimer = 0;
                    }
                    else {
                        // ⭐ ZAPISZ WYNIKI
                        lastMinigameResult.minigameName = "TRENING WITALNOSCI";
                        lastMinigameResult.scoreAchieved = miniGameScore;
                        lastMinigameResult.statGained = miniGameScore;
                        lastMinigameResult.statName = "Witalnosc";
                        lastMinigameResult.expGained = miniGameScore * 2;
                        lastMinigameResult.levelBefore = player.level;

                        player.stats.vitality += miniGameScore;
                        gainExp(player, miniGameScore * 2);
                        player.maxHp = calculateMaxHp(player);
                        player.currentHp = player.maxHp;

                        lastMinigameResult.levelAfter = player.level;
                        state = MINIGAME_RESULTS;
                    }
                }
            }

            // Obsługa wpisywania nazwy
            if (inputActive && state == NOWA_GRA) {
                if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                    if (!playerNameInput.empty()) {
                        playerNameInput.pop_back();
                    }
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    if (!playerNameInput.empty() && selectedSaveSlot > 0) {
                        // Utwórz nową grę
                        player.name = playerNameInput;
                        player.level = 1;
                        player.exp = 0;
                        player.expToNext = 100;
                        player.gold = 100;
                        player.stats.strength = 10;
                        player.stats.agility = 10;
                        player.stats.vitality = 10;
                        player.stats.magic = 10;
                        player.stats.strengthMult = 1.0;
                        player.stats.defenseMult = 1.0;
                        player.stats.healthMult = 1.0;
                        player.stats.magicMult = 1.0;
                        player.maxHp = calculateMaxHp(player);
                        player.currentHp = player.maxHp;
                        player.energy = 100;
                        player.maxEnergy = 100;
                        player.skillPoints = 0;
                        player.wins = 0;
                        player.losses = 0;
                        player.saveSlot = selectedSaveSlot;
                        player.arenaLevel = 0;
                        player.weapon = weapons[0];
                        player.armor = armors[0];
                        player.shield = shields[0];

                        saveGame(player);
                        playerNameInput = "";
                        inputActive = false;
                        selectedSaveSlot = 0;
                        state = MENU;
                    }
                }
                else if (event.keyboard.keycode >= ALLEGRO_KEY_A && event.keyboard.keycode <= ALLEGRO_KEY_Z) {
                    if (playerNameInput.length() < 20) {
                        char c = 'A' + (event.keyboard.keycode - ALLEGRO_KEY_A);
                        if (!(event.keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT)) {
                            c = c - 'A' + 'a';
                        }
                        playerNameInput += c;
                    }
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    if (playerNameInput.length() < 20) {
                        playerNameInput += ' ';
                    }
                }
            }

            // Obsługa wpisywania odpowiedzi w minigrze matematycznej
            if (mathInputActive && state == MINIGRA_MAGIA) {
                if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                    if (!mathInput.empty()) {
                        mathInput.pop_back();
                    }
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    if (!mathInput.empty()) {
                        try {
                            int userAnswer = stoi(mathInput);
                            if (userAnswer == mathAnswer) {
                                miniGameScore++;
                                miniGameStage++;
                                mathInput = "";
                                generateNewMathProblem(mathA, mathB, mathOperation, mathAnswer, miniGameStage, miniGameTimeLimit);
                                miniGameTimer = 0;
                            }
                            else {
                                // ⭐ ZAPISZ WYNIKI
                                lastMinigameResult.minigameName = "TRENING MAGII";
                                lastMinigameResult.scoreAchieved = miniGameScore;
                                lastMinigameResult.statGained = miniGameScore;
                                lastMinigameResult.statName = "Magia";
                                lastMinigameResult.expGained = miniGameScore * 2;
                                lastMinigameResult.levelBefore = player.level;

                                player.stats.magic += miniGameScore;
                                gainExp(player, miniGameScore * 2);

                                lastMinigameResult.levelAfter = player.level;
                                state = MINIGAME_RESULTS;
                            }
                        }
                        catch (...) {
                            mathInput = "";
                        }
                    }
                }
                else if (event.keyboard.keycode >= ALLEGRO_KEY_0 && event.keyboard.keycode <= ALLEGRO_KEY_9) {
                    mathInput += ('0' + (event.keyboard.keycode - ALLEGRO_KEY_0));
                }
                else if (event.keyboard.keycode >= ALLEGRO_KEY_PAD_0 && event.keyboard.keycode <= ALLEGRO_KEY_PAD_9) {
                    mathInput += ('0' + (event.keyboard.keycode - ALLEGRO_KEY_PAD_0));
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_MINUS || event.keyboard.keycode == ALLEGRO_KEY_PAD_MINUS) {
                    if (mathInput.empty()) {
                        mathInput += '-';
                    }
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            if (state == MINIGRA_AGILITY) {
                if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) leftPressed = false;
                if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) rightPressed = false;
            }
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int current_w = fullscreen ? screen_w : windowed_w;
            int current_h = fullscreen ? screen_h : windowed_h;

            // TITLE SCREEN - wybór nowa gra / wczytaj
            if (state == TITLE_SCREEN) {
                Button newGameBtn = createButton(current_w / 2 - 150 * scale_x, 400 * scale_y, 300 * scale_x, 60 * scale_y,
                    "NOWA GRA", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button loadGameBtn = createButton(current_w / 2 - 150 * scale_x, 500 * scale_y, 300 * scale_x, 60 * scale_y,
                    "WCZYTAJ ZAPIS", al_map_rgb(50, 100, 200), al_map_rgb(255, 255, 255), al_map_rgb(70, 120, 220));

                if (isMouseOverButton(newGameBtn, mouseX, mouseY)) {
                    state = NOWA_GRA;
                    playerNameInput = "";
                    selectedSaveSlot = 0;
                    inputActive = false;
                }
                else if (isMouseOverButton(loadGameBtn, mouseX, mouseY)) {
                    state = TRAINING; // Używamy TRAINING jako stan wyboru zapisu do wczytania
                }
            }
            // NOWA GRA - wybór miejsca i nazwa
            else if (state == NOWA_GRA) {
                // Przyciski wyboru miejsca zapisu
                for (int i = 1; i <= 3; i++) {
                    Button slotBtn = createButton(current_w / 2 - 150 * scale_x, (250 + i * 70) * scale_y, 300 * scale_x, 50 * scale_y,
                        "", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                    if (isMouseOverButton(slotBtn, mouseX, mouseY)) {
                        selectedSaveSlot = i;
                    }
                }

                // Pole tekstowe
                Button inputBox = createButton(current_w / 2 - 200 * scale_x, 550 * scale_y, 400 * scale_x, 50 * scale_y,
                    "", al_map_rgb(50, 50, 70), al_map_rgb(255, 255, 255), al_map_rgb(70, 70, 100));
                if (isMouseOverButton(inputBox, mouseX, mouseY)) {
                    inputActive = true;
                }

                // Przycisk powrotu
                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 650 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                if (isMouseOverButton(backBtn, mouseX, mouseY)) {
                    state = TITLE_SCREEN;
                    playerNameInput = "";
                    selectedSaveSlot = 0;
                    inputActive = false;
                }
            }
            // TRAINING - używamy jako stan wyboru zapisu do wczytania
            else if (state == TRAINING) {
                for (int i = 1; i <= 3; i++) {
                    Button slotBtn = createButton(current_w / 2 - 200 * scale_x, (250 + i * 80) * scale_y, 400 * scale_x, 60 * scale_y,
                        "", al_map_rgb(100, 100, 150), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 180));
                    if (isMouseOverButton(slotBtn, mouseX, mouseY)) {
                        if (saveGameExists(i)) {
                            player.saveSlot = i;
                            if (loadGame(player)) {
                                state = MENU;
                            }
                        }
                    }
                }

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 650 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                if (isMouseOverButton(backBtn, mouseX, mouseY)) {
                    state = TITLE_SCREEN;
                }
            }
            // MENU
            else if (state == MENU) {
                Button arenaBtn = createButton(current_w / 2 - 150 * scale_x, 340 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ARENA", al_map_rgb(200, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 70, 70));
                Button shopBtn = createButton(current_w / 2 - 150 * scale_x, 430 * scale_y, 300 * scale_x, 60 * scale_y,
                    "SKLEP", al_map_rgb(50, 50, 200), al_map_rgb(255, 255, 255), al_map_rgb(70, 70, 220));
                Button statsBtn = createButton(current_w / 2 - 150 * scale_x, 520 * scale_y, 300 * scale_x, 60 * scale_y,
                    "STATYSTYKI", al_map_rgb(150, 50, 150), al_map_rgb(255, 255, 255), al_map_rgb(170, 70, 170));
                Button trainingBtn = createButton(current_w / 2 - 150 * scale_x, 610 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ARENA TRENINGOWA", al_map_rgb(100, 200, 100), al_map_rgb(255, 255, 255), al_map_rgb(120, 220, 120));
                Button saveBtn = createButton(current_w / 2 - 150 * scale_x, 730 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ZAPISZ GRE", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button manageBtn = createButton(current_w / 2 - 150 * scale_x, 810 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ZARZADZAJ ZAPISAMI", al_map_rgb(150, 100, 50), al_map_rgb(255, 255, 255), al_map_rgb(180, 120, 70));
                Button exitBtn = createButton(current_w / 2 - 300 * scale_x, 810 * scale_y, 130 * scale_x, 60 * scale_y,
                    "wyjscie", al_map_rgb(150, 0, 50), al_map_rgb(255, 255, 255), al_map_rgb(180, 20, 70));

                if (isMouseOverButton(arenaBtn, mouseX, mouseY)) {
                    state = ARENA;
                }
                else if (isMouseOverButton(shopBtn, mouseX, mouseY)) {
                    state = SHOP;
                }
                else if (isMouseOverButton(statsBtn, mouseX, mouseY)) {
                    state = STATYSTYKI;
                }
                else if (isMouseOverButton(trainingBtn, mouseX, mouseY)) {
                    state = ARENA_TRENINGOWA;
                }
                else if (isMouseOverButton(saveBtn, mouseX, mouseY)) {
                    saveGame(player);
                    showSaveMessage = true;
                    saveMessageTimer = 0;
                }
                else if (isMouseOverButton(manageBtn, mouseX, mouseY)) {
                    state = ZARZAD;
                }
                else if (isMouseOverButton(exitBtn, mouseX, mouseY)) {
                    running = false;
                }

            }
            // ARENA TRENINGOWA - wybór minigry
            else if (state == ARENA_TRENINGOWA) {
                Button strengthBtn = createButton(current_w / 2 - 150 * scale_x, 300 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING SILY", al_map_rgb(200, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 70, 70));
                Button magicBtn = createButton(current_w / 2 - 150 * scale_x, 390 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING MAGII", al_map_rgb(100, 50, 200), al_map_rgb(255, 255, 255), al_map_rgb(120, 70, 220));
                Button agilityBtn = createButton(current_w / 2 - 150 * scale_x, 480 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING ZRECZNOSCI", al_map_rgb(50, 200, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 220, 70));
                Button vitalityBtn = createButton(current_w / 2 - 150 * scale_x, 570 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING WITALNOSCI", al_map_rgb(200, 100, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 120, 70));
                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));

                if (isMouseOverButton(strengthBtn, mouseX, mouseY)) {
                    state = MINIGRA_SILA;
                    resetStrengthGame(miniGameTimer, miniGameTimeLimit, miniGameStage, strengthButtonsClicked,
                        strengthButtonsTotal, strengthButtonX, strengthButtonY, strengthButtonActive,
                        current_w, current_h, scale_x, scale_y);
                    miniGameScore = 0;
                }
                else if (isMouseOverButton(magicBtn, mouseX, mouseY)) {
                    state = MINIGRA_MAGIA;
                    miniGameTimer = 0;
                    miniGameStage = 1;
                    miniGameScore = 0;
                    mathInput = "";
                    mathInputActive = true;
                    generateNewMathProblem(mathA, mathB, mathOperation, mathAnswer, miniGameStage, miniGameTimeLimit);
                }
                else if (isMouseOverButton(agilityBtn, mouseX, mouseY)) {
                    state = MINIGRA_AGILITY;
                    miniGameTimer = 0;
                    miniGameStage = 1;
                    miniGameScore = 0;
                    playerX = current_w / 2;
                    balls.clear();
                    ballSpawnTimer = 0;
                    ballSpawnInterval = 0.5f;
                    survivalTimer = 0;
                    leftPressed = false;
                    rightPressed = false;
                }
                else if (isMouseOverButton(vitalityBtn, mouseX, mouseY)) {
                    state = MINIGRA_WITALNOSC;
                    miniGameTimer = 0;
                    miniGameStage = 1;
                    miniGameScore = 0;
                    generateNewLetter(targetLetter, miniGameStage, miniGameTimeLimit);
                    letterShown = true;
                }
                else if (isMouseOverButton(backBtn, mouseX, mouseY)) {
                    state = MENU;
                }
            }
            // MINIGRA SIŁA - klikanie przycisków
            else if (state == MINIGRA_SILA && strengthButtonActive) {
                Button targetBtn = createButton(strengthButtonX, strengthButtonY, 100 * scale_x, 100 * scale_y,
                    "KLIK!", al_map_rgb(255, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(255, 150, 150));

                if (isMouseOverButton(targetBtn, mouseX, mouseY)) {
                    miniGameScore++;
                    strengthButtonsClicked++;
                    miniGameTimer = 0;
                    miniGameStage++;
                    miniGameTimeLimit = (5.0f * 5.0f) / miniGameStage;

                    if (strengthButtonsClicked >= strengthButtonsTotal) {
                        // ⭐ ZAPISZ WYNIKI
                        lastMinigameResult.minigameName = "TRENING SILY";
                        lastMinigameResult.scoreAchieved = miniGameScore;
                        lastMinigameResult.statGained = miniGameScore;
                        lastMinigameResult.statName = "Sila";
                        lastMinigameResult.expGained = miniGameScore * 2;
                        lastMinigameResult.levelBefore = player.level;

                        player.stats.strength += miniGameScore;
                        gainExp(player, miniGameScore * 2);

                        lastMinigameResult.levelAfter = player.level;
                        state = MINIGAME_RESULTS;
                    }
                    else {
                        // Nowy przycisk
                        strengthButtonX = (rand() % (int)(current_w - 200 * scale_x)) + 100 * scale_x;
                        strengthButtonY = (rand() % (int)(current_h - 400 * scale_y)) + 200 * scale_y;
                    }
                }
            }
            // MINIGRA MAGIA - matematyka
            else if (state == MINIGRA_MAGIA && mathInputActive) {
                // Pole tekstowe do wpisywania odpowiedzi
                Button inputBox = createButton(current_w / 2 - 100 * scale_x, 450 * scale_y, 200 * scale_x, 50 * scale_y,
                    "", al_map_rgb(50, 50, 70), al_map_rgb(255, 255, 255), al_map_rgb(70, 70, 100));

                if (isMouseOverButton(inputBox, mouseX, mouseY)) {
                    mathInputActive = true;
                }
            }
           // ARENA - wybor przeciwnika
            else if (state == ARENA) {
                float btnW = 220 * scale_x;
                float btnH = 50 * scale_y;
                float startX = 50 * scale_x;
                float startY = 150 * scale_y;
                float gapX = 20 * scale_x;
                float gapY = 15 * scale_y;

                for (int i = 0; i < 30; i++) {
                    int row = i / 6;
                    int col = i % 6;
                    float x = startX + col * (btnW + gapX);
                    float y = startY + row * (btnH + gapY);

                    Button enemyBtn = createButton(x, y, btnW, btnH, "",
                        al_map_rgb(150, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(200, 70, 70));

                    if (isMouseOverButton(enemyBtn, mouseX, mouseY)) {
                        if (player.level >= enemies[i].level) {
                            selectedEnemy = i;

                            // ⭐ PARABOLICZNE SKALOWANIE
                            int levelDiff = player.level - enemies[i].level;

                            // HP bonus: 8x²
                            int hpBonus = 8 * levelDiff * levelDiff;

                            // DMG bonus: 3x²
                            int dmgBonus = 3 * levelDiff * levelDiff;

                            enemies[i].hp = enemies[i].maxHp + hpBonus;
                            enemies[i].maxHp = enemies[i].hp;
                            enemies[i].damage += dmgBonus;

                            player.currentHp = player.maxHp;
                            combo = 0;
                            state = FIGHT;
                            break;
                        }
                        else {
                            // ⭐ OPCJONALNIE: Można dodać dźwięk lub efekt "zablokowane"
                            // Na razie nic się nie dzieje przy kliknięciu zablokowanego
                        }
                    }
                }

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 850 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                if (isMouseOverButton(backBtn, mouseX, mouseY)) {
                    state = MENU;
                }
                }
            // WALKA
            else if (state == FIGHT && selectedEnemy >= 0) {
                Button attackBtn = createButton(100 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "ATAK (100%)", al_map_rgb(150, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(180, 70, 70));
                Button heavyBtn = createButton(350 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "CIEZKI (180%)", al_map_rgb(200, 30, 30), al_map_rgb(255, 255, 255), al_map_rgb(220, 50, 50));
                Button quickBtn = createButton(600 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "SZYBKI (60%)", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button magicBtn = createButton(850 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "MAGIA (120%)", al_map_rgb(100, 50, 200), al_map_rgb(255, 255, 255), al_map_rgb(120, 70, 220));
                Button defenseBtn = createButton(1100 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "OBRONA", al_map_rgb(50, 100, 150), al_map_rgb(255, 255, 255), al_map_rgb(70, 120, 180));

                if (isMouseOverButton(attackBtn, mouseX, mouseY)) {
                    combatChoice = 1;
                }
                else if (isMouseOverButton(heavyBtn, mouseX, mouseY)) {
                    combatChoice = 2;
                }
                else if (isMouseOverButton(quickBtn, mouseX, mouseY)) {
                    combatChoice = 3;
                }
                else if (isMouseOverButton(magicBtn, mouseX, mouseY)) {
                    combatChoice = 4;
                }
                else if (isMouseOverButton(defenseBtn, mouseX, mouseY)) {
                    combatChoice = 5;
                }

                // Wykonaj turę walki
                if (combatChoice > 0) {
                    Enemy& enemy = enemies[selectedEnemy];
                    playerDefending = false;
                    int playerDamage = calculateDamage(player);
                    double hitChance = 0.85;

                    if (combatChoice == 1) hitChance = 0.85;
                    else if (combatChoice == 2) { playerDamage = (int)(playerDamage * 1.8); hitChance = 0.65; }
                    else if (combatChoice == 3) { playerDamage = (int)(playerDamage * 0.6); hitChance = 0.95; }
                    else if (combatChoice == 4) { playerDamage = (int)(playerDamage * 1.2 + player.stats.magic * 3); hitChance = 0.75; }
                    else if (combatChoice == 5) playerDefending = true;

                    if (!playerDefending) {
                        double roll = (double)rand() / RAND_MAX;
                        if (roll < hitChance) {
                            int finalDamage = max(1, playerDamage - enemy.defense / 2);
                            enemy.hp -= finalDamage;
                            combo++;
                        }
                        else {
                            combo = 0;
                        }
                    }

                    // Atak wroga
                    if (enemy.hp > 0) {
                        int enemyDamage = enemy.damage + rand() % 40;  // Losowy bonus 0-39
                        enemyDamage = (int)(enemyDamage * 1.3);  // Mnożnik x1.3
                        double enemyHitChance = 0.75 - (player.stats.agility * 0.01);
                        if (enemyHitChance < 0.3) enemyHitChance = 0.3;
                        double roll = (double)rand() / RAND_MAX;

                        if (roll < enemyHitChance) {
                            int defense = calculateDefense(player);
                            if (playerDefending) defense *= 2;
                            int finalDamage = max(1, enemyDamage - defense);
                            player.currentHp -= finalDamage;
                        }
                    }

                    // Sprawdź koniec walki
                    if (enemy.hp <= 0) {
                        player.wins++;
                        player.gold += enemy.gold;
                        if (enemy.firstTimeReward) {
                            player.gold += enemy.gold * 2;
                            enemy.firstTimeReward = false;
                        }
                        gainExp(player, enemy.exp);
                        state = ARENA;
                    }
                    else if (player.currentHp <= 0) {
                        player.losses++;
                        player.gold /= 2;
                        player.currentHp = player.maxHp / 2;
                        state = ARENA;
                    }

                    combatChoice = -1;
                }
            }
            // SKLEP
            else if (state == SHOP) {
                Button buyWeaponBtn = createButton(100 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "KUP BRON", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button buyArmorBtn = createButton(650 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "KUP ZBROJE", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button buyShieldBtn = createButton(1200 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "KUP TARCZE", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 800 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));

                if (isMouseOverButton(buyWeaponBtn, mouseX, mouseY)) {
                    int nextLevel = player.weapon.level + 1;
                    if (nextLevel < (int)weapons.size() && player.gold >= weapons[nextLevel].price) {
                        player.gold -= weapons[nextLevel].price;
                        player.weapon = weapons[nextLevel];
                    }
                }
                else if (isMouseOverButton(buyArmorBtn, mouseX, mouseY)) {
                    int nextLevel = player.armor.level + 1;
                    if (nextLevel < (int)armors.size() && player.gold >= armors[nextLevel].price) {
                        player.gold -= armors[nextLevel].price;
                        player.armor = armors[nextLevel];
                        player.maxHp = calculateMaxHp(player);
                        player.currentHp = player.maxHp;
                    }
                }
                else if (isMouseOverButton(buyShieldBtn, mouseX, mouseY)) {
                    int nextLevel = player.shield.level + 1;
                    if (nextLevel < (int)shields.size() && player.gold >= shields[nextLevel].price) {
                        player.gold -= shields[nextLevel].price;
                        player.shield = shields[nextLevel];
                    }
                }
                else if (isMouseOverButton(backBtn, mouseX, mouseY)) {
                    state = MENU;
                }
            }
            // STATYSTYKI
            else if (state == STATYSTYKI) {
                // Przyciski +20 do statystyk
                Button addStrBtn = createButton(current_w / 2 + 200 * scale_x, 420 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button addAgiBtn = createButton(current_w / 2 + 200 * scale_x, 470 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button addVitBtn = createButton(current_w / 2 + 200 * scale_x, 520 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button addMagBtn = createButton(current_w / 2 + 200 * scale_x, 570 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));

                if (isMouseOverButton(addStrBtn, mouseX, mouseY)) {
                    if (player.skillPoints >= 20) {
                        player.stats.strength += 20;
                        player.skillPoints -= 20;
                    }
                    else {
                        showNoPointsMessage = true;
                        noPointsMessageTimer = 0;
                    }
                }
                else if (isMouseOverButton(addAgiBtn, mouseX, mouseY)) {
                    if (player.skillPoints >= 20) {
                        player.stats.agility += 20;
                        player.skillPoints -= 20;
                    }
                    else {
                        showNoPointsMessage = true;
                        noPointsMessageTimer = 0;
                    }
                }
                else if (isMouseOverButton(addVitBtn, mouseX, mouseY)) {
                    if (player.skillPoints >= 20) {
                        player.stats.vitality += 20;
                        player.skillPoints -= 20;
                        player.maxHp = calculateMaxHp(player);
                        player.currentHp = player.maxHp;
                    }
                    else {
                        showNoPointsMessage = true;
                        noPointsMessageTimer = 0;
                    }
                }
                else if (isMouseOverButton(addMagBtn, mouseX, mouseY)) {
                    if (player.skillPoints >= 20) {
                        player.stats.magic += 20;
                        player.skillPoints -= 20;
                    }
                    else {
                        showNoPointsMessage = true;
                        noPointsMessageTimer = 0;
                    }
                }

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 800 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                if (isMouseOverButton(backBtn, mouseX, mouseY)) {
                    state = MENU;
                }
            }
            // LEVEL UP SCREEN
            else if (state == LEVEL_UP_SCREEN) {
                Button continueBtn = createButton(current_w / 2 - 100 * scale_x, 600 * scale_y, 200 * scale_x, 60 * scale_y,
                    "KONTYNUUJ", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                if (isMouseOverButton(continueBtn, mouseX, mouseY)) {
                    state = stateBeforeLevelUp;
                }
            }
            // ZARZĄDZANIE ZAPISAMI
            else if (state == ZARZAD) {
                for (int i = 1; i <= 3; i++) {
                    Button deleteBtn = createButton(current_w / 2 + 220 * scale_x, (230 + i * 90) * scale_y, 120 * scale_x, 50 * scale_y,
                        "USUN", al_map_rgb(200, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 70, 70));
                    if (isMouseOverButton(deleteBtn, mouseX, mouseY)) {
                        if (saveGameExists(i)) {
                            deleteSave(i);
                            showDeleteMessage = true;
                            deleteMessageTimer = 0;
                            deletedSlotNumber = i;
                        }
                    }
                }

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                if (isMouseOverButton(backBtn, mouseX, mouseY)) {
                    state = MENU;
                }
            }
            else if (state == MINIGAME_RESULTS) {
                Button continueBtn = createButton(current_w / 2 - 150 * scale_x, 700 * scale_y, 300 * scale_x, 60 * scale_y,
                    "KONTYNUUJ", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                if (isMouseOverButton(continueBtn, mouseX, mouseY)) {
                    state = ARENA_TRENINGOWA;
                }
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            int current_w = fullscreen ? screen_w : windowed_w;
            int current_h = fullscreen ? screen_h : windowed_h;

            al_clear_to_color(al_map_rgb(20, 30, 50));

            // TITLE SCREEN
            if (state == TITLE_SCREEN) {
                al_draw_text(titleFont, al_map_rgb(255, 215, 0), current_w / 2, 200 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "MISTRZOWIE OSTRZA");

                Button newGameBtn = createButton(current_w / 2 - 150 * scale_x, 400 * scale_y, 300 * scale_x, 60 * scale_y,
                    "NOWA GRA", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button loadGameBtn = createButton(current_w / 2 - 150 * scale_x, 500 * scale_y, 300 * scale_x, 60 * scale_y,
                    "WCZYTAJ ZAPIS", al_map_rgb(50, 100, 200), al_map_rgb(255, 255, 255), al_map_rgb(70, 120, 220));

                newGameBtn.isHovered = isMouseOverButton(newGameBtn, mouseX, mouseY);
                loadGameBtn.isHovered = isMouseOverButton(loadGameBtn, mouseX, mouseY);

                drawButton(newGameBtn, font);
                drawButton(loadGameBtn, font);

                al_draw_text(smallFont, al_map_rgb(150, 150, 150), current_w / 2, current_h - 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Nacisnij F11 aby przelaczyc tryb pelnoekranowy");
            }
            // NOWA GRA - wybór miejsca i wpisanie nazwy
            else if (state == NOWA_GRA) {
                al_draw_text(titleFont, al_map_rgb(255, 215, 0), current_w / 2, 100 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "NOWA GRA");

                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 200 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Wybierz miejsce zapisu:");

                // Przyciski wyboru miejsca
                for (int i = 1; i <= 3; i++) {
                    Button slotBtn = createButton(current_w / 2 - 150 * scale_x, (250 + i * 70) * scale_y, 300 * scale_x, 50 * scale_y,
                        "", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));

                    if (selectedSaveSlot == i) {
                        slotBtn.color = al_map_rgb(50, 150, 50);
                        slotBtn.hoverColor = al_map_rgb(70, 180, 70);
                    }

                    slotBtn.isHovered = isMouseOverButton(slotBtn, mouseX, mouseY);
                    drawButton(slotBtn, font);

                    char slotText[128];
                    if (saveGameExists(i)) {
                        ifstream file(getSaveFileName(i));
                        string name;
                        int level;
                        getline(file, name);
                        file >> level;
                        file.close();
                        sprintf(slotText, "Miejsce %d - %s (Lvl %d)", i, name.c_str(), level);
                    }
                    else {
                        sprintf(slotText, "Miejsce %d - [PUSTE]", i);
                    }

                    al_draw_text(smallFont, al_map_rgb(255, 255, 255),
                        current_w / 2, (250 + i * 70 + 15) * scale_y, ALLEGRO_ALIGN_CENTRE, slotText);
                }

                // Pole tekstowe
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 520 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Wpisz imie postaci:");
                drawTextInput(current_w / 2 - 200 * scale_x, 550 * scale_y, 400 * scale_x, 50 * scale_y,
                    playerNameInput, inputActive, font);

                // Przycisk powrotu
                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 650 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                backBtn.isHovered = isMouseOverButton(backBtn, mouseX, mouseY);
                drawButton(backBtn, font);

                if (selectedSaveSlot > 0 && !playerNameInput.empty()) {
                    al_draw_text(smallFont, al_map_rgb(100, 255, 100), current_w / 2, 720 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, "Nacisnij ENTER aby rozpoczac gre");
                }


            }
            // TRAINING - stan wyboru zapisu do wczytania
            else if (state == TRAINING) {
                al_draw_text(titleFont, al_map_rgb(100, 150, 255), current_w / 2, 100 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "WCZYTAJ ZAPIS");

                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 200 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Wybierz zapis do wczytania:");

                for (int i = 1; i <= 3; i++) {
                    Button slotBtn = createButton(current_w / 2 - 200 * scale_x, (250 + i * 80) * scale_y, 400 * scale_x, 60 * scale_y,
                        "", al_map_rgb(100, 100, 150), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 180));
                    slotBtn.isHovered = isMouseOverButton(slotBtn, mouseX, mouseY);
                    drawButton(slotBtn, font);

                    char slotText[128];
                    if (saveGameExists(i)) {
                        ifstream file(getSaveFileName(i));
                        string name;
                        int level, gold;
                        getline(file, name);
                        file >> level;
                        file >> gold; // pomijamy exp
                        file >> gold; // pomijamy expToNext
                        file >> gold; // teraz gold
                        file.close();
                        sprintf(slotText, "Miejsce %d - %s", i, name.c_str());
                        al_draw_text(font, al_map_rgb(255, 255, 255),
                            current_w / 2, (255 + i * 80) * scale_y, ALLEGRO_ALIGN_CENTRE, slotText);
                        sprintf(slotText, "Poziom %d | Zloto: %d", level, gold);
                        al_draw_text(smallFont, al_map_rgb(200, 200, 200),
                            current_w / 2, (280 + i * 80) * scale_y, ALLEGRO_ALIGN_CENTRE, slotText);
                    }
                    else {
                        sprintf(slotText, "Miejsce %d - [PUSTE]", i);
                        al_draw_text(font, al_map_rgb(150, 150, 150),
                            current_w / 2, (265 + i * 80) * scale_y, ALLEGRO_ALIGN_CENTRE, slotText);
                    }
                }

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 650 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                backBtn.isHovered = isMouseOverButton(backBtn, mouseX, mouseY);
                drawButton(backBtn, font);
            }
            // MENU
            else if (state == MENU) {
                al_draw_text(titleFont, al_map_rgb(255, 215, 0), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "MENU GLOWNE");

                // Statystyki gracza - zawsze widoczne
                char statsText[512];
                sprintf(statsText, "%s | Poziom: %d | Zloto: %d",
                    player.name.c_str(), player.level, player.gold);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 150 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, statsText);

                sprintf(statsText, "HP: %d/%d | Energia: %d/%d | EXP: %d/%d",
                    player.currentHp, player.maxHp, player.energy, player.maxEnergy, player.exp, player.expToNext);
                al_draw_text(smallFont, al_map_rgb(200, 200, 200), current_w / 2, 180 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, statsText);

                sprintf(statsText, "Zwyciestwa: %d | Porazki: %d | Punkty umiejetnosci: %d",
                    player.wins, player.losses, player.skillPoints);
                al_draw_text(smallFont, al_map_rgb(200, 200, 200), current_w / 2, 210 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, statsText);

                // Ekwipunek
                sprintf(statsText, "Bron: %s (+%d) | Zbroja: %s (+%d) | Tarcza: %s (+%d)",
                    player.weapon.name.c_str(), player.weapon.value,
                    player.armor.name.c_str(), player.armor.defenseValue,
                    player.shield.name.c_str(), player.shield.defenseValue);
                al_draw_text(smallFont, al_map_rgb(180, 180, 255), current_w / 2, 250 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, statsText);

                Button arenaBtn = createButton(current_w / 2 - 150 * scale_x, 340 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ARENA", al_map_rgb(200, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 70, 70));
                Button shopBtn = createButton(current_w / 2 - 150 * scale_x, 430 * scale_y, 300 * scale_x, 60 * scale_y,
                    "SKLEP", al_map_rgb(50, 50, 200), al_map_rgb(255, 255, 255), al_map_rgb(70, 70, 220));
                Button statsBtn = createButton(current_w / 2 - 150 * scale_x, 520 * scale_y, 300 * scale_x, 60 * scale_y,
                    "STATYSTYKI", al_map_rgb(150, 50, 150), al_map_rgb(255, 255, 255), al_map_rgb(170, 70, 170));
                Button trainingBtn = createButton(current_w / 2 - 150 * scale_x, 610 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ARENA TRENINGOWA", al_map_rgb(100, 200, 100), al_map_rgb(255, 255, 255), al_map_rgb(120, 220, 120));
                Button saveBtn = createButton(current_w / 2 - 150 * scale_x, 730 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ZAPISZ GRE", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button manageBtn = createButton(current_w / 2 - 150 * scale_x, 810 * scale_y, 300 * scale_x, 60 * scale_y,
                    "ZARZADZAJ ZAPISAMI", al_map_rgb(150, 100, 50), al_map_rgb(255, 255, 255), al_map_rgb(180, 120, 70));
                Button exitBtn = createButton(current_w / 2 - 300 * scale_x, 810 * scale_y, 130 * scale_x, 60 * scale_y,
                    "wyjscie", al_map_rgb(150, 0, 50), al_map_rgb(255, 255, 255), al_map_rgb(180, 20, 70));

                arenaBtn.isHovered = isMouseOverButton(arenaBtn, mouseX, mouseY);
                shopBtn.isHovered = isMouseOverButton(shopBtn, mouseX, mouseY);
                statsBtn.isHovered = isMouseOverButton(statsBtn, mouseX, mouseY);
                trainingBtn.isHovered = isMouseOverButton(trainingBtn, mouseX, mouseY);
                saveBtn.isHovered = isMouseOverButton(saveBtn, mouseX, mouseY);
                manageBtn.isHovered = isMouseOverButton(manageBtn, mouseX, mouseY);
                exitBtn.isHovered = isMouseOverButton(exitBtn, mouseX, mouseY);

                drawButton(arenaBtn, font);
                drawButton(shopBtn, font);
                drawButton(statsBtn, font);
                drawButton(trainingBtn, smallFont);
                drawButton(saveBtn, font);
                drawButton(manageBtn, smallFont);
                drawButton(exitBtn, smallFont);
                if (showSaveMessage) {
                    char saveText[64];
                    sprintf(saveText, "ZAPISANO W SLOCIE: %d !!", player.saveSlot);
                    al_draw_text(titleFont, al_map_rgb(255, 200, 200), current_w / 2, current_h / 2,
                        ALLEGRO_ALIGN_CENTRE, saveText);
                }
                al_draw_text(smallFont, al_map_rgb(150, 150, 150), current_w / 2, current_h - 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "F11 - Tryb pelnoekranowy");
            }
            // ARENA
            else if (state == ARENA) {
                al_draw_text(titleFont, al_map_rgb(255, 100, 100), current_w / 2, 30 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "ARENA WALKI");

                char statsText[256];
                sprintf(statsText, "Poziom: %d | Zloto: %d | Zwyciestwa: %d | Porazki: %d",
                    player.level, player.gold, player.wins, player.losses);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 100 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, statsText);

                float btnW = 220 * scale_x;
                float btnH = 50 * scale_y;
                float startX = 50 * scale_x;
                float startY = 150 * scale_y;
                float gapX = 20 * scale_x;
                float gapY = 15 * scale_y;

                for (int i = 0; i < 30; i++) {
                    int row = i / 6;
                    int col = i % 6;
                    float x = startX + col * (btnW + gapX);
                    float y = startY + row * (btnH + gapY);

                    // ⭐ SPRAWDŹ CZY PRZECIWNIK JEST ODBLOKOWANY
                    bool isUnlocked = (player.level >= enemies[i].level);

                    // ⭐ KOLORY: zielony dla odblokowanych, szary dla zablokowanych
                    ALLEGRO_COLOR btnColor = isUnlocked ? al_map_rgb(150, 50, 50) : al_map_rgb(60, 60, 60);
                    ALLEGRO_COLOR hoverColor = isUnlocked ? al_map_rgb(200, 70, 70) : al_map_rgb(80, 80, 80);
                    ALLEGRO_COLOR textColor = isUnlocked ? al_map_rgb(255, 255, 255) : al_map_rgb(120, 120, 120);

                    Button enemyBtn = createButton(x, y, btnW, btnH, "",
                        btnColor, al_map_rgb(255, 255, 255), hoverColor);
                    enemyBtn.isHovered = isMouseOverButton(enemyBtn, mouseX, mouseY) && isUnlocked;
                    drawButton(enemyBtn, smallFont);

                    // ⭐ NAZWA PRZECIWNIKA
                    char text[128];
                    sprintf(text, "%d. %s", i + 1, enemies[i].name.c_str());
                    al_draw_text(smallFont, textColor, x + btnW / 2, y + 5 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, text);

                    // ⭐ INFORMACJE: HP dla odblokowanych, "ZABLOKOWANY" dla zablokowanych
                    if (isUnlocked) {
                        // Oblicz skalowane statystyki do wyświetlenia
                        int levelDiff = player.level - enemies[i].level;
                        int hpBonus = 3 * levelDiff * levelDiff;
                        int scaledHP = enemies[i].maxHp + hpBonus;

                        sprintf(text, "Lvl %d | HP: %d", enemies[i].level, scaledHP);
                        al_draw_text(smallFont, al_map_rgb(200, 200, 200),
                            x + btnW / 2, y + 28 * scale_y, ALLEGRO_ALIGN_CENTRE, text);
                    }
                    else {
                        sprintf(text, "🔒 Wymagany Lvl %d", enemies[i].level);
                        al_draw_text(smallFont, al_map_rgb(255, 100, 100),
                            x + btnW / 2, y + 28 * scale_y, ALLEGRO_ALIGN_CENTRE, text);
                    }
                }

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 850 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                backBtn.isHovered = isMouseOverButton(backBtn, mouseX, mouseY);
                drawButton(backBtn, font);
                }
            // WALKA
            else if (state == FIGHT && selectedEnemy >= 0) {
                Enemy& enemy = enemies[selectedEnemy];

                al_draw_text(titleFont, al_map_rgb(255, 100, 100), current_w / 2, 30 * scale_y,

                    ALLEGRO_ALIGN_CENTRE, "WALKA!");
                int levelDiff = player.level - enemy.level;
                if (levelDiff > 0) {
                    char bonusText[64];
                    sprintf(bonusText, "Bonus trudności: +%d HP, +%d DMG",
                        3 * levelDiff * levelDiff,
                        (3 * levelDiff * levelDiff) / 10);
                    al_draw_text(smallFont, al_map_rgb(255, 200, 100), current_w / 2, 80 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, bonusText);
                }


                // Rysuj postać (zielony kwadrat)
                drawPlayer(200 * scale_x, 300 * scale_y, 150 * scale_x);
                al_draw_text(font, al_map_rgb(255, 255, 255), 275 * scale_x, 470 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, player.name.c_str());
                drawHealthBar(150 * scale_x, 500 * scale_y, 250 * scale_x, 30 * scale_y, player.currentHp, player.maxHp, al_map_rgb(50, 200, 50));

                // Rysuj przeciwnika (czerwony kwadrat)
                drawEnemy(1150 * scale_x, 300 * scale_y, 150 * scale_x);
                al_draw_text(font, al_map_rgb(255, 255, 255), 1225 * scale_x, 470 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, enemy.name.c_str());
                drawHealthBar(1100 * scale_x, 500 * scale_y, 250 * scale_x, 30 * scale_y, enemy.hp, enemy.maxHp, al_map_rgb(200, 50, 50));

                // Combo
                char comboText[64];
                sprintf(comboText, "COMBO: %dx", combo);
                al_draw_text(font, al_map_rgb(255, 255, 0), current_w / 2, 250 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, comboText);

                // Przyciski ataku
                Button attackBtn = createButton(100 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "ATAK (100%)", al_map_rgb(150, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(180, 70, 70));
                Button heavyBtn = createButton(350 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "CIEZKI (180%)", al_map_rgb(200, 30, 30), al_map_rgb(255, 255, 255), al_map_rgb(220, 50, 50));
                Button quickBtn = createButton(600 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "SZYBKI (60%)", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button magicBtn = createButton(850 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "MAGIA (120%)", al_map_rgb(100, 50, 200), al_map_rgb(255, 255, 255), al_map_rgb(120, 70, 220));
                Button defenseBtn = createButton(1100 * scale_x, 700 * scale_y, 200 * scale_x, 60 * scale_y,
                    "OBRONA", al_map_rgb(50, 100, 150), al_map_rgb(255, 255, 255), al_map_rgb(70, 120, 180));

                attackBtn.isHovered = isMouseOverButton(attackBtn, mouseX, mouseY);
                heavyBtn.isHovered = isMouseOverButton(heavyBtn, mouseX, mouseY);
                quickBtn.isHovered = isMouseOverButton(quickBtn, mouseX, mouseY);
                magicBtn.isHovered = isMouseOverButton(magicBtn, mouseX, mouseY);
                defenseBtn.isHovered = isMouseOverButton(defenseBtn, mouseX, mouseY);

                drawButton(attackBtn, smallFont);
                drawButton(heavyBtn, smallFont);
                drawButton(quickBtn, smallFont);
                drawButton(magicBtn, smallFont);
                drawButton(defenseBtn, smallFont);
            }
            // SKLEP
            else if (state == SHOP) {
                al_draw_text(titleFont, al_map_rgb(100, 150, 255), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "SKLEP");

                char goldText[64];
                sprintf(goldText, "Zloto: %d", player.gold);
                al_draw_text(font, al_map_rgb(255, 215, 0), current_w / 2, 150 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, goldText);

                // Wyświetl ekwipunek
                char equipText[256];
                sprintf(equipText, "Bron: %s (+%d DMG)", player.weapon.name.c_str(), player.weapon.value);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 250 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, equipText);
                sprintf(equipText, "Zbroja: %s (+%d DEF)", player.armor.name.c_str(), player.armor.defenseValue);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 300 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, equipText);
                sprintf(equipText, "Tarcza: %s (+%d DEF)", player.shield.name.c_str(), player.shield.defenseValue);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 350 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, equipText);

                // Dostępne ulepszenia
                al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2, 450 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Dostepne ulepszenia:");

                int nextWeaponLvl = player.weapon.level + 1;
                if (nextWeaponLvl < (int)weapons.size()) {
                    sprintf(equipText, "Bron: %s (+%d DMG) - %d zlota",
                        weapons[nextWeaponLvl].name.c_str(), weapons[nextWeaponLvl].value, weapons[nextWeaponLvl].price);
                    al_draw_text(smallFont, al_map_rgb(200, 200, 200), 50 * scale_x, 520 * scale_y, 0, equipText);
                }
                else {
                    al_draw_text(smallFont, al_map_rgb(150, 150, 150), 50 * scale_x, 520 * scale_y, 0, "Bron: MAX");
                }

                int nextArmorLvl = player.armor.level + 1;
                if (nextArmorLvl < (int)armors.size()) {
                    sprintf(equipText, "Zbroja: %s (+%d DEF) - %d zlota",
                        armors[nextArmorLvl].name.c_str(), armors[nextArmorLvl].defenseValue, armors[nextArmorLvl].price);
                    al_draw_text(smallFont, al_map_rgb(200, 200, 200), 600 * scale_x, 520 * scale_y, 0, equipText);
                }
                else {
                    al_draw_text(smallFont, al_map_rgb(150, 150, 150), 600 * scale_x, 520 * scale_y, 0, "Zbroja: MAX");
                }

                int nextShieldLvl = player.shield.level + 1;
                if (nextShieldLvl < (int)shields.size()) {
                    sprintf(equipText, "Tarcza: %s (+%d DEF) - %d zlota",
                        shields[nextShieldLvl].name.c_str(), shields[nextShieldLvl].defenseValue, shields[nextShieldLvl].price);
                    al_draw_text(smallFont, al_map_rgb(200, 200, 200), 1150 * scale_x, 520 * scale_y, 0, equipText);
                }
                else {
                    al_draw_text(smallFont, al_map_rgb(150, 150, 150), 1150 * scale_x, 520 * scale_y, 0, "Tarcza: MAX");
                }

                Button buyWeaponBtn = createButton(100 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "KUP BRON", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button buyArmorBtn = createButton(650 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "KUP ZBROJE", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button buyShieldBtn = createButton(1200 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "KUP TARCZE", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 800 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));

                buyWeaponBtn.isHovered = isMouseOverButton(buyWeaponBtn, mouseX, mouseY);
                buyArmorBtn.isHovered = isMouseOverButton(buyArmorBtn, mouseX, mouseY);
                buyShieldBtn.isHovered = isMouseOverButton(buyShieldBtn, mouseX, mouseY);
                backBtn.isHovered = isMouseOverButton(backBtn, mouseX, mouseY);

                drawButton(buyWeaponBtn, smallFont);
                drawButton(buyArmorBtn, smallFont);
                drawButton(buyShieldBtn, smallFont);
                drawButton(backBtn, font);
            }
            // STATYSTYKI
            else if (state == STATYSTYKI) {
                al_draw_text(titleFont, al_map_rgb(200, 100, 200), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "STATYSTYKI");

                char statText[256];
                sprintf(statText, "Imie: %s", player.name.c_str());
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 150 * scale_y, ALLEGRO_ALIGN_CENTRE, statText);
                sprintf(statText, "Poziom: %d | EXP: %d/%d", player.level, player.exp, player.expToNext);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 200 * scale_y, ALLEGRO_ALIGN_CENTRE, statText);
                sprintf(statText, "HP: %d/%d | Energia: %d/%d", player.currentHp, player.maxHp, player.energy, player.maxEnergy);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 250 * scale_y, ALLEGRO_ALIGN_CENTRE, statText);
                sprintf(statText, "Zloto: %d | Punkty umiejetnosci: %d", player.gold, player.skillPoints);
                al_draw_text(font, al_map_rgb(255, 215, 0), current_w / 2, 300 * scale_y, ALLEGRO_ALIGN_CENTRE, statText);

                al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2, 370 * scale_y, ALLEGRO_ALIGN_CENTRE, "Ulepszanie statystyk (20 punktow = +20 do statystyki):");

                // Statystyki z przyciskami +20
                sprintf(statText, "Sila: %d", player.stats.strength);
                al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2 - 100 * scale_x, 420 * scale_y, 0, statText);
                Button addStrBtn = createButton(current_w / 2 + 200 * scale_x, 420 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                addStrBtn.isHovered = isMouseOverButton(addStrBtn, mouseX, mouseY);
                drawButton(addStrBtn, smallFont);

                sprintf(statText, "Zrecznosc: %d", player.stats.agility);
                al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2 - 100 * scale_x, 470 * scale_y, 0, statText);
                Button addAgiBtn = createButton(current_w / 2 + 200 * scale_x, 470 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                addAgiBtn.isHovered = isMouseOverButton(addAgiBtn, mouseX, mouseY);
                drawButton(addAgiBtn, smallFont);

                sprintf(statText, "Witalnosc: %d", player.stats.vitality);
                al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2 - 100 * scale_x, 520 * scale_y, 0, statText);
                Button addVitBtn = createButton(current_w / 2 + 200 * scale_x, 520 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                addVitBtn.isHovered = isMouseOverButton(addVitBtn, mouseX, mouseY);
                drawButton(addVitBtn, smallFont);

                sprintf(statText, "Magia: %d", player.stats.magic);
                al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2 - 100 * scale_x, 570 * scale_y, 0, statText);
                Button addMagBtn = createButton(current_w / 2 + 200 * scale_x, 570 * scale_y, 80 * scale_x, 40 * scale_y,
                    "+20", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                addMagBtn.isHovered = isMouseOverButton(addMagBtn, mouseX, mouseY);
                drawButton(addMagBtn, smallFont);

                sprintf(statText, "Zwyciestwa: %d | Porazki: %d", player.wins, player.losses);
                al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2, 650 * scale_y, ALLEGRO_ALIGN_CENTRE, statText);

                sprintf(statText, "Obrazenia: x%.2f | Obrona: x%.2f", player.stats.strengthMult, player.stats.defenseMult);
                al_draw_text(font, al_map_rgb(150, 150, 255), current_w / 2, 700 * scale_y, ALLEGRO_ALIGN_CENTRE, statText);
                sprintf(statText, "Zdrowie: x%.2f | Magia: x%.2f", player.stats.healthMult, player.stats.magicMult);
                al_draw_text(font, al_map_rgb(150, 150, 255), current_w / 2, 750 * scale_y, ALLEGRO_ALIGN_CENTRE, statText);

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 800 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                backBtn.isHovered = isMouseOverButton(backBtn, mouseX, mouseY);
                drawButton(backBtn, font);

                // Komunikat o braku punktów
                if (showNoPointsMessage) {
                    al_draw_text(titleFont, al_map_rgb(255, 100, 100), current_w / 2, current_h / 2,
                        ALLEGRO_ALIGN_CENTRE, "BRAK WYSTARCZAJACEJ ILOSCI PUNKTOW!");
                }
            }
            // LEVEL UP SCREEN
            else if (state == LEVEL_UP_SCREEN) {
                al_clear_to_color(al_map_rgb(30, 40, 60));

                al_draw_text(titleFont, al_map_rgb(255, 215, 0), current_w / 2, 200 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "*** AWANS POZIOMU! ***");

                char levelText[128];
                sprintf(levelText, "Nowy poziom: %d", player.level);
                al_draw_text(titleFont, al_map_rgb(255, 255, 255), current_w / 2, 300 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, levelText);

                al_draw_text(font, al_map_rgb(200, 200, 255), current_w / 2, 400 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Otrzymujesz 2 punkty umiejetnosci!");

                Button continueBtn = createButton(current_w / 2 - 100 * scale_x, 600 * scale_y, 200 * scale_x, 60 * scale_y,
                    "KONTYNUUJ", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                continueBtn.isHovered = isMouseOverButton(continueBtn, mouseX, mouseY);
                drawButton(continueBtn, font);
            }
            // ZARZĄDZANIE ZAPISAMI
            else if (state == ZARZAD) {
                al_draw_text(titleFont, al_map_rgb(200, 150, 50), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "ZARZADZANIE ZAPISAMI");

                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 150 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Dostepne zapisy:");

                for (int i = 1; i <= 3; i++) {
                    char slotText[256];
                    if (saveGameExists(i)) {
                        ifstream file(getSaveFileName(i));
                        string name;
                        int level, gold;
                        getline(file, name);
                        file >> level;
                        file >> gold; // pomijamy exp
                        file >> gold; // pomijamy expToNext
                        file >> gold; // teraz gold
                        file.close();

                        sprintf(slotText, "Miejsce %d: %s", i, name.c_str());
                        al_draw_text(font, al_map_rgb(255, 255, 255),
                            current_w / 2 - 200 * scale_x, (230 + i * 90) * scale_y, 0, slotText);
                        sprintf(slotText, "Poziom %d | Zloto: %d", level, gold);
                        al_draw_text(smallFont, al_map_rgb(200, 200, 200),
                            current_w / 2 - 200 * scale_x, (260 + i * 90) * scale_y, 0, slotText);

                        Button deleteBtn = createButton(current_w / 2 + 220 * scale_x, (230 + i * 90) * scale_y, 120 * scale_x, 50 * scale_y,
                            "USUN", al_map_rgb(200, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 70, 70));
                        deleteBtn.isHovered = isMouseOverButton(deleteBtn, mouseX, mouseY);
                        drawButton(deleteBtn, smallFont);
                    }
                    else {
                        sprintf(slotText, "Miejsce %d: [PUSTE]", i);
                        al_draw_text(font, al_map_rgb(150, 150, 150),
                            current_w / 2 - 200 * scale_x, (240 + i * 90) * scale_y, 0, slotText);
                    }
                }

                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));
                backBtn.isHovered = isMouseOverButton(backBtn, mouseX, mouseY);
                drawButton(backBtn, font);
                if (showDeleteMessage) {
                    char deleteText[64];
                    sprintf(deleteText, "USUNIETO ZAPIS Z GNIAZDA: %d !!", deletedSlotNumber);
                    al_draw_text(titleFont, al_map_rgb(255, 100, 100), current_w / 2, current_h / 2,
                        ALLEGRO_ALIGN_CENTRE, deleteText);
                }
            }

            // ARENA TRENINGOWA - menu wyboru
            else if (state == ARENA_TRENINGOWA) {
                al_draw_text(titleFont, al_map_rgb(100, 200, 100), current_w / 2, 100 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "ARENA TRENINGOWA");

                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 200 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Wybierz trening:");

                Button strengthBtn = createButton(current_w / 2 - 150 * scale_x, 300 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING SILY", al_map_rgb(200, 50, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 70, 70));
                Button magicBtn = createButton(current_w / 2 - 150 * scale_x, 390 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING MAGII", al_map_rgb(100, 50, 200), al_map_rgb(255, 255, 255), al_map_rgb(120, 70, 220));
                Button agilityBtn = createButton(current_w / 2 - 150 * scale_x, 480 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING ZRECZNOSCI", al_map_rgb(50, 200, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 220, 70));
                Button vitalityBtn = createButton(current_w / 2 - 150 * scale_x, 570 * scale_y, 300 * scale_x, 60 * scale_y,
                    "TRENING WITALNOSCI", al_map_rgb(200, 100, 50), al_map_rgb(255, 255, 255), al_map_rgb(220, 120, 70));
                Button backBtn = createButton(current_w / 2 - 100 * scale_x, 700 * scale_y, 200 * scale_x, 50 * scale_y,
                    "POWROT", al_map_rgb(100, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(130, 130, 130));

                strengthBtn.isHovered = isMouseOverButton(strengthBtn, mouseX, mouseY);
                magicBtn.isHovered = isMouseOverButton(magicBtn, mouseX, mouseY);
                agilityBtn.isHovered = isMouseOverButton(agilityBtn, mouseX, mouseY);
                vitalityBtn.isHovered = isMouseOverButton(vitalityBtn, mouseX, mouseY);
                backBtn.isHovered = isMouseOverButton(backBtn, mouseX, mouseY);

                drawButton(strengthBtn, font);
                drawButton(magicBtn, font);
                drawButton(agilityBtn, smallFont);
                drawButton(vitalityBtn, smallFont);
                drawButton(backBtn, font);
            }
            // MINIGRA SIŁA
            else if (state == MINIGRA_SILA) {
                al_draw_text(titleFont, al_map_rgb(255, 100, 100), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "TRENING SILY");

                char scoreText[128];
                sprintf(scoreText, "Punkty: %d | Kliknij %d/%d przyciskow",
                    miniGameScore, strengthButtonsClicked, strengthButtonsTotal);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 120 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, scoreText);

                // Timer
                float remaining = miniGameTimeLimit - miniGameTimer;
                sprintf(scoreText, "Czas: %.2f s", remaining);
                al_draw_text(font, remaining < 2.0f ? al_map_rgb(255, 100, 100) : al_map_rgb(255, 255, 255),
                    current_w / 2, 160 * scale_y, ALLEGRO_ALIGN_CENTRE, scoreText);

                // Przycisk do kliknięcia
                if (strengthButtonActive) {
                    Button targetBtn = createButton(strengthButtonX, strengthButtonY, 100 * scale_x, 100 * scale_y,
                        "KLIK!", al_map_rgb(255, 100, 100), al_map_rgb(255, 255, 255), al_map_rgb(255, 150, 150));
                    targetBtn.isHovered = isMouseOverButton(targetBtn, mouseX, mouseY);
                    drawButton(targetBtn, font);
                }
            }
            // MINIGRA MAGIA
            else if (state == MINIGRA_MAGIA) {
                al_draw_text(titleFont, al_map_rgb(150, 100, 255), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "TRENING MAGII");

                char scoreText[128];
                sprintf(scoreText, "Punkty: %d", miniGameScore);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 120 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, scoreText);

                // Timer
                float remaining = miniGameTimeLimit - miniGameTimer;
                sprintf(scoreText, "Czas: %.2f s", remaining);
                al_draw_text(font, remaining < 2.0f ? al_map_rgb(255, 100, 100) : al_map_rgb(255, 255, 255),
                    current_w / 2, 160 * scale_y, ALLEGRO_ALIGN_CENTRE, scoreText);

                // Zadanie
                char problemText[128];
                char opChar = '+';
                if (mathOperation == 1) opChar = '-';
                else if (mathOperation == 2) opChar = '*';

                sprintf(problemText, "%d %c %d = ?", mathA, opChar, mathB);
                al_draw_text(titleFont, al_map_rgb(255, 255, 0), current_w / 2, 300 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, problemText);

                // Pole input
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 420 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Wpisz odpowiedz i nacisnij ENTER:");
                drawTextInput(current_w / 2 - 100 * scale_x, 450 * scale_y, 200 * scale_x, 50 * scale_y,
                    mathInput, mathInputActive, font);
            }
            // MINIGRA AGILITY
            else if (state == MINIGRA_AGILITY) {
                al_draw_text(titleFont, al_map_rgb(100, 255, 100), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "TRENING ZRECZNOSCI");

                char scoreText[128];
                sprintf(scoreText, "Punkty: %d | Etap: %d", miniGameScore, miniGameStage);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 120 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, scoreText);

                // Timer przetrwania
                float timeToNextBonus = 5.0f - survivalTimer;
                sprintf(scoreText, "Nastepne +5 punktow za: %.1f s", timeToNextBonus);
                al_draw_text(smallFont, al_map_rgb(255, 255, 100), current_w / 2, 160 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, scoreText);

                al_draw_text(smallFont, al_map_rgb(200, 200, 200), current_w / 2, 190 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "Strzalki: <- -> | Unikaj czerwonych kulek! | Zlap zlote!");

                // Rysuj gracza (zielony kwadrat)
                drawPlayer(playerX, current_h - 120 * scale_y, 60 * scale_x);

                // Rysuj kulki
                for (const auto& ball : balls) {
                    if (ball.active) {
                        ALLEGRO_COLOR ballColor = ball.isGolden ? al_map_rgb(255, 215, 0) : al_map_rgb(200, 50, 50);
                        al_draw_filled_circle(ball.x, ball.y, 15 * scale_x, ballColor);
                        al_draw_circle(ball.x, ball.y, 15 * scale_x, al_map_rgb(255, 255, 255), 2);
                    }
                }
            }
            // MINIGRA WITALNOŚĆ
            else if (state == MINIGRA_WITALNOSC) {
                al_draw_text(titleFont, al_map_rgb(255, 150, 50), current_w / 2, 50 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "TRENING WITALNOSCI");

                char scoreText[128];
                sprintf(scoreText, "Punkty: %d | Etap: %d", miniGameScore, miniGameStage);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 120 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, scoreText);

                // Timer
                float remaining = miniGameTimeLimit - miniGameTimer;
                sprintf(scoreText, "Czas: %.2f s", remaining);
                al_draw_text(font, remaining < 2.0f ? al_map_rgb(255, 100, 100) : al_map_rgb(255, 255, 255),
                    current_w / 2, 160 * scale_y, ALLEGRO_ALIGN_CENTRE, scoreText);

                // Litera
                if (letterShown) {
                    char letterStr[2] = { targetLetter, '\0' };
                    al_draw_text(titleFont, al_map_rgb(255, 255, 0), current_w / 2, 350 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, letterStr);

                    al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2, 500 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, "Nacisnij odpowiednia litere!");
                }
            }
            else if (state == MINIGAME_RESULTS) {
                al_clear_to_color(al_map_rgb(30, 40, 60));

                al_draw_text(titleFont, al_map_rgb(100, 255, 100), current_w / 2, 100 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, "=== WYNIKI TRENINGU ===");

                // Nazwa minigry
                al_draw_text(titleFont, al_map_rgb(255, 215, 0), current_w / 2, 200 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, lastMinigameResult.minigameName.c_str());

                // Ramka z wynikami
                al_draw_filled_rectangle(current_w / 2 - 400 * scale_x, 300 * scale_y,
                    current_w / 2 + 400 * scale_x, 650 * scale_y, al_map_rgb(40, 50, 70));
                al_draw_rectangle(current_w / 2 - 400 * scale_x, 300 * scale_y,
                    current_w / 2 + 400 * scale_x, 650 * scale_y, al_map_rgb(100, 200, 255), 3);

                char resultText[256];

                // Zdobyte punkty
                sprintf(resultText, "Zdobyte punkty: %d", lastMinigameResult.scoreAchieved);
                al_draw_text(font, al_map_rgb(255, 255, 255), current_w / 2, 340 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, resultText);

                // Przyrost statystyki
                sprintf(resultText, "%s: +%d", lastMinigameResult.statName.c_str(), lastMinigameResult.statGained);
                al_draw_text(font, al_map_rgb(100, 255, 100), current_w / 2, 400 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, resultText);

                // Zdobyte EXP
                sprintf(resultText, "Doswiadczenie: +%d EXP", lastMinigameResult.expGained);
                al_draw_text(font, al_map_rgb(255, 200, 100), current_w / 2, 460 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, resultText);

                // Poziom przed i po
                if (lastMinigameResult.levelAfter > lastMinigameResult.levelBefore) {
                    // AWANS POZIOMU!
                    sprintf(resultText, "*** AWANS POZIOMU! %d -> %d ***",
                        lastMinigameResult.levelBefore, lastMinigameResult.levelAfter);
                    al_draw_text(font, al_map_rgb(255, 255, 0), current_w / 2, 520 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, resultText);

                    al_draw_text(smallFont, al_map_rgb(200, 200, 255), current_w / 2, 560 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, "+20 punktow umiejetnosci!");
                }
                else {
                    sprintf(resultText, "Poziom: %d", lastMinigameResult.levelBefore);
                    al_draw_text(font, al_map_rgb(200, 200, 200), current_w / 2, 520 * scale_y,
                        ALLEGRO_ALIGN_CENTRE, resultText);
                }

                // Statystyki gracza
                sprintf(resultText, "HP: %d/%d | Punkty umiejetnosci: %d",
                    player.currentHp, player.maxHp, player.skillPoints);
                al_draw_text(smallFont, al_map_rgb(180, 180, 180), current_w / 2, 600 * scale_y,
                    ALLEGRO_ALIGN_CENTRE, resultText);

                // Przycisk kontynuuj
                Button continueBtn = createButton(current_w / 2 - 150 * scale_x, 700 * scale_y, 300 * scale_x, 60 * scale_y,
                    "KONTYNUUJ", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255), al_map_rgb(70, 180, 70));
                continueBtn.isHovered = isMouseOverButton(continueBtn, mouseX, mouseY);
                drawButton(continueBtn, font);
                }

                al_flip_display();
        }
    }

    al_destroy_font(font);
    al_destroy_font(titleFont);
    al_destroy_font(smallFont);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}
