//
// Created by Semir on 2020/4/4.
//

#ifndef RETOGRAPH_DATASTRUCTURE_H
#define RETOGRAPH_DATASTRUCTURE_H

#include<bits/stdc++.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <deque>
#include <queue>
#include <ctime>
#include <set>
#include <stack>
#include <vector>
#include <climits>
#include "unistd.h"

using namespace std;

#define OTHER '^'
#define ERROR -1

// NFA结构体
struct NFA {                              // 用一个结构体保存dfa信息
    int char_cnt;                         // 输入字符的类别数
    int state_cnt;                        // 状态的数量
    int start_state;                      // 开始状态
    int end_state_cnt;                    // 终止状态的个数
    set<char> chars;                      // 可输入各种的字符
    vector<int> states;                   // 状态的编号
    vector<int> end_states;               // 终止状态列表
    vector<map<char, vector<int>>> jump;  // 跳转hash表, 每个state一个hash, 保存这个字符可能到的所有下一状态
};

// DFA结构体
struct DFA {                          // 用一个结构体保存dfa信息
    int char_cnt;                     // 输入字符的类别数
    int state_cnt;                    // 状态的数量
    int start_state;                  // 开始状态
    int end_state_cnt;                // 终止状态的个数
    set<char> chars;                  // 可输入各种的字符
    vector<int> states;               // 状态的编号
    set<int> end_states;              // 终止状态列表
    vector<map<char, int>> jump;  // 跳转hash表, 每个state一个hash
};

//NFA转化为DFA的辅助状态
struct PathNode {
    set<int> toNodeSet;
    char path;
    bool end;
    PathNode() : toNodeSet(), path(), end() {}
    PathNode(set<int> toNodeSet, char path, bool end) : toNodeSet(toNodeSet), path(path), end(end) {}
};

#endif //RETOGRAPH_DATASTRUCTURE_H
