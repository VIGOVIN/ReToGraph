#include "DataStructure.h"
#include "Algorithms.h"

const string FILE_SUFFIX = ".dot";
const string NFA_FILE_PAth = "../dot/nfa";
const string DFA_FILE_PAth = "../dot/dfa";
const string MIN_DFA_FILE_PATH = "../dot/min_dfa";

int main() {
    string rexp;
    init();
    cout << "please input the re_expression:\n->$ ";
    cin >> rexp;

    NFA* nfa = convertRexpToNfa(rexp);
    DFA* dfa = nfaToDfa(nfa);
    DFA* minDfa = dfaToMinDfa(dfa);

    //打印重要信息
    printNfaInfo(nfa);
    printDfaInfo(dfa);
    printMinDfaInfo(minDfa);


    //生成dot文件
    makeNfaDotFile(*nfa, NFA_FILE_PAth + FILE_SUFFIX);
    makeDfaDotFile(*dfa, DFA_FILE_PAth + FILE_SUFFIX);
    makeDfaDotFile(*minDfa, MIN_DFA_FILE_PATH + FILE_SUFFIX);

    system("pause");
    return 0;
}