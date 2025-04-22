
#include "engine.hpp"

int main() {
  engine eng;

  cout << "=== Existing Databases ===\n";
  eng.printDb();

  string dbName = "college",db2="users";
  cout << "\n=== Creating Database: " << dbName << " ===\n";
  eng.createDb(dbName);
  cout << "\n=== Creating Database: " << db2 << " ===\n";
  eng.createDb(db2);
  eng.selectDb(dbName);

  cout << "=== Created Databases ===\n";
  eng.printDb();
  // Switch to the new database
  eng.selectDb(dbName);
  cout << "\n=== Using Database: " << dbName << " ===\n";

  // Define columns
  vector<column *> cols;
  cols.push_back(new column("id", "int"));
  cols.push_back(new column("name", "string"));
  cols.push_back(new column("cgpa", "float"));

  cout << "\n=== Creating Table: students ===\n";
  eng.createTable("students", cols);
  eng.createTable("teachers", cols);
  eng.createTable("hod",cols);

  // cout << "\n=== Tables in " << dbName << " ===\n";


  return 0;
}
