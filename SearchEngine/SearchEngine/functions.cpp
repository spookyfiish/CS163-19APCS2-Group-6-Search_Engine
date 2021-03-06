#include "SearchEngine.h"

using namespace std;

#pragma region Time Measurement
clock_t ___start_time = clock();

void start() {
    ___start_time = clock();
}

double close() {
    return double(clock()-___start_time) / CLOCKS_PER_SEC;
}
#pragma endregion

#pragma region Utilities

string toString(int i) {
    string str;
    if (!i)
        return "0";
    while (i) {
        str = char(i % 10 + '0') + str;
        i /= 10;
    }
    return str;
}
vector<string> split(string queries) {
    vector<string> arr;
    int old_pos = 0;
    for (int i = 1; i < queries.length() - 1; i++)
        if (queries[i] == ' ' && queries[size_t(i) - 1] != ' ' && queries[size_t(i) + 1] != ' ') {
            while (queries[old_pos] == ' ') old_pos++;
            arr.emplace_back(queries.substr(old_pos, size_t(i) - old_pos));
            old_pos = i;
        }
    while (queries[old_pos] == ' ') old_pos++;
    arr.emplace_back(queries.substr(old_pos, queries.length() - old_pos));
    return arr;
}

bool isValidChar(char &ch) {
    return (ch >= '0' && ch <= '9') ||
           (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch == '#' || ch == '$' || ch == '*');
}

string toLower(string str) {
    for (int i = 0; i < str.length(); ++i)
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 'a' - 'A';
    return str;
}
string getValidWord(string wrd) {
    int i = 0;
    while (i < wrd.length()) {
        if (isValidChar(wrd[i]))
            i++;
        else wrd.erase(i, 1);
    }
    return wrd;
}
string getValidText(string str) {
    str = getValidWord(str);
    str = toLower(str);
    return str;
}

string getPrefix(string txt) {
    string pre;
    int i = 0;
    while (i < txt.length()) {
        if ((txt[i] >= '0' && txt[i] <= '9') || (txt[i] >= 'a' && txt[i] <= 'z') || (txt[i] >= 'A' && txt[i] <= 'Z'))
            break;
        else pre += txt[i];
        i++;
    }
    return pre;
}
string getSuffix(string txt) {
    string suf;
    int i = txt.length() - 1;
    while (i >= 0) {
        if ((txt[i] >= '0' && txt[i] <= '9') || (txt[i] >= 'a' && txt[i] <= 'z') || (txt[i] >= 'A' && txt[i] <= 'Z'))
            break;
        else suf += txt[i];
        i--;
    }
    return suf;
}
void WriteInColor(int color, string text) {
    cout << "\x1B[" << color << "m" << text << "\033[0m";
}
void WriteInColor(int color, char c) {
    cout << "\x1B[" << color << "m" << c << "\033[0m";
}
#pragma endregion

#pragma region Trie implementation
int TrieNode::numTrieNode = 0;

bool Trie::isStopWord(string Word) {
    TrieNode* tmp = root;
    Word = getValidText(Word);
    for (int i = 0; i < Word.length(); i++) {
        if (tmp->p[map[Word[i]]])
            tmp = tmp->p[map[Word[i]]];
        else return false;
    }
    return tmp->stopWord;
}

void Trie::input(string &filename, int file) {
#pragma warning(suppress : 4996)
    FILE *fin = fopen(filename.c_str(), "r");
    if (fin == NULL) return;

    char ch;
    int pos = 0;
    bool inTitle = true, inTitle_tmp = true;
    TrieNode *tmp = root;
    bool toggle = false;
    do {
        ch = getc(fin);

        if (toggle && (ch == ' ' || ch == '\n')) {
            pos++;
            toggle = false;
        }
        if (ch != ' ' && ch != '\n') toggle = true;

        if (ch == '.') inTitle_tmp = false;
        if (ch == ' ' || ch == '\n' || ch == EOF) { //new word
            if (tmp != root) { //check not consecutive space
                if (!tmp->stopWord) {
                    if (tmp->fileRoot == nullptr || tmp->fileRoot->file != file)
                        tmp->fileRoot = new FileNode (file, tmp->fileRoot);
                    tmp->fileRoot->pos.emplace_back(pos);
                    if (inTitle) {
                        if (tmp->inTitleRoot == nullptr || tmp->inTitleRoot->file != file)
                            tmp->inTitleRoot = new FileNode (file, tmp->inTitleRoot);
                        tmp->inTitleRoot->pos.emplace_back(pos);
                    }
                }

                //preparing for new word
                tmp = root;
                if (!inTitle_tmp) inTitle = false; //word right before '.' not put in the list inTitleRoot
            }
        } else if (!isValidChar(ch))
            continue;
        else { //valid character
            if (!tmp->p[map[ch]]) tmp->p[map[ch]] = new TrieNode;
            tmp = tmp->p[map[ch]];
        }

    } while (ch != EOF);

    fclose(fin);
}

void Trie::input(ifstream& in, int file, bool inTitle) {
    string str;
    int cnt = 0;
    do {
        in >> str; //cout << file << " " << str << endl;
        if(str.size()) insert(str, file, cnt++, inTitle);
    } while (in.good()); //cout << endl;
}

void Trie::insert(string &Word, int file, int pos, bool inTitle) {
    //if (!root) root = new TrieNode;

    TrieNode* tmp = root;
    Word = getValidText(Word); //cout << file << " " << Word << endl;
    for (int i = 0; i < Word.length(); ++i) {
        //int subtrahend = (Word[i] >= 'a') ? 'a' : '0' - 26;
        if (!tmp->p[map[Word[i]]])
            tmp->p[map[Word[i]]] = new TrieNode;
        tmp = tmp->p[map[Word[i]]];
    }

    if (tmp->stopWord) return;
    if (file == -1)
        tmp->stopWord = true;
    else {
        if (tmp->fileRoot == nullptr || tmp->fileRoot->file != file)
            tmp->fileRoot = new FileNode (file, tmp->fileRoot);
        tmp->fileRoot->pos.emplace_back(pos);

        if (inTitle) {
            if (tmp->inTitleRoot == nullptr || tmp->inTitleRoot->file != file)
                tmp->inTitleRoot = new FileNode (file, tmp->inTitleRoot);
            tmp->inTitleRoot->pos.emplace_back(pos);
        }
    }
}
void Trie::search(string &Word, int ans[], int &count, bool inTitle) {
    if (!root) return;
    TrieNode* tmp = root;
    string tempWord = Word;
    Word = getValidText(Word);
    for (int i = 0; i < Word.length(); ++i) {
        if (!tmp->p[map[Word[i]]]) return;
        tmp = tmp->p[map[Word[i]]];
    }

    if (tmp->stopWord) return;
    FileNode *fileRoot = (inTitle ? tmp->inTitleRoot : tmp->fileRoot);
    for (FileNode *p = fileRoot; p != nullptr && count < 5; p = p->Next) ans[count++] = p->file;
}
void Trie::insert_sl(string Word) {
    if (!root)
        root = new TrieNode;
    TrieNode* tmp = root;
    string tempWord = Word;
    Word = getValidText(Word);
    if (Word.length() <= 0) return;
    for (int i = 0; i < Word.length(); ++i) {
        if (!tmp->p[map[Word[i]]])
            tmp->p[map[Word[i]]] = new TrieNode;
        tmp = tmp->p[map[Word[i]]];
    }
    tmp->s = tempWord;
}
//basically search, but returns FileNode =)))
FileNode* Trie::searchFilesToScore(string& Word, bool intitle) {
    if (!root) return nullptr;
    TrieNode* tmp = root;
    Word = getValidText(Word);
    for (int i = 0; i < Word.length(); ++i) {
        if (!tmp->p[map[Word[i]]]) return nullptr;
        tmp = tmp->p[map[Word[i]]];
    }
    if (tmp->stopWord) return nullptr;
    return (intitle ? tmp->inTitleRoot : tmp->fileRoot);
}
string getCorrectString(string& s,int i) {
    while (s.length() < i) s += ' ';
    return s;
}
void Trie::saveTree(fstream& out, TrieNode* root) {
    if (!out.is_open() ||root == nullptr) return;
    int size = root->s.size();
    out.write((char*)&size, sizeof(size));
    out.write(root->s.c_str(), root->s.size());
    string content = "";
    for (int i = 0; i < trieCharSize; i++) content += (root->p[i] != nullptr ? '1' : '0');
    out.write(content.c_str(), trieCharSize);
    for (int i = 0; i < trieCharSize; i++) saveTree(out, root->p[i]);
}
void Trie::readTree(fstream& in, TrieNode* root) {
    if (!in.is_open()) return;
    string content;
    int size = 0;
    in.read((char*)&size, sizeof(size));
    content.resize(size);
    in.read((char*)&content[0], size);
    root->s = content.c_str();
    content.resize(trieCharSize);
    in.read((char*)&content[0], trieCharSize);
    for (int i = 0; i < trieCharSize; i++) {
        if (content[i] == '1') {
            root->p[i] = new TrieNode;
            readTree(in, root->p[i]);
        }
    }
}
TrieNode* Trie::getSuggestion(TrieNode* root, string Word) {
    if (root == nullptr) return root;
    if (Word.length() == 0) return root;
    if (root->p[map[Word[0]]]) {
        char tmp = Word[0];
        Word = Word.erase(0,1);
        return Trie::getSuggestion(root->p[map[tmp]], Word);
    }
    return nullptr;
}
#define MAX_RESULT 6
void Trie::getResult(TrieNode* root, vector<string>& resultSet) {
    if (!root) return;
    if (root->s.length() > 0) resultSet.push_back(root->s);
    if (resultSet.size() >= MAX_RESULT) return;
    for (int i = 0; i < trieCharSize; ++i)
        if (root->p[i])
            getResult(root->p[i], resultSet);
}
TrieNode* Trie::searchSuggestion(string Word) {
    return getSuggestion(root, getValidText(Word));
}
#pragma endregion

#pragma region Search Engine Algorithms
void SearchEngine::loadDataList(ifstream &in) {
    string filename;
    while (getline(in, filename)) //changed this from in >> filename
        if (filename.size()) dataList.emplace_back(filename);

    searchEngineNumOfDataFiles = dataList.size();

    cout << "Data list loaded in " << close() * 1000 << " millisecond(s).\n";
}
void SearchEngine::input_stop_words(string path) {
    ifstream input;
    input.open(path);

    string stopWords;
    int numStopWords = 0;

    if (!input.is_open()) {
        cout << "Stop words could not be loaded." << endl;
        return;
    }

    while (!input.eof()) {
        input >> stopWords;
        root->insert(stopWords, STOPWORD);
        numStopWords++;
    }

    cout << numStopWords << " stop word(s) loaded in " << close() * 1000 << " millisecond(s).\n";
}
void SearchEngine::input(int st) {
    int progress = 0, p;
    string fileName;

    for (int i = st; i < searchEngineNumOfDataFiles; ++i) {
        p = trunc((double) 100 * (i - st) / searchEngineNumOfDataFiles);
        if (p / 25 > progress) cout << 25 * (++progress) << "% files loaded in " << close() * 1000 << " millisecond(s).\n";

        fileName = WORKPLACE + dataList[i];
        //ifstream dataIn{ fileName };
        root->input(fileName, i);
        //root->input(dataIn, i);
    }
}

int getFlag(string Word) {
    if (Word == "AND") return 0;
    if (Word == "OR") return 1;
    if (Word.length() >= 7 && Word.substr(0, 7) == "intitle") return 6;
    if (Word.length() >= 8 && Word.substr(0, 8) == "filetype") return 7;
    if (Word.find("..") != string::npos) return 10; // in range a..b
    switch (Word[0]) {
//        case '$':
//            return 2;
        case '+':
            return 3;
        case '-':
            return 4;
        case '"':
            return 5;
        case '#':
            return 8;
        case '~':
            return 9;
    }
    switch (Word[Word.length() - 1]) {
//        case '$':
//            return 2;
        case '+':
            return 3;
        case '-':
            return 4;
        case '"':
            return 5;
        case '#':
            return 8;
    }
    return -1;
}
void removeParamenter(string& str, char c) {
    str.erase(std::remove(str.begin(), str.end(), c), str.end());
}
vector<SearchTask> SearchEngine::breakDown(string txt) {
    vector<SearchTask> w;
    w.push_back(SearchTask());
    vector<string> s = split(txt);
    bool need_push = false;
    for (int i = 0; i < s.size(); i++) {
        if (root->isStopWord(s[i]) && w.back().function == 5) s[i] = "*"; //Check for stop word for function 5
        if (need_push) {
            w.push_back(SearchTask());
            need_push = false;
        }
        int flag = getFlag(s[i]);
        if (flag == -1) {
            if (w.back().function < 2 && w.back().function > -1) w.back().words2.push_back(s[i]);
            else w.back().words.push_back(s[i]);
        }
        else {
            if (flag == 7) {
                if (!w.back().isEmpty()) w.push_back(SearchTask());
                w.back().function = flag;
                w.back().words.push_back(s[i].substr(9, s[i].length() - 9)); //filetype:
                need_push = true;
                continue;
            }
            if (flag == 6) {
                if (!w.back().isEmpty() && w.back().function != 6) w.push_back(SearchTask());
                w.back().function = flag;
                w.back().words.push_back(s[i].substr(8, s[i].length() - 8)); //intitle:
                continue;
            }
            if(flag > 2 && flag < 5) {
                if (!w.back().isEmpty()) w.push_back(SearchTask());
                w.back().function = flag;
                removeParamenter(s[i], '+');
                removeParamenter(s[i], '-');
                w.back().words.push_back(getValidText(s[i]));
                need_push = true;
                continue;
            }
            if (flag == 9) {
                if (!w.back().isEmpty()) w.push_back(SearchTask());
                w.back().function = flag;
                removeParamenter(s[i], '~');
                w.back().words.push_back(getValidText(s[i]));
                need_push = true;
                continue;
            }
            if (flag == 8) {
                if (!w.back().isEmpty()) w.push_back(SearchTask());
                w.back().function = flag;
                w.back().words.push_back(getValidText(s[i]));
                need_push = true;
                continue;
            }
            if (flag == 5) {
                if (!w.back().isEmpty() && w.back().function != flag) w.push_back(SearchTask());
                removeParamenter(s[i], '"');
                if (!root->isStopWord(s[i])) w.back().words.push_back(getValidText(s[i]));
                if (w.back().function == flag) {
                    w.push_back(SearchTask());
                    continue;
                }
                w.back().function = flag;
                continue;
            }
            if (flag == 10) {
                if (!w.back().isEmpty() && w.back().function != flag) w.push_back(SearchTask());
                w.back().function = flag;
                w.back().curr = (s[i].find('$') != string::npos);
                string r; //cout << s[i] << endl;
                s[i].push_back('.');
//                removeParamenter(s[i], '$');
//                int k = -1, k1 = -1;
//                for (int l = 0; l < s[i].length(); l++) if (s[i][l] == '.') {
//                        if (k == -1) k = l;
//                        else k1 = l;
//                    }
//                if (k != -1) {
//                    w.back().words.push_back(getValidText(s[i].substr(0,k)));
//                    w.back().words2.push_back(getValidText(s[i].substr(k1,s[i].length()-k1)));
//                }else w.back().words.push_back(getValidText(s[i]));
                for (int l = 0; l<s[i].size(); l++)
                    if (s[i][l] != '.') {
                        if (s[i][l] >= '0' && s[i][l] <= '9')
                            r.push_back(s[i][l]);
                    } else if (r.size()) {
                        if (w.back().words.size() == 0) {
                            w.back().words.push_back(r);
                            r.clear();
                            continue;
                        } else if (w.back().words2.size() == 0) {
                            w.back().words2.push_back(r);
                            r.clear();
                            continue;
                        }
                    }cout << w.back().words[0] << " " << w.back().words2[0] << endl;
                need_push = true;
                continue;
            }
            w.back().function = flag;
        }
    }
    auto comp = [](const SearchTask& a, const SearchTask& b)
    {
        return a.function > b.function;
    };
    sort(w.begin(), w.end(), comp);
    return w;
}
void printCharacter(int n, char c) {
    for (int j = 0; j < n; j++) cout << c;
}
void printCharacterColor(int n, char c, int color) {
    for (int j = 0; j < n; j++) WriteInColor(color, c);
}
// Normal, Exact, +, General, intitle, money
string func_toString[7] = { "Result for single words",
                            "Result for exact phrase",
                            "Result for + operator",
                            "Result for single words search",
                            "Result for intitle search", "Result for $..$ or $ search", "Result for # search"};
int func_toColor[7] = { 0,44,41,45,43,42,104 };
void SearchEngine::search(string &Word, ResultSet*& score) {
    int count = 0, ans[5];
    vector<SearchTask> tasks = breakDown(Word);
    memset(score, 0, searchEngineNumOfDataFiles * sizeof(int));
    for (int i = 0; i < tasks.size(); i++) {
        if (true) {
            cout << "Task: " << tasks[i].function << " ";
            for (int j = 0; j < tasks[i].words.size(); j++) cout << tasks[i].words[j] << " ";
            cout << "|";
            for (int j = 0; j < tasks[i].words2.size(); j++) cout << tasks[i].words2[j] << " ";
            cout << endl;
        }
        //double time = close(), time1;
        switch (tasks[i].function) {
            case 5:
                operator9(tasks[i].words, score);//operator10 * also search exact
                break;
            case 1: //OR
                operator9(tasks[i].words, score);
                operator9(tasks[i].words2, score);
                break;
            case 0: //AND
                vector<int> w1 = operator9(tasks[i].words, score);
                vector<int> w2 = operator9(tasks[i].words2, score);
                for (int j = 0; j < searchEngineNumOfDataFiles; ++j) {
                    int k = 0;
                    for (int i = 0; i < w1.size(); i++) if (w1[i] == j) { k++; break; }
                    for (int i = 0; i < w2.size(); i++) if (w2[i] == j) { k++; break; }
                    if (k >= 2) score[j].score += 10;
                    else score[j].score=- 1;
                }
                break;
        }
        for (int k = 0; k < tasks[i].words.size(); k++) {
            switch (tasks[i].function) {
                case 4:
                    operator3(tasks[i].words[k], score);//  -
                    break;
                case 3:
                    operator5(tasks[i].words[k], score);//  +
                    break;
                case 6:
                    operator4(tasks[i].words[k], score);//  intitle:
                    break;
                case 7:
                    operator6(tasks[i].words[k], score);//  filetype:
                    break;
                case 8:
                    operator8(tasks[i].words[k], score);// #
                    break;
                case 9:
                    operator12(tasks[i].words[k], score); // ~
                    break;
                case 10:
                    operator11(stoi(tasks[i].words[0]), stoi(tasks[i].words2[0]), tasks[i].curr, score);
                    break;
                case -1:
                    addScore(tasks[i].words[k], score);
                    break;
            }
            //time1 = close();
            //cout << time1 - time << endl;
            //time = time1;
        }
    }
    rankResult(ans, count, score);
    int k = 0;
    char c;
    bool accept = false;
    double time = close();
    do {
        system("cls");
        if (!count)
            cout << "No matches found in " << time << " second(s).\n\n";
        else{
            for (int i = 1; i < 7; i++) {
                printCharacter(3, ' ');
                printCharacterColor(10, ' ', func_toColor[i]);
                cout << " - " << func_toString[i] << endl;
            }
            cout << count << " matches found in " << time << " second(s).\n\n";
            cout << "Printing result NO." << k+1 << " with the score of " << score[ans[k]].score * -1 << endl;
            WriteInColor(90, "Use ^ and v to navigate through the result set. ENTER to continue searching.");
            cout << endl << endl;
            writeText(ans[k], score);
        }
        cout << endl << endl;
        do { c = _getch(); }
        while((int)c != 80 && (int)c != 72 && (int) c != 13);
        if ((int)c == 80) k++;
        else if ((int)c == 72) k--;
        if (k < 0) k = count - 1;
        else if (k >= count) k = 0;

    } while ((int) c != 13);
}
//used for general case
void SearchEngine::addScore(string query, ResultSet*& score) {
    FileNode* files;
    files = root->searchFilesToScore(query);
    query = getValidText(query);
    for (files; files != nullptr; files = files->Next) {
        if (score[files->file].score >= 0) {
            score[files->file].score += files->pos.size();
            score[files->file].addPos(files->pos, 3);
        }
    }
}

void SearchEngine::operator12(string query, ResultSet *&score) {
    FileNode* files;
    int syn = synMap[query] - 1; cout << syn << endl;

    for (int i = 0; i<Synonyms[syn].size(); i++) {
        query = Synonyms[syn][i];

        files = root->searchFilesToScore(query);
        query = getValidText(query);
        for (files; files != nullptr; files = files->Next) {
            if (score[files->file].score >= 0) {
                score[files->file].score += files->pos.size();
                score[files->file].addPos(files->pos, 3);
            }
        }
    }
}

//only used for the word behind "filetype:" operator
void SearchEngine::operator6(string filetype, ResultSet*& score) {
    for (int i = 0; i < searchEngineNumOfDataFiles; i++)
        if (dataList[i].substr(dataList[i].length() - filetype.length(), filetype.length()) != filetype)
            score[i].score = -1;
}
//only used for # operator
void SearchEngine::operator8(string query, ResultSet*& score)
{
    FileNode* files = root->searchFilesToScore(query);
    for (; files != nullptr; files = files->Next)
    {
        score[files->file].score += files->pos.size() * 4;
        score[files->file].addPos(files->pos, 6);
    }
}
//only used for the word behind "+" operator
vector<int> SearchEngine::operator5(string query, ResultSet*& score) {
    FileNode* files;
    files = root->searchFilesToScore(query);
    vector<int> resultSet;
    for (int i = searchEngineNumOfDataFiles - 1; i >= 0 && files != nullptr; i--)
        if (i == files->file) {
            score[i].score += files->pos.size();
            score[i].addPos(files->pos, 2);
            files = files->Next;
            resultSet.emplace_back(i);
        }
        else
            score[i].score = -1;
    return resultSet;
}
void SearchEngine::operator11(int a, int b, bool curr, ResultSet*& score) {
    for (int i = a; i <= b; i++)
    {
        string word = (curr ? "$" + toString(i) : toString(i));
        FileNode* files = root->searchFilesToScore(word);

        for (; files != nullptr; files = files->Next)
        {
            score[files->file].score += files->pos.size()*5;
            score[files->file].addPos(files->pos, 5);
        }
    }
}
//only used for the word behind "intitle:" operator
void SearchEngine::operator4(string query, ResultSet*& score) {
    FileNode* files;
    files = root->searchFilesToScore(query, true); //search inTitle

    for (files; files != nullptr; files = files->Next) {
        score[files->file].score++;
        score[files->file].addPos(files->pos, 4);
    }
}
//only used for the word behind "-" operator
void SearchEngine::operator3(string query, ResultSet*& score) {
    FileNode* files;
    files = root->searchFilesToScore(query);

    for (files; files != nullptr; files = files->Next)
        score[files->file].score = -1;
}
FileNode* getFileNode(FileNode* root, int index) {
    if (root == nullptr) return nullptr;
    if (root->file == index) return root;
    return getFileNode(root->Next, index);
}
vector<int> SearchEngine::operator9(vector<string> query, ResultSet*& score) {
    //For triming the *
    vector<int> resultSet;
    while (query.size() > 0 && query.back() == "*") query.pop_back();
    if (query.size() <= 0) return resultSet;
    if (query.size() == 1) return operator5(query[0], score);
    FileNode* old = root->searchFilesToScore(query[0]);
    if (old == nullptr)	return resultSet;
    for (int i = 1; i < query.size(); i++) {
        if (query[i] == "*") continue;
        FileNode* files;
        files = root->searchFilesToScore(query[i]);
        bool flag = false;
        for (files; files != nullptr; files = files->Next) {
            FileNode* in_old = getFileNode(old, files->file);
            if (in_old == nullptr) continue;
            for (int k = 0; k < in_old->pos.size(); k++) for (int j = 0; j < files->pos.size(); j++) {
                    if (files->pos[j] - in_old->pos[k] == i) {
                        flag = true;
                        if (i == query.size() - 1) {
                            if (score[files->file].score >= 0) {
                                score[files->file].score += 100;
                                Field field{in_old->pos[k] ,files->pos[j], 1 };
                                score[files->file].field.emplace_back(field);
                                resultSet.emplace_back(files->file);
                            }
                        }
                    }
                }
        }
        if (!flag) break;
    }
    return resultSet;
}
/*void SearchEngine::operator9(string query, ResultSet*& score)
{
	vector <string> wordVector = split(query.substr(1, query.length() - 2));
	vector <FileNode*> files;
	for (int i = 0; i < wordVector.size(); i++)
		files.emplace_back(root->searchFilesToScore(wordVector[i]));

	#pragma region HEAP_DECLEARATION
	typedef tuple <FileNode*, PosNode*, int> HeapNode;
	//file - pos - word idx
	auto heap_comp = [](const HeapNode& a, const HeapNode& b)
	{
		FileNode *aFile, *bFile;
		PosNode *aPos, *bPos;
		tie(aFile, aPos, ignore) = a;
		tie(bFile, bPos, ignore) = b;

		return (aFile->file > bFile->file) || (aFile->file == bFile->file && aPos > bPos);
	};
	priority_queue <HeapNode, vector <HeapNode>, decltype(heap_comp)> pq (heap_comp);
	#pragma endregion 

	for (int i = 0; i < files.size(); i++)
	{
		if (files[i] != nullptr)
			return;
		pq.push(make_tuple(files[i], files[i]->posRoot, i));
	}

	int cnt_word = 0, pre_file = 0, exp_pos;
	while (!pq.empty())
	{
		FileNode* file;
		PosNode* pos;
		int word;
		tie(file, pos, word) = pq.top();
		pq.pop();

		//Proccess continuous
		if (file->file != pre_file)
			cnt_word = 0;
		if (word == cnt_word && (!word || exp_pos == pos->pos))
		{
			++cnt_word;
			exp_pos = pos->pos + wordVector[word].size();
			if (cnt_word >= wordVector.size())
			{
				score[file->file] += wordVector.size();
				cnt_word = 0;
			}
		}
		else cnt_word = 0;
		pre_file = file->file;

		//Push next pos
		pos = pos->Next;
		if (pos != nullptr)
			pq.push(make_tuple(file, pos, word));
		else
		{
			file = file->Next;
			if (file != nullptr)
			{
				pos = file->posRoot;
				pq.push(make_tuple(file, pos, word));
			}
		}
	}
}*/



void SearchEngine::rankResult(int ans[], int &count, ResultSet*& score) {
    count = 0;
    int k = 0;
    for (int i = 0; i < 5; ++i) {
        int max = 0;
        for (int j = 1; j < searchEngineNumOfDataFiles; ++j)
            if (score[j].score > score[max].score)
                max = j;
        if (score[max].score <= 0) break;
        count++;
        ans[k++] = max;
        score[max].score *= -1;
    }
}
void SearchEngine::writeText(int i, ResultSet*& rs) {
    string fileName = WORKPLACE + dataList[i];
#pragma warning(suppress : 4996)
    FILE* fin = fopen(fileName.c_str(), "r");
    if (fin == NULL) return;
    string msg = "Found match(es) from " + fileName;
    WriteInColor(cyan, msg);
    cout << endl;
    int* func = new int[MAX_WORDS_DATA] {0};
    rs[i].getPrintableField(func);
    int pos = 0;
    bool toggle = false, title = true;
    char ch;
    do {
        ch = getc(fin);
        if (toggle && (ch == ' ' || ch == '\n')) {
            pos++;
            toggle = false;
        }
        if(ch != ' ' && ch != '\n') toggle = true;
        WriteInColor(((title && func[pos] == 0) ? 100 : (toggle ? func_toColor[func[pos]] : 0)), ch);

        if (ch == '.') title = false;
    } while (ch != EOF);
    delete func;
    cout << endl << endl;
}

void SearchEngine::saveFile(fstream &out, FileNode *fileRoot) {
    if (!out.is_open() || fileRoot == nullptr) return;

    out.write((char*) &fileRoot->file, sizeof(int));

    int size = fileRoot->pos.size();
    out.write((char*) &size, sizeof(int));
    out.write((char*) &fileRoot->pos[0], size * sizeof(int));

    bool Next = (fileRoot->Next != nullptr);
    out.write((char*) &Next, sizeof(bool));

    if (Next) saveFile(out, fileRoot->Next);
}

void SearchEngine::loadFile(fstream& in, FileNode* fileRoot) {
    if (!in.is_open()) return;

    in.read((char*) &fileRoot->file, sizeof(int));

    int size = 0;
    in.read((char*) &size, sizeof(int));
    fileRoot->pos.resize(size);
    in.read((char*) &fileRoot->pos[0], size * sizeof(int));

    bool Next;
    in.read((char*) &Next, sizeof(bool));

    if (Next) {
        fileRoot->Next = new FileNode;
        loadFile(in, fileRoot->Next);
    }
}

void SearchEngine::saveTrie(fstream& out, TrieNode* root) {
    if (!out.is_open() ||root == nullptr) return;

    out.write((char*) &root->stopWord, sizeof(bool));

    bool hasFile = (root->fileRoot != nullptr);
    out.write((char*) &hasFile, sizeof(bool));
    if (hasFile) saveFile(out, root->fileRoot);

    hasFile = (root->inTitleRoot != nullptr);
    out.write((char*) &hasFile, sizeof(bool));
    if (hasFile) saveFile(out, root->inTitleRoot);

    bool hasChild[trieCharSize];
    for (int i = 0; i < trieCharSize; i++) hasChild[i] = (root->p[i] != nullptr);
    out.write((char*) hasChild, trieCharSize * sizeof(bool));

    for (int i = 0; i < trieCharSize; i++)
        if (hasChild[i]) saveTrie(out, root->p[i]);
}
void SearchEngine::loadTrie(fstream& in, TrieNode* root) { //cout << "done";
    if (!in.is_open()) return;

    in.read((char*) &root->stopWord, sizeof(bool));

    bool hasFile;
    in.read((char*) &hasFile, sizeof(bool));
    if (hasFile) {
        root->fileRoot = new FileNode;
        loadFile(in, root->fileRoot);
    }

    in.read((char*) &hasFile, sizeof(bool));
    if (hasFile) {
        root->inTitleRoot = new FileNode;
        loadFile(in, root->inTitleRoot);
    }

    bool hasChild[trieCharSize];
    in.read((char*) hasChild, trieCharSize * sizeof(bool));

    for (int i = 0; i < trieCharSize; i++) {
        if (hasChild[i]) {
            root->p[i] = new TrieNode;
            loadTrie(in, root->p[i]);
        }
    }
}

void SearchEngine::reset() {
    dataList.clear();
    root = new Trie;
}

void SearchEngine::saveData() {
    fstream out {WORKPLACE + "data.bin",ios::out | ios::binary};
    int stringSize;
    out.write((char*) &searchEngineNumOfDataFiles, sizeof(int));
    for (int i = 0; i<searchEngineNumOfDataFiles; i++) {
        stringSize = dataList[i].size();
        out.write((char*) &stringSize, sizeof(int));
        out.write(dataList[i].c_str(), stringSize * sizeof(char));
    }
    saveTrie(out, root->root);
}

void SearchEngine::loadData() {
    start();

    fstream in {WORKPLACE + "data.bin",ios::in | ios::binary};
    if (!in.is_open() || !in.good()) {
        index();
        return;
    }

    reset();

    int stringSize;
    in.read((char*) &searchEngineNumOfDataFiles, sizeof(int));
    dataList.resize(searchEngineNumOfDataFiles);
    for (int i = 0; i<searchEngineNumOfDataFiles; i++) {
        in.read((char*) &stringSize, sizeof(int));
        dataList[i].resize(stringSize);
        in.read((char*) &dataList[i][0], stringSize * sizeof(char)); //cout << dataList[i] << endl;
    }
    loadTrie(in, root->root);
    cout << "Data loaded successfully in " << close() * 1000 << " millisecond(s).\n\n";
}

void SearchEngine::index() {
    start();
    reset();

    ifstream in {WORKPLACE + "___index.txt"};
    input_stop_words(STOPWORD_PATH);
    loadDataList(in);
    input();
    in.close();
    cout << searchEngineNumOfDataFiles << " data file(s) loaded successfully in " << close() * 1000 << " millisecond(s).\n\n";
}

void SearchEngine::reIndex() {
    start();

    ifstream in{WORKPLACE + "___reindex.txt"};
    int st = searchEngineNumOfDataFiles;
    loadDataList(in);
    input(st);
    in.close();
    cout << searchEngineNumOfDataFiles - st << " extra data file(s) loaded successfully in " << close() * 1000 << " millisecond(s).\n\n";
}

void SearchEngine::loadSynonyms() {
    ifstream in {WORKPLACE + "synonyms.txt"};
    if (!in.is_open()) {
        cout << "Synonyms could not be loaded.\n\n";
        return;
    }

    int size;
    in >> size;
    Synonyms.resize(size);
    for (int i = 0; i<Synonyms.size(); i++) {
        in >> size;
        Synonyms[i].resize(size);
        for (int j = 0; j<Synonyms[i].size(); j++) {
            in >> Synonyms[i][j];
            synMap[Synonyms[i][j]] = i+1;
        }
    }

    cout << Synonyms.size() << " synonym(s) loaded in " << close() * 1000 << " millisecond(s).\n";
}

#pragma endregion

int getMaxLength(vector<string>& s) {
    if (s.size() <= 0) return 0;
    int max = s[0].length();
    for (int i = 0; i < s.size(); i++) if (s[i].length() > max) max = s[i].length();
    return max%2 == 0 ? max : max+1;
}
void UI::print() {
    system("cls");
    int maxContent = getMaxLength(content);
    int maxQuery = getMaxLength(content) * 3 / 4;
    //Print top
    cout << (char)201;
    printCharacter(offset_x * 2 + maxContent, (char)205);
    cout << (char)187<<endl;
    for (int i = 0; i < content.size() + offset_y*2; i++) {
        if ((i >= offset_y && i < content.size() + offset_y)) {
            cout << (char)186;
            printCharacter(offset_x, ' ');
            cout << content[i-offset_y];
            printCharacter(offset_x + maxContent- content[i - offset_y].length(), ' ');
            cout << (char)186 << endl;
        }
        else {
            cout << (char)186;
            printCharacter(maxContent+offset_x*2, ' ');
            cout << (char)186 << endl;
        }
    }
    //Print bottom
    cout << (char)200;
    printCharacter((offset_x * 2 + maxContent)/2-maxQuery/2-1 - offset_subbox_x, (char)205);
    cout << (char)203;
    printCharacter(maxQuery+offset_subbox_x*2, (char)205);
    cout << (char)203;
    printCharacter((offset_x * 2 + maxContent)/2-maxQuery /2-1 - offset_subbox_x, (char)205);
    cout << (char)188 << endl;
    for (int i = 0; i < sub_box.size() + offset_subbox_y * 2; i++) {
        printCharacter((offset_x * 2 + maxContent) / 2 - maxQuery / 2 - offset_subbox_x, ' ');
        if ((i >= offset_subbox_y && i < sub_box.size() + offset_subbox_y)) {
            cout << (char)186;
            printCharacterColor(offset_subbox_x, ' ', k == i ? 44 : 36);
            WriteInColor(k == i ? 44 : 36,sub_box[i - offset_subbox_y].length() > maxQuery ? sub_box[i - offset_subbox_y].substr(0,maxQuery-3) + "..." : sub_box[i - offset_subbox_y]);
            printCharacterColor(offset_subbox_x + maxQuery - (sub_box[i - offset_subbox_y].length() > maxQuery ? maxQuery : sub_box[i - offset_subbox_y].length()), ' ', k == i ? 44 : 36);
            cout << (char)186 << endl;
        }
        else {
            cout << (char)186;
            printCharacter(maxQuery + offset_subbox_x * 2, ' ');
            cout << (char)186 << endl;
        }
    }
    printCharacter((offset_x * 2 + maxContent) / 2 - maxQuery / 2 - offset_subbox_x, ' ');
    cout << (char)200;
    printCharacter(maxQuery + offset_subbox_x * 2, (char)205);
    cout << (char)188 << endl;
}
string SearchTask::getQuery(vector<string>& s) {
    string ss = "";
    for (int i = 0; i < s.size(); i++) ss += s[i];
    return ss;
}