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

    cout << "Podaj imie swojego gladiatora: ";
    getline(cin, player.name);

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
        cout << "6. Zakoncz gre\n\n";
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
