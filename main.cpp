#include "unordered_map"
#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
using namespace std;

struct columnInfo {
  string dataType;
  int size;
};

struct tableInfo {
  int columnCount;
  unordered_map<string, columnInfo> columns;
};

class dataBase {
private:
  string name;
  fstream dbFile;
  unordered_map<string, tableInfo> tables;

public:
  dataBase(string n) : name(n) {
    dbFile.open(name + ".meta", ios::in | ios::out);
  }

  void loadTables() {
    if (!dbFile.is_open())
      return;

    // Read the entire table list (comma-separated)
    string tablesLine;
    getline(dbFile, tablesLine);
    stringstream tableNamesStream(tablesLine);
    string tableName;

    while (getline(tableNamesStream, tableName, ',')) {
      if (tableName.empty())
        continue;

      fstream metaFile(tableName + ".meta", ios::in);
      if (!metaFile.is_open()) {
        cerr << "Failed to open metadata file for table: " << tableName << endl;
        continue;
      }

      string headerLine;
      getline(metaFile, headerLine);
      stringstream metaHeader(headerLine);

      string nameFromFile, columnCountStr;
      metaHeader >> nameFromFile >> columnCountStr;

      tableInfo tinfo;
      tinfo.columnCount = stoi(columnCountStr);

      string colLine;
      while (getline(metaFile, colLine)) {
        stringstream iss(colLine);
        string colName, colType, sizeStr;
        iss >> colName >> colType >> sizeStr;

        int colSize = stoi(sizeStr);
        tinfo.columns[colName] = {colType, colSize};
      }

      tables[tableName] = tinfo;
      metaFile.close();
    }

    dbFile.clear(); // reset flags in case of EOF
    dbFile.seekg(0);
  }
  void createTable(string tableName,unordered_map<string,columnInfo> columns){}

  void insertIntoTable(string tableName, const vector<string> &values) {
    // 1. Check if the table exists
    if (tables.find(tableName) == tables.end()) {
      cerr << "Table " << tableName << " does not exist.\n";
      return;
    }

    tableInfo &tinfo = tables[tableName];

    // 2. Check if value count matches column count
    if (values.size() != tinfo.columnCount) {
      cerr << "Expected " << tinfo.columnCount << " values, but got "
           << values.size() << ".\n";
      return;
    }

    // 3. Get column info in insertion order
    vector<pair<string, columnInfo>> columnList;
    for (const auto &col : tinfo.columns) {
      columnList.push_back(col);
    }

    // 4. Validate each value
    for (int i = 0; i < values.size(); ++i) {
      string value = values[i];
      string colName = columnList[i].first;
      columnInfo &colInfo = columnList[i].second;

      // Type check
      if (colInfo.dataType == "int") {
        for (char c : value) {
          if (!isdigit(c)) {
            cerr << "Invalid int value for column " << colName << ": " << value
                 << "\n";
            return;
          }
        }
      } else if (colInfo.dataType == "bool") {
        if (!(value == "true" || value == "false" || value == "0" ||
              value == "1")) {
          cerr << "Invalid bool value for column " << colName << ": " << value
               << "\n";
          return;
        }
      } else if (colInfo.dataType == "string") {
        // strings are allowed anything, so no check
      } else {
        cerr << "Unknown datatype " << colInfo.dataType << " for column "
             << colName << "\n";
        return;
      }

      // Size check
      if (value.size() > colInfo.size) {
        cerr << "Value '" << value << "' exceeds size limit (" << colInfo.size
             << ") for column " << colName << "\n";
        return;
      }

      // For int types, check numeric size (e.g., 99 for size = 2)
      if (colInfo.dataType == "int") {
        int numeric = stoi(value);
        int maxVal = pow(10, colInfo.size) - 1;
        if (numeric > maxVal) {
          cerr << "Integer value " << numeric << " exceeds max allowed "
               << maxVal << " for column " << colName << "\n";
          return;
        }
      }
    }

    // All checks passed, insert the data
    // insert(tableName, values);
  }

};
