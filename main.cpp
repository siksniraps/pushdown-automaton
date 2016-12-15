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
			return symbol < rhs.symbol || symbol == rhs.symbol && stackSymbol < rhs.stackSymbol;
		}
	};
	struct TransitionValue {
		std::string nextStateName;
		std::string nextStackSymbol;
		TransitionValue(){}
		TransitionValue(std::string nextStateName, std::string nextStackSymbol) : nextStateName(nextStateName), nextStackSymbol(nextStackSymbol) {}
		bool operator<(const TransitionValue& rhs) const {
			return nextStateName < rhs.nextStateName || nextStateName == rhs.nextStateName && nextStackSymbol < rhs.nextStackSymbol;
		}
	};
	

	struct State {
		std::string name;
		bool accepting;
		std::multimap<TransitionKey, TransitionValue> transitions;
		State() {}
		State(std::string name, bool accepting) : name(name), accepting(accepting){}
		bool operator<(const State& rhs) const {
			return name < rhs.name;
		}
	};

	struct Configuration {
		State *currentState;
		std::stack<std::string> stack;
		std::string word;
		Configuration(State *currentState, std::stack<std::string> stack, std::string word) : currentState(currentState), stack(stack), word(word) {
		}


	};

	std::map<std::string, State> states;
	std::set<std::string> alphabet;
	std::set<std::string> stackAlphabet;
	
	std::string initialStateName;
	std::string initialStackSymbol;
	
	bool acceptsWhenEmptyStack;
	bool good;
	
	const std::string epsilon;

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


public:

	PushdownAutomaton() : good(false), epsilon("e"){}

	void load(std::string filename) {

		std::fstream fin;
		fin.open(filename.c_str(), std::ios::in);

		if (fin) {
			//read list of states
			std::string statesLine;
			std::getline(fin, statesLine);
			//read alphabet
			readLineIntoSet(fin, alphabet);
			//read stack alphabet
			readLineIntoSet(fin, stackAlphabet);
			//read initial state
			std::getline(fin, initialStateName);
			//read initial stack symbol
			std::getline(fin, initialStackSymbol);
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
			//read transitions only if everything else has been read successfully
			if (!fin.fail()) {
				while (std::getline(fin, line)) {
					std::stringstream iss(line);
					std::string stateName, symbol, stackSymbol, nextStateName, nextStackSymbol;
					iss >> stateName >> symbol >> stackSymbol >> nextStateName >> nextStackSymbol;

					addTransition(stateName, symbol, stackSymbol, nextStateName, nextStackSymbol);
				}
				//everything is read, set the automaton in good state
				good = true;
			}
			fin.close();
		}
		
	}

	bool verify(std::string word) {
		std::vector<Configuration*> configurations;
		std::stack<std::string> stack;
		stack.push(initialStackSymbol);
		configurations.push_back(new Configuration(&states[initialStateName], stack, word));

		bool valid = false;

		while (!valid && configurations.size()) {
			Configuration* config = configurations.back();
			configurations.pop_back();


			std::string symbol = epsilon;
			std::vector<std::pair<std::multimap<TransitionKey, TransitionValue>::iterator, std::multimap<TransitionKey, TransitionValue>::iterator> > transitionIterators;
			if (word.length()) {
				std::string symbol = word.substr(0, 1);

				TransitionKey symbolNoStackKey(symbol, epsilon);
				TransitionKey symbolStackKey(symbol, stack.top());

				transitionIterators.push_back(config->currentState->transitions.equal_range(symbolNoStackKey));
				transitionIterators.push_back(config->currentState->transitions.equal_range(symbolStackKey));
			}

			TransitionKey noSymbolStackKey(epsilon, stack.top());
			TransitionKey noSymbolNoStackKey(epsilon, epsilon);


			transitionIterators.push_back(config->currentState->transitions.equal_range(noSymbolStackKey));
			transitionIterators.push_back(config->currentState->transitions.equal_range(noSymbolNoStackKey));



			for (unsigned int k = 0; !valid && k < transitionIterators.size(); k++) {
				for (std::multimap<TransitionKey, TransitionValue>::iterator it = transitionIterators[k].first; !valid && it != transitionIterators[k].second; it++) {
					Configuration *copyConfig = new Configuration(*config);
					//change new config state
					copyConfig->currentState = &states[it->second.nextStateName];
					//change new config stack top symbol
					if (it->first.stackSymbol != epsilon) {
						stack.pop();
					}
					if (it->second.nextStackSymbol != epsilon) {
						stack.push(it->second.nextStackSymbol);
					}
					//change new config word
					if (it->first.symbol != epsilon) {
						copyConfig->word = copyConfig->word.erase(0, 1);
					}
					configurations.push_back(copyConfig);

					//check for accepting state
					valid = acceptsWhenEmptyStack ? copyConfig->stack.empty() : copyConfig->currentState->accepting;

				}
			}
			delete config;
		}


		
		
		for (int i = 0; i < configurations.size(); i++) {
			Configuration *config = configurations.back();
			configurations.pop_back();
			delete config;
			
		}
		return valid;
	}

	bool isGood() {
		return good;
	}
};






void promptInputWord(std::string &word) {
	std::cout << "Enter a word:\n";
	std::getline(std::cin, word);
}


int main() {

    PushdownAutomaton automaton;
    automaton.load("automaton.in");
	if (automaton.isGood()) {
		std::string word;
		bool exit = false;
		do {
			promptInputWord(word);
			if (word == ":q") {
				exit = true;
			}
			else {
				std::cout << (automaton.verify(word) ? "Accepted" : "Not Accepted") << std::endl << std::endl;
			}

		} while (!exit);
	}
	else {
		std::cout << "Bad automaton input file" << std::endl;
	}
	
	
	return 0;
}




