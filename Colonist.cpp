/*
 * Name: Thomas Jervey
 * Email: [Redacted]
 * Date: 12/10/2019
 * Function: Contains the basic functionality of a Colonist that is just trying to make a living.
*/

#include <cstdlib>
#include "Colony.h"

using namespace std;

//Constructors
Colonist::Colonist(int startingAge, int birthDay, char givenGender, House* newHome){
  age = startingAge;
  birthday = birthDay;
  carrying = 0;
  hunger = 100;
  sickStart = 0;
  married = false;
  sick = false;
  sickDiscovered = false;
  gender = givenGender;
  home = newHome;
  spouse = NULL;
  objective = NULL;
  position[0] = newHome->position[0];
  position[1] = newHome->position[1];
}

//Methods
void Colonist::setAge(int newAge){
  age = newAge;
}

void Colonist::setCarrying(int carryAmount){
  carrying = carryAmount;
}

void Colonist::setSickStart(int start){
  sickStart = start;
}

void Colonist::changeHunger(int increment){
  hunger += increment;
}

void Colonist::setSick(bool isSick){
  sick = isSick;
}

void Colonist::setSickDiscovered(bool isDiscovered){
  sickDiscovered = isDiscovered;
}

void Colonist::setSpouse(Colonist* newSpouse){
  married = true;
  spouse = newSpouse;
}

void Colonist::removeSpouse(){
  married = false;
  spouse = NULL;
}

void Colonist::setHome(House* newHome){
  home = newHome;
}

void Colonist::assignObjective(Resource* newObjective){
  objective = newObjective;
  if(newObjective)
    objectiveType = newObjective->type;
}

void Colonist::setPosition(int y, int x){
  position[0] = y;
  position[1] = x;
}

int Colonist::getAge(){
  return age;
}

int Colonist::getBirthday(){
  return birthday;
}

int Colonist::getCarrying(){
  return carrying;
}

int Colonist::getHunger(){
  return hunger;
}

int Colonist::getSickStart(){
  return sickStart;
}

bool Colonist::getMarried(){
  return married;
}

bool Colonist::getSick(){
  return sick;
}

bool Colonist::getSickDiscovered(){
  return sickDiscovered;
}

char Colonist::getObjectiveType(){
  return objectiveType;
}

char Colonist::getGender(){
  return gender;
}

Colonist* Colonist::getSpouse(){
  if(spouse)
    return spouse;
  else
    return NULL;
}

House* Colonist::getHome(){
  return home;
}

Resource* Colonist::getObjective(){
  if(objective)
    return objective;
  else
    return NULL;
}

Colonist& Colonist::operator-(int increment){
  hunger -= increment;
  return *this;
}

Colonist& Colonist::operator+(int increment){
  hunger += increment;
  return *this;
}
