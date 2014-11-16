#include<iostream>
#include<string>
#include<vector>
using namespace std;

class Position;//prototype, to help me describe Transition wiht Position
class Transition
{
private:
	Position* toPosition;
	char _symbol;//this variable is private, so I am going to use symbol() to make my life a bit easier
	int _length;//zero, if the transition is empty
	string targetName;//for debugging
public:
	Transition()
	{
		toPosition = NULL;
		_symbol = ' ';
		targetName = "TARGET NOT SET";
	}
	Transition(Position* toPosition, char symbol, string targetName)
	{
		setStuff(toPosition, symbol, targetName);
	}
	void setStuff(Position* toPosition, char symbol, string targetName)
	{
		this->toPosition = toPosition;
		_symbol = symbol;
		this->targetName = targetName;
		if (_symbol == '0')
			_length = 0;
		else
			_length = 1;
	}
	void printInfo()
	{
		cout << "symbol: " << _symbol << " to: " << targetName << endl;
	}
	char symbol()
	{
		return _symbol;
	}
	int length()
	{
		return _length;
	}
	Position* target()
	{
		return toPosition;
	}
};
class Position
{//позиция
private:
	string _name;//for debugging
	int _index;
	bool _final;//final position, if we have checked all the symbols
	bool _hasEpsilon;//if it is final, but there is an ampty transition to another position
	std::vector<Transition> _transition;
public:
	Position()
	{
		//transitions = 0;
		_name = "NOT SET";
		_final = false;
	}
	Position(int index, string name, bool isFinal)
	{
		_index = index;//indexing, so can copy the transitions when joining two automations
		_name = name;
		_final = isFinal;
	}
	Position(int index, string name, Transition* firstTrans, bool isFinal)
	{
		_transition.push_back(*firstTrans);
		_index = index;
		_name = name;
		_final = isFinal;
	}
	void addTransition(Transition* transition)
	{
		_transition.push_back(*transition);//the Vector is copying whatever is pointed by *transition
		if (transition->symbol() == '0')
			_hasEpsilon = true;
	}
	void printName()
	{
		cout << _name << endl;
	}
	void printInfo()
	{
		cout << "position: " << _name;
		if (_final)
			cout << " final position";
		cout << endl;
		for (int i = 0; i < transitions(); i++)
		{
			cout << "   ";
			_transition[i].printInfo();
		}
	}
	Transition* transition(int index)
	{
		return &_transition[index];
	}
	string name()
	{
		return _name;
	}
	int index()
	{
		return _index;
	}
	int transitions()
	{
		return _transition.size();
	}
	bool isFinal()
	{
		return _final;
	}
	bool hasEpsilon()
	{
		return _hasEpsilon;
	}
};
//every object from class Transition points to an object from class Position
//every Position object is a package of Transition objects
//this way the positions are connected by eachother
class Automation
{
private:
	std::vector<Position> _position;
	bool hasEpsilonCycled(std::vector<Transition*>* crrPath)
	{//protection from a full empty cycle
		int pathLength = (int)(*crrPath).size();
		int* helper = new int[pathLength];
		for (int i = 0, crrLen = 0; i < pathLength; i++)
		{
			if ((*crrPath)[i]->symbol() != '0')
				crrLen++;
			helper[i] = crrLen;
		}
		for (int i = 0; i < pathLength - 1; i++)
		{
			if ((*crrPath)[i] == (*crrPath)[pathLength - 1] &&//both parts of the path are the same (comparing pointers!)
				(*crrPath)[i]->symbol() == '0' &&//and are both empty
				helper[i] == helper[pathLength - 1])//and the word has not gotten longer
			{
				delete helper;
				return true;//yep- we have made a full empty cycle
			}
		}
		delete helper;
		return false;
	}
	bool _recognize(char* word, int crrLength, Position* position, std::vector<Transition*>* crrPath)//word for checking, current position, traversed path so far
	{//recognising a word
		//after every transition, a symbol is "deleted" from the word, and then is sent to be recognised further
		cout << "Received word: " << word << " At position: " << position->name() << endl;//for debugging
		if (crrLength == 0 && position->isFinal())
			return true;//all symbols are deleted and we are on a final position
		else
			for (int i = 0; i < position->transitions(); i++)
			{
			crrPath->push_back(position->transition(i));//memorizing the path so far, for protection from a full empty cycle
			if (hasEpsilonCycled(crrPath) == false)//lets check if we have done one
				if (word[0] == position->transition(i)->symbol() ||
					position->transition(i)->symbol() == '0')//if there is an empty transition
					//there could be two transitions with the same symbol, leading to different positions
					if (_recognize(word + position->transition(i)->length(),//instead of "position->transition[i].length()" you can put the number one (1), but this way the empty transition will be treated as an usual transition
						strlen(word + position->transition(i)->length()),
						position->transition(i)->target(),
						crrPath))//remove the first symbol and then continue regocniizng further
						return true;//the program is here, is has chosen the right transition and havs recognized the word
			crrPath->pop_back();//the work after choosing the last transition is done
			/*
			return recognize(word + 1, strlen(word + 1), position->transition[i].target());
			//works only if the automation is determined
			*/
			}
		/*//for debugging
		//if the program has not yet passed "return true;" means that the word is not recognised due to the following reasons:
		if (crrLength != 0)
		cout << "No transtion for the current symbol from the current position." << endl;
		if (crrLength == 0)
		cout << "Reached end of word at a non-final postition." << endl;
		*/
		return false; //either the word is not recognized, or the program has taken a wrong turn (if it is undetermined)
	}
	void _generate(string* crrWord, int size, Position* position, std::vector<Transition*>* crrPath)//generating all word that are N digits long
	{
		//cout << "Current position: " << position->getName() << endl;//debugging
		if (position->transitions() == 0 && size != 0)
		{
			//cout << "No transitions from position." << endl;//more debugging
			return;
		}
		if (size == 0)
		{//the word is as long as it is supposed to be
			if (position->isFinal())
				cout << *crrWord << endl;//the program has reached a final position
			if (position->hasEpsilon() == false)//there is no empty transition to a potentialy final position
				return;
			return;//the program is not at a final position
		}
		for (int i = 0; i < position->transitions(); i++)
		{
			crrPath->push_back(position->transition(i));//memorizing the path so far- in case of a full empty cycle
			if (hasEpsilonCycled(crrPath) == false)//checking if one has been made
			{
				string newPrefix;
				if (position->transition(i)->symbol() == '0')//if an empty transition is about to be made
					newPrefix = "";
				else
					newPrefix = position->transition(i)->symbol();
				*crrWord = *crrWord + newPrefix;//adding a new symbol to what has been generated so far
				_generate(crrWord,
					size - position->transition(i)->length(),//"ordering" all prefixes to be generated
					position->transition(i)->target(),
					crrPath);
				if (position->transition(i)->length() != 0)
					crrWord->pop_back();//work with this symbol is done, it is removed
			}
			crrPath->pop_back();//work after the last chosen transition has been done
		}
	}
	int _longestWordLength(Position* position, Transition* fromTrans)
	{
		int longestWord = 0;//if a starting value is not set, a random one will be asserted
		int potentialLW;
		for (int i = 0; i < position->transitions(); i++)//if there are no transitions from the position, it will be returned that, the number of all reconizable words from there are zero
		{
			potentialLW = _longestWordLength(position->transition(i)->target(), position->transition(i));
			if (longestWord < potentialLW)
				longestWord = potentialLW;
		}
		if (fromTrans->symbol() == '0')
			return longestWord;//last transition has been empty, the word has not gotten longer
		else
			return longestWord + 1;//from the last position to here, only one symbol has been added
	}
	bool _hasCycle(std::vector<Position*>* path, Position* position)
	{
		path->push_back(position);
		bool has = false;//if it ramins "false" it means there is no found cycle
		/*
		//debugging
		cout << "Current path: ";
		for (int i = 0; i < (int)(path->size()); i++)
		cout << (*path)[i] << " ";
		cout << endl;
		*/
		for (int i = 0; i < (int)(path->size()) - 1; i++)
		{
			if (position == (*path)[i])
			{
				has = true;
				break;
			}
		}//no cycle so far, but it is not know if there are such somewhere else
		if (has == false)
			for (int i = 0; i < position->transitions(); i++)
			{
			if (_hasCycle(path, position->transition(i)->target()))
			{
				has = true;
				break;
			}
			}
		path->pop_back();
		return has;
	}
	bool isTransSet(Position* from, Transition* potTrans)
	{
		for (int i = 0; i < from->transitions(); i++)
		{
			if (from->transition(i)->symbol() == potTrans->symbol() &&
				from->transition(i)->target() == potTrans->target())
				return true;
		}
		return false;
	}
public:
	Automation(bool isStartFinal)
	{
		Position start(0, "start", isStartFinal);
		_position.push_back(start);
	}
	Automation(Automation* a, Automation* b, char connectionType)
	{
		if (connectionType == 'u')
		{//обединение
			Position start(0, "start",
				a->position(0)->isFinal() || b->position(0)->isFinal());//if one of the starting positions of the automations is also a final, the new starting position must be final too
			_position.push_back(start);
			for (int i = 0; i < a->positions(); i++)
				createPosition(a->position(i)->isFinal());
			for (int i = 0; i < b->positions(); i++)
				createPosition(b->position(i)->isFinal());
			for (int i = 0; i < a->positions(); i++)
			{
				for (int t = 0; t < a->position(i)->transitions(); t++)
					setTransition(a->position(i)->index() + 1,//+1 there is a new starting position, so all the positions of the first automation are going to have indexes that are larger with one (1)
					a->position(i)->transition(t)->target()->index() + 1,
					a->position(i)->transition(t)->symbol());
			}
			for (int i = 0; i < b->positions(); i++)
			{
				for (int t = 0; t < b->position(i)->transitions(); t++)
					setTransition(b->position(i)->index() + a->positions() + 1,//the number of the positions of the first automation plus the new starting position
					b->position(i)->transition(t)->target()->index() + a->positions() + 1,
					b->position(i)->transition(t)->symbol());
			}
			//copying the transitions of the starting positions onto the new starting position
			for (int i = 0; i < a->position(0)->transitions(); i++)
				setTransition(0, a->position(0)->transition(i)->target()->index() + 1,
				a->position(0)->transition(i)->symbol());
			for (int i = 0; i < b->position(0)->transitions(); i++)
				setTransition(0, b->position(0)->transition(i)->target()->index() + a->positions() + 1,
				b->position(0)->transition(i)->symbol());
		}
		//==========================================================================================================================
		else if (connectionType == '.')
		{
			cout << "No such operation!\n";
		}
	}
	Automation(Automation* original, char copyType)
	{
		//still in development
	}
	void createPosition(bool isFinal)
	{
		Position tempPos(_position.size(), std::to_string(_position.size()), isFinal);
		_position.push_back(tempPos);
	}
	void setTransition(int from, int to, char symbol)
	{
		if (from == to && symbol == '0')
			cout << "Empty cycle- connection is unacceptable!\n";
		else
		{
			if (from < positions() || to < positions())
			{
				Transition tempTrans;
				tempTrans.setStuff(&_position[to], symbol, _position[to].name());
				if (isTransSet(&_position[from], &tempTrans) == false)
					_position[from].addTransition(&tempTrans);
				else
					cout << "Transition already set!\n";
			}
			else
				cout << "No such position/positions!\n";
		}
	}
	bool recognize(char* word)
	{
		std::vector<Transition*> crrPath;
		return _recognize(word, strlen(word), &_position[0], &crrPath);
	}
	void generate(int size)
	{
		std::vector<Transition*> crrPath;
		string generated = "";
		_generate(&generated, size, &_position[0], &crrPath);
	}
	int longestWordLength()
	{
		if (!hasCycle())
		{
			int longestWord = 0;//a random value will be set, if one is not explicitly set
			int potentialLW;
			for (int i = 0; i < _position[0].transitions(); i++)
			{
				potentialLW = _longestWordLength(_position[0].transition(i)->target(), _position[0].transition(i));
				if (longestWord < potentialLW)
					longestWord = potentialLW;
			}
			return longestWord;
		}
		else
			return -1;
	}
	void generateAll()
	{
		if (!hasCycle())
		{
			int longest = longestWordLength();
			for (int i = 1; i <= longest; i++)
				generate(i);
		}
		else
			cout << "There is a cycle in the automation, all the words are infinite." << endl;
	}
	void generateAll(int maxSize)
	{
		for (int i = 1; i <= maxSize; i++)
			generate(i);
	}
	bool hasCycle()
	{
		std::vector<Position*> path;
		path.push_back(&(_position[0]));
		for (int i = 0; i < _position[0].transitions(); i++)
		{
			if (_hasCycle(&path, _position[0].transition(i)->target()))
				return true;
		}
		return false;
	}
	void printInfo()
	{
		for (int i = 0; i < positions(); i++)
			_position[i].printInfo();
	}
	Position* position(int index)
	{
		return &_position[index];
	}
	int positions()
	{
		return _position.size();
	}
};
int main()
{
	Automation automation(true);
	automation.createPosition(false);//1
	automation.createPosition(true);//2
	automation.createPosition(false);//3
	automation.setTransition(0, 1, 'a');
	automation.setTransition(0, 3, 'b');
	automation.setTransition(1, 0, 'a');
	automation.setTransition(1, 2, 'b');
	automation.setTransition(2, 1, 'a');
	automation.setTransition(2, 3, 'b');
	automation.setTransition(3, 0, 'b');
	automation.setTransition(3, 2, 'a');
	//testing the full empty cycle protection
	//automation.setTransition(1, 2, '0');
	//automation.setTransition(2, 1, '0');
	
	/*
	Automation a(false);//0
	a.createPosition(false);//1
	a.createPosition(false);//2
	a.createPosition(false);//3
	a.createPosition(true);//4
	a.setTransition(0, 1, 'a');
	a.setTransition(0, 2, 'b');
	a.setTransition(1, 3, 'c');
	a.setTransition(2, 3, 'a');
	a.setTransition(3, 4, 'b');
	a.setTransition(4, 0, 'a');
	a.setTransition(4, 0, 'b');
	Automation b(false);//0
	b.createPosition(false);//1
	b.createPosition(false);//2
	b.createPosition(false);//3
	b.createPosition(false);//4
	b.createPosition(true);//5
	b.setTransition(0, 1, 'a');
	b.setTransition(1, 2, 'b');
	b.setTransition(2, 3, 'c');
	b.setTransition(3, 4, 'a');
	b.setTransition(3, 0, 'c');
	b.setTransition(4, 5, 'b');
	b.setTransition(5, 1, 'a');
	Automation automation(&a, &b, 'u');
	*/
	/*
	Automation automation(false);//0
	automation.createPosition(false);//1
	automation.createPosition(false);//2
	automation.createPosition(false);//3
	automation.createPosition(false);//4
	automation.createPosition(false);//5
	automation.createPosition(false);//6
	automation.createPosition(true);//7
	automation.createPosition(false);//8
	automation.createPosition(false);//9
	automation.createPosition(true);//10
	automation.createPosition(true);//11
	automation.setTransition(0, 1, 'a');
	automation.setTransition(0, 2, '0');
	automation.setTransition(0, 3, 'b');
	automation.setTransition(1, 1, '0');//this type of transition is not allowed
	automation.setTransition(1, 4, 'b');
	automation.setTransition(1, 5, 'a');
	automation.setTransition(2, 1, 'a');
	automation.setTransition(2, 5, 'c');
	automation.setTransition(3, 6, '0');
	automation.setTransition(3, 2, 'c');
	automation.setTransition(3, 2, 'c');
	automation.setTransition(3, 11, 'a');
	automation.setTransition(4, 7, 'a');
	automation.setTransition(5, 4, '0');
	automation.setTransition(5, 7, 'b');
	automation.setTransition(5, 8, 'c');
	automation.setTransition(5, 6, 'c');
	automation.setTransition(6, 8, '0');
	automation.setTransition(6, 9, 'a');
	automation.setTransition(6, 11, 'b');
	automation.setTransition(7, 10, 'c');
	automation.setTransition(8, 7, 'a');
	automation.setTransition(8, 9, 'a');
	//automation.setTransition(8, 2, 'c');//a bigger cycle
	automation.setTransition(8, 10, 'b');
	automation.setTransition(9, 10, 'a');
	//automation.setTransition(10, 1, 'a');//cycle beginning-to-end
	automation.setTransition(9, 11, 'c');
	//testing the full empty cycle protection
	//automation.setTransition(1, 2, '0');
	//automation.setTransition(2, 3, '0');
	//automation.setTransition(3, 4, '0');
	//automation.setTransition(4, 5, '0');
	//automation.setTransition(5, 1, '0');
	*/
	automation.printInfo();
	cout << automation.recognize("baabaab") << endl;
	int longestWord = automation.longestWordLength();
	if (longestWord != -1)
	{
		cout << "Longest possible word: " << longestWord << endl;
		automation.generateAll();
	}
	else
	{
		cout << "There is no longest word.\n";
		automation.generateAll(6);
	}
	system("pause");
	return 0;
}
