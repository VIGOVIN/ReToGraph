//
// Created by Semir on 2020/4/4.
//

#include "Algorithms.h"
#include "DataStructure.h"

static set<char> normalCharset;
static map<char, int> operatorWeightMap;

const int W_UNION = 10;
const int W_CONCATE = 20;
const int W_KLEENE = 30;
const int W_BRA_LEFT = 40;
const int W_BRA_RIGHT = -1;
const string ERROR_STR = "!!! ERROR !!!";
const char EPSILON = '~';


/**
 * 数据初始化
 */
void init() {
    normalCharset.clear();
    for (int i = 0; i <= 9; i++) {
        normalCharset.insert('0' + i);
    }
    for (int i = 0; i < 26; i++) {
        normalCharset.insert('a' + i);
        normalCharset.insert('A' + i);
    }
    operatorWeightMap.insert(pair<char, int> ('|', W_UNION));
    operatorWeightMap.insert(pair<char, int> ('+', W_CONCATE));
    operatorWeightMap.insert(pair<char, int> ('*', W_KLEENE));
    operatorWeightMap.insert(pair<char, int> ('(', W_BRA_LEFT));
    operatorWeightMap.insert(pair<char, int> (')', W_BRA_RIGHT));
//    cout << normalCharset.size() << endl;
}


/**
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> NFA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 */


/**
 * 判断是否是字母或数字
 * @param c
 * @return
 */
bool isNormalCharset(char c) {
    return normalCharset.count(c) != 0;
}

/**
 * 判断是否是运算符
 * @param c
 * @return
 */
bool isOperator(char c) {
    return operatorWeightMap.count(c) > 0;
}

/**
 * 优先级比较 判断是否应该将rexp中指向的字符进栈
 * @param charInStack 运算符栈栈顶元素
 * @param charInRexp rexp中的运算符 除去')'
 * @return
 */
bool shouldPush(const stack<char> oprStack, char charInRexp) {
    if (oprStack.empty() || oprStack.top() == '(') {
        return true;
    }
    return operatorWeightMap.at(charInRexp) > operatorWeightMap.at(oprStack.top());
}

/**
 * 不断将操作符栈中的元素转移到rexpDeque 直到弹出一个左括号
 * @param rexpDeque
 * @param oprStack
 */
void moveToDequeUntilBra(deque<char> &rexpDeque, stack<char> &oprStack) {
    char c = oprStack.top();
    while (!oprStack.empty() && c != '(') {
        rexpDeque.push_back(c);
        oprStack.pop();
        c = oprStack.top();
    }
    if (!oprStack.empty()) {
        oprStack.pop();
    }
}

/**
 * 将操作符栈中的元素转移到 rexpDeque
 * @param rexpDeque
 * @param oprStack
 */
void moveToDeque(deque<char> &rexpDeque, stack<char> &oprStack) {
    char c = oprStack.top();
    rexpDeque.push_back(c);
    oprStack.pop();
}

/**
 * 将操作符栈中的元素全部转移到 rexpDeque
 * @param rexpDeque
 * @param oprStack
 */
void moveAllToDeque(deque<char> &rexpDeque, stack<char> &oprStack) {
    while (!oprStack.empty()) {
        rexpDeque.push_back(oprStack.top());
        oprStack.pop();
    }
}

/**
 * 补全中缀表达式缺失的“+”
 * @param rexp 原始正则式
 */
void convertRexpWithConca(string &rexp) {
    bool needConcaOp = false;
    int i = 0;
    while (rexp[i] != '\0') {
        bool isNormalChar = isNormalCharset(rexp[i]);
        if (!needConcaOp) {
            if (isNormalChar || rexp[i] == ')' || rexp[i] == '*') {
                needConcaOp = true;
            }
        } else {
            if (isNormalChar || rexp[i] == '(') {
                rexp.insert(i, "+");
            }
            if (rexp[i] == '|' || rexp[i] == '+') {
                needConcaOp = false;
            }
        }
        i++;
    }
}

/**
 * 将中缀表达式转化为后缀表达式
 * @param rexp 带连接符的中缀表达式
 */
void convertRexpToSuffix(string &rexp) {
    deque<char> rexpDeque;
    stack<char> oprStack;
    int i = 0;
    while (rexp[i] != '\0') {
        if (isOperator(rexp[i])) {
            //运算符 判断是否应该入栈
            if (rexp[i] == ')') {
                moveToDequeUntilBra(rexpDeque, oprStack);
            } else if (shouldPush(oprStack, rexp[i])) {
                oprStack.push(rexp[i]);
            } else {
                moveToDeque(rexpDeque, oprStack);
                oprStack.push(rexp[i]);
            }
        } else {
            //非运算符 进rexp队列
            rexpDeque.push_back(rexp[i]);
        }
        i++;
    }
    moveAllToDeque(rexpDeque, oprStack);
    rexp = "";
    while (!rexpDeque.empty()) {
        rexp += rexpDeque.front();
        rexpDeque.pop_front();
    }
}


/**
 * 构造单个元素的NFA子图并压入队列
 * @param nfaDeque
 * @param c
 */
void makeSingleJumpNfa(deque<NFA*> &nfaDeque, char c) {
    NFA* nfa = new NFA;
    nfa->char_cnt = 1;              // 输入字符的类别数
    nfa->state_cnt = 2;             // 状态的数量
    nfa->start_state = 0;           // 开始状态
    nfa->end_state_cnt = 1;         // 终止状态的个数
    nfa->chars = {c};               // 可输入各种的字符
    nfa->states = {0, 1};           // 状态的编号
    nfa->end_states = {1};          // 终止状态列表
    nfa->jump.resize(2);
    nfa->jump[0] = {{c, {1}}}; // 跳转hash表, 每个state一个hash
    nfaDeque.push_back(nfa);
}

/**
 * 从队列中取出 2 个元素 进行或运算 并压入队列
 * @param nfaDeque
 */
void makeUnionNfa(deque<NFA*> &nfaDeque) {
    NFA* secondNfa = nfaDeque.back();
    nfaDeque.pop_back();
    NFA* firstNfa = nfaDeque.back();
    nfaDeque.pop_back();

    NFA* nfa = new NFA;
    nfa->chars.clear();
    nfa->chars.insert(firstNfa->chars.begin(), firstNfa->chars.end());
    nfa->chars.insert(secondNfa->chars.begin(), secondNfa->chars.end());
    nfa->chars.insert(EPSILON);
    nfa->char_cnt = nfa->chars.size();

    nfa->state_cnt = firstNfa->state_cnt + secondNfa->state_cnt + 2;
    vector<int> states(nfa->state_cnt);
    for (int i = 0; i < states.size(); i++) {
        states[i] = i;
    }
    nfa->states = states;
    vector<map<char, vector<int>>> jump(nfa->state_cnt);
    jump[0] = {{EPSILON, {1, 1 + firstNfa->state_cnt}}};    //从起始节点增加2条边
    for (int i = 0; i < firstNfa->state_cnt; i++) {
        map<char, vector<int>>::iterator iterator;
        for (iterator = firstNfa->jump[i].begin(); iterator != firstNfa->jump[i].end(); iterator++) {
            vector<int> vec =  iterator->second;
            for (int j = 0; j < vec.size(); j++) {
                vec[j] += 1;
            }
            iterator->second = vec;
        }
        jump[i + 1] = firstNfa->jump[i];
    }
    jump[firstNfa->state_cnt] = {{EPSILON, {nfa->state_cnt - 1}}};
    int stride = 1 + firstNfa->state_cnt;
    for (int i = 0; i < secondNfa->state_cnt; i++) {
        map<char, vector<int>>::iterator iterator;
        for (iterator = secondNfa->jump[i].begin(); iterator != secondNfa->jump[i].end(); iterator++) {
            vector<int> vec = iterator->second;
            for (int j = 0; j < vec.size(); j++) {
                vec[j] += stride;
            }
            iterator->second = vec;
        }
        jump[i + stride] = secondNfa->jump[i];
    }
    jump[firstNfa->state_cnt + secondNfa->state_cnt] = {{EPSILON, {nfa->state_cnt - 1}}};
    nfa->jump = jump;

    nfa->start_state = 0;
    nfa->end_states = {nfa->state_cnt - 1};
    nfa->end_state_cnt = 1;

    delete firstNfa;
    delete secondNfa;

    nfaDeque.push_back(nfa);
}

/**
 * 从队列中取出 2 个元素 进行连接运算 并压入队列
 * @param nfaDeque
 */
void makeConcateNfa(deque<NFA*> &nfaDeque) {
    NFA* secondNfa = nfaDeque.back();
    nfaDeque.pop_back();
    NFA* firstNfa = nfaDeque.back();
    nfaDeque.pop_back();

    NFA* nfa = firstNfa;
    nfa->chars.insert(secondNfa->chars.begin(), secondNfa->chars.end());
    nfa->chars.insert(EPSILON);
    nfa->char_cnt = nfa->chars.size();

    int stride = nfa->state_cnt;
    nfa->state_cnt += secondNfa->state_cnt;
    vector<int> states(nfa->state_cnt);
    for (int i = 0; i < states.size(); i++) {
        states[i] = i;
    }
    nfa->states = states;
    nfa->jump.resize(nfa->state_cnt);
//    vector<map<char, vector<int>>> jump(nfa->state_cnt);
    nfa->jump[stride - 1] = {{EPSILON, {stride}}};
    for (int i = 0; i < secondNfa->state_cnt; i++) {
        map<char, vector<int>>::iterator iterator;
        for (iterator = secondNfa->jump[i].begin(); iterator != secondNfa->jump[i].end(); iterator++) {
            vector<int> vec = iterator->second;
            for (int j = 0; j < vec.size(); j++) {
                vec[j] += stride;
            }
            iterator->second = vec;
        }
        nfa->jump[i + stride] = secondNfa->jump[i];
    }

    nfa->start_state = 0;
    nfa->end_states = {nfa->state_cnt - 1};
    nfa->end_state_cnt = 1;

    delete secondNfa;

    nfaDeque.push_back(nfa);
}

/**
 * 从队列中取出一个元素 进行闭包运算 并压入队列
 * @param nfaDeque
 */
void makeKleeneNfa(deque<NFA*> &nfaDeque) {
    NFA* originNfa = nfaDeque.back();
    nfaDeque.pop_back();

    NFA* nfa = new NFA;
    nfa->chars.clear();
    nfa->chars.insert(originNfa->chars.begin(), originNfa->chars.end());
    nfa->chars.insert(EPSILON);
    nfa->char_cnt = nfa->chars.size();

    nfa->state_cnt = originNfa->state_cnt + 2;
    vector<int> states(nfa->state_cnt);
    for (int i = 0; i < states.size(); i++) {
        states[i] = i;
    }
    nfa->states = states;
    vector<map<char, vector<int>>> jump(nfa->state_cnt);
    jump[0] = {{EPSILON, {1, nfa->state_cnt - 1}}};
    for (int i = 0; i < originNfa->state_cnt; i++) {
        map<char, vector<int>>::iterator iterator;
        for (iterator = originNfa->jump[i].begin(); iterator != originNfa->jump[i].end(); iterator++) {
            vector<int> vec = iterator->second;
            for (int j = 0; j < vec.size(); j++) {
                vec[j] += 1;
            }
            iterator->second = vec;
        }
        jump[i + 1] = originNfa->jump[i];
    }
    jump[originNfa->state_cnt] = {{EPSILON, {1, nfa->state_cnt - 1}}};
    nfa->jump = jump;

    nfa->start_state = 0;
    nfa->end_states = {nfa->state_cnt - 1};
    nfa->end_state_cnt = 1;

    delete originNfa;

    nfaDeque.push_back(nfa);
}

/**
 * 构造 NFA
 * @param rexp
 * @return
 */
NFA* rexpToNfa(string rexp) {
    deque<NFA*> nfaDeque;
    int i = 0;
    while (rexp[i] != '\0') {
        if (!isOperator(rexp[i])) {
            makeSingleJumpNfa(nfaDeque, rexp[i]);
        } else {
            switch (rexp[i]) {
                case '|':
                    makeUnionNfa(nfaDeque);
                    break;
                case '+':
                    makeConcateNfa(nfaDeque);
                    break;
                case '*':
                    makeKleeneNfa(nfaDeque);
                    break;
                default:
                    cout << ERROR_STR << endl;
                    break;
            }
        }
        i++;
    }
//    cout << "++++++++++++++++++++++ " << nfaDeque.size() << " ++++++++++++++++++++++" << endl;
    return nfaDeque.front();
}

/**
 * 将正则表达式转化为包含连接符的后缀表达式
 * @param rexp
 */
NFA* convertRexpToNfa(string &rexp) {
    convertRexpWithConca(rexp);
    convertRexpToSuffix(rexp);
    NFA* nfa = rexpToNfa(rexp);
    return nfa;
}

/**
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< NFA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 */


/**
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> DFA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 */

/**
 * 获取可输入的字符 （除去EPSILON）
 * @param nfa
 * @return
 */
vector<char> getPathVector(const NFA* nfa) {
    int pathNum = nfa->chars.count(EPSILON) == 0 ? nfa->chars.size() : nfa->chars.size() - 1;
    vector<char> paths(pathNum);
    set<char>::iterator iterator;
    int i = 0;
    for (iterator = nfa->chars.begin(); iterator != nfa->chars.end(); iterator++) {
        if (*iterator == EPSILON) {
            continue;
        }
        paths[i] = *iterator;
        i++;
    }
    return paths;
}


/**
 * 深度优先搜索 从一个节点 查找其通过空转移所能到达的所有节点
 * @param nfa
 * @param stateCluster
 * @param state
 */
void dfsWithEpsilon(const NFA* nfa, set<int> &stateCluster, int state){
    if (stateCluster.count(state) == 0) {
        stateCluster.insert(state);
    } else {
        return;
    }
    map<char, vector<int>> jumpMap = nfa->jump[state];
    if (jumpMap.size() > 0 && jumpMap.count(EPSILON) > 0) {
        vector<int> stateVec = jumpMap[EPSILON];
        for (int i = 0; i < stateVec.size(); i++) {
            dfsWithEpsilon(nfa, stateCluster, stateVec[i]);
        }
    }
}

/**
 * 根据 NFA 和一个状态 找出能够空转移到的状态集
 * @param nfa
 * @param state
 * @return
 */
set<int> findStateClusterWithEpsilon(const NFA* nfa, int state) {
    set<int> stateCluster;
    stateCluster.clear();
    dfsWithEpsilon(nfa, stateCluster, state);
    return stateCluster;
}

/**
 * 根据 NFA 的一个状态集 和 一个输入的字符 找到所能到达的新的状态集
 * @param nfa
 * @param stateCluster
 * @param path
 * @return
 */
set<int> findStateClusterByCluster(const NFA* nfa, set<int> stateCluster, char path) {
    set<int> coreCluster;
    set<int> toCluster;
    for (int i = 0; i < nfa->jump.size(); i++) {
        if (stateCluster.count(i) > 0) {
            map<char, vector<int>> jumpMap = nfa->jump[i];
            if (jumpMap.count(path) > 0) {
                vector<int> vec = jumpMap[path];
                for (int j = 0; j < vec.size(); j++) {
                    coreCluster.insert(vec[j]);
                }
            } else {
                continue;
            }
        }
    }
    set<int>::iterator iterator;
    for (iterator = coreCluster.begin(); iterator != coreCluster.end(); iterator++) {
        dfsWithEpsilon(nfa, toCluster, *iterator);
    }
    toCluster.insert(coreCluster.begin(), coreCluster.end());
    return toCluster;
}

/**
 * 深度优先搜索构建 DFA
 * @param nfa
 * @param paths
 * @param routerMap
 * @param stateClusterSet
 * @param stateCluster
 */
void dfs(const NFA* nfa, vector<char> paths,
        map<set<int>, vector<PathNode*>> &routerMap,
        set<set<int>> &stateClusterSet,
        set<int> stateCluster) {

    if (stateClusterSet.count(stateCluster) > 0) {
        return;
    }
    stateClusterSet.insert(stateCluster);
    for (int i = 0; i < paths.size(); i++) {
        set<int> subStateCluster = findStateClusterByCluster(nfa, stateCluster, paths[i]);
        if (subStateCluster.size() == 0) {
            continue;
        }
        bool end = subStateCluster.count(nfa->end_states[0]) > 0;
        PathNode* pathNode = new PathNode(subStateCluster, paths[i], end);
        if (routerMap.count(stateCluster) > 0) {
            routerMap[stateCluster].push_back(pathNode);
        } else {
            routerMap.insert({stateCluster, {pathNode}});
        }
        dfs(nfa, paths, routerMap, stateClusterSet, subStateCluster);
    }
}

/**
 * 将 NFA 转化为 DFA
 * @param nfa
 * @return
 */
DFA* nfaToDfa(NFA* nfa) {

    /**
     * 根据 NFA 完成填充以下数据结构
     * stateClusterSet
     * routerMap
     */

    vector<char> paths = getPathVector(nfa);

    map<set<int>, vector<PathNode*>> routerMap;
    set<set<int>> stateClusterSet;

    set<int> initialStateCluster = findStateClusterWithEpsilon(nfa, nfa->start_state);
    dfs(nfa, paths, routerMap, stateClusterSet, initialStateCluster);

//    cout << routerMap.size() << endl;
//    cout << stateClusterSet.size() << endl;

    DFA* dfa = new DFA;

    map<set<int>, int> stateSetMap;
    set<set<int>>::iterator stateClusterIter;
    stateSetMap.insert({initialStateCluster, 0});
    dfa->start_state = 0;
    dfa->states.push_back(0);
    int state = 1;
    for (stateClusterIter = stateClusterSet.begin(); stateClusterIter != stateClusterSet.end(); stateClusterIter++) {
        if (stateSetMap.count(*stateClusterIter) > 0) {
            continue;
        }
        stateSetMap.insert({(*stateClusterIter), state});
        dfa->states.push_back(state);
        state++;
    }
    dfa->state_cnt = state;

    dfa->char_cnt = 0;
    dfa->chars.clear();
    dfa->end_state_cnt = 0;
    dfa->end_states.clear();
    dfa->jump.resize(dfa->state_cnt);

    //若初始状态也是终态
    if (initialStateCluster.count(nfa->end_states[0]) > 0) {
        dfa->end_states.insert(0);
    }

    map<set<int>, vector<PathNode*>>::iterator mapIter;
    for (mapIter = routerMap.begin(); mapIter != routerMap.end(); mapIter++) {
        int fromState = stateSetMap[mapIter->first];
        vector<PathNode*> pathNodeVec = mapIter->second;
        map<char, int> jumpMap;
        for (int i = 0; i < pathNodeVec.size(); i++) {
            dfa->chars.insert(pathNodeVec[i]->path);
            int toState = stateSetMap[pathNodeVec[i]->toNodeSet];
            jumpMap.insert({pathNodeVec[i]->path, toState});
            if (pathNodeVec[i]->end) {
                dfa->end_states.insert(toState);
            }
        }
        dfa->jump[fromState] = jumpMap;
    }
    dfa->char_cnt = dfa->chars.size();
    dfa->end_state_cnt = dfa->end_states.size();

//    cout << dfa->state_cnt << endl;

    return dfa;
}


/**
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DFA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 */


/**
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MIN_DFA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 */

/**
 * 根据DFA中的stateIndex找到它在stateSetVec中所属集合的下标
 * @param stateSetVec
 * @param stateIndex
 * @return
 */
int findSetIndex(vector<set<int>> &stateSetVec, int stateIndex) {
    for (int i = 0; i < stateSetVec.size(); i++) {
        if (stateSetVec[i].count(stateIndex) > 0) {
            return i;
        }
    }
    cout << ERROR_STR << endl;
    return -1;
}

/**
 * 找到stateIndex所指向的状态在stateSetVec中的jumpMap
 * @param dfa
 * @param stateSetVec
 * @param stateIndex
 * @return
 */
map<char, int> makeTempMap(DFA* dfa, vector<set<int>> &stateSetVec, int stateIndex) {
    map<char, int> tempMap;
    map<char, int> dfaJumpMap = dfa->jump[stateIndex];
    for (map<char, int>::iterator iter = dfaJumpMap.begin(); iter != dfaJumpMap.end(); iter++) {
        tempMap[iter->first] = findSetIndex(stateSetVec, iter->second);
    }

    return tempMap;
}

/**
 * 判断一个状态集合是否是终态
 * @param dfa
 * @param stateSet
 * @return
 */
bool isEndSet(DFA* dfa, set<int> &stateSet) {
    if (stateSet.size() <= 0) {
        return false;
    }
    return dfa->end_states.count(*(stateSet.begin())) > 0;
}

/**
 * 将jumpMap中的每个元素的状态集合作为一个元素放入stateSetVec
 * @param stateSetVec
 * @param jumpMap
 */
void insertIntoStateSetVec(vector<set<int>> &stateSetVec, map<map<char, int>, set<int>> &jumpMap) {
    for (map<map<char, int>, set<int>>::iterator iter = jumpMap.begin(); iter != jumpMap.end(); iter++) {
        stateSetVec.push_back(iter->second);
    }
}

/**
 * 将DFA转化为MIN_DFA 需要完成3个重要的数据结构
 * stateSetVec endJumpMapSet noEndJumpMapSet
 * @param dfa
 * @return
 */
DFA* dfaToMinDfa(DFA* dfa) {
    //标志位为1，代表是终态 为0代表非终态
    vector<set<int>> stateSetVec;
    dfa->end_state_cnt == dfa->state_cnt ? stateSetVec.resize(1) : stateSetVec.resize(2);
    for (int i = 0; i < dfa->state_cnt; i++) {
        if (dfa->end_states.count(dfa->states[i]) > 0) {
            stateSetVec[0].insert(dfa->states[i]);
        } else {
            stateSetVec[1].insert(dfa->states[i]);
        }
    }
//    cout << stateSetVec.size() << endl;

    set<map<map<char, int>, set<int>>> endJumpMapSet;
    set<map<map<char, int>, set<int>>> noEndJumpMapSet;

    while (true) {

        for (int i = 0; i < stateSetVec.size(); i++) {
            map<map<char, int>, set<int>> jumpMap;
            for (set<int>::iterator stateMapIter = stateSetVec[i].begin(); stateMapIter != stateSetVec[i].end(); stateMapIter++) {
                int stateIndex = *stateMapIter;
                map<char, int> tempMap = makeTempMap(dfa, stateSetVec, stateIndex);
                jumpMap[tempMap].insert(stateIndex);
            }
            if (isEndSet(dfa, stateSetVec[i])) {
                endJumpMapSet.insert(jumpMap);
            } else {
                noEndJumpMapSet.insert(jumpMap);
            }
        }

        bool shouldSplit = false;


        for (set<map<map<char, int>, set<int>>>::iterator iter = endJumpMapSet.begin(); iter != endJumpMapSet.end(); iter++) {
            if ((*iter).size()) {
                map<map<char, int>, set<int>> jumpMap = *iter;
                if (jumpMap.size() > 1) {
                    shouldSplit = true;
                    break;
                }
            }
        }

        if (!shouldSplit) {
            for (set<map<map<char, int>, set<int>>>::iterator iter = noEndJumpMapSet.begin(); iter != noEndJumpMapSet.end(); iter++) {
                if ((*iter).size()) {
                    map<map<char, int>, set<int>> jumpMap = *iter;
                    if (jumpMap.size() > 1) {
                        shouldSplit = true;
                        break;
                    }
                }
            }
        }

        if (shouldSplit) {
            stateSetVec.clear();
            for (set<map<map<char, int>, set<int>>>::iterator iter = endJumpMapSet.begin(); iter != endJumpMapSet.end(); iter++) {
                if ((*iter).size()) {
                    map<map<char, int>, set<int>> jumpMap = *iter;
                    insertIntoStateSetVec(stateSetVec, jumpMap);
                }
            }
            for (set<map<map<char, int>, set<int>>>::iterator iter = noEndJumpMapSet.begin(); iter != noEndJumpMapSet.end(); iter++) {
                if ((*iter).size()) {
                    map<map<char, int>, set<int>> jumpMap = *iter;
                    insertIntoStateSetVec(stateSetVec, jumpMap);
                }
            }
            endJumpMapSet.clear();
            noEndJumpMapSet.clear();
        } else {
            break;
        }

    }

//    cout << stateSetVec.size() << endl;
//    cout << endJumpMapSet.size() << endl;
//    cout << noEndJumpMapSet.size() << endl;

    //根据 stateSetVec endJumpMapSet noEndJumpMapSet 构建最小dfa

    DFA* minDfa = new DFA;
    minDfa->char_cnt = dfa->char_cnt;
    minDfa->state_cnt = stateSetVec.size();
    minDfa->states.resize(stateSetVec.size());
    for (int i = 0; i < stateSetVec.size(); i++) {
        minDfa->states[i] = i;
        if (isEndSet(dfa, stateSetVec[i])) {
            minDfa->end_states.insert(i);
        }
    }
    minDfa->start_state = findSetIndex(stateSetVec, dfa->start_state);
    minDfa->end_state_cnt = minDfa->end_states.size();
    minDfa->chars = dfa->chars;
    minDfa->jump.resize(stateSetVec.size());
    for (set<map<map<char, int>, set<int>>>::iterator iter = endJumpMapSet.begin();
    iter != endJumpMapSet.end(); iter++) {
        if ((*iter).size()) {
            minDfa->jump[findSetIndex(stateSetVec, *(((*iter).begin())->second.begin()))]
                = ((*iter).begin())->first;
        }
    }
    for (set<map<map<char, int>, set<int>>>::iterator iter = noEndJumpMapSet.begin();
    iter != noEndJumpMapSet.end(); iter++) {
        if ((*iter).size()) {
            minDfa->jump[findSetIndex(stateSetVec, *(((*iter).begin())->second.begin()))]
                = ((*iter).begin())->first;
        }
    }
//    cout << minDfa->chars.size() << endl;

    return minDfa;
}


/**
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< MIN_DFA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 */


void printNfaInfo(NFA* nfa) {
    cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> NFA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
    cout << "The count of states: " << nfa->state_cnt << endl;
    cout << "All the states: " << endl;
    for (int i = 0; i < nfa->state_cnt; i++) {
        cout << nfa->states[i] << " ";
    }
    cout << endl;
    cout << "Start state: " << nfa->start_state << endl;
    cout << "End state: " << nfa->end_states[0] << endl;
    cout << "The count of transform conditions: " << nfa->char_cnt << endl;
    cout << "All the transform conditions: " << endl;
    set<char>::iterator charsIter;
    for (charsIter = nfa->chars.begin(); charsIter != nfa->chars.end(); charsIter++) {
        cout << *charsIter << " ";
    }
    cout << endl;
    cout << "All the transform: " << endl;
    for (int i = 0; i < nfa->state_cnt; i++) {
        cout << "From State" << nfa->states[i] << endl;
        map<char, vector<int>> jumpMap = nfa->jump[i];
        map<char, vector<int>>::iterator jumpMapIter;
        for (jumpMapIter = jumpMap.begin(); jumpMapIter != jumpMap.end(); jumpMapIter++) {
            cout << "To ";
            vector<int> vec = jumpMapIter->second;
            bool firstFlag = true;
            for (int j = 0; j < vec.size(); j++) {
                if (firstFlag) {
                    cout << "State" << vec[j];
                    firstFlag = false;
                } else {
                    cout << "/State" << vec[j];
                }
            }
            cout << " With Condition " << jumpMapIter->first << endl;
        }
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< NFA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}


void printDfaInfo(DFA* dfa) {
    cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> DFA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
    cout << "The count of states: " << dfa->state_cnt << endl;
    cout << "All the states: " << endl;
    for (int i = 0; i < dfa->state_cnt; i++) {
        cout << dfa->states[i] << " ";
    }
    cout << endl;
    cout << "Start state: " << dfa->start_state << endl;
    cout << "The count of end states: " << dfa->end_states.size() << endl;
    cout << "All the end states: " << endl;
    set<int>::iterator endStatesIter;
    for (endStatesIter = dfa->end_states.begin(); endStatesIter != dfa->end_states.end(); endStatesIter++) {
        cout << *endStatesIter << " ";
    }
    cout << endl;
    cout << "The count of transform conditions: " << dfa->char_cnt << endl;
    cout << "All the transform conditions: " << endl;
    set<char>::iterator charsIter;
    for (charsIter = dfa->chars.begin(); charsIter != dfa->chars.end(); charsIter++) {
        cout << *charsIter << " ";
    }
    cout << endl;
    cout << "All the transform: " << endl;
    for (int i = 0; i < dfa->state_cnt; i++) {
        cout << "From State" << dfa->states[i] << endl;
        map<char, int> jumpMap = dfa->jump[i];
        map<char, int>::iterator jumpMapIter;
        for (jumpMapIter = jumpMap.begin(); jumpMapIter != jumpMap.end(); jumpMapIter++) {
            cout << "To State" << jumpMapIter->second;
            cout << " With Condition " << jumpMapIter->first << endl;
        }
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DFA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}


void printMinDfaInfo(DFA* dfa) {
    cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MIN_DFA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
    cout << "The count of states: " << dfa->state_cnt << endl;
    cout << "All the states: " << endl;
    for (int i = 0; i < dfa->state_cnt; i++) {
        cout << dfa->states[i] << " ";
    }
    cout << endl;
    cout << "Start state: " << dfa->start_state << endl;
    cout << "The count of end states: " << dfa->end_states.size() << endl;
    cout << "All the end states: " << endl;
    set<int>::iterator endStatesIter;
    for (endStatesIter = dfa->end_states.begin(); endStatesIter != dfa->end_states.end(); endStatesIter++) {
        cout << *endStatesIter << " ";
    }
    cout << endl;
    cout << "The count of transform conditions: " << dfa->char_cnt << endl;
    cout << "All the transform conditions: " << endl;
    set<char>::iterator charsIter;
    for (charsIter = dfa->chars.begin(); charsIter != dfa->chars.end(); charsIter++) {
        cout << *charsIter << " ";
    }
    cout << endl;
    cout << "All the transform: " << endl;
    for (int i = 0; i < dfa->state_cnt; i++) {
        cout << "From State" << dfa->states[i] << endl;
        map<char, int> jumpMap = dfa->jump[i];
        map<char, int>::iterator jumpMapIter;
        for (jumpMapIter = jumpMap.begin(); jumpMapIter != jumpMap.end(); jumpMapIter++) {
            cout << "To State" << jumpMapIter->second;
            cout << " With Condition " << jumpMapIter->first << endl;
        }
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< MIN_DFA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}


/**
 * 根据 NFA 生成 dot 文件
 * @param nfa
 * @param dot_file
 */
void makeNfaDotFile(NFA nfa, string dot_file) {
    // 合并起点终点相同的边, 用一个矩阵来存储
    vector<vector<vector<char>>> merge_jump(nfa.state_cnt, vector<vector<char>>(nfa.state_cnt, vector<char>()));
    for (int i = 0; i < nfa.state_cnt; ++i) {
        auto sm = nfa.jump[i];
        for (auto m : sm) {
            for (auto v : m.second) {
                merge_jump[i][v].push_back(m.first);
            }
        }
    }
    // cout << "dot_file: " << dot_file << endl;
    ofstream write;
    write.open(dot_file, ios::out | ios::trunc);
    // 写入文件开始
    write << "digraph {" << endl;
    // 写入状态
    for (int i = 0; i < nfa.state_cnt; ++i) {
        write << "\t" + to_string(i) + " [label=\"" + to_string(i) + "\"";
        if (count(nfa.end_states.begin(), nfa.end_states.end(), i)) {
            write << " color=red shape=doublecircle";  // 如果是终止状态, 那么结点为红色, 且为双圆型
        }
        write << "]" << endl;
    }
    // * 可以合并的边, 检测到连续3个或以上的 +1递增 的条件就合并, 用 - 连接
    for (int i = 0; i < nfa.state_cnt; ++i) {
        for (int j = 0; j < nfa.state_cnt; ++j) {
            if (merge_jump[i][j].size()) {
                string cond = "\"";
                cond += merge_jump[i][j][0];
                cond += ',';
                int ins = 1;
                for (int k = 1; k < merge_jump[i][j].size(); ++k) {
                    if (merge_jump[i][j][k] == merge_jump[i][j][k - 1] + 1) {
                        ins += 1;
                    } else {
                        if (ins >= 3) {
                            for (int l = 0; l < 2 * ins - 1; ++l) {
                                cond.pop_back();
                            }
                            cond += '-';
                            cond += merge_jump[i][j][k - 1];
                            cond += ',';
                            ins = 1;
                        }
                    }
                    cond += merge_jump[i][j][k];
                    cond += ',';
                    // cout << "cond: " << cond << endl;
                }
                if (ins >= 3) {
                    for (int l = 0; l < 2 * ins - 1; ++l) {
                        cond.pop_back();
                    }
                    cond += '-';
                    cond += merge_jump[i][j].back();
                    cond.push_back(',');
                    ins = 1;
                }
                cond.erase(cond.end() - 1);
                cond += '"';
                // cout << "cond = " << cond << endl;
                write << "\t" + to_string(i) + " -> " + to_string(j) + " [label=" + cond + "]" << endl;
            }
        }
    }
    write << "}";
    write.close();
}

/**
 * 根据 DFA 生成 dot 文件
 * @param dfa
 * @param dot_file
 */
void makeDfaDotFile(DFA dfa, string dot_file) {
    // 合并起点终点相同的边, 用一个矩阵来存储
    vector<vector<vector<char>>> merge_jump(dfa.state_cnt, vector<vector<char>>(dfa.state_cnt, vector<char>()));
    for (int i = 0; i < dfa.state_cnt; ++i) {
        auto sm = dfa.jump[i];
        for (auto m : sm) {
            merge_jump[i][m.second].push_back(m.first);
        }
    }
    ofstream write;
    write.open(dot_file, ios::out | ios::trunc);
    // 写入文件开始
    write << "digraph {" << endl;
    // 写入状态
    for (int i = 0; i < dfa.state_cnt; ++i) {
        write << "\t" + to_string(i) + " [label=\"" + to_string(i) + "\"";
        if (count(dfa.end_states.begin(), dfa.end_states.end(), i)) {
            write << " color=red shape=doublecircle";  // 如果是终止状态, 那么结点为红色, 且为双圆型
        }
        write << "]" << endl;
    }
    // * 可以合并的边, 检测到连续3个或以上的 +1递增 的条件就合并, 用 - 连接
    for (int i = 0; i < dfa.state_cnt; ++i) {
        for (int j = 0; j < dfa.state_cnt; ++j) {
            if (merge_jump[i][j].size()) {
                string cond = "\"";
                cond += merge_jump[i][j][0];
                cond += ',';
                int ins = 1;
                for (int k = 1; k < merge_jump[i][j].size(); ++k) {
                    if (merge_jump[i][j][k] == merge_jump[i][j][k - 1] + 1) {
                        ins += 1;
                    } else {
                        if (ins >= 3) {
                            for (int l = 0; l < 2 * ins - 1; ++l) {
                                cond.pop_back();
                            }
                            cond += '-';
                            cond += merge_jump[i][j][k - 1];
                            cond += ',';
                            ins = 1;
                        }
                    }
                    cond += merge_jump[i][j][k];
                    cond += ',';
                    // cout << "cond: " << cond << endl;
                }
                if (ins >= 3) {
                    for (int l = 0; l < 2 * ins - 1; ++l) {
                        cond.pop_back();
                    }
                    cond += '-';
                    cond += merge_jump[i][j].back();
                    cond.push_back(',');
                    ins = 1;
                }
                cond.erase(cond.end() - 1);
                cond += '"';
                // cout << "cond = " << cond << endl;
                write << "\t" + to_string(i) + " -> " + to_string(j) + " [label=" + cond + "]" << endl;
            }
        }
    }
    write << "}";
    write.close();
}

