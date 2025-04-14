#include "unordered_map"
#include "algorithm"
#include <cstring>
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

struct tableInfo2 {
  int columnCount;
  vector<pair<string, columnInfo>> columns;
};

class dataBase {
private:
  string name;
  fstream dbFile;
  unordered_map<string, tableInfo> tables;
  unordered_map<string, tableInfo2> tables2;

public:
  dataBase(string n) : name(n) {
    dbFile.open(name + ".meta", ios::out);
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
      tableInfo2 tinfo2;
      tinfo.columnCount = stoi(columnCountStr);
      tinfo2.columnCount = stoi(columnCountStr);

      string colLine;
      int i = 0;
      while (getline(metaFile, colLine)) {
        stringstream iss(colLine);
        string colName, colType, sizeStr;
        iss >> colName >> colType >> sizeStr;

        int colSize = stoi(sizeStr);
        tinfo.columns[colName] = {colType, colSize};
        tinfo2.columns[i] = {colName, {colType, colSize}};
        i++;
      }

      tables[tableName] = tinfo;
      tables2[tableName] = tinfo2;
      metaFile.close();
    }

    dbFile.clear(); // reset flags in case of EOF
    dbFile.seekg(0);
  }

  void createTable(string tableName, vector<pair<string, columnInfo>> columns) {
    // 1. Check if table already exists
    unordered_map<string, columnInfo> columnFortables;
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
      columnFortables[col.first] = col.second;
      metaFile << col.first << " " << col.second.dataType << " "
               << to_string(col.second.size) << "\n";
    }

    metaFile.close();

    // 5. Add table to in-memory tables map
    tableInfo tinfo;
    tableInfo2 tinfo2;
    tinfo2.columnCount = columns.size();
    tinfo2.columns = columns;
    tinfo.columnCount = columns.size();
    tinfo.columns = columnFortables;
    tables[tableName] = tinfo;
    tables2[tableName] = tinfo2;

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

    tableInfo2 &tinfo2 = tables2[tableName];

    // 2. Check if value count matches column count
    if (values.size() != tinfo2.columnCount) {
      cerr << "Expected " << tinfo2.columnCount << " values, but got "
           << values.size() << ".\n";
      return;
    }

    // 4. Validate each value
    for (int i = 0; i < values.size(); ++i) {
      string value = values[i];
      string colName = tinfo2.columns[i].first;
      columnInfo &colInfo = tinfo2.columns[i].second;

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

  void insert(string tableName, const vector<string> &values) {
    fstream metaFile(tableName + ".tbl", ios::out | ios::binary | ios::app);
    if (!metaFile.is_open()) {
      cout << endl << "Error opening table file for insertion" << endl;
      metaFile.close();
      return;
    }
    char flag = 1;
    metaFile.write(&flag, 1);

    for (int i = 0; i < values.size(); i++) {
      string padded =
          values[i].substr(0, tables2[tableName].columns[i].second.size);
      padded.resize(tables2[tableName].columns[i].second.size, '\0');
      metaFile.write(padded.c_str(), tables2[tableName].columns[i].second.size);
    }
    metaFile.close();
    return;
  }
  void selectAll(const string &tableName) {
    if (tables2.find(tableName) == tables2.end()) {
      cout << "Table not found\n";
      return;
    }

    tableInfo2 &table = tables2[tableName];
    const auto &columns = table.columns;

    // Calculate size of each row (excluding the deletion flag)
    int rowSize = 0;
    for (const auto &[_, col] : columns)
      rowSize += col.size;

    fstream file(tableName + ".tbl", ios::in | ios::binary);
    if (!file) {
      cout << "Failed to open file\n";
      return;
    }

    // Print column headers
    for (const auto &[colName, _] : columns)
      cout << colName << "\t";
    cout << "\n";

    while (true) {
      char flag;
      file.read(&flag, 1); // read the deletion flag
      if (file.eof())
        break;

      vector<char> rowBuffer(rowSize);
      file.read(rowBuffer.data(), rowSize);
      if (file.gcount() < rowSize)
        break;

      if (flag != 1)
        continue; // skip if marked as deleted

      int offset = 0;
      for (const auto &[colName, colInfo] : columns) {
        string rawData(rowBuffer.data() + offset, colInfo.size);

        if (colInfo.dataType == "int") {
          int value;
          memcpy(&value, rawData.data(), sizeof(int));
          cout << value << "\t";
        } else if (colInfo.dataType == "bool") {
          bool b = (rawData[0] == '1' || rawData[0] == 't');
          cout << (b ? "true" : "false") << "\t";
        } else if (colInfo.dataType == "string") {
          rawData.erase(find(rawData.begin(), rawData.end(), '\0'),
                        rawData.end());
          cout << rawData << "\t";
        }

        offset += colInfo.size;
      }
      cout << "\n";
    }

    file.close();
  }
};

int main() {
  // Create database
  dataBase db("myDB");

  // Load tables from the main metadata file (if any)
  db.loadTables();

  // Define columns for a new table
  vector<pair<string, columnInfo>> columns;
  columns.push_back({"active", {"bool", 5}});
  columns.push_back({"id", {"int", 3}});
  columns.push_back({"name", {"string", 10}});
  columns.push_back({"age", {"int", 3}});

  vector<pair<string, columnInfo>> column2;
  column2.push_back({"sno", {"int", 3}});
  column2.push_back({"name", {"string", 15}});
  column2.push_back({"roll_no.", {"int", 4}});
  column2.push_back({"degree", {"string", 10}});

  // Create a table called "users"
  db.createTable("users", columns);
  db.createTable("college_details", column2);

  vector<string> row2 = {"001", "Lucy", "5042", "B.Tech_IT"};
  // Insert a valid row into the "users" table
  vector<string> row1 = {"false", "101", "alice", "20"};
  db.insertIntoTable("users", row1);
  db.insertIntoTable("college_details", row2);
  db.selectAll("users");
  cout<<endl;
  db.selectAll("college_details");

  return 0;
}
