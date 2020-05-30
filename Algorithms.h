//
// Created by Semir on 2020/4/4.
//

#ifndef RETOGRAPH_ALGORITHMS_H
#define RETOGRAPH_ALGORITHMS_H

#include "DataStructure.h"

void init();

//正则式转化为NFA
NFA* convertRexpToNfa(string &rexp);
//正则式转化为DFA
DFA* nfaToDfa(NFA* nfa);
//DFA最小化
DFA* dfaToMinDfa(DFA* dfa);
//打印NFA相关信息
void printNfaInfo(NFA* nfa);
//打印DFA相关信息
void printDfaInfo(DFA* dfa);
//打印MIN_DFA相关信息
void printMinDfaInfo(DFA* dfa);
//生成NFA的dot文件
void makeNfaDotFile(NFA nfa, string dot_file);
//生成DFA的dot文件
void makeDfaDotFile(DFA dfa, string dot_file);

#endif //RETOGRAPH_ALGORITHMS_H
