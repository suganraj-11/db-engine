#include "unordered_map"
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <unordered_map>
#include <vector>
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
    dbFile.open(name+".meta",ios::out);
    dbFile.close();
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

  void createTable(string tableName,
                   unordered_map<string, columnInfo> columns) {
    // 1. Check if table already exists
    if (tables.find(tableName) != tables.end()) {
      cerr << "Table '" << tableName << "' already exists.\n";
      return;
    }

    // 2. Create metadata file
    fstream metaFile(tableName + ".meta", ios::out);
    if (!metaFile.is_open()) {
      cerr << "Failed to create metadata file for table: " << tableName << endl;
      return;
    }

    // 3. Write table header: tableName columnCount (as string)
    metaFile << tableName << " " << to_string(columns.size()) << "\n";

    // 4. Write each column's metadata: colName dataType size (as string)
    for (const auto &col : columns) {
      metaFile << col.first << " " << col.second.dataType << " "
               << to_string(col.second.size) << "\n";
    }

    metaFile.close();

    // 5. Add table to in-memory tables map
    tableInfo tinfo;
    tinfo.columnCount = columns.size();
    tinfo.columns = columns;
    tables[tableName] = tinfo;

    // 6. Append this table name to the main DB metadata file
    dbFile.clear();            // clear EOF flag if set
    dbFile.seekp(0, ios::end); // move to end for appending
    dbFile << tableName << ",";
  }

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
        if (!(value == "true" || value == "false")) {
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
    insert(tableName, values);
  }

  void insert(const string &tableName, const vector<string> &values) {
    if (tables.find(tableName) == tables.end()) {
      cerr << "Table '" << tableName << "' not found.\n";
      return;
    }

    const tableInfo &tinfo = tables[tableName];

    // Open .tbl file in binary append mode
    fstream dataFile(tableName + ".tbl", ios::out | ios::binary | ios::app);
    if (!dataFile.is_open()) {
      cerr << "Failed to open .tbl file for table: " << tableName << endl;
      return;
    }

    // Preserve column order
    vector<pair<string, columnInfo>> orderedColumns;
    for (const auto &col : tinfo.columns) {
      orderedColumns.push_back(col);
    }

    for (size_t i = 0; i < values.size(); ++i) {
      const string &val = values[i];
      const columnInfo &col = orderedColumns[i].second;

      // Pad/truncate the value to the defined column size
      string padded = val.substr(0, col.size);
      padded.resize(col.size, '\0'); // pad with null characters

      // Write to binary file
      dataFile.write(padded.c_str(), col.size);
    }

    dataFile.close();
  }
};




int main() {
  // Create database
  dataBase db("myDB");

  // Load tables from the main metadata file (if any)
  db.loadTables();

  // Define columns for a new table
  unordered_map<string, columnInfo> columns;
  columns["id"] = {"int", 3};          // max value = 999
  columns["name"] = {"string", 10};    // up to 10 characters
  columns["active"] = {"bool", 5};     // "true"/"false" or "0"/"1"

  // Create a table called "users"
  db.createTable("users", columns);

  // Insert a valid row into the "users" table
  vector<string> row1 = {"101", "alice", "true"};
  db.insertIntoTable("users", row1);

  return 0;
}
