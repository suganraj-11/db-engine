#include "fstream"
#include <array>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class dataBase {
public:
  string dbName;
  vector<string> tables;

  dataBase(string n) : dbName(n) {
    fstream file(dbName+".tables",ios::out);
    file.close();
    loadTables();
  }

  //Load the table names into the tables vector 
  void loadTables() {
    string fileName = dbName + ".tables";
    fstream file(fileName, ios::in | ios::out);
    if (file) {
      string line;
      while (getline(file, line, ',')) {
        tables.push_back(line);
      }
      return;
    }
    cout << "Error opening file" << endl;
  }

  void createTable(string tableName, vector<array<string, 3>> columns) {
    string fileName = tableName + ".meta";
    fstream file(fileName, ios::in);
    if (file) {
      cout << "Table with the name " << tableName << " already exists" << endl;
      file.close();
      return;
    }
    file.close();
    file.open(fileName, ios::out);
    if (!file) {
      cout << endl << "Error creating file" << endl;
      file.close();
      return;
    }
    string fileName2=dbName+".tables";
    fstream file2(fileName2,ios::out);
    if(file2<<tableName<<","){
      tables.push_back(tableName);
    }
    file << tableName << endl;
    for (auto v : columns) {
      file << v[0] << "," << v[1] << "," << v[2] << endl;
    }
    cout << "Table created successfully" << endl;
    file.close();

    return;
  }

  void getTables() {
    cout << "Tables:" << endl;
    int i=1;
    for (string table : tables) {
      cout <<i<<":"<< table << endl;
    }
    return;
  }
};

int main() {
  string tableName = "table1";
  vector<array<string, 3>> columns = {{"sno", "int", "4"},
                                      {"name", "string", "15"},
                                      {"phone number", "int", "10"}};
  dataBase db1("dbTest");
  db1.createTable(tableName, columns);
  db1.getTables();

  string tableName2 = "table2";
  vector<array<string, 3>> columns2 = {{"sno2", "int", "4"},
                                      {"name2", "string", "15"},
                                      {"phone number2", "int", "10"}};
  dataBase db2("dbTest2");
  db2.createTable(tableName2, columns2);
  db2.getTables();

  return 0;
}
