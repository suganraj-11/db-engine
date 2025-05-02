
#include <iostream>
#include <fstream>
#include <cstdint>
using namespace std;

const int NODE_SIZE = 20;
const int PAGE_SIZE = 1024;
const char* FILENAME = "avltree.dat";

struct Node {
    int key;
    int value;
    int left_offset;
    int right_offset;
    int height;

    Node(int k = 0, int v = 0) : key(k), value(v), left_offset(-1), right_offset(-1), height(1) {}
};

int readRoot(fstream &file) {
    file.seekg(0);
    int root;
    file.read((char*)&root, sizeof(int));
    return root;
}

void writeRoot(fstream &file, int offset) {
    file.seekp(0);
    file.write((char*)&offset, sizeof(int));
}

void writeNode(fstream &file, int offset, const Node &node) {
    file.seekp(offset);
    file.write((char*)&node, sizeof(Node));
}

Node readNode(fstream &file, int offset) {
    Node node;
    file.seekg(offset);
    file.read((char*)&node, sizeof(Node));
    return node;
}

int height(fstream &file, int offset) {
    if (offset == -1) return 0;
    return readNode(file, offset).height;
}

void updateHeight(fstream &file, int offset) {
    Node node = readNode(file, offset);
    node.height = 1 + max(height(file, node.left_offset), height(file, node.right_offset));
    writeNode(file, offset, node);
}

int getBalance(fstream &file, int offset) {
    if (offset == -1) return 0;
    Node node = readNode(file, offset);
    return height(file, node.left_offset) - height(file, node.right_offset);
}

int rotateRight(fstream &file, int y_offset) {
    Node y = readNode(file, y_offset);
    Node x = readNode(file, y.left_offset);
    int x_offset = y.left_offset;

    y.left_offset = x.right_offset;
    x.right_offset = y_offset;

    writeNode(file, y_offset, y);
    writeNode(file, x_offset, x);
    updateHeight(file, y_offset);
    updateHeight(file, x_offset);
    return x_offset;
}

int rotateLeft(fstream &file, int x_offset) {
    Node x = readNode(file, x_offset);
    Node y = readNode(file, x.right_offset);
    int y_offset = x.right_offset;

    x.right_offset = y.left_offset;
    y.left_offset = x_offset;

    writeNode(file, x_offset, x);
    writeNode(file, y_offset, y);
    updateHeight(file, x_offset);
    updateHeight(file, y_offset);
    return y_offset;
}

int insert(fstream &file, int offset, int key, int value, int &node_count) {
    if (offset == -1) {
        int new_offset = sizeof(int) + NODE_SIZE * node_count++;
        Node node(key, value);
        writeNode(file, new_offset, node);
        return new_offset;
    }

    Node node = readNode(file, offset);

    if (key < node.key)
        node.left_offset = insert(file, node.left_offset, key, value, node_count);
    else if (key > node.key)
        node.right_offset = insert(file, node.right_offset, key, value, node_count);
    else
        return offset; // duplicate

    writeNode(file, offset, node);
    updateHeight(file, offset);

    int balance = getBalance(file, offset);

    if (balance > 1 && key < readNode(file, node.left_offset).key)
        return rotateRight(file, offset);
    if (balance < -1 && key > readNode(file, node.right_offset).key)
        return rotateLeft(file, offset);
    if (balance > 1 && key > readNode(file, node.left_offset).key) {
        node.left_offset = rotateLeft(file, node.left_offset);
        writeNode(file, offset, node);
        return rotateRight(file, offset);
    }
    if (balance < -1 && key < readNode(file, node.right_offset).key) {
        node.right_offset = rotateRight(file, node.right_offset);
        writeNode(file, offset, node);
        return rotateLeft(file, offset);
    }

    return offset;
}

int search(fstream &file, int offset, int key) {
    if (offset == -1) return -1;
    Node node = readNode(file, offset);
    if (key == node.key) return node.value;
    if (key < node.key) return search(file, node.left_offset, key);
    return search(file, node.right_offset, key);
}

int main() {
    fstream file(FILENAME, ios::in | ios::out | ios::binary);

    if (!file) {
        file.open(FILENAME, ios::out | ios::binary);
        int root = -1;
        file.write((char*)&root, sizeof(int));
        file.close();
        file.open(FILENAME, ios::in | ios::out | ios::binary);
    }

    int root_offset = readRoot(file);
    file.seekg(0, ios::end);
    int node_count = (static_cast<std::streamoff>(file.tellg()) - sizeof(int)) / NODE_SIZE;

    int choice;
    while (true) {
        cout << "\nMenu:\n1. Insert\n2. Search\n3. Exit\nEnter choice: ";
        cin >> choice;

        if (choice == 1) {
            int key;
            cout << "Enter key to insert: ";
            cin >> key;
            root_offset = insert(file, root_offset, key, node_count, node_count);
            writeRoot(file, root_offset);
            cout << "Inserted key " << key << " with value " << node_count - 1 << ".\n";
        } else if (choice == 2) {
            int key;
            cout << "Enter key to search: ";
            cin >> key;
            int value = search(file, root_offset, key);
            if (value == -1)
                cout << "Key not found.\n";
            else
                cout << "Key found with value: " << value << "\n";
        } else if (choice == 3) {
            cout << "Exiting.\n";
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    file.close();
    return 0;
}
