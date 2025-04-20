
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

  table(string &n, vector<column *> &col) : name(n), columns(col) {
    fstream file(n + ".cols", ios::out);
    file.close();
  }

  void loadColumns() {
    fstream file(name + ".cols", ios::in | ios::out);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".cols file.\n";
      return;
    }

    string line, colName, colType;
    getline(file, line);
    stringstream ss(line);
    ss >> name;

    while (getline(file, line)) {
      ss.clear();
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

  db(string &n) : name(n) {
    fstream file(n + ".tables", ios::out | ios::app);
    file.close();
  }

  void loadTables() {
    fstream file(name + ".tables", ios::in | ios::out);
    if (!file.is_open()) {
      cout << "Error opening " << name << ".tables file.\n";
      return;
    }

    string word, line;
    getline(file, line);
    stringstream ss(line);

    while (ss >> word) {
      fstream file2(word + ".cols", ios::in | ios::out);
      string line2, colName, colType;
      vector<column *> columns;
      stringstream ss2;

      if (!file2.is_open()) {
        cout << "Error opening " << word << ".cols file.\n";
        return;
      }

      getline(file2, line2); // skip table name line
      while (getline(file2, line2)) {
        ss2.clear();
        ss2.str(line2);
        ss2 >> colName >> colType;
        columns.push_back(new column(colName, colType));
      }

      tables.push_back(new table(word, columns));
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
    file << n << " ";
    file.close();

    table *newTable = new table(n, columns);
    tables.push_back(newTable);
  }

  void printTables() {
    for (auto &v : tables) {
      cout << v->name << ":" << endl;
      cout << "---------------------------" << endl;
      for (auto &t : v->columns) {
        cout << t->name << "\t" << t->type << endl;
      }
    }
  }
};

class engine {
private:
  vector<db *> dataBases;

public:
  db *currDb;
  engine() {
    loadDb();
    currDb = nullptr;
    fstream file("engine.meta", ios::out | ios::app);
    file.close();
  }

  void printDb() {
    cout << "count: " << dataBases.size();
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
        cout << "Database with the name " << n << " already exists.\n";
        return;
      }
    }

    fstream file("engine.meta", ios::out | ios::app);
    if (!file.is_open()) {
      cout << "Error opening engine.meta file\n";
      return;
    }

    file << n << " ";
    file.close();

    file.open(n + ".tables", ios::out);
    if (!file.is_open()) {
      cout << "Error creating " << n << ".tables file\n";
      return;
    }
    file.close();

    db *Db = new db(n);
    dataBases.push_back(Db);
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
};

#endif // _ENGINE_H
