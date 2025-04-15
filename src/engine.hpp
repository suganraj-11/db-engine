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
  int size;

public:
  column(string n, string t, int s) : name(n), type(t), size(s) {}
};

class table {
private:
  string name;
  vector<column> columns;

public:
};

class db {

public:
  string name;
  db(string n) : name(n) {}
};

class engine {
private:
  vector<db *> dataBases;

public:
  engine() {
    loadDb();
  }

  void printDb(){
    cout<<"count:"<<dataBases.size();
    for(auto &v:dataBases){
      cout<<endl<<v->name;
    }
  }

  void loadDb() {
    fstream file("engine.meta", ios::in);
    if(!file.is_open()){
      cout<<"Error opening engine.meta file\n";
      return;
    }
    string line, word;
    getline(file, line);
    stringstream ss(line);
    while (ss>>word) {
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
};

#endif // !_ENGINE_H
