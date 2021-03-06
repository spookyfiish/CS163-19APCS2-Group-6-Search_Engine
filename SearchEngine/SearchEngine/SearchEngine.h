#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <utility>
#include <tuple>
#include <unordered_map>
#include <conio.h>

using namespace std;

#define cyan 36
#define blueWithBG 44
#define trieCharSize 39 //a->z, 0->9, *#$, 1st

#define STOPWORD -1
#define INTITLE true

#define MAX_QUERY_LENGTH 55

#define MAX_WORDS_DATA 10000

const string WORKPLACE = "../SearchEngine/Data/";
//const string WORKPLACE = "/Users/ducanchu/Documents/Assignments/CS163/CS163-19APCS2-Group-6-SearchEngine/SearchEngine/SearchEngine/Data/";
const string BINARY_HISTORY_PATH = WORKPLACE + "history_bin.bin";
const string STOPWORD_PATH = WORKPLACE + "stopWords.txt";
const string DATA_PATH = WORKPLACE + "dataList.txt";

void start();
double close();

struct FileNode {
    int file;
    vector<int> pos;
    FileNode *Next;

    FileNode() : Next(nullptr) {}
    FileNode(int f, FileNode* ne) {
        file = f;
        Next = ne;
    }
};
struct SearchTask {
    int function = -1;
    bool curr;
    vector<string> words;
    vector<string> words2;
    bool isEmpty() {
        return (words.size() == 0 && words2.size() == 0);
    }
    string getQuery(vector<string>& s);
};
struct Field {
    int f = -1, l = -1;
    int function = 0;
};
struct ResultSet {
    vector<Field> field;
    int score = 0;
    void addPos(vector<int>& pos, int c) {
        for (int i = 0; i < pos.size(); i++) {
            Field f{pos[i],pos[i],c};
            field.emplace_back(f);
        }
    }
    void getPrintableField(int*& arr) {
        if (field.size() > 1000) field.clear();
        for (int k = 0; k < field.size(); k++) {
            for (int i = field[k].f - 1 > 0 ? field[k].f - 1 : 0; i <= field[k].l - 1; i++) arr[i] = field[k].function;
        }
    }
    void clean() {
        field.clear();
        score = 0;
    }
};
struct TrieNode {
    static int numTrieNode;
    TrieNode* p[trieCharSize]{ nullptr };

    bool stopWord = false;
    FileNode *fileRoot = nullptr;
    FileNode *inTitleRoot = nullptr;

    TrieNode() {
        stopWord = false;
        fileRoot = nullptr;
        inTitleRoot = nullptr;
        numTrieNode++;
    }
    string s;
};

struct Trie {
    TrieNode* root = nullptr;

    int map[255];

    Trie() {
        root = new TrieNode;
        for (int i = 0; i<26; i++) map['a' + i] = map['A' + i] = i;
        for (int i = 0; i<10; i++) map['0' + i] = 26 + i;
        map['*'] = 36;
        map['#'] = 37;
        map['$'] = 38;
    }

    void input(string &filename, int file);
    void input(ifstream& in, int file, bool inTitle = false);
    void insert(string &Word, int file, int pos = -1, bool inTitle = false); //file == -1 if Word is stopword
    void search(string &Word, int ans[], int &count, bool inTitle = false);

    //Phu
    FileNode* searchFilesToScore(string& Word, bool intitle = false);

    //Tuong
    bool isStopWord(string Word);
    void insert_sl(string Word);
    TrieNode* getSuggestion(TrieNode* root, string Word);
    TrieNode* searchSuggestion(string Word);
    void getResult(TrieNode* root, vector<string>& resultSet);
    void saveTree(fstream& out,TrieNode* root);
    void readTree(fstream& in,TrieNode* root);
};

//Tuong
struct UI {
    vector<string> content;
    vector<string> sub_box;
    int offset_x = 10,offset_y = 0;
    int offset_subbox_x = 2, offset_subbox_y = 0;
    int k=0;
    void print();
};

struct SearchEngine {
    int searchEngineNumOfDataFiles;
    vector<string> dataList;
    vector<vector<string>> Synonyms;
    unordered_map<string, int> synMap;
    Trie *root = new Trie;

    void loadDataList(ifstream &in);
    void input_stop_words(string path);
    void input(int st = 0);

    vector<SearchTask> breakDown(string txt);

    void search(string &Word, ResultSet*& score);

    void addScore(string query, ResultSet*& score);// Tuong checked
    //   operator1 is "AND" - Tuong
    //   operator2 is "OR" - Tuong
    //   operator3 is "-" - Tuong
    void operator3(string query, ResultSet*& score); // Tuong checked
    void operator4(string query, ResultSet*& score);

    //   operator5 is "+" - Tuong
    vector<int> operator5(string query, ResultSet*& score); //Tuong checked
    //
    void operator6(string filetype, ResultSet*& score);
    //   operator7 is "$" - DONE automatically - Thien
    //   operator8 is "#" - DONE automatically - Thien
    void operator8(string query, ResultSet*& score);
    vector<int> operator9(vector<string> query, ResultSet*& score); //this is operator10
    //   operator11 is "$" - Thien
    //   operator12 is "~" - An
    void operator11(int a, int b, bool curr, ResultSet*& score);
    void operator12(string query, ResultSet*& score);

    void rankResult(int ans[], int &count, ResultSet*& score);

    void writeText(int i, ResultSet*& rs);

    void saveFile(fstream& out, FileNode *fileRoot);
    void loadFile(fstream& in, FileNode* fileRoot);
    void saveTrie(fstream& out, TrieNode* root);
    void loadTrie(fstream& in, TrieNode* root);
    void reset();
    void saveData();
    void loadData();
    void index();
    void reIndex();
    void loadSynonyms();
};