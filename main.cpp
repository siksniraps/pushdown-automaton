#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <iterator>
#include <unordered_set>
#include <set>
#include <map>


class PushdownAutomaton {
private:

	
	struct TransitionKey {
		std::string symbol;
		std::string stackSymbol;
		TransitionKey(){}
		TransitionKey(std::string symbol, std::string stackSymbol) : symbol(symbol), stackSymbol(stackSymbol){}
		bool operator<(const TransitionKey& rhs) const {
			return symbol < rhs.symbol || stackSymbol < rhs.stackSymbol;
		}
	};
	struct TransitionValue {
		std::string nextStateName;
		std::string nextStackSymbol;
		TransitionValue(){}
		TransitionValue(std::string nextStateName, std::string nextStackSymbol) : nextStateName(nextStateName), nextStackSymbol(nextStackSymbol) {}
		bool operator<(const TransitionValue& rhs) const {
			return nextStateName < rhs.nextStateName || nextStackSymbol < rhs.nextStackSymbol;
		}
	};
	

	struct State {
		std::string name;
		bool accepting;
		std::map<TransitionKey, TransitionValue> transitions;
		State() {}
		State(std::string name, bool accepting) : name(name), accepting(accepting){}
		bool operator<(const State& rhs) const {
			return name < rhs.name;
		}
	};
	std::map<std::string, State> states;
	std::set<std::string> alphabet;
	std::set<std::string> stackAlphabet;
	std::stack<std::string> stack;
	std::string initialState;
	std::string initialStackSymbol;
	State currentState;
	bool acceptsWhenEmptyStack;

	void readLineIntoSet(std::istream &in, std::set<std::string> &s) {
	    std::string line;
	    std::getline(in, line);
		std::stringstream iss(line);
		std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::inserter(s, s.begin()));
	}

	void storeStates(std::string statesLine, std::string acceptingStatesLine) {
		//read accepting states 
	    std::set<std::string> acceptingStates;
	    std::stringstream iss(acceptingStatesLine);
        std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::inserter(acceptingStates, acceptingStates.begin()));
		
		//read all states and create state objects with information whether that are accepting
		iss.clear();
		iss.str(statesLine);
		std::istream_iterator<std::string> begin(iss), end;
		while (begin != end) {
			State state(*begin, acceptingStates.find(*begin) != acceptingStates.end());
			states.insert(std::make_pair(*begin++, state));
		}
	}

	void addTransition(std::string stateName, std::string symbol, std::string stackSymbol, std::string nextStateName, std::string nextStackSymbol) {
		states[stateName].transitions.insert(std::make_pair(TransitionKey(symbol, stackSymbol), TransitionValue(nextStateName, nextStackSymbol)));
	}

	bool isAccepting() {
		return acceptsWhenEmptyStack ? stack.empty() : currentState.accepting;
	}

public:
	void load(std::string filename) {

		std::fstream fin;
		fin.open(filename.c_str(), std::ios::in);

		//read list of states
		std::string statesLine;
		std::getline(fin, statesLine);
		//read alphabet
		readLineIntoSet(fin, alphabet);
		//read stack alphabet
		readLineIntoSet(fin, stackAlphabet);
		//read initial state
		std::getline(fin, initialState);
		//read initial stack symbol
		std::getline(fin, initialStackSymbol);
		stack.push(initialStackSymbol);
		//read accepting states
		std::string acceptingStatesLine;
		std::getline(fin, acceptingStatesLine);
		//fill set of states
		storeStates(statesLine, acceptingStatesLine);
		
		std::string line;
		//read accept mode
		std::getline(fin, line);
		acceptsWhenEmptyStack = (line == "E");
		//read transitions
		while (std::getline(fin, line)) {
			std::stringstream iss(line);
			std::istream_iterator<std::string> begin(iss);
			addTransition(*begin++, *begin++, *begin++, *begin++, *begin);
		}
		fin.close();
	}

	bool verify(std::string word) {
		for (unsigned int i = 0; i < word.length(); i++) {
			TransitionKey key(std::string(1, word.at(i)), stack.top());
			stack.pop();
			TransitionValue value = currentState.transitions[key];
			currentState = states[value.nextStateName];
			stack.push(value.nextStackSymbol);
		}
		
		return isAccepting();
	}
};


void promptInputWord(std::string &);


int main() {

    PushdownAutomaton automaton;
    automaton.load("automaton.in");
	std::string word;
	promptInputWord(word);

	std::cout << (automaton.verify(word) ? "Accepted" : "Not Accepted") << std::endl;

	std::cin >> word;

	return 0;
}



void promptInputWord(std::string &word) {
	std::cout << "Enter a word:\n";
	std::getline(std::cin, word);
}
