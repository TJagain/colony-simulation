/*
 * Name: Thomas Jervey
 * Email: [Redacted]
 * Date: 12/10/2019
 * Function: Colony Simulation system with a wide variety of functionality!.
 * Note: This simulation can scale from hours to decades but it's not exactly optimized so proceed at your own risk!
*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <ctime>
#include "Colony.h"

using namespace std;

//Declarations
bool roll(double);

//Constructor
Colony::Colony(){
  cout << "Create Colony" << endl;
  end = false;
  hour = 0;
  day = 0;
  lastPrint = 0;
  lastOutbreak = 0;
  totalFood = STARTING_FOOD;
  totalWood = 0;
  totalStone = 0;
  gatheredFood = 0;
  gatheredWood = 0;
  gatheredStone = 0;
  illnessDeaths = 0;
  hungerDeaths = 0;
  ageDeaths = 0;
  colonistBirths = 0;
  colonistMarriages = 0;
  allTimeFood = 0;
  allTimeWood = 0;
  allTimeStone = 0;
  allTimeDeaths = 0;
  allTimeBirths = 0;
  allTimeMarriages = 0;
  allTimeColonists = STARTING_COUPLES * 3;
  Colony::initGrid();
  checkResources();
  initializeColonists();
  srand(time(NULL));
}

//Destructor
Colony::~Colony(){
  for (auto resource : resources){
    delete resource;
  }
  for (auto house : houses){
    delete house;
  }
  for (auto colonist : colonists){
    delete colonist;
  }
}

/*

[----- Handle Grid -----]

*/
void Colony::PrintGrid (){//Print the map, along with a summary of the simulation in the designated time period
  if (day == 0){//The initial print, no summary needed
    cout << "-----Initial Map-----" << endl;
  }else{
    string summaryPrefix = (end) ? "Final" :
                           (lastPrint >= TIME_CENTURY) ? "Century" :
                           (lastPrint >= TIME_DECADE) ? "Decade" :
                           (lastPrint >= TIME_YEAR) ? "Yearly" :
                           (lastPrint >= TIME_MONTH) ? "Monthly" :
                           (lastPrint >= TIME_DAY) ? "Daily" : "Hourly";
    string timeStats = "";
    if (summaryPrefix == "Hourly"){
      string timePrefix = (hour > 10) ? "" : "0";
      timeStats = " | Current Time - " + timePrefix + to_string(hour) + ":00";
    }
    cout << "-----" << summaryPrefix << " Summary-----" << endl;
    cout << "Days Passed - " << day << timeStats << endl;
    cout << "[Resource Stats]" << endl;
    if (summaryPrefix == "Final"){
      cout << "Total Food - " << allTimeFood << endl;
      cout << "Total Wood - " << allTimeWood << endl;
      cout << "Total Stone - " << allTimeStone << endl;
    }else{
      cout << "Current Resources: Food - " << totalFood << " Wood - " << totalWood << " Stone - " << totalStone << endl;
      cout << "Gathered Resources: Food - " << gatheredFood << " Wood - " << gatheredWood << " Stone - " << gatheredStone << endl;
    }
    cout << "[Colonist Stats]" << endl;
    if (summaryPrefix == "Final"){
      cout << "Total Colonists - " << allTimeColonists << endl;
      cout << "Total Deaths - " << allTimeDeaths << endl;
      cout << "Total Births - " << allTimeBirths << endl;
      cout << "Total Marriages - " << allTimeMarriages << endl;
    }else{
      cout << "Current Colonists: " << colonists.size() << endl;
      cout << "Colonist Deaths: Illness - " << illnessDeaths << " Hunger - " << hungerDeaths << " Age - " << ageDeaths << endl;
      cout << "Colonist Births: " << colonistBirths << endl;
      cout << "Colonist Marriages: " << colonistMarriages << endl;
      if ((day - lastOutbreak) < (lastPrint / TIME_DAY)){
        cout << "[Outbreak Occured!]" << endl;
        int spreadCount = 0;
        for (auto colonist : colonists){
          if (colonist->getSick() || colonist->getSickStart() > lastOutbreak)
            spreadCount++;
        }
        cout << "Current Spread Count: " << spreadCount << endl;
      }
    }
  }
  //Print the map
  for (int y = 0; y < GRID_DIMENSIONS; y++){
    cout << string(GRID_DIMENSIONS*2, '-') << "-" << endl;
    for (int x = 0; x < GRID_DIMENSIONS; x++){
      cout << "|" << grid[y][x];
    }
    cout << "|" << endl;
  }
  cout << string(GRID_DIMENSIONS*2, '-') << "-" << endl;
  lastPrint = 0;
  gatheredFood = 0;
  gatheredWood = 0;
  gatheredStone = 0;
  illnessDeaths = 0;
  hungerDeaths = 0;
  ageDeaths = 0;
  colonistBirths = 0;
  colonistMarriages = 0;
}

void Colony::UpdateGrid(){//Draw the map, including colonists on spaces that are open
  //Remove Colonists
  for (int y = 0; y < GRID_DIMENSIONS; y++)
    for (int x = 0; x < GRID_DIMENSIONS; x++)
      if (grid[y][x] == 'c' || grid[y][x] == 'C')
        grid[y][x] = ' ';
  //Redraw Colonists
  for (auto colonist : colonists){
    int position[2];
    position[0] = colonist->position[0];
    position[1] = colonist->position[1];
    char gridTile = grid[position[0]][position[1]];
    if (gridTile == ' ' || gridTile == 'C' || gridTile == 'c')
      grid[position[0]][position[1]] = (colonist->getAge() > AGE_YOUNG) ? 'C' : 'c';
  }
}

void Colony::initGrid(){//Initialize the map to open spaces
  for (int y = 0; y < GRID_DIMENSIONS; y++)
    for (int x = 0; x < GRID_DIMENSIONS; x++)
      grid[y][x] = ' ';
}

bool Colony::checkBounds(int y, int x){//Verify that the designate position is possible (should not ever happen but better to be safe)
  if (y > GRID_DIMENSIONS)
    return false;
  if (x > GRID_DIMENSIONS)
    return false;
  return true;
}

bool Colony::isOpen(int y, int x){//Target cell is open (no resource or house)
  char currentCharacter = grid[y][x];
  if (currentCharacter == ' ' || currentCharacter == 'C')
    return true;
  return false;
}

int Colony::getOpenCells(int rowNumber, int openCells[], char row[]){//Returns all the open cells in a specified row
  int cellNumber = 0;
  for (int x = 0; x < GRID_DIMENSIONS; x++){
    if (isOpen(rowNumber, x)) {
      openCells[cellNumber] = x;
      cellNumber++;
    }
  }
  return cellNumber;
}

void shuffleArray(int arr[], int size){//Uses Fisher-Yates algorithm to shuffle array
  for (int i = size - 1; i > 0; i--) {
    int j = rand() % (i + 1);//Get random number between 0 and i
    swap(arr[i], arr[j]);
  }
}

bool Colony::getRandomSpace(int position[]){//Finds a random open cell on the map, returns a boolean based on whether the operation succeeded
  //Generate Y range then shuffle randomly so that we don't have to worry about repeats
  int columnOrder[GRID_DIMENSIONS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  shuffleArray(columnOrder, GRID_DIMENSIONS);
  for (int i = 0; i < GRID_DIMENSIONS; i++){
    int yValue = columnOrder[i];
    int openCells[GRID_DIMENSIONS] = {0};
    int cellNumber = getOpenCells(yValue, openCells, grid[yValue]);
    if (cellNumber > 0){
      //Get a random cell from the possible cells
      int xValue = openCells[rand() % (cellNumber)];
      position[0] = yValue;
      position[1] = xValue;
      return true; //Return instead of break so we know when all cells are occupied
    }
  }
  //All cells are occupied if it gets to here
  end = true;
  return false;
}

/*

[----- Handle Resources -----]

*/
void Colony::generateResource(char resourceType){//Creates a new resource of the specified type at a random position
  int position[2];
  bool gotSpace = getRandomSpace(position);
  if (gotSpace){
    grid[position[0]][position[1]] = resourceType;
    Resource* newResource = new Resource;
    newResource->health = RESOURCE_HEALTH;
    newResource->type = resourceType;
    newResource->position[0] = position[0];
    newResource->position[1] = position[1];
    resources.push_back(newResource);
  }else {
    cout << "Cannot spawn resource" << endl;
  }
}

void Colony::checkResources(){
  int resourceCount[RESOURCE_NUMBER] = {0};
  //Record resource amounts
  for (auto resource : resources){
    for (int i = 0; i < RESOURCE_NUMBER; i++){
      if (resource->type == RESOURCE_LIST[i])
        resourceCount[i]++;
    }
  }
  //Add a resource if not max
  for (int i = 0; i < RESOURCE_NUMBER; i++){
    if (resourceCount[i] < RESOURCE_MAX) {
      generateResource(RESOURCE_LIST[i]);
    }
  }
}

void Colony::removeResource(Resource* exhaustedResource){
  grid[exhaustedResource->position[0]][exhaustedResource->position[1]] = ' ';
  int index = 0;
  for (auto resource : resources){
    if (resource == exhaustedResource){
      resources.erase(resources.begin() + index);
      break;
    }
    index++;
  }
  for (auto colonist : colonists){
    if (colonist->getObjective() == exhaustedResource)
      colonist->assignObjective(NULL);
  }
  delete exhaustedResource;
}

/*

[----- Handle Houses -----]

*/
House* Colony::generateHouse() {//Creates a new house at a random position
  int position[2];
  bool gotSpace = getRandomSpace(position);
  if (gotSpace){
    grid[position[0]][position[1]] = 'H';
    House* newHouse = new House;
    newHouse->position[0] = position[0];
    newHouse->position[1] = position[1];
    houses.push_back(newHouse);
    return newHouse;
  }else {
    return NULL;
  }
}

/*

[----- Handle Colonists -----]

*/
Resource* Colony::getObjective(){
  //generate current resource occupation
  int resourceOccupation[RESOURCE_NUMBER] = {0};
  double minimumFood = (double)colonists.size() * RESOURCE_FOOD_MINIMUM;
  resourceOccupation[0] = -(int)minimumFood;
  for (auto colonist : colonists){
    Resource* objective = colonist->getObjective();
    if (objective){
      char objectiveType = objective->type;
      for (int i = 0; i < RESOURCE_NUMBER; i++){
        if (objectiveType == RESOURCE_LIST[i]){
          resourceOccupation[i]++;
          break;
        }
      }
    }
  }
  //get resource with lowest occupation
  int lowestIndex = 0;
  for (int i = 1; i < RESOURCE_NUMBER; i++){
    if (resourceOccupation[i] < resourceOccupation[lowestIndex])
      lowestIndex = i;
  }
  char resourceType = RESOURCE_LIST[lowestIndex];
  //pick random tile of that resource
  int currentTiles = 0;
  Resource* tileList[resources.size() - 1];
  for (auto resource : resources){
    if (resource->type == resourceType) {
      tileList[currentTiles] = resource;
      currentTiles++;
    }
  }
  if (currentTiles > 0){
    int randomIndex = rand() % (currentTiles);
    return tileList[randomIndex];
  }else {
    return NULL;
  }
}

void Colony::initializeColonists(){//A new colony is established!
  for (int i = 0; i < STARTING_COUPLES; i++) {
    House* newHouse = generateHouse();//Should not have to check if house exists, grid should not be full at the start
    int birthday = day % AGE_DAYS;
    int randomAge = rand() % (STARTING_AGE_MAX - STARTING_AGE_MIN + 1) + STARTING_AGE_MIN;
    Colonist* husband = new Colonist(randomAge, birthday, 'M', newHouse);
    husband->assignObjective(getObjective());
    colonists.push_back(husband);
    newHouse->owner = husband;//I swear I'm not being sexist
    newHouse->occupants.push_back(husband);
    
    randomAge = rand() % (STARTING_AGE_MAX - STARTING_AGE_MIN + 1) + STARTING_AGE_MIN;
    Colonist* wife = new Colonist(randomAge, birthday, 'F', newHouse);
    wife->assignObjective(getObjective());
    colonists.push_back(wife);
    newHouse->inheritor = wife;
    newHouse->occupants.push_back(wife);
    
    husband->setSpouse(wife);
    wife->setSpouse(husband);
    
    char gender = (roll(50.0)) ? 'M' : 'F';
    randomAge = rand() % (AGE_YOUNG + 1);
    Colonist* child = new Colonist(randomAge, birthday, gender, newHouse);
    child->assignObjective(getObjective());
    colonists.push_back(child);
    newHouse->occupants.push_back(child);
  }
}

int getDirection(int currentPosition, int targetPosition){//calculate direction towards the colonist's objective
  int distance = targetPosition - currentPosition;
  if (distance > 0)
    return 1;
  else if (distance < 0)
    return -1;
  else
    return 0;
}

void Colony::moveColonists(){//Based on a number of factors, move (or don't move) a colonist in the neccesary direction
  int targetPosition[2];
  for (auto colonist : colonists){
    //Decide where to go
    if (hour >= TIME_LATE || hour < TIME_MORNING || colonist->getCarrying() || colonist->getSickDiscovered()){
      targetPosition[0] = colonist->getHome()->position[0];
      targetPosition[1] = colonist->getHome()->position[1];
    }else {
      Resource* objective = colonist->getObjective();
      if (objective){
        targetPosition[0] = objective->position[0];
        targetPosition[1] = objective->position[1];
      }else {
        targetPosition[0] = colonist->getHome()->position[0];
        targetPosition[1] = colonist->getHome()->position[1];
      }
    }
    int currentY = colonist->position[0];
    int currentX = colonist->position[1];
    //If already there
    if (currentY == targetPosition[0] && currentX == targetPosition[1]){
      //If dropping off resources
      if (colonist->getCarrying() > 0){
        char resourceType = colonist->getObjectiveType();
        switch (resourceType){
          case 'F':
            totalFood += colonist->getCarrying();
            gatheredFood += colonist->getCarrying();
            allTimeFood += colonist->getCarrying();
            break;
          case 'W':
            totalWood += colonist->getCarrying();
            gatheredWood += colonist->getCarrying();
            allTimeWood += colonist->getCarrying();
            break;
          case 'S':
            totalStone += colonist->getCarrying();
            gatheredStone += colonist->getCarrying();
            allTimeStone += colonist->getCarrying();
            break;
        }
        colonist->setCarrying(0);
        if (!colonist->getObjective())
          colonist->assignObjective(getObjective());
      }else if(hour < TIME_LATE && hour >= TIME_MORNING  && !colonist->getSickDiscovered()){
        //If picking up resources
        Resource* resource = colonist->getObjective();
        if (resource){
          int age = colonist->getAge();
          int carryAmount = (age > AGE_YOUNG && age < AGE_OLD) ? RESOURCE_STRONG_CARRY : RESOURCE_WEAK_CARRY;
          int resourceHealth = resource->health;
          if (carryAmount >= resourceHealth){
            carryAmount = resourceHealth;
            removeResource(resource);
          }else{
            resource->health = resourceHealth - carryAmount;
          }
          colonist->setCarrying(carryAmount);
        } else {//Resource exhuasted
          colonist->assignObjective(getObjective());
        }
      }
    //Otherwise, move in target direction
    }else {
      int yChange = getDirection(currentY, targetPosition[0]);
      int xChange = getDirection(currentX, targetPosition[1]);
      if (checkBounds(currentY + yChange, currentX + xChange)){
        colonist->position[0] = currentY + yChange;
        colonist->position[1] = currentX + xChange;
      }
    }
  }
}

void Colony::colonistBirth(House* home){//A new life is brought into this miserable world
  char gender = (roll(50.0)) ? 'M' : 'F';
  Colonist* baby = new Colonist(0, day % AGE_DAYS, gender, home);
  baby->assignObjective(getObjective());//Yes, month old babies have to pull their weight in this colony
  colonists.push_back(baby);
  home->occupants.push_back(baby);
  colonistBirths++;
  allTimeBirths++;
  allTimeColonists++;
}

void Colony::colonistDeath(Colonist* deadColonist, int colonistIndex){//RIP
  //Update home
  Colonist* spouse = deadColonist->getSpouse();
  House* home = deadColonist->getHome();
  if (spouse)
    spouse->setSpouse(NULL);
  if (home->owner == deadColonist || home->inheritor == deadColonist){
    if (home->owner == deadColonist){
      if (home->inheritor){
        home->owner = home->inheritor;
      }else {
        home->owner = NULL;
      }
    }
    home->inheritor = NULL;
  }
  int occupantIndex = -1;
  int currentIndex = 0;
  //Remove dead colonist and pick new inheritor if needed
  for (auto occupant : home->occupants){
    if (occupant){
      if (occupant == deadColonist){
        occupantIndex = currentIndex;
      }else if (occupant != home->owner && !home->inheritor){
        int occupantAge = occupant->getAge();
        home->inheritor = occupant;
      }
    }
    currentIndex++;
  }
  if (occupantIndex != -1)
    home->occupants.erase(home->occupants.begin() + occupantIndex);
  colonists.erase(colonists.begin() + colonistIndex);
  if (colonists.size() <= 0)
    end = true;//Stop program
  
  allTimeDeaths++;
  delete deadColonist;
}

void Colony::consumeFood(int totalHunger){//Calculate food eaten per colonist based on the total amount of hunger of ALL colonists
  if (totalFood > 0 && totalHunger > 0){
    int foodPile = (totalHunger > totalFood) ? totalFood : totalHunger;
    for (auto colonist : colonists){
      int hunger = 100 - colonist->getHunger();
      double consumptionPercent = (double)hunger/(double)totalHunger;
      double consumption = (double)foodPile * consumptionPercent;
      if (totalFood >= (int)consumption){//Double check
        *colonist + (int)consumption;
        totalFood -= consumption;
      }
    }
  }
}

void Colony::attemptMarriage(Colonist* husband, Colonist* wife){//'husband' and 'wife' are not neccesarily accurate representations of gender
  House* newHouse = NULL;
  bool husbandHouse = false;
  bool wifeHouse = false;
  if (husband == husband->getHome()->owner){
    husbandHouse = true;
    newHouse = husband->getHome();
  }else if(wife == wife->getHome()->owner){
    wifeHouse = true;
    newHouse = wife->getHome();
  }else {
    for (auto house : houses){
      if (!house->owner){
        newHouse = house;
        break;
      }
    }
  }
  if (!newHouse && totalWood >= HOUSE_WOOD && totalStone > HOUSE_STONE){
    totalWood -= HOUSE_WOOD;
    totalStone -= HOUSE_STONE;
    newHouse = generateHouse();
  }
  if (newHouse){
    if (!husbandHouse){
      int index = 0;
      House* home = husband->getHome();
      for (auto occupant : home->occupants){
        if (occupant == husband){
          home->occupants.erase(home->occupants.begin() + index);
          break;
        }
        index++;
      }
      husband->setHome(newHouse);
      
      newHouse->occupants.push_back(husband);
    }
    
    if (!wifeHouse){
      int index = 0;
      House* home = wife->getHome();
      for (auto occupant : home->occupants){
        if (occupant == wife){
          home->occupants.erase(home->occupants.begin() + index);
          break;
        }
        index++;
      }
      wife->setHome(newHouse);
      newHouse->occupants.push_back(husband);
    }
    newHouse->owner = (wifeHouse) ? wife: husband;
    newHouse->inheritor = (wifeHouse) ? husband : wife;
    husband->setSpouse(wife);
    wife->setSpouse(husband);
    colonistMarriages++;
    allTimeMarriages++;
  }
}

void Colony::colonistInteraction(){//'Interaction' consists of being in the same square as another colonist
  for (int y = 0; y < GRID_DIMENSIONS; y++){
    for (int x = 0; x < GRID_DIMENSIONS; x++){
      //Get colonists in square
      bool sickness = false;
      vector<Colonist*> contacts;
      for (auto colonist : colonists){
        if(colonist->position[0] == y && colonist->position[1] == x)
          contacts.push_back(colonist);
        if (colonist->getSick() && !colonist->getSickDiscovered())
          sickness = true;
      }
      //Perform interaction
      int currentIndex = 0;
      for (auto contact : contacts){
        //Check every other colonist, no repeats
        if (currentIndex < contacts.size() - 1){
          for(int i = currentIndex + 1; i < contacts.size(); i++){
            Colonist* partner = contacts[i];
            //Check if they can marry
            if ((contact->getGender() != partner->getGender() || GAY_MARRIAGE)
                 && contact->getAge() >= AGE_YOUNG
                 && partner->getAge() >= AGE_YOUNG
                 && !contact->getSpouse()
                 && !partner->getSpouse()
                 && abs(contact->getAge() - partner->getAge()) <= MARRIAGE_AGE_THRESHOLD){
              attemptMarriage(contact, partner);
            }
          }
        }
        //Spread sickness
        if (sickness && !contact->getSick() && contact->getSickStart() < lastOutbreak && hour < TIME_MORNING){//Gotta be awake
          if (roll(SICK_SPREAD_CHANCE)){
            contact->setSick(true);
            contact->setSickStart(day);
          }
        }
        currentIndex++;
      }
    }
  }
}

/*

[----- Handle General Functionality -----]

*/
bool roll(double rate){//Uses a more advanced technique to generate a random number between 0 and 100 and compare that with the chances to succeed
  if (rate <= 0)
    return false;
  double randomNumber = (double)(rand() % 10001);
  return (randomNumber <= rate * 10000.0);//outbreak chance cannot be properly calculated if there are decimals left
}

void Colony::progressNight(){
  day++;
  int dayOfYear = day % AGE_DAYS;
  int totalHunger = 0;
  int currentIndex = 0;
  vector<Colonist*> deadColonists;//Removing from a vector while parsing had some issues so this is a lazy workaround (sorry)
  vector<int> deadIndexs;
  for (auto colonist : colonists){
    //Hunger
    *colonist - DAILY_HUNGER;
    int hunger = colonist->getHunger();
    if (hunger <= 0){
      hungerDeaths++;
      deadColonists.push_back(colonist);
      deadIndexs.push_back(currentIndex);
      currentIndex++;
      continue;
    }else{
      totalHunger += (100 - hunger);
    }
    //Birthday
    if (colonist->getBirthday() == dayOfYear)
      colonist->setAge(colonist->getAge() + 1);
    //Old Age
    if (colonist->getAge() > AGE_OLD){//Sometimes get an error here, because apparently colonist no longer exists
      if(roll(AGE_DEATH_RATE)){        //but why does it allow me to call getBirthday() a few lines up???
        ageDeaths++;
        deadColonists.push_back(colonist);
        deadIndexs.push_back(currentIndex);
        currentIndex++;
        continue;
      }
    }
    //Sickness
    if (colonist->getSick()){
      if (!colonist->getSickDiscovered()){
        if (roll(SICK_DISCOVERY_CHANCE))
          colonist->setSickDiscovered(true);
      }else {
        if(roll(SICK_RECOVER_CHANCE)){
          colonist->setSick(false);
          colonist->setSickDiscovered(false);
        }
      }
      if (day - colonist->getSickStart() > SICK_LETHAL_TIME){
        if (roll(SICK_DEATH_CHANCE)){
          illnessDeaths++;
          deadColonists.push_back(colonist);
          deadIndexs.push_back(currentIndex);
        }
      }
    }
    currentIndex++;
  }
  for (int i = 0; i < deadColonists.size(); i++)
      colonistDeath(deadColonists[i], deadIndexs[i] - i);
  if (!end){
    //Check for births
    for (auto house : houses){
      Colonist* owner = house->owner;
      if (owner){
        int ownerAge = owner->getAge();
        Colonist* spouse = owner->getSpouse();
        if (spouse && owner->getAge() < AGE_OLD && spouse->getAge() < AGE_OLD){//Young enough to have kids
          for (int i = 0; i < 2; i++){
            if (house->position[i] == owner->position[i] && house->position[i] == spouse->position[i]){//Both in house
              double numChildren = -2.0;//Mom and Dad don't count!
              for (auto occupant : house->occupants)
                numChildren++;
              if (roll(BIRTH_RATE - (numChildren * BIRTH_DECREASE_PER_CHILD)))
                colonistBirth(house);
            }
          }
        }
      }
    }
    //Check resources
    if(roll(RESOURCE_SPAWN_RATE * colonists.size())){
      //Get random resource
      int randomIndex = (rand() % RESOURCE_NUMBER < 2) ? 0 : rand() % RESOURCE_NUMBER;//More likely to choose food
      char randomResource = RESOURCE_LIST[randomIndex];
      //Count tiles of resource
      int currentTiles = 0;
      for (auto resource : resources){
        if (resource->type == randomResource) {
          currentTiles++;
        }
      }
      if (currentTiles < RESOURCE_MAX)
        generateResource(randomResource);
    }
    //Check outbreak
    if(roll(SICK_OUTBREAK_CHANCE * ((double)(day - lastOutbreak) * SICK_OUTBREAK_INCREMENT))){
      if (colonists.size() > 0){
        int randomIndex = rand() % colonists.size();
        Colonist* sickBoi = colonists[randomIndex];
        if (!sickBoi->getSick()){//Sickboi is already sick
          sickBoi->setSick(true);
          sickBoi->setSickStart(day);
        }
      }
      lastOutbreak = day;
    }
    consumeFood(totalHunger);
    hour = 0;
  }
}

void Colony::ProgressTime(){
  if (!end){
    hour++;
    lastPrint++;
    if (hour >= TIME_NIGHT)
      progressNight();
    moveColonists();
    colonistInteraction();
  }
}