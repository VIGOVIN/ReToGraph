//
// Created by Semir on 2020/4/4.
//

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <cstring>

using namespace std;


struct PathNode {
    set<int> toNodeSet;
    char path;
    bool end;
    PathNode() : toNodeSet(), path(), end() {}
    PathNode(set<int> toNodeSet, char path, bool end) : toNodeSet(toNodeSet), path(path), end(end) {}
};


void test01() {
    map<char, int> charmap;
    charmap.insert(pair<char, int> ('a', 10));
    charmap.insert(pair<char, int> ('b', 10));
    charmap.insert(pair<char, int> ('c', 10));
    charmap.insert({'d', 100});
    cout << charmap.size() << endl;
    for (auto x : charmap) {
        cout << x.first << " " << x.second << endl;
    }
//    cout << charmap['c'] << " " << charmap['k'] << " " << charmap['a'] << endl;
    cout << charmap.count('c') << " " << charmap.count('k') << " " << charmap.count('a') << endl;
}

void test02() {
    vector<map<char, vector<int>>> jump;
    map<char, vector<int>> map1 = { {'a', {1, 2, 3}}, {'b', {4, 5}}, {'c', {6, 7}} };
    map<char, vector<int>> map2 = { {'d', {8, 9}}, {'e', {10, 11}}, {'f', {12, 13}} };
    jump.push_back(map1);
    jump.push_back(map2);
    cout << jump.size() << endl;
    map<char, vector<int>> map3 = jump[1];
    map3 = { {'D', {8, 8, 8}}, {'E', {9, 9, 9}}, {'F', {10, 10, 10}}};
    jump[1] = map3;
    cout << jump.size() << endl;
}

void test03() {
    vector<map<char, vector<int>>> jump;
    map<char, vector<int>> map1 = { {'a', {1, 2, 3}}, {'b', {4, 5}}, {'c', {6, 7}} };
    map<char, vector<int>> map2 = { {'d', {8, 9}}, {'e', {10, 11}}, {'f', {12, 13}} };
    jump.push_back(map1);
    jump.push_back(map2);
    cout << jump.size() << endl;
    map<char, vector<int>>::iterator iterator;
    for (iterator = jump[1].begin(); iterator != jump[1].end(); iterator++) {
        vector<int> vec = iterator->second;
        for (int j = 0; j < vec.size(); j++) {
            vec[j] += 100;
        }
        iterator->second = vec;
    }
    cout << jump.size() << endl;
}

void test04() {
    vector<map<char, vector<int>>> jump;
    map<char, vector<int>> map1 = { {'a', {1, 2, 3}}, {'b', {4, 5}}, {'c', {6, 7}} };
    map<char, vector<int>> map2 = { {'d', {8, 9}}, {'e', {10, 11}}, {'f', {12, 13}} };
    jump.push_back(map1);
    jump.push_back(map2);
    cout << jump.size() << endl;
    map<char, vector<int>>::iterator iterator;
    for (iterator = jump[1].begin(); iterator != jump[1].end(); iterator++) {
        vector<int> vec = iterator->second;
        for (int j = 0; j < vec.size(); j++) {
            vec[j] += 100;
        }
        iterator->second = vec;
    }
    cout << jump.size() << endl;
    jump.resize(5);
    cout << jump.size() << endl;
}

void test05() {
    set<int> toNodeSet = {1, 2, 3};
    PathNode* node = new PathNode(toNodeSet, 'c', true);
    cout << node->end << endl;
    PathNode* node1 = new PathNode;
    node1->end = false;
    cout << node1->end << endl;
    cout << node1->toNodeSet.size() << endl;
}

void test06() {
    set<int> toNodeSet = {1, 2, 3};
    set<int>::iterator iterator;
    for (iterator = toNodeSet.begin(); iterator != toNodeSet.end(); iterator++) {
        cout << *iterator << endl;
    }
}

void test07() {
    int size = 10;
    vector<int> vec(size);
    for (int i = 30; i <= 100; i += 10) {
        vec.push_back(i);
    }
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

void test08() {
    int size = 10;
    vector<int> vec;
    for (int i = 30; i <= 100; i += 10) {
        vec.push_back(i);
    }
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

void test09() {
    int size = 10;
    vector<int> vec;
    vec.push_back(10);
    vec.push_back(40);
    vec.push_back(30);
    cout << vec[0] << endl;
    cout << vec[1] << endl;
    cout << vec[4] << endl;
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

void test10() {
    set<set<int>> stateClusterSet;
    set<int> stateCluster1 = {1, 2, 3};
    set<int> stateCluster2 = {4, 2, 3};
    set<int> stateCluster3 = {5, 2, 3};
    set<int> stateCluster4 = {6, 2, 3};
    stateClusterSet.insert(stateCluster1);
    stateClusterSet.insert(stateCluster2);
    stateClusterSet.insert(stateCluster3);
    cout << stateClusterSet.size() << endl;
    cout << stateClusterSet.count(stateCluster4) << endl;

}


void test11() {
    string s = "dot -Tpng first.dot -o first.png";
    string ss = "dot -Tpng E/CppProject/DotTest/first.dot -o  \\E:\\\\CppProject\\DotTest\\first.png";
    system(ss.c_str());
}

void test12() {
    string s1 = "-r";
    string s2 = "(a*|b*)*";
    char chars1[100];
    char chars2[100];
    char* charsset[100];
    strcpy(chars1, s1.c_str());
    strcpy(chars2, s2.c_str());
    cout << chars1 << endl;
    cout << chars2 << endl;
    charsset[1] = chars1;
    charsset[2] = chars2;
}

void test13() {
    vector<set<int>> vec;
    vec.resize(2);
    for (int i = 0; i < 10; i++) {
        vec[0].insert(i);
    }
    for (int i = 20; i < 30; i++) {
        vec[1].insert(i);
    }
    cout << vec.size() << endl;
    set<int> newSet;
    for (int i = 40; i < 50; i++) {
        newSet.insert(i);
    }
    vec.push_back(newSet);
    cout << vec.size() << endl;
}

void test14() {
    set<int> set1;
    for (int i = 0; i < 10; i++) {
        set1.insert(i);
    }
    cout << set1.size() << endl;
    set<int> set2;
    set2 = set1;
    cout << set2.size() << endl;
}

int main() {
    test14();
    return 0;
}