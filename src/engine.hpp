
#ifndef _ENGINE_H
#define _ENGINE_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

class column {
public:
  string name;
  string type;
  column(string n, string t) : name(n), type(t) {}
  ~column() = default;
};

class table {
public:
  string name;
  vector<column *> columns;

  table(string &n, vector<column *> &col) : name(n), columns(col) {
    fstream file(n + ".cols", ios::out); // clears existing file
    file << name << " " << columns.size() << endl;
    for (auto &c : columns) {
      file << c->name << " " << c->type << endl;
    }
    file.close();
  }

  void loadColumns() {
    fstream file(name + ".cols", ios::in);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".cols file.\n";
      return;
    }

    string line;
    getline(file, line); // skip first line (table name + size)
    while (getline(file, line)) {
      string colName, colType;
      stringstream ss(line);
      ss >> colName >> colType;
      columns.push_back(new column(colName, colType));
    }
  }

  ~table() {
    for (auto c : columns)
      delete c;
  }
};

class db {
public:
  vector<table *> tables;
  string name;

  db(string &n) : name(n) {}

  void loadTables() {
    fstream file(name + ".tables", ios::in);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".tables file.\n";
      return;
    }

    string tableName;
    while (getline(file, tableName)) {
      fstream file2(tableName + ".cols", ios::in);
      if (!file2.is_open()) {
        cout << "Error opening " << tableName << ".cols file.\n";
        continue;
      }

      string firstLine;
      getline(file2, firstLine); // skip table name + column count
      string line;
      vector<column *> cols;
      while (getline(file2, line)) {
        string colName, colType;
        stringstream ss(line);
        ss >> colName >> colType;
        cols.push_back(new column(colName, colType));
      }

      tables.push_back(new table(tableName, cols));
    }
  }

  void createTable(string n, vector<column *> columns) {
    for (auto &v : tables) {
      if (v->name == n) {
        cout << "Table with the name " << n << " already exists.\n";
        return;
      }
    }

    fstream file(n + ".cols", ios::out);
    if (!file.is_open()) {
      cout << "Error opening file " << n << ".cols\n";
      return;
    }

    file << n << " " << columns.size() << endl;
    for (auto &col : columns) {
      file << col->name << " " << col->type << endl;
    }
    file.close();

    file.open(name + ".tables", ios::app);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".tables file.\n";
      return;
    }
    file << n << endl;
    file.close();

    table *newTable = new table(n, columns);
    tables.push_back(newTable);
  }

  void printTables() {
    for (auto &v : tables) {
      cout << v->name << ":\n---------------------------\n";
      for (auto &t : v->columns) {
        cout << t->name << "\t" << t->type << endl;
      }
    }
  }

  ~db() {
    for (auto t : tables)
      delete t;
  }
};

class engine {
private:
  vector<db *> dataBases;
  db *currDb;

public:
  engine() {
    loadDb();
    currDb = nullptr;
  }

  void printDb() {
    cout << "Databases (" << dataBases.size() << "):";
    for (auto &v : dataBases) {
      cout << "\n- " << v->name;
    }
    cout << endl;
  }

  void loadDb() {
    fstream file("engine.meta", ios::in);
    if (!file.is_open()) {
      cout << "Error opening engine.meta file\n";
      return;
    }

    string dbName;
    while (getline(file, dbName)) {
      db *newDb = new db(dbName);
      newDb->loadTables();
      dataBases.push_back(newDb);
    }
    file.close();
  }

  void createDb(string n) {
    for (auto &v : dataBases) {
      if (v->name == n) {
        cout << "Database with the name " << n << " already exists.\n";
        return;
      }
    }

    fstream file("engine.meta", ios::app);
    if (!file.is_open()) {
      cout << "Error opening engine.meta file\n";
      return;
    }

    file << n << endl;
    file.close();

    fstream tablesFile(n + ".tables", ios::out);
    if (!tablesFile.is_open()) {
      cout << "Error creating " << n << ".tables file\n";
      return;
    }
    tablesFile.close();

    db *Db = new db(n);
    dataBases.push_back(Db);
    cout << "Database " << n << " created.\n";
  }

  void selectDb(string n) {
    for (auto &v : dataBases) {
      if (v->name == n) {
        currDb = v;
        cout << "Database " << n << " selected.\n";
        return;
      }
    }
    cout << "Database " << n << " not found.\n";
  }

  db *getCurrentDb() {
    return currDb;
  }

  void createTable(string n,vector<column*> columns){
    if(currDb){
      return currDb->createTable(n,columns);
    }
  }

  void printTables(string n){
    if(currDb)
      return currDb->printTables();
  }

  ~engine() {
    for (auto dbPtr : dataBases)
      delete dbPtr;
  }
};

#endif
