//============================================================================
// Name        : Monopoly.cpp
// Author      : Uriya Bartal
// Version     :
// Copyright   : Uriya Bartal
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <fstream>
#include <memory>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

//data defines:
//TODO: Enum?
#define PRICE 0
#define SERIES 1
#define RENT 2
#define HOUSES_1 3
#define HOUSES_2 4
#define HOUSES_3 5
#define HOUSES_4 6
#define HOTEL 7
#define MORTGAGE 8
#define HOUSE_COST 9
#define TYPE 10

//card types:
#define CHANCE 1
#define COMMUNITY_CHEST 2

//graphic defines
#define LEFT_MARGIN 6
#define RIGHT_MARGIN 6
#define TOP_MARGIN 6
#define BOTTOM_MARGIN 6
#define RIGHT_DICE_H_LOCATION 4
#define LEFT_DICE_H_LOCATION 7
#define RIGHT_DICE_V_LOCATION 4
#define LEFT_DICE_V_LOCATION 7

#define PURPLE 0
#define LIGHT_BLUE 1
#define VIOLET 2
#define ORANGE 3
#define RED 4
#define YELLOW 5
#define DARK_GREEN 6
#define DARK_BLUE 7

//class Player

class Player {
public:
	string name;
	bool isAlive;
	int money;
	int location;
	bool isInJail;
	int numOfJailFreeCards;
	int numOfRailroads;
	int numOfUtilities;
	int numOfDoubles;
	int numOfJailAttempts;
	int lastMove;
	int assetWorth;
	list<int> assets;

	Player() :
			name("DEFAULT"), isAlive(true), money(1500), location(0), isInJail(
					false), numOfJailFreeCards(0), numOfRailroads(0), numOfUtilities(
					0), numOfDoubles(0), numOfJailAttempts(0), lastMove(0), assetWorth(
					0) {
		srand(time(0));
	}

	;

	void setName(string name) {
		this->name = name;
	}
	const string getName() const {
		return name;
	}
	bool sell();
	bool mortgage();
	bool unmortgage();
	bool develop();
	bool undevelop();

	bool rollDice() {
		//return true if double was achieved!
		bool doubleDice;
		int dice1 = rand() % 6 + 1;
		int dice2 = rand() % 6 + 1;
		cout << "Dice Results: " << dice1 << " " << dice2 << endl;
		doubleDice = (dice1 == dice2);

		if (doubleDice) {
			numOfDoubles++;
		} else {
			numOfDoubles = 0;
			if (isInJail) {
				numOfJailAttempts++;
			}
		}

		if (numOfDoubles == 3) {
			cout << "3 Consecutive Doubles: GO TO JAIL" << endl;
			location = 10;
			isInJail = true;
			return true;
		}

		if ((!isInJail) || doubleDice) {
			advance(dice1 + dice2);
			lastMove = dice1 + dice2;
			isInJail = false;
			numOfJailAttempts = 0;
		}

		return doubleDice;
	}
	void advance(int steps) {
		if (location + steps > 39) {
			cout << "Passed GO: Collect 200" << endl;
			money += 200;
		}
		location = (location + steps) % 40;
	}

};

//Location Classes:
class Location {
private:
	string name;
public:
	virtual bool visit(Player& p) =0;

	void setName(string name) {
		this->name = name;
	}
	const string getName() const {
		return name;
	}
	Location(string name) :
			name(name) {
	}
	;
	~Location() = default;

	bool demandMoney(Player& payer, Player* payee, int sumToPay) {
		cout << "Sum to Pay: " << sumToPay << endl;
		if (payer.money < sumToPay) {
			cout << "insufficient funds!" << endl;
			return false;
			//TODO: resolve
		} else {
			payer.money -= sumToPay;
			payee->money += sumToPay;
			cout << "Payed!!" << endl;
			return true;
		}
	}
	virtual const string getOwner() const {
		return "";
	}
	virtual const string getShortName() const {
		return "---";
	}

};

Player defaultPlayer;

class Asset: public Location {
private:
	string shortName;

public:
	Player* owner;
	bool isMortgaged;
	int data[11];
	//Asset(): Location(string("")), owner(-1){};
	//~Asset()=default;
	Asset(string name) :
			Location(name), shortName(""), owner(nullptr), isMortgaged(false) {
	}

	void setShortName(string shortName) {
		this->shortName = shortName;
	}
	const string getOwner() const override {
		if (owner == nullptr) {
			return "";
		}
		return owner->name;
	}
	virtual const string getShortName() const {
		return shortName;
	}

	virtual bool pay(Player& payer)=0;

	bool visit(Player& p) {
		bool owned;
		cout << p.name << " Visiting " << Location::getName() << endl;
		if (owner == nullptr) {
			cout << "Not owned" << endl;
			owned = false;
		} else {
			cout << "Owned by " << owner->name << endl;
			owned = true;
			pay(p);
		}
		return (owned);
	}
	/*override {
	 if (owner == "") {
	 char c = '0';
	 int action = 0;
	 while (!action) {
	 cout << "Do you want to purchase " << Location::getName()
	 << " for " << Asset::getPrice() << " ?" << endl;
	 cin >> c;
	 switch (c) {
	 case 'y':
	 purchase(p);
	 action = 1;
	 break;
	 case 'n':
	 action = 1;
	 break;
	 case 'e':
	 exit(0);
	 break;
	 }
	 }
	 }
	 }
	 */

	virtual bool purchase(Player &p) {
		cout << "Base Purchase" << endl;
		if (p.money < data[PRICE]) {
			cout << "Insufficient Funds to buy " << Location::getName() << endl;
			return false;
		} else {
			p.money -= data[PRICE];
			cout << "Purchasing " << Location::getName() << "for "
					<< data[PRICE] << "$" << endl;
			p.assetWorth += data[PRICE];
			p.assets.push_back(p.location);
			owner = &p;
			return true;
		}
	}
	;
}
;

class Street: public Asset {
private:
	int development;
	bool series;

public:
	Street() :
			Asset("") {
	}
	;
	Street(string name) :
			Asset(name), development(RENT), series(false) {
	}
	;
	/*bool visit(Player& p) override {

	 }
	 ;*/
	void setSeries(bool newSeries) {
		series = newSeries;
	}
	const bool getSeries() const {
		return series;
	}

	bool pay(Player& payer) {
		int sumToPay = data[development];

		//check if has all series and no development
		if (development == RENT && series) {
			sumToPay *= 2;
		}
		//check if mortgaged
		if (Asset::isMortgaged) {
			cout << "Street is mortgaged, no charge." << endl;
			return true;
		}

		return Location::demandMoney(payer, owner, sumToPay);
	}

};

class Railroad: public Asset {
public:
	Railroad() :
			Asset("") {
	}
	;
	Railroad(string name) :
			Asset(name) {
	}
	;
	/*bool visit(Player& p) override {
	 }
	 ;*/
	bool pay(Player& payer) override {
		int sumToPay;
		//check if mortgaged
		if (Asset::isMortgaged) {
			cout << "Railroad is mortgaged, no charge." << endl;
			return true;
		}
		cout << "Owner has " << Asset::owner->numOfRailroads << "Railroads";
		//check if has all series and no development
		switch (Asset::owner->numOfRailroads) {
		case 1:
			sumToPay = 25;
			break;
		case 2:
			sumToPay = 50;
			break;
		case 3:
			sumToPay = 100;
			break;
		case 4:
			sumToPay = 200;
			break;
		}

		return Location::demandMoney(payer, owner, sumToPay);
		/*if (payer.money < sumToPay) {
		 cout << "insufficient funds!" << endl;
		 return false;
		 //TODO: resolve
		 } else {
		 payer.money -= sumToPay;
		 owner->money += sumToPay;
		 cout << "Payed!!" << endl;
		 return true;
		 }*/
	}
	bool purchase(Player &p) override {
		cout << "Railroad Purchase" << endl;
		bool wasPurchased = Asset::purchase(p);
		if (wasPurchased) {
			p.numOfRailroads++;
			cout << "numOfRailroad Inc " << p.numOfRailroads << endl;
		}
		return wasPurchased;
	}
};

class Utility: public Asset {
public:
	Utility(string name) :
			Asset(name) {
	}
	;
	/*bool visit(Player& p) override {
	 }
	 ;*/
	bool pay(Player& payer) override {
		int sumToPay = 0;
		//check if mortgaged
		if (Asset::isMortgaged) {
			cout << "Utility is mortgaged, no charge." << endl;
			return true;
		}
		if (payer.numOfUtilities == 1) {
			sumToPay = 4 * payer.lastMove;
		} else if (payer.numOfUtilities == 2) {
			sumToPay = 10 * payer.lastMove;
		}
		return Location::demandMoney(payer, owner, sumToPay);

	}
	bool purchase(Player &p) override {
		cout << "Utility Purchase" << endl;
		bool wasPurchased = Asset::purchase(p);
		if (wasPurchased) {
			p.numOfUtilities++;
			cout << "numOfUtilities Inc " << p.numOfUtilities << endl;
		}
		return wasPurchased;
	}
};

class Card: public Location {
private:
	bool isChance;
	string messages[17];
	vector<int> cardPermutation;
	int cnt;
public:
	Card(string name) :
			Location(name), cnt(0) {
		if (name == "Community Chest") {
			initMessages("CommunityChest.cfg");
			//cout << "HERE" << endl;
			isChance = false;

		}
		if (name == "Chance") {
			initMessages("Chance.cfg");
			isChance = true;
		}

		//create random permutation
		std::srand(unsigned(std::time(0)));

		// set some values:
		for (int i = 1; i < 16; ++i)
			cardPermutation.push_back(i);

		// using built-in random generator:
		std::random_shuffle(cardPermutation.begin(), cardPermutation.end());

		// print out content:
		std::cout << "myvector contains:";
		for (std::vector<int>::iterator it = cardPermutation.begin();
				it != cardPermutation.end(); ++it)
			std::cout << ' ' << *it;

	}
	bool visit(Player& p) override {
		if (isChance) {
			cout << "Chance:" << endl;
		} else {
			cout << "Community Chest:" << endl;

		}
		cout << messages[cnt++] << endl;
		return true;
	}
	;
	bool initMessages(string cfgFileName) {
		string line;
		ifstream myfile(cfgFileName);
		if (myfile.is_open()) {
			while (getline(myfile, line) && line != "") {
				cout << "Line:" << line << '\n';
				int line_num = stoi(line.substr(0, 2));

				messages[line_num] = line.substr(2, 200);
				cout << line_num << ":" << line.substr(2, 30) << " ADDED"
						<< endl;
			}
			cout << "before close" << endl;
			//myfile.close();
			cout << "finished card file reading" << endl;
		}

		else {
			cout << "Unable to open file" << endl;
			return false;
		}

		return true;
	}
	bool getMessage();
	bool performAction(int action_number);
};

class Jail: public Location {
public:
	Jail() :
			Location("JAIL") {
	}
	;
	bool visit(Player& p) override {
		if (p.isInJail) {
			cout << "Jail" << endl;
			tryEscaping(p);
		} else {
			cout << "Just Visiting..." << endl;
		}
		return true;
	}

	bool tryEscaping(Player& p) {
		if (p.numOfJailAttempts < 3) {
			cout << "Try Escaping: r=roll dice c=card, p=pay 50$" << endl;
		} else {
			cout << "Try Escaping: c=card, p=pay 50$" << endl;
		}
		char c = 'Q';
		while (c != 'c' && c != 'p' && p.isInJail) {
			cin >> c;
			switch (c) {
			case 'c':
				if (p.numOfJailFreeCards > 0) {
					cout << "Used get out of jail free card to get out" << endl;
					p.numOfJailFreeCards--;
					p.isInJail = false;
					p.numOfJailAttempts = 0;
				}

				break;
			case 'p':
				cout << "Resolve 50$ payment" << endl;
				p.isInJail = false;
				p.numOfJailAttempts = 0;
				break;

			case 'r':
				if (p.numOfJailAttempts < 2) {
					return p.rollDice();
				}
				if (p.numOfJailAttempts == 2) {
					if (p.rollDice()) {
						return true;
					} else {
						continue;
					}
				}
				break;
			} //end switch

		} //end while

		//got out of jail the hard way after 3 attempts
		if (p.numOfJailAttempts == 2) {
			p.advance(p.lastMove);
			return false;
		}

	} //end func

};
//end class

class GoToJail: public Location {
public:
	GoToJail() :
			Location("GO TO JAIL") {
	}
	;
	bool visit(Player& p) override {
		cout << "GO TO JAIL, don't collect 200$" << endl;
		p.isInJail = true;
		p.location = 10;
		return true;
	}
	;

};

class Go: public Location {
public:
	Go() :
			Location("GO") {
	}
	;
	bool visit(Player& p) override {
		cout << p.name << ": GO: Collect 200" << endl;
		p.money += 200;
		return true;
	}
	;
};

class FreeParking: public Location {
public:
	FreeParking() :
			Location("Free Parking") {
	}
	;
	bool visit(Player& p) override {
		cout << p.name << ": Free Parking" << endl;
		return true;
	}
	;
};
class LuxuryTax: public Location {
public:
	LuxuryTax() :
			Location("Luxury Tax") {
	}
	;
	bool visit(Player& p) override {
		cout << "Luxury Tax: pay 75$" << endl;
		Location::demandMoney(p, &defaultPlayer, 75);
	}
	;
};

class IncomeTax: public Location {
public:
	IncomeTax() :
			Location("Income Tax") {
	}
	;
	bool visit(Player& p) override {
		int sumToPay;
		char c = 'Q';
		cout << "Income Tax: 1- pay 10% of your assets, 2- pay 200$";
		while (c != '1' && c != '2') {

			cin >> c;
			switch (c) {
			case '1':
				sumToPay = 0.1 * (p.assetWorth + p.money);

				break;
			case '2':
				sumToPay = 200;
				break;
			}
		}

		return Location::demandMoney(p, &defaultPlayer, sumToPay);
	}
	;
};
//TODO: singleton design pattern?
class Board {
private:
	map<int, pair<int, int>> board_locations;
	string board[BOTTOM_MARGIN + 11 + TOP_MARGIN];
	string gameData[13][40];
public:
	shared_ptr<Location> locations[40];
	Board() {
		/*int streetLocations[22] = { 1, 3, 6, 8, 9, 11, 13, 14, 16, 18, 19, 21,
		 23, 24, 26, 28, 29, 31, 33, 34, 37, 39 };
		 cout << "start of making uniques" << endl;

		 for (int i = 0; i < 40; i++) {
		 locations[i] = nullptr;
		 }
		 cout << "start of making uniques" << endl;
		 */
		/*
		 for (int l : streetLocations) {
		 //cout <<l<<endl;
		 locations[l] = make_unique<Street>();
		 }

		 locations[0] = make_unique<Go>();

		 locations[2] = make_shared<Card>("Community Chest");
		 locations[17] = locations[2];
		 locations[32] = locations[2];

		 locations[7] = make_shared<Card>("Chance");
		 locations[22] = locations[7];
		 locations[36] = locations[7];

		 locations[10] = make_unique<Jail>();
		 locations[20] = make_unique<FreeParking>();
		 locations[30] = make_unique<GoToJail>();

		 locations[4] = make_unique<Tax>("Income Tax");
		 locations[38] = make_unique<Tax>("Luxury Tax");

		 locations[12] = make_unique<Utility>("Electric Company");
		 locations[27] = make_unique<Utility>("Water Works");

		 locations[5] = make_unique<Railroad>("");
		 locations[15] = make_unique<Railroad>("");
		 locations[25] = make_unique<Railroad>("");
		 locations[35] = make_unique<Railroad>("");
		 */
		cout << "end of making uniques" << endl;

		//initialize board map
		for (int i = 0; i < 10; i++) {
			board_locations[i] = pair<int, int>(0, i);
		}
		for (int i = 10; i < 20; i++) {
			board_locations[i] = pair<int, int>(i % 10, 10);
		}
		for (int i = 20; i < 30; i++) {
			board_locations[i] = pair<int, int>(10, 10 - (i % 10));
		}
		for (int i = 30; i < 40; i++) {
			board_locations[i] = pair<int, int>(10 - (i % 10), 0);
		}

		//initialize board string array
		for (string& line : board) {
			string temp(LEFT_MARGIN + 11 + RIGHT_MARGIN, ' ');
			line = temp;
		}
	}

	void printBoard1() {
		for (auto loc : board_locations) {
			//cout << loc.second.first << "," << loc.second.second << endl;
			board[TOP_MARGIN + loc.second.first][LEFT_MARGIN + loc.second.second] =
					'*';
			//cout << board[loc.second.first][loc.second.second];
		}
		for (string& line : board) {
			cout << line << endl;
		}
	}

	void printBoard(Player* players, int numOfPlayers) {
		for (int i = 0; i < 40; i++) {
			for (int j = 0; j < numOfPlayers; j++) {
				if (players[j].location == i) {
					cout << j;
				} else {
					cout << " ";
				}
			}
			cout << locations[i]->getShortName() << " : "
					<< locations[i]->getName() << "("
					<< locations[i]->getOwner() << ")" << endl;
		}
	}

	bool loadConfig() {
		string line;
		ifstream myfile("Config.cfg");
		if (myfile.is_open()) {
			while (getline(myfile, line) && line != "") {
				//cout << "Line:" << line << '\n';
				int line_num = stoi(line.substr(0, 2));

				locations[line_num]->setName(line.substr(2, 30));
				cout << line_num << ":" << line.substr(2, 30) << " ADDED"
						<< endl;
			}
			myfile.close();
		}

		else {
			cout << "Unable to open file" << endl;
			return false;
		}
		return true;
	}
	bool loadCSV() {
		string line;
		ifstream myfile("monopoly.csv");
		bool firstChance = true;
		bool firstCChest = true;
		int c, cc;
		if (myfile.is_open()) {
			//skip first line
			getline(myfile, line);
			while (getline(myfile, line) && line != "") {
				vector<string> v = splitIntoTokens(line);
				cout << v[1] << endl;
				int pos = stoi(v[2]);
				cout << pos << endl;
				//set name
				switch (v[13].at(0)) {
				case 'S': {
					locations[pos] = make_unique<Street>(v[0]);
					Street* s = ((Street*) locations[pos].get());
					s->setShortName(v[1]);
					for (int i = 0; i < 10; i++) {
						s->data[i] = stoi(v[i + 3]);
					}
					break;
				}
				case 'U': {
					locations[pos] = make_unique<Utility>(v[0]);
					Utility* u = ((Utility*) locations[pos].get());
					u->setShortName(v[1]);
					u->data[PRICE] = stoi(v[3]);
					u->data[MORTGAGE] = stoi(v[11]);
				}
					break;
				case 'R': {
					locations[pos] = make_unique<Railroad>(v[0]);
					Railroad* r = ((Railroad*) locations[pos].get());
					r->setShortName(v[1]);
					r->data[PRICE] = stoi(v[3]);
					r->data[MORTGAGE] = stoi(v[11]);
					break;
				}
				case 'G': {
					locations[pos] = make_unique<Go>();
					break;
				}
				case 'J': {
					locations[pos] = make_unique<Jail>();
					break;
				}
				case 'F': {
					locations[pos] = make_unique<FreeParking>();
					break;
				}
				case 'H': {
					locations[pos] = make_unique<GoToJail>();
					break;
				}
				case 'I': {
					locations[pos] = make_unique<IncomeTax>();
					break;
				}
				case 'L':
					locations[pos] = make_unique<LuxuryTax>();
					break;
				case 'C': {
					if (firstChance) {
						locations[pos] = make_shared<Card>("Chance");
						c = pos;
						firstChance = false;
					} else {
						locations[pos] = locations[c];
					}
					break;
				}
				case 'B': {
					if (firstCChest) {
						locations[pos] = make_shared<Card>("Community Chest");
						cc = pos;
						firstCChest = false;
					} else {
						locations[pos] = locations[cc];
					}
					//cout <<"card created" <<endl;
					break;
				}

				} //end switch

			} // end while
			myfile.close();
		} else {
			cout << "Unable to open file" << endl;
			return false;
		}
		return true;
	}

	std::vector<std::string> splitIntoTokens(string line) {
		std::vector<std::string> result;

		std::stringstream lineStream(line);
		std::string cell;

		while (std::getline(lineStream, cell, ',')) {
			result.push_back(cell);
		}
		// This checks for a trailing comma with no data after it.
		if (!lineStream && cell.empty()) {
			// If there was a trailing comma then add an empty element.
			result.push_back("");
		}
		return result;
	}
	bool checkSeries(Player& owner, int series) {
		int streetLocations[22] = { 1, 3, 6, 8, 9, 11, 13, 14, 16, 18, 19, 21,
				23, 24, 26, 28, 29, 31, 33, 34, 37, 39 };
		int seriesSize;
		if (series == 0 || series == 7) {
			seriesSize = 2;
		} else {
			seriesSize = 3;
		}
		int cnt = 0;
		for (int i : streetLocations) {
			Street* s = (Street*) locations[i].get();
			if ((s->data[SERIES]) == series
					&& s->getOwner() == owner.getName()) {
				cnt++;
			}

		}
		if (cnt == seriesSize) {
			return true;
		} else
			return false;
	}

};

int main() {

	//initialize board locations map

	int numOfPlayers = 4;

	/*
	 int numOfPlayers = 0;
	 while (numOfPlayers < 2 || numOfPlayers > 4) {
	 cout << "Please enter number of players (2-4):" << endl;
	 cin >> numOfPlayers;
	 }
	 */

	Player players[numOfPlayers];

	Board gameBoard;
	gameBoard.loadCSV();
	//exit(0);

	//gameBoard.loadConfig();
	gameBoard.printBoard(players, numOfPlayers);

	/*
	 *
	 */
	/*
	 for (int i = 0; i < numOfPlayers; i++) {
	 string name;
	 cout << "Player " << i + 1 << ": Please enter your name" << endl;
	 cin >> name;
	 players[i].setName(name);

	 }
	 */
	players[0].setName("Johnny");
	players[1].setName("Michael");
	players[2].setName("Isaac");
	players[3].setName("David");

	cout << string(50, '\n');
	char c = '0';
	int cur = 0;
	bool diceRolled = false;
	bool purchaseable = false;
	while (c != 'e') {
		Player& p = players[cur];

		cout << "Current Player: " << cur << " Cash Balance: " << p.money; // players[cur].getName();
		if (p.isInJail && !diceRolled) {
			diceRolled = (gameBoard.locations[p.location]->visit(p));
			if (!diceRolled && p.numOfJailAttempts == 2) {
				//got out of jail the hard way
				gameBoard.printBoard(players, numOfPlayers);
				purchaseable = !(gameBoard.locations[p.location]->visit(p));
				p.numOfJailAttempts = 0;
			}
		}
		cin >> c;

		switch (c) {

		case 'r':	//roll dice
			if (!diceRolled) {
				diceRolled = !(p.rollDice());
				gameBoard.printBoard(players, numOfPlayers);
				purchaseable = !(gameBoard.locations[p.location]->visit(p));
				cout << purchaseable << endl;

			}

			break;

		case 'p': //purchase
			if (purchaseable) {
				//cout << "Purchase" << endl;
				Asset* a = (Asset*) gameBoard.locations[p.location].get();
				purchaseable = !(a->purchase(p));
				if (a->data[TYPE] == 'S' && !purchaseable) {
					Street* s = (Street*) gameBoard.locations[p.location].get();
					s->setSeries(gameBoard.checkSeries(p, a->data[SERIES]));
					cout << "Series status: " << s->getSeries() << endl;
				}
			}

			break;
		case 'n': //end turd + move to next player
			if (diceRolled) {
				cur = (cur + 1) % 4;

				diceRolled = false;
				purchaseable = false;
			} else {
				cout << "Please roll dice first" << endl;
			}
			break;
		}

	}

	cout << "game quit" << endl;

	return 0;
}

