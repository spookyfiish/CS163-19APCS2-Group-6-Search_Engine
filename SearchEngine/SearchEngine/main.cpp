﻿#include "SearchEngine.h"
#include "iostream"
#include "string"

ResultSet* score = new ResultSet[11268];

void loadHistory(Trie& history) {
    fstream history_f{ BINARY_HISTORY_PATH,ios::out | ios::in | ios::binary };
    history.readTree(history_f, history.root);
    history_f.close();
}
void saveHistory(Trie& history, string queries) {
    history.insert_sl(queries);
    fstream history_f{ BINARY_HISTORY_PATH,ios::out | ios::in | ios::binary };
    history.saveTree(history_f, history.root);
    history_f.close();
}

int main() {
    ifstream in;
    SearchEngine se;

    start();
    se.loadSynonyms();

    start();
    se.loadData();

    string key = "";

    Trie history;
    loadHistory(history);

//    while (1) {
//        cout << "What do you want to search? ('exit' to exit) ";
//        getline(cin, key);
//        if (key == "exit")
//            break;
//        if (key == "index") {
//            se.index();
//            continue;
//        }
//        if (key == "reindex") {
//            se.reIndex();
//            continue;
//        }
//        start();
//        se.search(key, score);
//    }

    UI ui;
    ui.content.push_back("                             _        ___                               ");
    ui.content.push_back("    ()                      | |      / (_)             o                ");
    ui.content.push_back("    /\\  _   __,   ,_    __  | |      \\__   _  _    __,     _  _    _    ");
    ui.content.push_back("   /  \\|/  /  |  /  |  /    |/ \\     /    / |/ |  /  | |  / |/ |  |/    ");
    ui.content.push_back("  /(__/|__/\\_/|_/   |_/\\___/|   |_/  \\___/  |  |_/\\_/|/|_/  |  |_/|__/  ");
    ui.content.push_back("                                                    /|                  ");
    ui.content.push_back("                                                    \\|                  ");
    ui.content.push_back("    Insert query here:");
    while (1) {
        char c;
        bool accept = false;
        do {
            //Read input
            c = _getch();
            if ((int)c == 8) {
                if (key.length() > 0) key = key.erase(key.length() - 1, 1);
            }
            else if ((int)c == 80) ui.k++;
            else if ((int)c == 72) ui.k--;
            else if ((int)c == 77) accept = true;
            else if((true) && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
                               (c >= 'A' && c <= 'Z') || c == ' ' || c == '+' || c == '-' || c == '$' || c == '.' || c == ',' || c == '*' || c == '"' || c == ':' || c == '#' || c == '~')) key += c;
            if (ui.k < 0) ui.k = 0;
            //Search
            TrieNode* sug = history.searchSuggestion(key);
            while (!ui.sub_box.empty()) ui.sub_box.pop_back();
            history.getResult(sug, ui.sub_box);
            if (ui.k >= ui.sub_box.size()) ui.k = 0;

            if (accept && ui.k < ui.sub_box.size()) {
                //Paste history into search key
                key = ui.sub_box[ui.k];
                accept = false;
            }
            ui.content.back() = "    Search: " + (key.length() > MAX_QUERY_LENGTH ? "..."+key.substr(key.length()-1-MAX_QUERY_LENGTH+3,MAX_QUERY_LENGTH-2) : key);
            ui.print();
        } while ((int)c != 13 || key.length() <= 0);
        if (key == "exit")
            break;
        if (key == "index") {
            se.index();
            //continue;
        } else
        if (key == "reindex") {
            se.reIndex();
            //continue;
        } else {
            saveHistory(history, key);
            start();
            se.search(key, score);
        }

        //Reset and start new search
        key = "";
        accept = false;
        ui.k = 0;
        for(int i = 0; i < 11268; i++) score[i].clean();
    }

    se.saveData();
    return 0;
}