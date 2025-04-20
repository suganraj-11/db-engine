#include <iostream>
#include "../src/engine.hpp"
using namespace std;



int main(){
  cout<<"Start of the Program\n";
  engine eng;
  eng.createDb("Database_1");
  eng.printDb();
  eng.selectDb("Database_1");
  vector<column*> columns; 
  columns.push_back(new column("id","int"));
  columns.push_back(new column("name","string"));
  columns.push_back(new column("active","bool"));

  // {{"id","int"},{"name","string"},{"active","bool"}};
  eng.create("users", columns);
  eng.create("courses", columns);
  return 0;

}
