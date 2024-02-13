/*
 * Name: Thomas Jervey
 * Email: [Redacted]
 * Date: 12/10/2019
 * Function: Declare all functions, variables, overloadings, and whatever else may be needed to make a cutting-edge Colonist object,
 *           capable of a INCREDIBLY vast number of different functions - with only the best and newest features!
 * Notes: sorry i'm really tired
*/

using namespace std; 

struct House;
struct Resource;

class Colonist {
  public:
    //Constructor
    Colonist(int, int, char, House*);
    //Varaiables
    int position[2];
    //Methods
    void setAge(int);
    void setCarrying(int);
    void changeHunger(int);
    void setSickStart(int);
    void setSick(bool);
    void setSickDiscovered(bool);
    void setSpouse(Colonist*);
    void removeSpouse();
    void setHome(House*);
    void assignObjective(Resource*);
    void setPosition(int, int);
    int getAge();
    int getBirthday();
    int getCarrying();
    int getHunger();
    int getSickStart();
    bool getMarried();
    bool getSick();
    bool getSickDiscovered();
    char getObjectiveType();
    char getGender();
    Colonist* getSpouse();
    House* getHome();
    Resource* getObjective();
    Colonist& operator-(int);
    Colonist& operator+(int);
  private:
    //Variables
    int age;
    int birthday;
    int carrying;
    int hunger;
    int sickStart;
    bool married;
    bool sick;
    bool sickDiscovered;
    char objectiveType;
    char gender;
    Colonist* spouse;
    House* home;
    Resource* objective;
};