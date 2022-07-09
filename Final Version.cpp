#include<bits/stdc++.h>
#define MAXN 30
#define TESTFILE "1.nfa"
using namespace std;

struct Dstate			//состояни в DFA
{
	char name;			//Название состояния（DFA）
	bool isStart;
	bool isEnd;			//Является ли это конечным состоянием（DFA）
	string transRow;    //Последовательность состояний, в которые превратится это состояние после прохождения через каждый символ в алфавите（DFA）
	set<int> element;	//После завершения процесса детерминации набор состояний, содержащихся в новом состоянии（DFA）
	int color;			//Номер, используемый для программы канонизация
};


//////////////////////////////////////////////L//////////////////////////////////////////////////////////////////
class L {
public:
	int state_num;						//Количество состояний（NFA）
	int start_state_num;
	set<int> start_state;
	int end_state_num;					//Количество состояний в наборе конечных состояний（NFA）
	set<int> end_state;					//Набор конечных состояний（NFA）
	string letters;						//набор символов（NFA）
	string letters_null;				//набор символов + ε（NFA）
	bool NFA[MAXN][MAXN][MAXN];			//NFA
	vector<Dstate> DFA_States;			//Набор состояний (DFA)
	vector<Dstate> DFA_MIN_States;		//Набор конечных состояний(minDFA)

	L(){
		state_num = 0;
		start_state_num = 0;
		state_num = 0;						
		start_state_num = 0;
		end_state_num = 0;					
		letters = "";					
		letters_null = "`"; // ` --- 空字符 96

		for (int i = 0;i < MAXN; i++)for (int j= 0;j < MAXN; j++)for (int k = 0; k< MAXN; k++) NFA[i][j][k] = false;
	};

	void readFile();
	////////////////////////////////процедура детерминизации//////////////////////////////////////////
	set<int> Closure(set<int> T);
	set<int> Move(set<int> T, char trans);
	void Expand(size_t state);
	void printDKA();
	void toDKA();
	//////////////////////////////////канонизация/////////////////////////////////////////////////////////////////
	void printKDKA();  // символ "K" - Это канонизация
	void toKDKA();     // символ "K" - Это канонизация
};




/////////////////////////////////////////LR/////////////////////////////////////////////////////////////////////

class LR {
public:
	int Rstate_num;						//Количество состояний（NFA）
	int Rstart_state_num;
	set<int> Rstart_state;
	int Rend_state_num;					//Количество состояний в наборе конечных состояний（NFA）
	set<int> Rend_state;					//Набор конечных состояний（NFA）
	string Rletters;						//набор символов（NFA）
	string Rletters_null;				//набор символов + ε（NFA）
	bool RNFA[MAXN][MAXN][MAXN];			//RNFA
	vector<Dstate> RDFA_States;			//Набор состояний (DFA)
	vector<Dstate> RDFA_MIN_States;		//Набор конечных состояний(minDFA)

	LR() {
		Rstate_num = 0;
		Rstart_state_num = 0;
		Rstate_num = 0;
		Rstart_state_num = 0;
		Rend_state_num = 0;
		Rletters = "";
		Rletters_null = "`"; // ` --- 空字符 96

		for (int i = 0; i < MAXN; i++)for (int j = 0; j < MAXN; j++)for (int k = 0; k < MAXN; k++) RNFA[i][j][k] = false;
	};
	void NFA_to_RNFA(L a);
	
	////////////////////////////////процедура детерминизации//////////////////////////////////////////
	set<int> RClosure(set<int> T);
	set<int> RMove(set<int> T, char trans);
	void RExpand(size_t state);
	void printRDKA();
	void toRDKA();
	//////////////////////////////////канонизация/////////////////////////////////////////////////////////////////
	void printKRDKA();  // символ "K" - Это канонизация , символ "R" - Это зеркальный
	void toKRDKA();     // символ "K" - Это канонизация , символ "R" - Это зеркальный
	
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char getAlpha(size_t i)
{
	return char(i) + 'A';
}


void L::readFile()
{
	ifstream in(TESTFILE);
	int start, end, state;
	char trans;

	in >> state_num;
	cout << "Maximum state number:" << state_num << endl;
	state_num++;

	in >> letters;
	cout << "Character set:" << letters << endl;
	letters_null = letters + '`';

	in >> start_state_num;
	cout << "The number of collections of the start state set: " << start_state_num << endl;
	cout << "Please enter all the states in the start state set：";
	for (int i = 0; i < start_state_num; i++)
	{
		in >> state;
		start_state.insert(state);
		cout << state << " ";
	}
	cout << endl;

	in >> end_state_num;
	cout << "The number of collections of the termination state set: " << end_state_num << endl;
	cout << "Please enter all the states in the termination state set：";
	for (int i = 0; i < end_state_num; i++)
	{
		in >> state;
		end_state.insert(state);
		cout << state << " ";
	}
	cout << endl;

	while (in >> start >> trans >> end)
	{
		cout << start << " " << trans << " " << end << endl;
		NFA[start][(int)(trans)-96][end] = true;
	}
	cout << endl;
	in.close();
}





set<int> L::Closure(set<int> T)   //Алгоритм epsilon_Closure: набор состояний NFA, которые могут быть достигнуты только с помощью ε-преобразования, начиная с некоторого состояния NFA s в T
{
	set<int> closure = T;
	stack<int> stack;
	for (int i : T) stack.push(i);

	while (!stack.empty())   //栈每次弹出首位，并以首位进行扩展
	{
		int cur = stack.top();
		stack.pop();

		for (int i = 0; i < state_num; i++)
			if (NFA[cur][int('`')-96][i] && closure.find(i) == closure.end())
			{
				closure.insert(i);
				stack.push(i);
			}
	}
	return closure;
}

set<int> L::Move(set<int> T, char trans)		// После того, как все состояния в наборе состояний "T" передаются символом “trans”, получается новое состояние.
{
	set<int> extension;
	for (int i : T)
	{
		for (int j = 0; j <= state_num; ++j)
		{
			if (NFA[i][(int)(trans)-96][j] == 1)
				extension.insert(j);
		}
	}
	return extension;
}

void L::Expand(size_t state)  //Обработать 'state'-й набор состояний (новое состояние в DFA)
{
	vector<int> intersection;
	intersection.resize(min(end_state.size(), DFA_States[state].element.size()));

	// Узнайте, есть ли пересечение между набором состояний и конечным набором
	auto it = set_intersection(end_state.begin(), end_state.end(), DFA_States[state].element.begin(), DFA_States[state].element.end(), intersection.begin());
	//Если набор состояний содержит конечное состояние
	DFA_States[state].isEnd = (it != intersection.begin());

	vector<int> intersection2;
	intersection2.resize(min(start_state.size(), DFA_States[state].element.size()));
	auto it2 = set_intersection(start_state.begin(), start_state.end(), DFA_States[state].element.begin(), DFA_States[state].element.end(), intersection2.begin());
	DFA_States[state].isStart = (it2 != intersection2.begin());



	for (size_t x = 0; x < letters.length(); ++x)//Для каждого символа в наборе символов
	{
		set<int> u = Closure(Move(DFA_States[state].element, letters[x]));

		if (u.empty()) //Если это пустой набор
		{
			DFA_States[state].transRow.push_back('?');
			continue;
		}

		bool flag = false; // Определите, появилось ли новое подмножество
		for (size_t i = 0; i < DFA_States.size(); ++i)//Определите, является ли результирующий набор состояний новым набором, который еще не появился
		{
			if (DFA_States[i].element == u) //Если нет
			{
				flag = true;
				DFA_States[state].transRow.push_back(getAlpha(i)); // Добавьте такое отношение передачи
				break;
			}
		}
		if (!flag) //Если это так
		{
			DFA_States.push_back(Dstate{ getAlpha(DFA_States.size()), false ,false, "", u }); //Создайте это новое состояние
			DFA_States[state].transRow.push_back(getAlpha(DFA_States.size() - 1));
		}
	}
}
void L::printDKA()
{
	cout << "a total of " << DFA_States.size() << " subsets were constructed as follows:" << endl;
	for (size_t i = 0; i < DFA_States.size(); ++i)
	{
		cout << getAlpha(i) << " = <";
		for (int state : DFA_States[i].element) //对于新状态(子集)中的原始状态
			cout << setw(2) << state << " ";
		cout << ">";

		if (DFA_States[i].isStart) cout << "\t[Start state]";
		if (DFA_States[i].isEnd) cout << "\t[End state]";
		cout << endl;
	}

	cout << endl << "The table of state transitions for ДКА is as follows:" << endl;
	for (char letter : letters)
		cout << "\t" << letter;
	cout << endl;

	for (size_t i = 0; i < DFA_States.size(); ++i)
	{
		cout << DFA_States[i].name << "\t";
		for (char c : DFA_States[i].transRow)
			cout << c << '\t';
		cout << endl;
	}
}
void L::toDKA()
{
	set<int> s0;  // Набор начальных состояний
	for (auto start : start_state) {
		s0.insert(start);
	}

	s0 = Closure(s0);  // Набор начальных состояний генерируется после передачи символа ε, генерируется новый набор состояний
	DFA_States.push_back(Dstate{ getAlpha(0),false , false, "", s0 });

	size_t done = 0;
	while (done < DFA_States.size())
	{
		Expand(done++); //Обрабатывайте вновь сгенерированные наборы состояний один за другим
	}

	printDKA();
}



void L::printKDKA()
{
	cout << "\n Кнонический автомат：" << endl << '\t';
	for (char c : letters)
		cout << c << '\t';
	cout << endl;

	for (Dstate state : DFA_MIN_States)
	{
		cout << state.name << '\t';
		for (char c : state.transRow)
			cout << c << '\t';
		cout << endl;
	}
	cout << endl;
}

void L::toKDKA() {
	int colors[MAXN][MAXN][MAXN];
	memset(colors, 0, sizeof(colors));

	DFA_MIN_States = DFA_States;

	//Первоначально разделен на <непринятое состояние: № 1> <принятое состояние: № 2>
	for (size_t i = 0; i < DFA_MIN_States.size(); ++i)
		DFA_MIN_States[i].color = (DFA_MIN_States[i].isEnd ? 2 : 1);
	int maxColor = 2;

	// DFA_MIN_States[n] -----letters[i]------>  DFA_MIN_States[j]，Разделите каждое состояние на группы
	for (size_t n = 0; n < DFA_MIN_States.size(); ++n)				//Для каждого состояние1
	{
		for (size_t i = 0; i < letters.length(); ++i)				//Для каждого символ
		{
			for (size_t j = 0; j < DFA_MIN_States.size(); ++j)		//Для каждого состояние2
			{
				// получить color
				int curNumber = DFA_MIN_States[j].color;
				char des = DFA_MIN_States[j].transRow[i]; // Получить название состояния, в которое передается i-й символ
				int desNumber = (des == '?') ? 100 : DFA_MIN_States[des - 'A'].color;
				//Если это другая группа после переноса
				if (curNumber != desNumber)
				{
					//для curNumber ---letters[i]--->desNumber ，При переводе в новую группу（desNumber）
					if (colors[curNumber][i][desNumber] == 0)
					{
						++maxColor;
						DFA_MIN_States[j].color = maxColor;
						colors[curNumber][i][desNumber] = maxColor;
					}
					else DFA_MIN_States[j].color = colors[curNumber][i][desNumber];
				}
			}
		}
	}



	for (auto it0 = DFA_MIN_States.begin(); it0 != DFA_MIN_States.end(); ++it0)
	{
		for (auto it1 = (it0 + 1); it1 != DFA_MIN_States.end(); ++it1)
		{
			// Если color отличается, то они не эквивалентны, тогда пропустите
			if (it0->color != it1->color) continue;


			char tmp1 = it0->name;
			char tmp2 = it1->name;
			cout << "Equivalent state：" << tmp1 << " " << tmp2 << endl;

			
			for (int state : it1->element) {
				it0->element.insert(state);
			}
			
			for (size_t x = 0; x < DFA_MIN_States.size(); ++x)
				replace(DFA_MIN_States[x].transRow.begin(), DFA_MIN_States[x].transRow.end(), tmp2, tmp1);

			it1 = DFA_MIN_States.erase(it1);
			it1--;
		}
	}

	printKDKA();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LR::NFA_to_RNFA(L a) {
	//Поменяйте местами стартовых состояний и финальных состояний
	Rstate_num = a.state_num;
	Rletters = a.letters;
	Rletters_null = a.letters_null;

	Rstart_state_num = a.end_state_num;
	Rend_state_num = a.start_state_num;

	Rstart_state = a.end_state;
	Rend_state = a.start_state;

	//Измените функцию перехода   i --j--> k  ===  k --j--> i
	for (int i = 0; i < MAXN; i++)
		for (int j = 0; j < MAXN; j++)
			for (int k = 0; k < MAXN; k++)
				if (a.NFA[i][j][k] == true)
					RNFA[k][j][i] = true;

	// print LR
	cout << "LR" << endl;
	cout << "Maximum state number:" << Rstate_num-1 << endl;
	cout << "Character set:" << Rletters << endl;
	cout << "The number of collections of the start state set: " << Rstart_state_num << endl;
	cout << "Please enter all the states in the start state set：";
	for (auto state : Rstart_state)
	{
		cout << state << " ";
	}
	cout << endl;

	cout << "The number of collections of the termination state set: " << Rend_state_num << endl;
	cout << "Please enter all the states in the termination state set：";
	for (auto state : Rend_state)
	{
		cout << state << " ";
	}
	cout << endl;

	for (int i = 0; i < MAXN; i++)
		for (int j = 0; j < MAXN; j++)
			for (int k = 0; k < MAXN; k++)
				if (RNFA[i][j][k] == true)
					cout << i << " " << (char)(j + 96) << " " << k << endl;
	cout << endl;
}


set<int> LR::RClosure(set<int> T)   //Алгоритм epsilon_Closure: набор состояний NFA, которые могут быть достигнуты только с помощью ε-преобразования, начиная с некоторого состояния NFA s в T
{
	set<int> closure = T;
	stack<int> stack;
	for (int i : T) stack.push(i);

	while (!stack.empty())   //栈每次弹出首位，并以首位进行扩展
	{
		int cur = stack.top();
		stack.pop();

		for (int i = 0; i < Rstate_num; i++)
			if (RNFA[cur][int('`') - 96][i] && closure.find(i) == closure.end())
			{
				closure.insert(i);
				stack.push(i);
			}
	}
	return closure;
}

set<int> LR::RMove(set<int> T, char trans)		// После того, как все состояния в наборе состояний "T" передаются символом “trans”, получается новое состояние.
{
	set<int> extension;
	for (int i : T)
	{
		for (int j = 0; j <= Rstate_num; ++j)
		{
			if (RNFA[i][(int)(trans)-96][j] == 1)
				extension.insert(j);
		}
	}
	return extension;
}

void LR::RExpand(size_t state)  //Обработать 'state'-й набор состояний (новое состояние в DFA)
{
	vector<int> intersection;
	intersection.resize(min(Rend_state.size(), RDFA_States[state].element.size()));

	// Узнайте, есть ли пересечение между набором состояний и конечным набором
	auto it = set_intersection(Rend_state.begin(), Rend_state.end(), RDFA_States[state].element.begin(), RDFA_States[state].element.end(), intersection.begin());
	//Если набор состояний содержит конечное состояние
	RDFA_States[state].isEnd = (it != intersection.begin());

	vector<int> intersection2;
	intersection2.resize(min(Rstart_state.size(), RDFA_States[state].element.size()));
	auto it2 = set_intersection(Rstart_state.begin(), Rstart_state.end(), RDFA_States[state].element.begin(), RDFA_States[state].element.end(), intersection2.begin());
	RDFA_States[state].isStart = (it2 != intersection2.begin());



	for (size_t x = 0; x < Rletters.length(); ++x)//Для каждого символа в наборе символов
	{
		set<int> u = RClosure(RMove(RDFA_States[state].element, Rletters[x]));

		if (u.empty()) //Если это пустой набор
		{
			RDFA_States[state].transRow.push_back('?');
			continue;
		}

		bool flag = false; // Определите, появилось ли новое подмножество
		for (size_t i = 0; i < RDFA_States.size(); ++i)//Определите, является ли результирующий набор состояний новым набором, который еще не появился
		{
			if (RDFA_States[i].element == u) //Если нет
			{
				flag = true;
				RDFA_States[state].transRow.push_back(getAlpha(i)); // Добавьте такое отношение передачи
				break;
			}
		}
		if (!flag) //Если это так
		{
			RDFA_States.push_back(Dstate{ getAlpha(RDFA_States.size()), false ,false, "", u }); //Создайте это новое состояние
			RDFA_States[state].transRow.push_back(getAlpha(RDFA_States.size() - 1));
		}
	}


}


void  LR::toRDKA()
{
	set<int> s0;  // Набор начальных состояний
	for (auto start : Rstart_state) {
		s0.insert(start);
	}

	s0 = RClosure(s0);  // Набор начальных состояний генерируется после передачи символа ε, генерируется новый набор состояний
	RDFA_States.push_back(Dstate{ getAlpha(0),false , false, "", s0 });

	size_t done = 0;
	while (done < RDFA_States.size())
	{
		RExpand(done++); //Обрабатывайте вновь сгенерированные наборы состояний один за другим
	}

	printRDKA();
	
}


void LR::printRDKA()
{
	
	cout << "a total of " << RDFA_States.size() << "  subsets were constructed, as follows:" << endl;
	for (size_t i = 0; i < RDFA_States.size(); ++i)
	{
		cout << getAlpha(i) << " = <";
		for (int state : RDFA_States[i].element) //对于新状态(子集)中的原始状态
			cout << setw(2) << state << " ";
		cout << ">";

		if (RDFA_States[i].isStart) cout << "\t[Start state]";
		if (RDFA_States[i].isEnd) cout << "\t[End state]";
		cout << endl;
	}

	cout << endl << "The Зеркальный ДКА state transition table is as follows：" << endl;
	for (char letter : Rletters)
		cout << "\t" << letter;
	cout << endl;

	for (size_t i = 0; i < RDFA_States.size(); ++i)
	{
		cout << RDFA_States[i].name << "\t";
		for (char c : RDFA_States[i].transRow)
			cout << c << '\t';
		cout << endl;
	}
}


void LR::toKRDKA() {
	int colors[MAXN][MAXN][MAXN];
	memset(colors, 0, sizeof(colors));

	RDFA_MIN_States = RDFA_States;

	//Первоначально разделен на <непринятое состояние: № 1> <принятое состояние: № 2>
	for (size_t i = 0; i < RDFA_MIN_States.size(); ++i)
		RDFA_MIN_States[i].color = (RDFA_MIN_States[i].isEnd ? 2 : 1);
	int maxColor = 2;

	// DFA_MIN_States[n] -----letters[i]------>  DFA_MIN_States[j]，Разделите каждое состояние на группы
	for (size_t n = 0; n < RDFA_MIN_States.size(); ++n)				//Для каждого состояние1
	{
		for (size_t i = 0; i < Rletters.length(); ++i)				//Для каждого символ
		{
			for (size_t j = 0; j < RDFA_MIN_States.size(); ++j)		//Для каждого состояние2
			{
				// получить color
				int curNumber = RDFA_MIN_States[j].color;
				char des = RDFA_MIN_States[j].transRow[i]; // Получить название состояния, в которое передается i-й символ
				int desNumber = (des == '?') ? 100 : RDFA_MIN_States[des - 'A'].color;



				//Если это другая группа после переноса
				if (curNumber != desNumber)
				{
					//для curNumber ---letters[i]--->desNumber ，При переводе в новую группу（desNumber）
					if (colors[curNumber][i][desNumber] == 0)
					{
						++maxColor;
						RDFA_MIN_States[j].color = maxColor;
						colors[curNumber][i][desNumber] = maxColor;
					}
					else RDFA_MIN_States[j].color = colors[curNumber][i][desNumber];
				}
			}
		}
	}



	for (auto it0 = RDFA_MIN_States.begin(); it0 != RDFA_MIN_States.end(); ++it0)
	{
		for (auto it1 = (it0 + 1); it1 != RDFA_MIN_States.end(); ++it1)
		{
			// Если color отличается, то они не эквивалентны, тогда пропустите
			if (it0->color != it1->color) continue;


			char tmp1 = it0->name;
			char tmp2 = it1->name;
			cout << "Equivalent state：" << tmp1 << " " << tmp2 << endl;

			for (size_t x = 0; x < RDFA_MIN_States.size(); ++x)
				replace(RDFA_MIN_States[x].transRow.begin(), RDFA_MIN_States[x].transRow.end(), tmp2, tmp1);

			it1 = RDFA_MIN_States.erase(it1);
			it1--;
		}
	}

	printKRDKA();
}

void LR::printKRDKA()
{
	cout << "\n Зеркальный автомат в канонической форме：" << endl << '\t';
	for (char c : Rletters)
		cout << c << '\t';
	cout << endl;

	for (Dstate state : RDFA_MIN_States)
	{
		cout << state.name << '\t';
		for (char c : state.transRow)
			cout << c << '\t';
		cout << endl;
	}
	cout << endl;
}

/////////////////////////////////////////////////////////////////////////////BA///////////////////////////////////////////////////////////////
struct BAstate
{
	pair<char, char> name;
	bool isStart;
	bool isEnd;
	vector<vector<pair<char, char>>> trans;
	//pair<char, char> trans[MAXN][MAXN];

};
vector<BAstate> BA;


void createBA(L L, LR LR) {
	vector<Dstate> a = L.DFA_MIN_States;
	vector<Dstate> b = LR.RDFA_MIN_States;

	for (size_t i = 0; i < a.size(); ++i) {
		bool isStart = false;
		if (a[i].isStart) isStart = true;
		//pair<char, char> tran[MAXN][MAXN];

		for (size_t j = 0; j < b.size(); ++j) {
			bool isEnd = false;
			if (b[j].isStart) isEnd = true;

			vector<int> intersection;
			intersection.resize(min(a[i].element.size(), b[j].element.size()));
			auto it = set_intersection(a[i].element.begin(), a[i].element.end(), b[j].element.begin(), b[j].element.end(), intersection.begin());
			if (it != intersection.begin()) { // пересечение
				pair<char, char> name = { a[i].name,b[j].name };

				vector<vector<pair<char, char>>> Tes;
				for (size_t n = 0; n < L.letters.length(); ++n) {	
					vector<pair<char, char>> tes;
					pair<char, char> t = { '?','?' };
					
					char des1 = a[i].transRow[n];
					for (size_t m = 0; m < b.size(); ++m) {
						char des2 = b[m].transRow[n];
						if(des1 != '?' && des2 != '?' && des2 == b[j].name)
							t={ des1, b[m].name };

						tes.push_back(t);
						//tran[n][m] = t;
					}
					Tes.push_back(tes);
				}		
				vector<vector<pair<char, char>>> tr;
				BA.push_back(BAstate{ { a[i].name,b[j].name },isStart,isEnd, Tes});
			}
		}
	}

	//print BA
	cout << "/////////////////////////// базисный автомат:////////////////////////////////" << endl;
	for (int i = 0; i < BA.size(); i++) {
		cout << "(" << BA[i].name.first << "," << BA[i].name.second << ")" << "\t";
		if (BA[i].isStart) cout << "[Start State]\t";
		if (BA[i].isEnd) cout << "[End State]\t";

		cout << endl;
	}

	for (int i = 0; i < BA.size(); i++) {
		for (char c : L.letters)
			cout <<"         \t" << c << "        \t";
	
		cout << endl;
		cout << "(" << BA[i].name.first << "," << BA[i].name.second << ")" << '\t';
		for (size_t n = 0; n < L.letters.length(); ++n) {
			for (size_t m = 0; m < L.letters.length(); ++m){
				//if(BA[i].trans[n][m].first != '?' && BA[i].trans[n][m].second != '?')		
					cout << "(" << BA[i].trans[n][m].first << "," << BA[i].trans[n][m].second << ")" ;
				
			}

			cout << "\t        \t";
		}
		cout << endl;
		cout << endl;
	}

}


int main() {
	L a;
	LR ra;
	
	/////////////////////////////////Инициализируйте автомат//////////////////////////////////////////
	a.readFile();
	ra.NFA_to_RNFA(a);

	////////////////////////////////процедура детерминизации//////////////////////////////////////////
	cout << "L" << endl;
	a.toDKA();
	a.toKDKA();
	//////////////////////////////////канонизация/////////////////////////////////////////////////////////////////
	cout << "LR" << endl;
	ra.toRDKA();
	ra.toKRDKA();

	////////////////////////////////базисный автомат//////////////////////////////////////////
	cout << "BA:" << endl;
	createBA(a, ra);

	system("pause");
	return 0;
}