
#include "engine.hpp"

int main() {
  engine e;

  // Step 1: Create a database
  e.createDb("testdb");
  e.selectDb("testdb");

  // Step 2: Define some columns
  vector<column *> cols;
  cols.push_back(new column("id", "int"));
  cols.push_back(new column("name", "string"));
  cols.push_back(new column("age", "int"));

  // Step 3: Create a table
  e.currDb->createTable("students", cols);

  // Step 4: Print tables
  e.currDb->printTables();

  // Step 5: Reload engine and load tables
  engine e2;
  e2.selectDb("testdb");
  e2.currDb->loadTables();

  cout << "\nLoaded tables from file:\n";
  e2.currDb->printTables();

  return 0;
}
