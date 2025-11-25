1238 2 kod

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

using namespace std;

struct Stats {
    int strength;
    int agility;
    int vitality;
    int magic;
};

struct Equipment {
    string name;
    int value;
    int price;
    int levelReq;
    string type;
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
    Equipment* weapon;
    Equipment* armor;
    Equipment* accessory;
    int skillPoints;
    int wins;
    int losses;
    int saveSlot;
};

struct SaveFile {
    string playerName;
    int level;
    int exp;
    int expToNext;
    int gold;
    int strength, agility, vitality, magic;
    int maxHp;
    int currentHp;
    int energy;
    int maxEnergy;
    string weaponName;
    int weaponValue;
    int weaponPrice;
    int weaponLevelReq;
    string armorName;
    int armorValue;
    int armorPrice;
    int armorLevelReq;
    string accessoryName;
    int accessoryValue;
    int accessoryPrice;
    int accessoryLevelReq;
    int skillPoints;
    int wins;
    int losses;
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
}

void displayEquipment(const Player& player) {
    cout << "\n--------------- EKWIPUNEK ---------------\n";
    cout << "! Bron: " << setw(30) << left
        << (player.weapon ? player.weapon->name + " (+" + to_string(player.weapon->value) + " DMG)" : "Brak") << "!\n";
    cout << "! Zbroja: " << setw(28) << left
        << (player.armor ? player.armor->name + " (+" + to_string(player.armor->value) + " DEF)" : "Brak") << "!\n";
    cout << "! Akcesoria: " << setw(25) << left
        << (player.accessory ? player.accessory->name : "Brak") << "!\n";
    cout << "-------------------------------------------\n";
}

string getSaveFileName(int slot) {
    return "save_" + to_string(slot) + ".txt";
}

bool saveGameExists(int slot) {
    ifstream file(getSaveFileName(slot));
    return file.good();
}

void saveGame(const Player& player, const vector<Equipment>& inventory) {
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
    file << player.maxHp << "\n";
    file << player.currentHp << "\n";
    file << player.energy << "\n";
    file << player.maxEnergy << "\n";

    if (player.weapon) {
        file << player.weapon->name << "\n";
        file << player.weapon->value << "\n";
        file << player.weapon->price << "\n";
        file << player.weapon->levelReq << "\n";
    }
    else {
        file << "BRAK\n0\n0\n0\n";
    }

    if (player.armor) {
        file << player.armor->name << "\n";
        file << player.armor->value << "\n";
        file << player.armor->price << "\n";
        file << player.armor->levelReq << "\n";
    }
    else {
        file << "BRAK\n0\n0\n0\n";
    }

    if (player.accessory) {
        file << player.accessory->name << "\n";
        file << player.accessory->value << "\n";
        file << player.accessory->price << "\n";
        file << player.accessory->levelReq << "\n";
    }
    else {
        file << "BRAK\n0\n0\n0\n";
    }

    file << player.skillPoints << "\n";
    file << player.wins << "\n";
    file << player.losses << "\n";

    file.close();
    cout << "\nGra zapisana w gniazdu " << player.saveSlot << "!\n";
}

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
    file >> save.maxHp >> save.currentHp >> save.energy >> save.maxEnergy;

    file.ignore();
    getline(file, save.weaponName);
    file >> save.weaponValue >> save.weaponPrice >> save.weaponLevelReq;

    file.ignore();
    getline(file, save.armorName);
    file >> save.armorValue >> save.armorPrice >> save.armorLevelReq;

    file.ignore();
    getline(file, save.accessoryName);
    file >> save.accessoryValue >> save.accessoryPrice >> save.accessoryLevelReq;

    file >> save.skillPoints >> save.wins >> save.losses;

    file.close();

    player.name = save.playerName;
    player.level = save.level;
    player.exp = save.exp;
    player.expToNext = save.expToNext;
    player.gold = save.gold;
    player.stats.strength = save.strength;
    player.stats.agility = save.agility;
    player.stats.vitality = save.vitality;
    player.stats.magic = save.magic;
    player.maxHp = save.maxHp;
    player.currentHp = save.currentHp;
    player.energy = save.energy;
    player.maxEnergy = save.maxEnergy;
    player.skillPoints = save.skillPoints;
    player.wins = save.wins;
    player.losses = save.losses;

    if (save.weaponName != "BRAK") {
        Equipment* weapon = new Equipment();
        weapon->name = save.weaponName;
        weapon->value = save.weaponValue;
        weapon->price = save.weaponPrice;
        weapon->levelReq = save.weaponLevelReq;
        weapon->type = "weapon";
        player.weapon = weapon;
    }

    if (save.armorName != "BRAK") {
        Equipment* armor = new Equipment();
        armor->name = save.armorName;
        armor->value = save.armorValue;
        armor->price = save.armorPrice;
        armor->levelReq = save.armorLevelReq;
        armor->type = "armor";
        player.armor = armor;
    }

    if (save.accessoryName != "BRAK") {
        Equipment* accessory = new Equipment();
        accessory->name = save.accessoryName;
        accessory->value = save.accessoryValue;
        accessory->price = save.accessoryPrice;
        accessory->levelReq = save.accessoryLevelReq;
        accessory->type = "accessory";
        player.accessory = accessory;
    }

    return true;
}

void deleteSave(int slot) {
    string filename = getSaveFileName(slot);
    if (remove(filename.c_str()) == 0) {
        cout << "\nZapis z gniazda " << slot << " zostal usuniety!\n";
    }
    else {
        cout << "\nBlad: Nie mozna usunac zapisu!\n";
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

int calculateMaxHp(const Player& player) {
    return 100 + player.stats.vitality * 10;
}

int calculateDamage(const Player& player) {
    int dmg = player.stats.strength * 2;
    if (player.weapon) dmg += player.weapon->value;
    return dmg;
}

int calculateDefense(const Player& player) {
    int def = player.stats.vitality;
    if (player.armor) def += player.armor->value;
    return def;
}

void levelUp(Player& player) {
    player.level++;
    player.skillPoints += 3;
    player.expToNext = static_cast<int>(player.expToNext * 1.5);
    player.maxHp = calculateMaxHp(player);
    player.currentHp = player.maxHp;
    player.energy = player.maxEnergy;

    clearScreen();
    cout << "\n\n";
    cout << "    -----------------------------------------\n";
    cout << "    !                                       !\n";
    cout << "    !        ★ AWANS POZIOMU! ★            !\n";
    cout << "    !                                       !\n";
    cout << "    !     Nowy poziom: " << setw(2) << player.level << "                !\n";
    cout << "    !     Otrzymujesz 3 punkty umiejetnosci !\n";
    cout << "    !                                       !\n";
    cout << "    -----------------------------------------\n\n";
    pause();
}

void gainExp(Player& player, int exp) {
    player.exp += exp;
    while (player.exp >= player.expToNext) {
        player.exp -= player.expToNext;
        levelUp(player);
    }
}

void strengthTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING SILY");

    cout << "Naciskaj ENTER tak szybko jak potrafisz przez 10 sekund!\n";
    cout << "Kazde nacisniecie daje punkty!\n\n";
    pause();

    int score = 0;
    auto startTime = chrono::steady_clock::now();
    auto endTime = startTime + chrono::seconds(10);

    cout << "\nSTART! Naciskaj ENTER!\n\n";

    while (chrono::steady_clock::now() < endTime) {
        if (cin.get() == '\n') {
            score++;
            cout << "Punkty: " << score << "\r" << flush;
        }
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int statGain = max(1, score / 10);
    int goldGain = score / 5;
    int expGain = score * 2;

    player.stats.strength += statGain;
    player.gold += goldGain;
    gainExp(player, expGain);
    player.energy = min(player.energy + 20, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU SILY ===\n\n";
    cout << "Wynik: " << score << " punktow\n";
    cout << "Sila +" << statGain << "\n";
    cout << "Zloto +" << goldGain << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void agilityTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING ZRECZNOSCI");

    cout << "Odgadnij liczby od 1 do 100!\n";
    cout << "Im szybciej, tym wiecej punktow!\n\n";
    pause();

    int score = 0;
    int rounds = 8;

    for (int i = 0; i < rounds; i++) {
        int target = rand() % 100 + 1;
        int guess;
        int attempts = 0;

        cout << "\nRunda " << (i + 1) << "/" << rounds << "\n";
        cout << "Odgadnij liczbe (1-100): ";

        while (true) {
            cin >> guess;
            attempts++;

            if (guess == target) {
                int points = max(1, 10 - attempts);
                score += points;
                cout << "Trafiles! (+" << points << " pkt)\n";
                break;
            }
            else if (guess < target) {
                cout << "Za nisko! Sprobuj ponownie: ";
            }
            else {
                cout << "Za wysoko! Sprobuj ponownie: ";
            }
        }
    }

    int statGain = max(1, score / 8);
    int goldGain = score / 4;
    int expGain = score * 2;

    player.stats.agility += statGain;
    player.gold += goldGain;
    gainExp(player, expGain);
    player.energy = min(player.energy + 20, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU ZRECZNOSCI ===\n\n";
    cout << "Wynik: " << score << " punktow\n";
    cout << "Zrecznosc +" << statGain << "\n";
    cout << "Zloto +" << goldGain << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void vitalityTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING WITALNOSCI");

    cout << "Przejdz przez serie wyzwan wytrzymalosciowych!\n";
    cout << "Wybieraj prawidlowe opcje aby zwiekszyc wynik!\n\n";
    pause();

    int score = 0;
    string challenges[] = {
        "Bieg na 10km", "Plywanie", "Wspinaczka", "Marsz z obciazeniem",
        "Cwiczenia cardio", "Trening interwałowy", "Yoga", "Crossfit"
    };

    for (int i = 0; i < 8; i++) {
        cout << "\nWyzwanie " << (i + 1) << ": " << challenges[i] << "\n";
        cout << "Wybierz intensywnosc (1-Lekka, 2-Srednia, 3-Ciezka): ";

        int choice;
        cin >> choice;

        if (choice >= 1 && choice <= 3) {
            int points = choice * (rand() % 3 + 3);
            score += points;
            cout << "Ukonczono! (+" << points << " pkt)\n";
        }
        else {
            cout << "Zly wybor! (0 pkt)\n";
        }
    }

    int statGain = max(1, score / 10);
    int goldGain = score / 5;
    int expGain = score * 2;

    player.stats.vitality += statGain;
    player.gold += goldGain;
    gainExp(player, expGain);
    player.energy = min(player.energy + 20, player.maxEnergy);
    player.maxHp = calculateMaxHp(player);
    player.currentHp = player.maxHp;

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU WITALNOSCI ===\n\n";
    cout << "Wynik: " << score << " punktow\n";
    cout << "Witalnosc +" << statGain << "\n";
    cout << "Max HP: " << player.maxHp << "\n";
    cout << "Zloto +" << goldGain << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void magicTraining(Player& player) {
    clearScreen();
    displayHeader("TRENING MAGII");

    cout << "Rozwiaz magiczne zagadki!\n";
    cout << "Kazda prawidlowa odpowiedz daje punkty!\n\n";
    pause();

    int score = 0;

    struct Question {
        string q;
        int answer;
    };

    vector<Question> questions = {
        {"2 + 2 * 2 = ?", 6},
        {"15 * 3 = ?", 45},
        {"100 / 4 = ?", 25},
        {"7 * 8 = ?", 56},
        {"144 / 12 = ?", 12},
        {"25 * 4 = ?", 100},
        {"81 / 9 = ?", 9},
        {"13 * 5 = ?", 65}
    };

    for (size_t i = 0; i < questions.size(); i++) {
        cout << "\nPytanie " << (i + 1) << ": " << questions[i].q << " ";
        int answer;
        cin >> answer;

        if (answer == questions[i].answer) {
            score += 10;
            cout << "Prawidlowo! (+10 pkt)\n";
        }
        else {
            cout << "Zle! Prawidlowa odpowiedz: " << questions[i].answer << "\n";
        }
    }

    int statGain = max(1, score / 10);
    int goldGain = score / 5;
    int expGain = score * 2;

    player.stats.magic += statGain;
    player.gold += goldGain;
    gainExp(player, expGain);
    player.energy = min(player.energy + 20, player.maxEnergy);

    clearScreen();
    cout << "\n=== WYNIKI TRENINGU MAGII ===\n\n";
    cout << "Wynik: " << score << " punktow\n";
    cout << "Magia +" << statGain << "\n";
    cout << "Zloto +" << goldGain << "\n";
    cout << "Doswiadczenie +" << expGain << "\n";
    pause();
}

void trainingMenu(Player& player) {
    while (true) {
        clearScreen();
        displayHeader("CENTRUM TRENINGOWE");
        displayPlayerStats(player);

        cout << "\n1. Trening Sily (Zwieksza obrazenia)\n";
        cout << "2. Trening Zrecznosci (Zwieksza trafienie i unik)\n";
        cout << "3. Trening Witalnosci (Zwieksza HP i obrone)\n";
        cout << "4. Trening Magii (Zwieksza umiejetnosci specjalne)\n";
        cout << "5. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: strengthTraining(player); break;
        case 2: agilityTraining(player); break;
        case 3: vitalityTraining(player); break;
        case 4: magicTraining(player); break;
        case 5: return;
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

        int enemyDamage = enemy.damage + rand() % 5;
        double enemyHitChance = 0.75 - player.stats.agility * 0.005;
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
        cout << "    !                                       !\n";
        cout << "    -----------------------------------------\n\n";

        player.wins++;
        player.gold += enemy.gold;
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

    player.energy -= 30;
    pause();
}

void arenaMenu(Player& player) {
    vector<Enemy> enemies = {
        {"Treningowy Manekn", 50, 50, 5, 2, 20, 10, 1},
        {"Poczatkujacy Wojownik", 80, 80, 8, 4, 35, 20, 2},
        {"Wykwalifikowany Wojownik", 120, 120, 12, 6, 55, 35, 3},
        {"Weteran Areny", 180, 180, 18, 9, 80, 55, 5},
        {"Elitarny Czempion", 250, 250, 25, 12, 120, 80, 7},
        {"Mistrz Areny", 350, 350, 35, 16, 180, 120, 10},
        {"Smok Rycerz", 500, 500, 45, 20, 250, 180, 13},
        {"Cieniowy Zabojca", 400, 400, 60, 15, 300, 220, 15},
        {"Wladca Demonow", 700, 700, 70, 25, 400, 300, 18},
        {"Smierc Wcielona", 1000, 1000, 90, 30, 600, 500, 20}
    };

    while (true) {
        clearScreen();
        displayHeader("ARENA WALKI");
        displayPlayerStats(player);

        if (player.energy < 30) {
            cout << "\n!!! Za malo energii! Idz na trening aby ja odzyskac !!!\n\n";
        }

        cout << "\nWybierz przeciwnika:\n\n";
        for (size_t i = 0; i < min(enemies.size(), static_cast<size_t>(player.level + 2)); i++) {
            cout << (i + 1) << ". " << setw(25) << left << enemies[i].name
                << " [Lvl " << setw(2) << enemies[i].level
                << " | HP: " << setw(4) << enemies[i].hp
                << " | Nagroda: " << enemies[i].gold << " zlota]\n";
        }
        cout << "0. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 0) return;

        if (choice > 0 && choice <= min(static_cast<int>(enemies.size()), player.level + 2)) {
            if (player.energy < 30) {
                cout << "\nZa malo energii!\n";
                pause();
                continue;
            }

            Enemy enemy = enemies[choice - 1];
            enemy.hp += (player.level - enemy.level) * 15;
            enemy.maxHp = enemy.hp;
            enemy.damage += (player.level - enemy.level) * 2;

            combat(player, enemy);
        }
        else {
            cout << "Zly wybor!\n";
            pause();
        }
    }
}

void shop(Player& player, vector<Equipment>& inventory) {
    vector<Equipment> weapons = {
        {"Zardzewialy Miecz", 5, 50, 1, "weapon"},
        {"Zelazne Ostrze", 10, 150, 3, "weapon"},
        {"Stalowa Katana", 18, 400, 5, "weapon"},
        {"Mithrilowa Klinga", 28, 800, 8, "weapon"},
        {"Smobojca", 40, 1500, 12, "weapon"},
        {"Legendarny Excalibur", 60, 3000, 15, "weapon"}
    };

    vector<Equipment> armors = {
        {"Polniana Zbroja", 3, 40, 1, "armor"},
        {"Skorzana Kamizelka", 8, 120, 3, "armor"},
        {"Kolczuga", 15, 350, 5, "armor"},
        {"Platowa Zbroja", 24, 700, 8, "armor"},
        {"Smoce Luski", 35, 1300, 12, "armor"},
        {"Boska Egida", 50, 2500, 15, "armor"}
    };

    vector<Equipment> accessories = {
        {"Brazowy Piersciern", 20, 60, 2, "accessory"},
        {"Srebrny Amulet", 35, 200, 4, "accessory"},
        {"Zloty Wisior", 50, 500, 7, "accessory"},
        {"Rubinowy Piersciern", 80, 1000, 10, "accessory"},
        {"Szmaragdowa Korona", 120, 2000, 13, "accessory"}
    };

    while (true) {
        clearScreen();
        displayHeader("SKLEP");
        displayPlayerStats(player);
        displayEquipment(player);

        cout << "\n1. Bronie\n";
        cout << "2. Zbroje\n";
        cout << "3. Akcesoria\n";
        cout << "4. Sprzedaj ekwipunek\n";
        cout << "5. Powrot\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 5) return;

        if (choice >= 1 && choice <= 3) {
            clearScreen();
            vector<Equipment>* items;
            string type;

            if (choice == 1) {
                items = &weapons;
                type = "BRONIE";
            }
            else if (choice == 2) {
                items = &armors;
                type = "ZBROJE";
            }
            else {
                items = &accessories;
                type = "AKCESORIA";
            }

            displayHeader(type);
            cout << "Twoje zloto: " << player.gold << "\n\n";

            for (size_t i = 0; i < items->size(); i++) {
                Equipment& item = (*items)[i];
                cout << (i + 1) << ". " << setw(25) << left << item.name
                    << " [+" << setw(2) << item.value
                    << " | " << item.price << " zlota | Wymaga lvl " << item.levelReq << "]\n";
            }
            cout << "0. Powrot\n\n";
            cout << "Wybor: ";

            int itemChoice;
            cin >> itemChoice;

            if (itemChoice > 0 && itemChoice <= items->size()) {
                Equipment& item = (*items)[itemChoice - 1];

                if (player.level < item.levelReq) {
                    cout << "\nZa niski poziom! Wymagany: " << item.levelReq << "\n";
                    pause();
                }
                else if (player.gold < item.price) {
                    cout << "\nZa malo zlota!\n";
                    pause();
                }
                else {
                    player.gold -= item.price;
                    Equipment* newItem = new Equipment(item);
                    inventory.push_back(*newItem);

                    if (item.type == "weapon") {
                        if (player.weapon) inventory.push_back(*player.weapon);
                        player.weapon = newItem;
                    }
                    else if (item.type == "armor") {
                        if (player.armor) inventory.push_back(*player.armor);
                        player.armor = newItem;
                    }
                    else {
                        if (player.accessory) inventory.push_back(*player.accessory);
                        player.accessory = newItem;
                    }

                    cout << "\nZakupiono: " << item.name << "!\n";
                    pause();
                }
            }
        }
        else if (choice == 4) {
            if (inventory.empty()) {
                cout << "\nBrak przedmiotow do sprzedania!\n";
                pause();
                continue;
            }

            clearScreen();
            displayHeader("SPRZEDAZ");

            for (size_t i = 0; i < inventory.size(); i++) {
                cout << (i + 1) << ". " << inventory[i].name
                    << " (" << inventory[i].price / 2 << " zlota)\n";
            }
            cout << "0. Powrot\n\n";
            cout << "Wybor: ";

            int sellChoice;
            cin >> sellChoice;

            if (sellChoice > 0 && sellChoice <= inventory.size()) {
                player.gold += inventory[sellChoice - 1].price / 2;
                cout << "\nSprzedano: " << inventory[sellChoice - 1].name << "!\n";
                inventory.erase(inventory.begin() + sellChoice - 1);
                pause();
            }
        }
    }
}

void upgradeStats(Player& player) {
    while (player.skillPoints > 0) {
        clearScreen();
        displayHeader("ULEPSZENIA STATYSTYK");
        displayPlayerStats(player);

        cout << "\nWybierz statystyke do ulepszenia:\n\n";
        cout << "1. Sila (+" << (player.stats.strength + 1) << ")\n";
        cout << "2. Zrecznosc (+" << (player.stats.agility + 1) << ")\n";
        cout << "3. Witalnosc (+" << (player.stats.vitality + 1) << ")\n";
        cout << "4. Magia (+" << (player.stats.magic + 1) << ")\n";
        cout << "5. Zapisz i wyjdz\n\n";
        cout << "Pozostale punkty: " << player.skillPoints << "\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;

        if (choice == 5) return;

        if (choice >= 1 && choice <= 4) {
            player.skillPoints--;

            if (choice == 1) player.stats.strength++;
            else if (choice == 2) player.stats.agility++;
            else if (choice == 3) {
                player.stats.vitality++;
                player.maxHp = calculateMaxHp(player);
                player.currentHp = player.maxHp;
            }
            else if (choice == 4) player.stats.magic++;

            cout << "\nStatystyka ulepszona!\n";
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    Player player;
    player.name = "Gladiator";
    player.level = 1;
    player.exp = 0;
    player.expToNext = 100;
    player.gold = 100;
    player.stats = { 5, 5, 5, 5 };
    player.maxHp = 100;
    player.currentHp = 100;
    player.energy = 100;
    player.maxEnergy = 100;
    player.weapon = nullptr;
    player.armor = nullptr;
    player.accessory = nullptr;
    player.skillPoints = 0;
    player.wins = 0;
    player.losses = 0;
    player.saveSlot = 1;

    vector<Equipment> inventory;

    clearScreen();
    cout << "\n\n";
    cout << "    -------------------------------------------------\n";
    cout << "    !                                               !\n";
    cout << "    !          GLADIATOR SOULS                      !\n";
    cout << "    !          Arena RPG Game                       !\n";
    cout << "    !                                               !\n";
    cout << "    !          Stworz swojego wojownika             !\n";
    cout << "    !          Trenuj, walcz i zwyciezaj!           !\n";
    cout << "    !                                               !\n";
    cout << "    -------------------------------------------------\n\n";

    cout << "1. Nowa gra\n";
    cout << "2. Wczytaj gre\n\n";
    cout << "Wybor: ";

    int startChoice;
    cin >> startChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (startChoice == 2) {
        clearScreen();
        cout << "\n--- WCZYTAJ GRE ---\n\n";
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
        cout << "\nWybor gniazda: ";

        int slotChoice;
        cin >> slotChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (slotChoice >= 1 && slotChoice <= 3) {
            player.saveSlot = slotChoice;
            if (loadGame(player)) {
                cout << "\nGra wczytana!\n";
                pause();
            }
            else {
                cout << "\nBlad: Nie mozna wczytac gry!\n";
                pause();
                return 1;
            }
        }
        else {
            cout << "\nZly wybor!\n";
            return 1;
        }
    }
    else {
        clearScreen();
        cout << "\n--- NOWA GRA ---\n\n";
        cout << "Wybierz gniazdo zapisu (1, 2 lub 3): ";
        int slotChoice;
        cin >> slotChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (slotChoice >= 1 && slotChoice <= 3) {
            player.saveSlot = slotChoice;

            if (saveGameExists(slotChoice)) {
                cout << "\nGniazdo zawiera juz zapis! Czy chcesz go nadpisac? (T/N): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (confirm != 'T' && confirm != 't') {
                    cout << "Anulowano.\n";
                    return 1;
                }
            }

            cout << "Podaj imie swojego gladiatora: ";
            getline(cin, player.name);
        }
        else {
            cout << "Zly wybor!\n";
            return 1;
        }
    }

    while (true) {
        clearScreen();
        displayHeader("MENU GLOWNE");
        displayPlayerStats(player);
        displayEquipment(player);

        cout << "\n1. Centrum Treningowe\n";
        cout << "2. Arena Walki\n";
        cout << "3. Sklep\n";
        cout << "4. Ulepszenia Statystyk\n";
        cout << "5. Odpoczynek (Przywroc HP i energie)\n";
        cout << "6. Zapisz gre\n";
        cout << "7. Zarzadzaj zapisami\n";
        cout << "8. Zakonczy gre\n\n";
        cout << "Wybor: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            trainingMenu(player);
            break;
        case 2:
            arenaMenu(player);
            break;
        case 3:
            shop(player, inventory);
            break;
        case 4:
            if (player.skillPoints > 0) {
                upgradeStats(player);
            }
            else {
                cout << "\nBrak dostepnych punktow umiejetnosci!\n";
                pause();
            }
            break;
        case 5:
            player.currentHp = player.maxHp;
            player.energy = player.maxEnergy;
            cout << "\nOdpoczales! HP i energia w pelni przywrocone!\n";
            pause();
            break;
        case 6:
            saveGame(player, inventory);
            pause();
            break;
        case 7:
            manageSaves();
            break;
        case 8:
            clearScreen();
            cout << "\n\n=== STATYSTYKI KONCOWE ===\n\n";
            displayPlayerStats(player);
            cout << "\nDziekujemy za gre!\n\n";
            return 0;
        default:
            cout << "\nZly wybor!\n";
            pause();
        }
    }

    return 0;
}









































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

using namespace std;

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
    cout << "\nGra zapisana w gniazdu " << player.saveSlot << "!\n";
}

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

    file.ignore();
    getline(file, save.weaponName);
    file >> save.weaponLevel >> save.weaponPrice;

    file.ignore();
    getline(file, save.armorName);
    file >> save.armorLevel >> save.armorPrice;

    file.ignore();
    getline(file, save.shieldName);
    file >> save.shieldLevel >> save.shieldPrice;

    file >> save.skillPoints >> save.wins >> save.losses >> save.arenaLevel;

    file.close();

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
        player.weapon.name = save.weaponName;
        player.weapon.level = save.weaponLevel;
        player.weapon.price = save.weaponPrice;
    }

    if (!save.armorName.empty() && save.armorName != "BRAK") {
        player.armor.name = save.armorName;
        player.armor.level = save.armorLevel;
        player.armor.price = save.armorPrice;
    }

    if (!save.shieldName.empty() && save.shieldName != "BRAK") {
        player.shield.name = save.shieldName;
        player.shield.level = save.shieldLevel;
        player.shield.price = save.shieldPrice;
    }

    return true;
}

void deleteSave(int slot) {
    string filename = getSaveFileName(slot);
    if (remove(filename.c_str()) == 0) {
        cout << "\nZapis z gniazda " << slot << " zostal usuniety!\n";
    }
    else {
        cout << "\nBlad: Nie mozna usunac zapisu!\n";
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

void levelUp(Player& player) {
    player.level++;
    player.skillPoints += 2;
    player.expToNext = static_cast<int>(player.expToNext * 1.5);
    player.maxHp = calculateMaxHp(player);
    player.currentHp = player.maxHp;
    player.energy = player.maxEnergy;

    clearScreen();
    cout << "\n\n";
    cout << "    -----------------------------------------\n";
    cout << "    !                                       !\n";
    cout << "    !        ★ AWANS POZIOMU! ★            !\n";
    cout << "    !                                       !\n";
    cout << "    !     Nowy poziom: " << setw(2) << player.level << "                !\n";
    cout << "    !     Otrzymujesz 2 punkty umiejetnosci !\n";
    cout << "    !                                       !\n";
    cout << "    -----------------------------------------\n\n";
    pause();
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

        int enemyDamage = enemy.damage + rand() % 5;
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
        {"Treningowy Manekn", 50, 50, 5, 2, 20, 10, 1, true},
        {"Poczatkujacy Wojownik", 80, 80, 8, 4, 35, 20, 2, true},
        {"Wykwalifikowany Wojownik", 120, 120, 12, 6, 55, 35, 3, true},
        {"Weteran Areny", 180, 180, 18, 9, 80, 55, 5, true},
        {"Elitarny Czempion", 250, 250, 25, 12, 120, 80, 7, true},
        {"Mistrz Areny", 350, 350, 35, 16, 180, 120, 10, true},
        {"Smok Rycerz", 500, 500, 45, 20, 250, 180, 13, true},
        {"Cieniowy Zabojca", 400, 400, 60, 15, 300, 220, 15, true},
        {"Wladca Demonow", 700, 700, 70, 25, 400, 300, 18, true},
        {"Smierc Wcielona", 1000, 1000, 90, 30, 600, 500, 20, true},
        {"Nieumarły Arcymag", 600, 600, 50, 35, 320, 280, 12, true},
        {"Bestia Mroku", 800, 800, 75, 28, 450, 350, 16, true},
        {"Krol Podziemi", 900, 900, 85, 32, 500, 400, 17, true},
        {"Tytanicka Bestia", 1100, 1100, 100, 38, 700, 600, 22, true},
        {"Szal Szamana", 650, 650, 55, 30, 350, 250, 14, true},
        {"Lodowy Byt", 720, 720, 65, 35, 400, 320, 15, true},
        {"Zakonnica Cieni", 500, 500, 70, 20, 300, 200, 13, true},
        {"Wojownik Oblakow", 550, 550, 48, 25, 280, 240, 12, true},
        {"Alchemik Zla", 480, 480, 60, 18, 260, 180, 11, true},
        {"Strażnik Bram", 850, 850, 80, 40, 550, 450, 19, true},
        {"Smok Czerni", 950, 950, 92, 42, 650, 500, 21, true},
        {"Anioł Zagłady", 1000, 1000, 88, 45, 700, 550, 23, true},
        {"Wampir Wielowiekowy", 700, 700, 68, 38, 400, 380, 18, true},
        {"Wilkolak Alfa", 620, 620, 72, 26, 350, 280, 16, true},
        {"Meduza Okrutna", 580, 580, 58, 22, 310, 220, 14, true},
        {"Lich Niesmiertelny", 880, 880, 78, 48, 600, 500, 22, true},
        {"Bestia Behemota", 1200, 1200, 110, 50, 800, 700, 25, true},
        {"Władca Czasu", 950, 950, 95, 52, 750, 600, 24, true},
        {"Destroyer Wszechświata", 1300, 1300, 120, 55, 900, 800, 26, true},
        {"Nieznany Byt z Mroku", 1500, 1500, 150, 60, 1200, 1000, 30, true}
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

int main() {
    srand(static_cast<unsigned>(time(0)));

    Player player;
    player.name = "Gladiator";
    player.level = 1;
    player.exp = 0;
    player.expToNext = 100;
    player.gold = 100;
    player.stats = { 5, 5, 5, 5, 1.0, 1.0, 1.0, 1.0 };
    player.maxHp = 100;
    player.currentHp = 100;
    player.energy = 100;
    player.maxEnergy = 100;
    player.weapon = {"", 0, 0, 0, 0, "weapon", -1};
    player.armor = {"", 0, 0, 0, 0, "armor", -1};
    player.shield = {"", 0, 0, 0, 0, "shield", -1};
    player.skillPoints = 0;
    player.wins = 0;
    player.losses = 0;
    player.saveSlot = 1;
    player.arenaLevel = 0;
    player.arenaMaxLevel = 10;
    player.defeatedBosses.resize(30, false);

    clearScreen();
    cout << "\n\n";
    cout << "    -------------------------------------------------\n";
    cout << "    !                                               !\n";
    cout << "    !          GLADIATOR SOULS                      !\n";
    cout << "    !          Arena RPG Game                       !\n";
    cout << "    !                                               !\n";
    cout << "    !          Stworz swojego wojownika             !\n";
    cout << "    !          Trenuj, walcz i zwyciezaj!           !\n";
    cout << "    !                                               !\n";
    cout << "    -------------------------------------------------\n\n";

    cout << "1. Nowa gra\n";
    cout << "2. Wczytaj gre\n\n";
    cout << "Wybor: ";

    int startChoice;
    cin >> startChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (startChoice == 2) {
        clearScreen();
        cout << "\n--- WCZYTAJ GRE ---\n\n";
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
        cout << "\nWybor gniazda: ";

        int slotChoice;
        cin >> slotChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (slotChoice >= 1 && slotChoice <= 3) {
            player.saveSlot = slotChoice;
            if (loadGame(player)) {
                cout << "\nGra wczytana!\n";
                pause();
            }
            else {
                cout << "\nBlad: Nie mozna wczytac gry!\n";
                pause();
                return 1;
            }
        }
        else {
            cout << "\nZly wybor!\n";
            return 1;
        }
    }
    else {
        clearScreen();
        cout << "\n--- NOWA GRA ---\n\n";

        cout << "Wybierz gniazdo zapisu (1, 2 lub 3): ";
        int slotChoice;
        cin >> slotChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (slotChoice >= 1 && slotChoice <= 3) {
            player.saveSlot = slotChoice;

            if (saveGameExists(slotChoice)) {
                cout << "\nGniazdo zawiera juz zapis! Czy chcesz go nadpisac? (T/N): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (confirm != 'T' && confirm != 't') {
                    cout << "Anulowano.\n";
                    return 1;
                }
            }

            cout << "Podaj imie swojego gladiatora: ";
            getline(cin, player.name);
        }
        else {
            cout << "Zly wybor!\n";
            return 1;
        }
    }

    while (true) {
        clearScreen();
        displayHeader("MENU GLOWNE");
        displayPlayerStats(player);
        displayEquipment(player);

        cout << "\n1. Centrum Treningowe\n";
        cout << "2. Arena Walki\n";
        cout << "3. Sklep\n";
        cout << "4. Ulepszenia Statystyk\n";
        cout << "5. Zapisz gre\n";
        cout << "6. Zarzadzaj zapisami\n";
        cout << "7. Zakonczy gre\n\n";

        cout << "Wybor: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            trainingMenu(player);
            break;
        case 2:
            arenaMenu(player);
            break;
        case 3:
            shop(player);
            break;
        case 4:
            if (player.skillPoints > 0) {
                upgradeStats(player);
            }
            else {
                cout << "\nBrak dostepnych punktow umiejetnosci!\n";
                pause();
            }
            break;
        case 5:
            saveGame(player);
            pause();
            break;
        case 6:
            manageSaves();
            break;
        case 7:
            clearScreen();
            cout << "\n\n=== STATYSTYKI KONCOWE ===\n\n";
            displayPlayerStats(player);
            cout << "\nDziekujemy za gre!\n\n";
            return 0;
        default:
            cout << "\nZly wybor!\n";
            pause();
        }
    }

    return 0;
}
