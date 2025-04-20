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
};

class table {
public:
  string name;
  vector<column *> columns;
  table(string &n, vector<column *> &col) : name(n), columns(col) {}
  void loadColumns() {
    fstream file(name + ".cols", ios::in | ios::out);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".cols file.\n";
      return;
    }
    string line;
    string colName, colType;
    getline(file, line);
    stringstream ss(line);
    ss >> name;
    while (getline(file, line)) {
      ss.str(line);
      ss >> colName >> colType;
      columns.push_back(new column(colName, colType));
    }
  }
};

class db {

public:
  vector<table *> tables;
  string name;
  db(string &n) : name(n) {}

  void loadTables() {
    fstream file(name + ".tables", ios::in | ios::out);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".tables file.\n";
      return;
    }
    string word;
    string line;
    getline(file, line);
    stringstream ss(line);
    while (ss >> word) {
      fstream file2(word + ".cols", ios::in | ios::out);
      string line2;
      stringstream ss2;
      string colName, colType;
      vector<column *> columns;
      if (!file2.is_open()) {
        cout << "Error opening " << word << ".cols file.\n";
        return;
      }
      getline(file2, line2);
      while (getline(file2, line2)) {
        ss2.str(line2);
        ss2 >> colName >> colType;
        columns.push_back(new column(colName, colType));
      }
      tables.push_back(new table(word, columns));
    }
    return;
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
    file << n << " " << reinterpret_cast<char *>(columns.size()) << "endl";
    for (auto &col : columns) {
      file << col->name << " " << col->type << endl;
    }
    file.close();
    file.open(name + ".tables", ios::in | ios::out);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".tables file.\n";
      return;
    }
    file << n << " ";
    file.close();
    table *newTable = new table(n, columns);
    tables.push_back(newTable);
    return;
  }
};

class engine {
private:
  vector<db *> dataBases;
  db* currDb;

public:
  engine() { 
    loadDb();
    currDb=nullptr;
  }

  void printDb() {
    cout << "count:" << dataBases.size();
    for (auto &v : dataBases) {
      cout << endl << v->name;
    }
  }

  void loadDb() {
    fstream file("engine.meta", ios::in);
    if (!file.is_open()) {
      cout << "Error opening engine.meta file\n";
      return;
    }
    string line, word;
    getline(file, line);
    stringstream ss(line);
    while (ss >> word) {
      dataBases.push_back(new db(word));
    }
  }
  void createDb(string n) {
    for (auto &v : dataBases) {
      if (v->name == n) {
        cout << "Database with the name " << n << "already exists \n";
        return;
      }
    }
    fstream file;
    file.open("engine.meta", ios::out);
    if (!file.is_open()) {
      cout << "Error opening engine.meta  file \n";
      return;
    }
    file << n << " ";
    file.close();
    file.open(n + ".db", ios::out);
    if (!file.is_open()) {
      cout << "Error creating " << n << ".db file\n";
      return;
    }
    file.close();
    db *Db = new db(n);
    dataBases.push_back(Db);
  }

  void selectDb(string n) {
    for (auto &v : dataBases) {
      if (v->name == n){
        currDb = v;
        cout<<"Database "<<n<<" selected.\n";
        return;
      }
    }
    cout << "Database " << n << " not found.\n";
    return;
  }
};

#endif // !_ENGINE_H
