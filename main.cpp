#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <iterator>
#include <set>


class PushdownAutomaton {
private:
	struct State {
		std::string name;
		bool accepting;
		State(std::string name, bool accepting) : name(name), accepting(accepting){}
	};
	std::set<State> states;
	std::set<std::string> alphabet;
	std::set<std::string> stackAlphabet;
	std::string initialState;
	std::string initialStackSymbol;
	int currentState;
	bool acceptsWhenEmptyStack;

	void readLineIntoSet(std::istream &in, std::set<std::string> &s) {
	    std::string line;
	    std::getline(in, line);
		std::stringstream iss(line);
		std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::inserter(s, s.begin()));
	}

	void storeStates(std::string statesLine, std::string acceptingStatesLine) {
	    std::set<std::string> acceptingStates;
	    std::stringstream iss(acceptingStatesLine);
        std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::inserter(acceptingStates, acceptingStates.begin()));
        iss.str(statesLine);
        std::istream_iterator<int> begin(iss), end;
        std::cout << statesLine;
        while(begin != end) std::cout << *begin++ << " ";
	}

public:
	void load(std::string filename) {
	    std::string statesLine;
		std::string acceptingStatesLine;

		std::fstream fin;
		fin.open(filename.c_str(), std::ios::in);

		//read list of states
		std::getline(fin, statesLine);
		//read alphabet
		readLineIntoSet(fin, alphabet);
		//read stack alphabet
		readLineIntoSet(fin, stackAlphabet);
		//read initial state
		std::getline(fin, initialState);
		//read initial stack symbol
		std::getline(fin, initialStackSymbol);
		//read accepting states
		std::getline(fin, acceptingStatesLine);

		//fill set of states
		storeStates(statesLine, acceptingStatesLine);

		fin.close();
	}
};


void promptInputWord(std::string &);


int main() {

    PushdownAutomaton automaton;
    automaton.load("automaton.in");
	std::string word;
	promptInputWord(word);


	return 0;
}



void promptInputWord(std::string &word) {
	std::cout << "Enter a word:\n";
	std::getline(std::cin, word);
}
