#include <iostream>
#include "../src/engine.hpp"
using namespace std;



int main(){
  engine eng;
  eng.createDb("Database_1");
  eng.createDb("Database_2");
  eng.printDb();
  eng.selectDb("Database_1");

}
