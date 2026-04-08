#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <algorithm>
#include <map>





//individual units for the army class
class Unit {
    public:
    Unit () : name("unknown"), type("unknown"), hp(10), strength(10), recruitable(false) {}


    Unit (std::string name, std::string type, int hp, int strength) : 
    name(name), type(type), hp(hp), 
        strength(strength), recruitable(false) {}

    //access
    std::string getName() const {
        return name;
    }

    std::string getType() const {
        return type;
    }

    int getStrength() const {
        return strength;
    }

    int getHP() const {
        return hp;
    }

    bool isAlive() const {
        return hp > 0;
    }

    void takeDamage(int l) {
        hp -= l;

        if (hp < 0) hp =0;
    }

    //recruitment
    void unlock() {
        recruitable = true;
    }

    bool canRecruit() const {
        return recruitable;
    }


    private:
   std::string name; //display name
   std::string type; //depreciated


   //
   int hp;
   int strength;
   
   bool recruitable;

};

//vector of units, used for battles
class Army {
    public:
    Army() = default;

//adds to unit to vector
    void recruit(const Unit& unit) {
        army.push_back(unit);
        recalc();
    }

    bool isAlive() const {
        return !army.empty();
    }

    int getTotalHP() const {return totalHP;}

    int getTotalStrength() const {return totalStrength;}

    const std::vector<Unit>& getUnits() const {return army;}

//deals damage based on str value of units
    void attack(Army& enemy) {
        if (army.empty() || !enemy.isAlive()) return;


        for (auto& attacker : army) {
            if (!attacker.isAlive()) continue;

            enemy.takeDamage(attacker.getStrength());
        }

        enemy.clear();

        enemy.recalc();
    }


   private:
   std::vector<Unit> army;
   int totalHP =0;
   int totalStrength=0; 

   //recalcs total values and removes empty units
void recalc() {
    clear();

        totalHP =0;
        totalStrength =0;
        for (const auto& u : army) {

            if(!u.isAlive()) continue;

            totalHP += u.getHP();

            totalStrength += u.getStrength();
        }
    }

    //reduces hp with minor variation
   void takeDamage(int dmg) {
    if(army.empty() || dmg<= 0) return;

    int actualDamage = dmg + (rand() % 5 -2);

    if (actualDamage <= 0) actualDamage =1;

    int index = (rand() % army.size());

    army[index].takeDamage(actualDamage);
   }

   //remove empty unit
   void clear() {
     if (army.empty()) return;

     std::vector<Unit> survivors;

     for(auto& u : army) {
        if(u.isAlive()) 
            survivors.push_back(u);
     }

     army = survivors;
     
   }



};

//individual map units, with links to others
class Region {
   
    public:

    Region(std::string name) :
        name(name), status("free"), north(nullptr), south(nullptr),
        east(nullptr), west(nullptr) {}

    std::string toString() const {
        std::ostringstream output;

        output << "You are in " << name 
        << ".\n" << "It is currently: " << status <<        
            "\nTo the \nNorth: " << (north ? north->name: "None") <<
             "\nSouth: " << (south ? south->name: "None")<<
            "\nWest: " << (west ? west->name: "None") << 
            "\nEast: " << (east ? east->name: "None") <<
            "\n";
            
            if(enemy) {
                output<<"There is an enemy here.\nEstimated strength: " << enemyArmy.getTotalStrength();
            }
            
            return output.str();
    }


    void setArmy(Army r) {
        enemyArmy = r;
        setEnemy(true);
    }

    Army& getArmy() {
        return enemyArmy;
    }
    void setStatus(std::string r) {

        status = r;
    }
    
    std::string getStatus() const {
        return status;
    }
  
    std::string getName() const {
        return name;
    }
    bool hasEnemy() const {return enemy;}

    void setEnemy(bool e) {enemy = e;}

    //pointers to all adjacent regions
    Region* north;
    Region* south;
    Region* west;
    Region* east;

    std::string status = "free";

    //used to trigger events
    std::string eventKey = "";
    private:
    Army enemyArmy;

    std::string name;
    bool enemy = false;

  

};

//the auto battler. Chooses a winner based on hp and str of player and a region army
class Battle {
    public:
    Battle(Army& a, Army& b) : armyA(a), armyB(b), winner(nullptr) {}

    void resolve() {
        int turn = 1;

        while (armyA.isAlive() && armyB.isAlive()) {
            std:: cout << "\n===========\nTURN " << turn << "\n===========\n";
            turn++;

            int hpABefore = armyA.getTotalHP();
            int hpBBefore = armyB.getTotalHP();

            armyA.attack(armyB);

            int hpBAfter = armyB.getTotalHP();

            std::cout << "You ";
            printDamageReport(hpBBefore, hpBAfter);

            if(!armyB.isAlive()) break;

            armyB.attack(armyA);

            int hpAAfter = armyA.getTotalHP();

            std::cout << "Enemy ";
            printDamageReport(hpABefore, hpAAfter);


            if(!armyA.isAlive()) break;

            std::cout << "\nHP Remaining: \n" <<
                "You: " << armyA.getTotalHP() <<
                " | Enemy: " << armyB.getTotalHP() << "\n";
        }

        if(armyA.isAlive()) winner = &armyA;
        else if (armyB.isAlive()) winner = &armyB;

        else winner = nullptr; 

        
    }


    Army* getWinner() const {return winner;}

    void clear() {winner = nullptr;}


    private:
    Army& armyA;
    Army& armyB;
    Army* winner;

    void printDamageReport(int before, int after) {

        std::cout << " dealt " << before - after << " damage\n";
    }
};

//player, holding their army andlocation and the ability ot move
class Player {
    public: 
    Player(std::string name, Region* start) :
        name(name), currentRegion(start), alive(true), respawnCounter(0) {}

    Player() : name("Dragon"), currentRegion(nullptr), alive(true), respawnCounter(0)  {}

    //movement
    void moveNorth() {
        if (currentRegion->north) {
            currentRegion = currentRegion->north; 
        }

        else 
            std::cout<<"Can't go north!\n";
    }

     void moveSouth() {
        if (currentRegion->south) {
            currentRegion = currentRegion->south; 
        }

        else 
            std::cout<<"Can't go south!\n";
    }

        void moveWest() {
        if (currentRegion->west) {
            currentRegion = currentRegion->west; 
        }

        else 
            std::cout<<"Can't go west!\n";
    }

     void moveEast() {
        if (currentRegion->east) {
            currentRegion = currentRegion->east; 
        }

        else 
            std::cout<<"Can't go east!\n";
    }

      
//respawn 
    void respawn(Region* r) {
        currentRegion = r;
        respawnCounter++;
        alive = true;

        army = Army();

        //army.recruit()

    }

    int getRespawnCount() const {return respawnCounter;}
   
    Army& getArmy() {
        return army;
    }
    Region* getRegion() {
        return currentRegion;
    }

    bool isAlive() const {return alive;}

    void setLiving(bool r) {alive = r;}


    private:

    bool alive = true;
    std::string name = "The Dark Lord";
    Region* currentRegion;
    Army army;

    int respawnCounter=0;
};

 
//holds all unit types
  struct UnitLibrary {
    public:

    Unit slime, goblin, skeleton, deepOne, abomination, lich, kraken, dragon, 
        militia, infantry, archers, paladin;

        //name, type, size, discipline, attack, defense
    UnitLibrary() :  slime("Slime", "slime", 20,5),
        goblin("Goblin", "goblin", 25,8),
        skeleton("Skeleton Warrior", "undead", 35, 15),
         deepOne("Deep One", "eldritch", 40,25),
         abomination("Abomination", "eldritch", 60,30),
         lich("Lich", "undead", 100,85),
         kraken("Kraken", "eldritch", 100, 50),
         dragon("Dragon", "dragon", 150, 50),


         militia("Militia", "human", 30,15),
         infantry("Infantry", "human", 40,25),
         archers("Archers", "human", 25,35),
         paladin("Paladin", "human", 60, 40){}
 };


 //used to print story events
class Event {
    public:

    Event() : title("Placeholder"), description("temp"), available(true) {}
    
    Event(std::string title) : title(title), description(title), available(true) {}

    void setDescription(std::string r) {
        description = r;
    }
    void enable() {available = true;}
    void disable() {available = false;}

    void trigger() {
        std::cout << "\n========\n" << title << "\n========\n"; 
        std::cout << description <<'\n';

        disable();
    }
    
    private:
    std::string title;
    std::string description;
    bool available;


};

//main game system
class Game {
    public:

    Game() : units(), player("Dragon", nullptr) {

        //create and name map
    for (int i =0; i < ROW; i++) {
        for (int j =0; j < COL; j++) {
            map[i][j] = new Region(regionNames[i][j]);
        }
    }

    //assign pointers to all regions
    for (int i =0; i < ROW; i++) {
        for(int j =0; j < COL; j++) {
            if(i > 0) map[i][j]->north = map[i-1][j];
            if(i < ROW-1) map[i][j]->south = map[i+1][j];
            if(j > 0) map[i][j]->west = map[i][j-1];
            if(j < COL -1) map[i][j]->east = map[i][j+1];

        }
    }

    //spawn enemeis, player, and events
    populateMap();
        player = Player("Dragon", map[3][2]);
            initEvents();




    player.getArmy().recruit(units.dragon); //starter unit
    }

    Player& getPlayer() {return player;}
    UnitLibrary& getUnitList() {return units;}

    void showIntro() {
        eventList["Prologue"].trigger();
        waitForEnter();
        eventList["Intro"].trigger();
        waitForEnter();
        eventList["Tutorial"].trigger();
        waitForEnter();
    }
        
void play() {

    //three lives before game over
    if(player.getRespawnCount() > 3) {
        eventList["Lose"].trigger();
        std::cout << "GAME OVER\n";

        gameOver();
        return;
    }

    //assign occupied status
    if(!player.getRegion()->hasEnemy() && player.getRegion()->getStatus() !="Occupied"){
        player.getRegion()->setStatus("Occupied");
    }


 


    //battle if there's an enemy in that spot
    if(player.getRegion()->hasEnemy() && 
        player.getArmy().isAlive() &&
        player.getRegion()->getArmy().isAlive()) {
            
            std::cout << "A battle in " << player.getRegion()->getName() <<"\n";

            Battle battle(player.getArmy(), player.getRegion()->getArmy());
            battle.resolve();

            Army* winner = battle.getWinner();


            if (winner == &player.getArmy()) {
                std::cout << "Victory! this region is now yours.\n";
                player.getRegion()->setEnemy(false);
                player.getRegion()->setStatus("Occupied");
            }

            else {
                std::cout << "Your army is defeated and you're forced to retreat";
                player.respawn(map[3][2]);
                player.getArmy().recruit(units.dragon);
            }

    //triggers events on certain regions
    if(!player.getRegion()->eventKey.empty() && eventList.count(player.getRegion()->eventKey)) {
        //handle unlocks tied to region
        Region* region = player.getRegion();

        eventList[region->eventKey].trigger();
        if(region->eventKey == "Goblin") units.goblin.unlock();
        else if (region->eventKey == "Slime") units.slime.unlock();
        else if (region->eventKey == "Lich") units.skeleton.unlock();
        else if (region->eventKey == "KrakenALT") units.deepOne.unlock();
        else if (region->eventKey == "Kraken") units.deepOne.unlock();
        else if (region->eventKey == "Secretary") {
            map[2][4]->eventKey = "KrakenALT";
            units.abomination.unlock();
        }
    }

}


        std::cout << "\n=======================\n";
        std::cout << player.getRegion()->toString();
        std::cout << "\nYour army HP: " <<player.getArmy().getTotalHP();

        std::cout << "\n--- Controls ---\n" 
            << "[R] Recruit\n"
            << "[M] Show Map\n" 
            << "Move:  [N]orth  [S]outh  [E]ast  [W]est";

        char in = getInput();

        //movement inputs  
        switch (in) {
            case 'n': player.moveNorth();break;
            case 's': player.moveSouth();break;
            case 'e': player.moveEast();break;
            case 'w': player.moveWest();break;
            case 'r': openRecruitMenu();break;
            case 'm': openMap();break;
            default: std::cout << "Invalid command.\n";break;

        }

        if (map[0][2]->getStatus() == "Occupied") {
            eventList["Ending"].trigger();
            gameOver();
        }
}


//spawns enemies and events
void populateMap() {
    //goblins
    Region& goblinCave = *map[3][1];
    goblinCave.setEnemy(true);
    goblinCave.getArmy().recruit(units.goblin);
    goblinCave.eventKey = "Goblin";

    //slime
    Region& slimeLake = *map[3][0];
    slimeLake.setEnemy(true);
    slimeLake.getArmy().recruit(units.slime);
    slimeLake.eventKey = "Slime";

    //watchtower
    Region& watchtower = *map[2][1];
    watchtower.setEnemy(true); //watchtower
    watchtower.status = "enemy";
    watchtower.getArmy().recruit(units.militia);
    watchtower.getArmy().recruit(units.archers);

    //castle
    Region& castle = *map[1][2];
    castle.setEnemy(true);
    castle.status = "enemy";
    castle.getArmy().recruit(units.infantry);
    castle.getArmy().recruit(units.archers);

     //sunken ship
    Region& ship = *map[3][4];
    ship.setEnemy(true);
    ship.status = "enemy";
    ship.getArmy().recruit(units.deepOne);


     //sunken ship
    Region& fishCove = *map[1][4];
    fishCove.setEnemy(true);
    fishCove.status = "enemy";
    fishCove.getArmy().recruit(units.deepOne);
    fishCove.getArmy().recruit(units.deepOne);




    //abyss
     Region& abyss = *map[2][4];
    abyss.setEnemy(true);
    abyss.status = "enemy";
    abyss.getArmy().recruit(units.deepOne);
    abyss.getArmy().recruit(units.kraken);
    abyss.eventKey = "Kraken";

       //citadel
     Region& citadel = *map[0][3];
    citadel.setEnemy(true);
    citadel.status = "enemy";
    citadel.getArmy().recruit(units.paladin);
    citadel.getArmy().recruit(units.infantry);
     citadel.getArmy().recruit(units.archers);

    //capital
    Region& capital = *map[0][2];
    capital.setEnemy(true);
    capital.status = "enemy";

    for (int i =0; i < 2; i++) {
    capital.getArmy().recruit(units.paladin);
    capital.getArmy().recruit(units.infantry);
    capital.getArmy().recruit(units.militia);
    capital.getArmy().recruit(units.archers);
    }

       //office tower
     Region& office = *map[1][0];
    office.setEnemy(true);
    office.status = "enemy";
    office.getArmy().recruit(units.abomination);
    office.getArmy().recruit(units.abomination);
    office.eventKey = "Secretary";

   //ancient ruin
     Region& ruin = *map[0][4];
    ruin.setEnemy(true);
    ruin.status = "enemy";
    ruin.getArmy().recruit(units.lich);
    for (int i =0; i < 3; i++) {
    ruin.getArmy().recruit(units.skeleton);
    }
        ruin.eventKey = "Lich";

    }


char getInput() {
    char input;

    std::cout << "\n>> ";
    std::cin >>input;

    return std::tolower(input);
}

void gameOver() {
    over = true;
}

bool isOver() {
    return over;
}
    private:

    bool over = false;

    Player player;
    UnitLibrary units;



    //map dimensions
    static constexpr int ROW = 4;
    static constexpr int COL =5;

    Region* map[ROW][COL];

    //used to access events. Titles here
  std::map<std::string, Event> eventList = {
    {"Prologue", Event("Prologue: The Demon King is a loser!?")},
    {"Intro", Event("Turned Into a Dragon by Truck-kun")},
    {"Tutorial", Event("Onboarded by an Eldritch Secretary")},
    {"Goblin", Event("Little Green Cowards")},
    {"Slime", Event("The Slime King's Grace")},
    {"Lich", Event("The 4,000 Year Old NEET")},
    {"Kraken", Event("Terror Squid")},
    {"KrakenALT", Event("The Octopus' Crush")},
    {"Secretary", Event("Promoted")},
    {"Lose", Event("Game Over")},
    {"Ending", Event("Choose Ending")}

  };

  //set body text for each event
  void initEvents() {
    eventList["Prologue"].setDescription("For hundreds of years, each of the Demon King's minions "
        "\nhas been defeated by a hero brought in from another world... Finally, he had enough "
        "\nhe snapped his fingers and a truck appeared beside him... "
    );
    eventList["Intro"].setDescription("You were struck by a flying ice cream truck! "
    "\nHow inconvenient.... "
    "\nWhen you awoke, you were in a cave full of gold and gems. " 
    "\nAnd you had wings! "
);
    eventList["Tutorial"].setDescription("A flailing mass of tentacles appeared! "
        "\n'I've been assigned to help you take over the world,' it says, in a strangely chipper voice "
        "\n'you have three chances before we pull funding!' "
        "\n'First, move west to the goblin cave, and you'll be able to recruit them' ");
    
    eventList["Goblin"].setDescription("The goblins cower in fear at your presence! "
    "\nThey agree to join you as long as you don't eat them. "
    "\nWere you going to anyway? "
    "\nYou can now recruit goblins. ");
    
    eventList["Slime"].setDescription("A giant blue slime rides out to meet you atop an even bigger, bluer slime "
    "\nA stray ember from your breath sets them both aflame "
    "\n'Blblblblbblb,' the king says "
    "\nYou can now recruit slimes. "); 

    eventList["Lich"].setDescription("An ancient skeleton sits at a ratty old desk, typing something "
    "\n'You can have my army, if you just leave me alone!' "
    "\nYou can now recruit skeletons") ;

    eventList["Kraken"].setDescription("'Blblblblblblblbblrbrblrrlblblbl' "
    "\nYou can now recruit deep ones ");

    eventList["KrakenALT"].setDescription("With one look at your assistant, the Kraken is smitten by love "
    "\nYou can now recruit deep ones ");

    eventList["Secretary"].setDescription("The same mass of tentacles from before appears before you. "
    "\n'There's gonna be a lot of paperwork for you destroying the tower... "
    "\nI'll have to come with you to make sure you don't pull something like this again' "
    "\nYou can now recruit abominations ");

    eventList["Lose"].setDescription("Your backers pull funding. ");
    
    eventList["Ending"].setDescription("This world is now yours... "
    "\nWhat now? "
    "\nREIGN OF TERROR? "
    "\nTyranny? "
    "\nGo hibernate? "
    "\nEstablish a liberal democracy? "
    "\nYour choice");

  }

  std::string regionNames[4][5] {
        {"Tower Tower","Watch Tower","Capital","Citadel","Legacy Infrastructure"},
        {"Office  Tower","Unpeaceful Village","Castle","Fish Village","Fishman Cove"},
        {"Wizard Tower","Watchtower","Peaceful Village","Coral Reef","Abyss"},
        {"Slime Lake","Goblin Cave","Dragon's Lair","Mountain","Sunken Costs"}
    };

    //view and recruit unlocked units
    void openRecruitMenu() {
        std::cout<<"\n--- RECRUITMENT ---\n";

        std::vector<Unit*> recruitables = {
            &units.goblin, &units.slime, &units.skeleton,
            &units.deepOne, &units.abomination, &units.kraken
        };

        std::vector<std::string> names = {
            "Goblin", "Slime", "Skeleton Warrior", "Deep One", "Abomination", "Kraken"
        };

        bool hasAvailable = false;

        for(size_t i =0; i < recruitables.size(); i++) {
            Unit* u = recruitables[i];

            if (u->canRecruit()) {
                hasAvailable = true;
                std::cout << i +1 << ". " << names[i] 
                    << " (HP: " << u->getHP() 
                    << ", STR: " << u->getStrength() << ")\n";
            }
        }

        if (!hasAvailable) {
            std::cout << "No unlocked units. Defeat bosses and conquer regions to unlock them!\n";
            waitForEnter();
            return;
        }

        int choice;
        std::cin >> choice;

        if (choice <=0 || choice > recruitables.size()) {
            std::cout << "Cancelled\n";
            return;
        }


        Unit* selected = recruitables[choice -1];

        if (!selected->canRecruit()) {std::cout << "That unit isn't unlocked.\n";
        return;}

        player.getArmy().recruit(*selected);

        std::cout << "Recruited " << selected->getName() << '\n';
        waitForEnter();
    }


    //map shows conquered and enemy regions as well as player location
    void openMap() {
            std::cout<<"\n--- MAP ---\n";

            for (int i =0; i < ROW; i++) {
                for (int j =0; j < COL; j++) {
                    Region* r = map[i][j];
                    char symbol = '?';

                    if(r == player.getRegion()) symbol = 'd';
                    else if (r == map[0][2]) symbol = 'C';
                    else if (!r->hasEnemy() && r->getStatus() == "Occupied") symbol = 'o';
                    else if (r->hasEnemy()) symbol = 'E';
                    else symbol = '.';

                    std::cout << symbol << " ";
                }
                std::cout <<"\n";
            }

            std::cout << "D = You | E = Enemy | O = Conquered | C = Capital | . = Free\n";

            std::cout<<"Enter any key to continue....";
            waitForEnter();
        }
        
        void waitForEnter() {
            std::cout<<"Enter any key to continue....";
            getInput();

        }


    

    };

