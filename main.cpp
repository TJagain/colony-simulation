/*
 * Name: Thomas Jervey
 * Email: [Redacted]
 * Date: 12/10/2019
 * Function: Runs the simulation with the designated parameters.
*/

#include <cstdlib>
#include "Colony.h"

using namespace std;

//Variables
const int DAY = 24;
const int MONTH = 24 * 30;
const int YEAR = 24 * 30 * 12;
const int DECADE = 24 * 30 * 12 * 10;
const int CENTURY = 24 * 30 * 12 * 100;

int main (int argc, char **argv)
{ 
  int simulationLength = YEAR * 2;//Put how long you want the simulation to run!
  int sumulationIncrement = MONTH;//Put how often you want simulation updates to print out!
  
	//Run Simulation
  Colony* simulation = new Colony;
  simulation->PrintGrid();
  for (int hour = 1; hour <= simulationLength && !simulation->end; hour++){
    simulation->ProgressTime();
    if (hour % sumulationIncrement == 0 && hour < simulationLength){
      simulation->UpdateGrid();
      simulation->PrintGrid();
    }
  }
  simulation->end = true;
  simulation->UpdateGrid();
  simulation->PrintGrid();
  
 	delete simulation;
   
   
	return 0; 
}