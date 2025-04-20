#ifndef _ENGINE_H
#define _ENGINE_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

class column {
private:
  string name;
  string type;

public:
  column(string n, string t) : name(n), type(t) {}
};

class table {
public:
  string name;
  vector<column*> columns;
  table(string n) : name(n) {}
  void loadColumns(){
    fstream file(name+".cols",ios::in|ios::out);
    if(!file.is_open()){
      cout<<"Error opening "<<name<<".cols file.\n";
      return;
    }
    string line;
    string colName,colType;
    getline(file,line);
    stringstream ss(line);
    ss>>name;
    while(getline(file,line)){
      ss.str(line);
      ss>>colName>>colType;
      columns.push_back(new column(colName,colType));
    }

  }
};

class db {

public:
  vector<table *> tables;
  string name;
  db(string n) : name(n) {}

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
      tables.push_back(new table(word));
    }
  }
  void createTable(string n, vector<column*> columns) {
    for (auto &v : tables) {
      if (v->name == n) {
        cout << "Table with the name " << n << " already exists.\n";
        return;
      }
    }
    table* newTable = new table(n);
    newTable->columns = columns;
    tables.push_back(newTable);
    return;
  }
};

class engine {
private:
  vector<db *> dataBases;

public:
  engine() { loadDb(); }

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

  db *selectDb(string n) {
    for (auto &v : dataBases) {
      if (v->name == n)
        return v;
    }
    cout << "Database " << n << " not found.\n";
    return nullptr;
  }
};

#endif // !_ENGINE_H
