/*
 * Name: Thomas Jervey
 * Email: [Redacted]
 * Date: 12/10/2019
 * Function: Declare all functions and variables that are neccesary for the simulation,
 *           including all of the constants which dictate the simulation's performance.
 * Note: All 'Chances' are measured as percents in decimal form
*/

#include <vector>
#include "World.h"
#include "Colonist.h"

using namespace std; 

//Structures
struct Resource {
  char type;//'F', 'W', or 'S'
  int health;
  int position[2];
};

struct House {
  vector<Colonist*> occupants;
  Colonist* owner;
  Colonist* inheritor;
  int position[2];
};

//Class
class Colony: public World {
  public:
		//Constants
    const int GRID_DIMENSIONS = 10;
    const int AGE_YOUNG = 14;
    const int AGE_OLD = 45;//Back then, this was probably considered old
    const int AGE_DAYS = 30;//Amount of days it takes to age up (years can be kinda long in this simulation)
    const double AGE_DEATH_RATE = 0.0025;
    const double BIRTH_RATE = 0.03;
    const double BIRTH_DECREASE_PER_CHILD = 0.00375;//Maximum of 8 (0.03/0.00375) children - baby boomers
    const int MARRIAGE_AGE_THRESHOLD = 15;//Can marry someone that's 15 years older/younger then you, love is love I guess
    const bool GAY_MARRIAGE = false;//If you want marriages to happen faster, colonists use the power of science to give birth despite being the same sex!
    const int STARTING_COUPLES = 3;//Each couple starts with 1 child
    const int STARTING_AGE_MAX = 35;//Max age of starting colonist (not the children)
    const int STARTING_AGE_MIN = 20;//Min age of starting colonist (not the children)
    const int STARTING_FOOD = 100;
    const int SICK_LETHAL_TIME = 7;//Duration (in days) before sickness can kill
    const double SICK_DISCOVERY_CHANCE = 0.2;
    const double SICK_SPREAD_CHANCE = 0.05;
    const double SICK_RECOVER_CHANCE = 0.4;//Once discovered
    const double SICK_DEATH_CHANCE = 0.15;
    const double SICK_OUTBREAK_CHANCE = 0.0001;
    const double SICK_OUTBREAK_INCREMENT = 0.2;//Percentage of SICK_OUTBREAK_CHANCE added per day since last outbreak (currently 20%)
    const char RESOURCE_LIST[3] = {'F', 'W', 'S'};//F - Food, W - Wood, S - Stone
    const int RESOURCE_NUMBER = 3;
    const int RESOURCE_STRONG_CARRY = 10;//Adults carry this
    const int RESOURCE_WEAK_CARRY = 5;//Children & Elderly carry this
    const int RESOURCE_HEALTH = 100;//Amount that can be harvested from a resource tile
    const int RESOURCE_MAX = 5;//Amount of tiles of one resource possible at one time
    const double RESOURCE_FOOD_MINIMUM = 0.3;//Percent of colonists who are automatically delegated to food (most important resource)
    const double RESOURCE_SPAWN_RATE = 0.035;//Per colonist
    const int HOUSE_WOOD = 1000;//Cost of building a house
    const int HOUSE_STONE = 1000;//^
    const int DAILY_HUNGER = 1;//Hunger decrease per night
    const int TIME_MORNING = 7;
    const int TIME_LATE = 20;
    const int TIME_NIGHT = 24;
    const int TIME_DAY = 24;
    const int TIME_MONTH = 24 * 30;
    const int TIME_YEAR = 24 * 30 * 12;
    const int TIME_DECADE = 24 * 30 * 12 * 10;
    const int TIME_CENTURY = 24 * 30 * 12 * 100;
    //Variables
    bool end;
    //Constructor/Destructor
    Colony();
    ~Colony();
    //Methods
		void PrintGrid();
		void UpdateGrid();
    void ProgressTime();
  private:
    //Variables
		int hour;
    int lastPrint;
    int lastOutbreak;
    int totalFood;
    int totalWood;
    int totalStone;
    int gatheredFood;
    int gatheredWood;
    int gatheredStone;
    int illnessDeaths;
    int hungerDeaths;
    int ageDeaths;
    int colonistBirths;
    int colonistMarriages;
    int allTimeFood;
    int allTimeWood;
    int allTimeStone;
    int allTimeDeaths;
    int allTimeBirths;
    int allTimeMarriages;
    int allTimeColonists;
    vector<Resource*> resources;
    vector<House*> houses;
    vector<Colonist*> colonists;
    //Methods
		void initGrid();
		bool checkBounds(int,int);
    bool isOpen(int, int);
    int getOpenCells(int, int[], char[]);
    bool getRandomSpace(int[]);
    void generateResource(char);
    void checkResources();
    void removeResource(Resource*);
    House* generateHouse();
    Resource* getObjective();
    void initializeColonists();
    void moveColonists();
    void colonistBirth(House*);
    void colonistDeath(Colonist*, int);
    void consumeFood(int);
    void attemptMarriage(Colonist*, Colonist*);
    void colonistInteraction();
    void progressNight();
};