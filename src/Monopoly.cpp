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
class Asset;

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
	list<Asset*> assets;
	Player* allPlayers;
	int numOfPlayers;

	Player() :
			name("BANK"), isAlive(true), money(1500), location(0), isInJail(
					false), numOfJailFreeCards(0), numOfRailroads(0), numOfUtilities(
					0), numOfDoubles(0), numOfJailAttempts(0), lastMove(0), assetWorth(
					0), allPlayers(), numOfPlayers(0) {
		srand(time(0));
	}

	void setName(string name) {
		this->name = name;
	}
	const string getName() const {
		return name;
	}
	bool sell();
	bool buy();
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
	bool hasLost(bool inDebt) {
		//checks if player is in debt and has all properties mortgaged or has no properties
		if (inDebt) {
			if (assets.empty()) {
				cout << "No Assets" << endl;
				return true;
			}
			if (getUnmortgaged().empty()) {
				cout << "No assets to mortgage" << endl;
				return true;
			}
		}
		return false;

	}

	list<string> getMortgaged();
	list<string> getUnmortgaged();
	list<string> getSeries();
	list<string> getDevelopedStreets();
	list<string> getAssetsForSale();
	void transferAssets(Player* payee);
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
		char c;
		bool inDebt = (payer.money < sumToPay);
		bool hasLost = payer.hasLost(inDebt);
		while (inDebt && !hasLost) {
			cout << "Insufficient funds! Please settle debt of " << sumToPay
					<< "$" << endl;
			cout << "Cash Balance: " << payer.money << "$" << endl; // players[cur].getName();
			cin >> c;

			switch (c) {

			case 'm':
				payer.mortgage();
				break; //end case m
			case 'u':
				payer.unmortgage();
				break;
			case 'd':
				payer.develop();
				break;
			case 'x':
				payer.undevelop();
				break;
			case 's':
				payer.sell();
				break;
			} //end switch
			inDebt = (payer.money < sumToPay);
			hasLost = payer.hasLost(inDebt);
		} //end while
		if (hasLost) {
			//cout << "Lost" <<endl;
			payer.transferAssets(payee);
			return false;
		} else {
			payer.money -= sumToPay;
			payee->money += sumToPay;
			cout << "Debt was payed!!" << endl;
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
			Location(name), shortName(""), owner(&defaultPlayer), isMortgaged(
					false) {
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
		if (owner == &defaultPlayer) {
			cout << "Not owned" << endl;
			owned = false;
		} else {
			cout << "Owned by " << owner->name << endl;
			owned = true;
			if (!pay(p)) {
				p.isAlive = false;
			}
		}
		return (owned);
	}

	virtual bool purchase(Player &p) {
		//cout << "Base Purchase" << endl;
		if (p.money < data[PRICE]) {
			cout << "Insufficient Funds to buy " << Location::getName() << endl;
			return false;
		} else {
			p.money -= data[PRICE];
			cout << "Purchasing " << Location::getName() << "for "
					<< data[PRICE] << "$" << endl;
			p.assetWorth += data[PRICE];
			p.assets.push_back(this);
			owner = &p;
			return true;
		}
	}
	bool unmortgage(Player& p) {
		//TODO: Check if this is correct
		cout << "Unmortgaging " << Location::getName() << "for "
				<< 1.1 * data[MORTGAGE] << "$" << endl;
		if (p.money < 1.1 * data[MORTGAGE]) {
			cout << "Insufficient funds. Unmortgage failed" << endl;
			return false;
		} else {
			p.money -= (int) (1.1 * data[MORTGAGE]);
			isMortgaged = false;
			//TODO: Check if this is correct
			p.assetWorth += data[PRICE];
			p.assetWorth -= data[MORTGAGE];
			return true;
		}
	}

	bool mortgage(Player& p) {
		cout << "Mortgaging " << Location::getName() << "for " << data[MORTGAGE]
				<< "$" << endl;
		isMortgaged = true;
		p.money += data[MORTGAGE];
		//TODO: Check if this is correct
		p.assetWorth -= data[PRICE];
		p.assetWorth += data[MORTGAGE];
		return true;
	}

	bool sell(Player& owner, Player* players, int playerToSellTo, int price) {
		if (players[playerToSellTo].money < price) {
			cout << "Buyer has insufficient funds. Sale cancelled." << endl;
		} else {
			this->owner = &players[playerToSellTo];
			if (isMortgaged) {
				owner.assetWorth -= this->data[PRICE];
			} else {
				owner.assetWorth -= this->data[MORTGAGE];
			}
			owner.assets.remove(this);
			owner.money += price;
			players[playerToSellTo].assets.push_back(this);
			players[playerToSellTo].money -= price;

			cout << this->getName() << "was successfully sold to "
					<< players[playerToSellTo].getName() << " for " << price
					<< endl;
			return true;
		}
		return false;
	}
	bool buy(Player& buyer, Player* players, int playerToBuyFrom, int price) {
		Player* owner = &players[playerToBuyFrom];
		if (buyer.money < price) {
			cout << "Buyer has insufficient funds. Sale cancelled." << endl;
		} else {
			this->owner = &buyer;
			if (isMortgaged) {
				owner->assetWorth -= this->data[MORTGAGE];
			} else {
				owner->assetWorth -= this->data[PRICE];
			}
			owner->assets.remove(this);
			owner->money += price;
			buyer.assets.push_back(this);
			buyer.money -= price;

			cout << this->getName() << "was successfully bought from "
					<< owner->getName() << " for " << price << endl;
			return true;
		}
		return false;
	}

}
;

class Street: public Asset {
public:
	int development;
	bool series;

	Street() :
			Asset(""), development(RENT), series(false) {
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
	bool develop(Player& p, int numOfHouses) {
		cout << "Developing " << Location::getName() << " for "
				<< numOfHouses * Asset::data[HOUSE_COST] << "$" << endl;
		if (p.money < numOfHouses * data[HOUSE_COST]) {
			cout << "insufficient funds" << endl;
			return false;
		}
		p.assetWorth += data[HOUSE_COST];
		development += numOfHouses;
		p.money -= numOfHouses * data[HOUSE_COST];
		return true;
	}

	bool undevelop(Player& p, int numOfHouses) {
		cout << "Removing " << numOfHouses << "houses from"
				<< Location::getName() << " for "
				<< numOfHouses * data[HOUSE_COST] / 2 << "$" << endl;

		p.assetWorth -= data[HOUSE_COST];
		development -= numOfHouses;
		p.money += (int) numOfHouses * data[HOUSE_COST] / 2;
		return true;

	}

	bool purchase(Player &p) override {
		//cout << "Street purchase" << endl;
		bool wasPurchased = Asset::purchase(p);
		if (wasPurchased) {
			series = checkSeriesAndSet(p);
			//TODO: make all series booleans true now!
			cout << "Purchased:" << *this << endl;
		}
		return wasPurchased;
	}
	bool checkSeriesAndSet(Player& p) {
		int seriesSize;
		if (data[SERIES] == 0 || data[SERIES] == 7) {
			seriesSize = 2;
		} else {
			seriesSize = 3;
		}
		int cnt = 0;
		list<Street*> seriesStreets;
		//s->setSeries(gameBoard.checkSeries(p, a->data[SERIES]));
		for (Asset* a : p.assets) {
			//DEBUG cout << *((Street*) a) << endl;

			if (a->data[TYPE] == 'S' && a->data[SERIES] == this->data[SERIES]
					&& !a->isMortgaged) {
				cnt++;
				//cout << "Pushed to seriesstreets:" << *((Street*) a) << endl;
				seriesStreets.push_back((Street*) a);
			}
		}
		//DEBUG cout << "cnt=" << cnt << " seriesSize=" << seriesSize << endl;

		//if series is made, set the series flag true for rest of the series
		if (cnt == seriesSize) {
			for (Street* s : seriesStreets) {
				s->series = true;
				cout << "Series flag turned on for: " << s->getName() << endl;
			}
			return true;
		} else {
			return false;
		}
	}
	friend ostream& operator<<(ostream& os, const Street& s);
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

	}
	bool purchase(Player &p) override {
		//cout << "Railroad Purchase" << endl;
		bool wasPurchased = Asset::purchase(p);
		if (wasPurchased) {
			p.numOfRailroads++;
			//cout << "numOfRailroad Inc " << p.numOfRailroads << endl;
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
		//cout << "Utility Purchase" << endl;
		bool wasPurchased = Asset::purchase(p);
		if (wasPurchased) {
			p.numOfUtilities++;
			//cout << "numOfUtilities Inc " << p.numOfUtilities << endl;
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
		if (!Location::demandMoney(p, &defaultPlayer, 75)) {
			p.isAlive = false;
			return false;
		}
		return true;
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

		if (!Location::demandMoney(p, &defaultPlayer, sumToPay)) {
			p.isAlive = false;
			return false;
		}
		return true;
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
					cout << j + 1;
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
					cout << "Add street:" << endl;
					for (int i = 0; i < 10; i++) {
						s->data[i] = stoi(v[i + 3]);
						cout << i << ":" << s->data[i] << endl;
					}
					s->data[10] = v[13].at(0);

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

	for (int i = 0; i < numOfPlayers; i++) {
		players[i].allPlayers = players;
		players[i].numOfPlayers = numOfPlayers;
	}

	//DEBUG
	players[0].setName("Johnny");
	players[1].setName("Michael");
	players[2].setName("Isaac");
	players[3].setName("David");

	//buy blue series for checking only
	((Street*) gameBoard.locations[39].get())->purchase(players[0]);
	((Street*) gameBoard.locations[37].get())->purchase(players[0]);
	cout << ((Street*) gameBoard.locations[37].get())->data[TYPE] << endl;
	cout << "GAME START" << endl;
//cout << string(50, '\n');
	((Street*) gameBoard.locations[4].get())->visit(players[0]);
	((Street*) gameBoard.locations[4].get())->visit(players[0]);
	((Street*) gameBoard.locations[4].get())->visit(players[0]);
	((Street*) gameBoard.locations[4].get())->visit(players[0]);

	//END DEBUG

	char c = '0';
	int cur = 0;
	bool diceRolled = false;
	bool purchaseable = false;
	while (c != 'e') {
		Player& p = players[cur];
		int cnt = 1;
		while (!players[cur].isAlive) {
			//skip over dead players
			cur = cur + 1 % numOfPlayers;
			cnt++;
			if (cnt == numOfPlayers - 1) {
				cout << "Game Over" << endl;
				//TODO : declare winner
				break;
			}
		}

		cout << "Current Player: " << cur + 1 << " Cash Balance: " << p.money; // players[cur].getName();
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
					//	Street* s = (Street*) gameBoard.locations[p.location].get();
					//	s->setSeries(gameBoard.checkSeries(p, a->data[SERIES]));
					//	cout << "Series status: " << s->getSeries() << endl;
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

		case 'm':
			p.mortgage();
			break; //end case m
		case 'u':
			p.unmortgage();
			break;
		case 'd':
			p.develop();
			break;
		case 'x':
			p.undevelop();
			break;
		case 's':
			p.sell();
			break;
		case 'b':
			p.buy();
			break;

		} //end switch
	} //end while

	cout << "game quit" << endl;

	return 0;
}

bool Player::mortgage() {
	list<string> unmortgagedAssets = getUnmortgaged();

	if (unmortgagedAssets.empty()) {
		cout << "No assets to mortgage" << endl;
		return false;
	}
	cout << "Choose asset to mortgage" << endl;

	string sho;
	cin >> sho;
	if (find(unmortgagedAssets.begin(), unmortgagedAssets.end(), sho)
			!= unmortgagedAssets.end()) {
		//cout << "Mortgaging " << sho << endl;
	}
	for (Asset* a : assets) {
		if (a->getShortName() == sho) {
			return (a->mortgage(*this));
		}
	}
	return false;
}
bool Player::unmortgage() {
	list<string> mortgagedAssets = getMortgaged();

	if (mortgagedAssets.empty()) {
		cout << "No assets to unmortgage" << endl;
		return false;
	}
	cout << "Choose asset to unmortgage" << endl;

	string sho;
	cin >> sho;
	if (find(mortgagedAssets.begin(), mortgagedAssets.end(), sho)
			!= mortgagedAssets.end()) {
		cout << "Unmortgaging " << sho << endl;
		for (Asset* a : assets) {
			if (a->getShortName() == sho) {
				return a->unmortgage(*this);
			}
		}
	}

	return false;
}

bool Player::develop() {
	list<string> seriesStreets = getSeries();
	if (seriesStreets.empty()) {
		cout << "No streets available for development" << endl;
		return false;
	}
	cout << "Choose asset to develop" << endl;

	string sho;
	cin >> sho;
	Street* toDevelop;
	if (find(seriesStreets.begin(), seriesStreets.end(), sho)
			!= seriesStreets.end()) {
		for (Asset* a : assets) {
			if (a->getShortName() == sho) {
				toDevelop = (Street*) a;
				break;
			}
		}

		int numOfHouses;
		cout << "Developing " << toDevelop->getName() << ":" << endl;

		cout << "Please type number of houses to build: " << endl;

		cin >> numOfHouses;
		if (numOfHouses > (HOTEL - toDevelop->development) || numOfHouses < 1) {
			cout << "Invalid number of houses to build" << endl;
			return false;
		} else {
			toDevelop->develop(*this, numOfHouses);
		}
	} else {
		return false;
	}
	return false;
}

bool Player::undevelop() {
	list<string> developedStreets = getDevelopedStreets();

	if (developedStreets.empty()) {
		cout << "No streets with houses available for removal" << endl;
		return false;
	}
	cout << "Choose street from which to remove houses" << endl;
	string sho;
	cin >> sho;
	Street* toUnDevelop;
	if (find(developedStreets.begin(), developedStreets.end(), sho)
			!= developedStreets.end()) {
		for (Asset* a : assets) {
			if (a->getShortName() == sho) {
				toUnDevelop = (Street*) a;
				break;
			}
		}
		int numOfHouses;

		cout << "Removing houses from " << toUnDevelop->getName() << ":"
				<< endl;
		cout << "Please type number of houses to remove: " << endl;

		cin >> numOfHouses;
		if (numOfHouses > toUnDevelop->development - 2 || numOfHouses < 1) {
			cout << "Invalid number of houses to remove" << endl;
			return false;
		} else {
			toUnDevelop->undevelop(*this, numOfHouses);
		}
	} else {
		return false;
	}
	return false;
}

bool Player::sell() {
	list<string> assetsForSale = getAssetsForSale();
	if (assetsForSale.empty()) {
		cout << "No streets available for sale (Try removing houses if needed)"
				<< endl;
		return false;
	}
	cout << "Choose street to sell" << endl;
	string sho;
	cin >> sho;
	Street* toSell;
	if (find(assetsForSale.begin(), assetsForSale.end(), sho)
			!= assetsForSale.end()) {
		for (Asset* a : assets) {
			if (a->getShortName() == sho) {
				toSell = (Street*) a;
				break;
			}
		}
		int playerToSellTo;

		cout << "Please select the player you wish to sell to: " << endl;

		for (int i = 0; i < numOfPlayers; i++) {
			if (&allPlayers[i] != this) {
				cout << i + 1 << ": " << allPlayers[i].name << endl;
			}
		}

		cin >> playerToSellTo;
		playerToSellTo--;
		if (playerToSellTo < 0 || playerToSellTo >= numOfPlayers
				|| &allPlayers[playerToSellTo] == this) {
			cout << "Invalid player selected" << endl;
			return false;
		} else {
			cout << "Selling " << toSell->getName() << " to "
					<< allPlayers[playerToSellTo].name << endl;
			cout << "Please type the amount for which the asset is being sold"
					<< "(type a negative number to cancel sale) " << endl;
			int sellingPrice;
			cin >> sellingPrice;
			if (sellingPrice < 0) {
				cout << "Sale cancelled" << endl;
				return false;
			}
			return toSell->sell(*this, allPlayers, playerToSellTo, sellingPrice);
		}
	} else {
		return false;
	}
	return false;
}

bool Player::buy() {
	int playerToBuyFrom;

	cout << "Please select the player you wish to buy from: " << endl;

	for (int i = 0; i < numOfPlayers; i++) {
		if (&allPlayers[i] != this) {
			cout << i + 1 << ": " << allPlayers[i].name << endl;
		}
	}

	cin >> playerToBuyFrom;
	playerToBuyFrom--;
	if (playerToBuyFrom < 0 || playerToBuyFrom >= numOfPlayers
			|| &allPlayers[playerToBuyFrom] == this) {
		cout << "Invalid player selected" << endl;
		return false;
	} else {

		list<string> assetsForSale =
				allPlayers[playerToBuyFrom].getAssetsForSale();
		if (assetsForSale.empty()) {
			cout << "No streets available to buy" << endl;
			return false;
		}
		cout << "Choose street to buy" << endl;
		string sho;
		cin >> sho;
		Street* toBuy;
		if (find(assetsForSale.begin(), assetsForSale.end(), sho)
				!= assetsForSale.end()) {
			for (Asset* a : allPlayers[playerToBuyFrom].assets) {
				if (a->getShortName() == sho) {
					toBuy = (Street*) a;
					break;
				}
			}
		} else {
			return false;
		}
		cout << "Buying " << toBuy->getName() << " from "
				<< allPlayers[playerToBuyFrom].name << endl;

		//TODO fix tuple list
		cout << "Please type the amount for which the asset is being bought"
				<< "(type a negative number to cancel purchase) " << endl;
		int buyingPrice;
		cin >> buyingPrice;
		if (buyingPrice < 0) {
			cout << "Purchase cancelled" << endl;
			return false;
		}
		return toBuy->buy(*this, allPlayers, playerToBuyFrom, buyingPrice);

	}

	return false;
}

list<string> Player::getDevelopedStreets() {
	list<string> developedStreets;
	for (Asset* a : assets) {
		if (a->data[TYPE] == 'S') {
			Street* s = (Street*) a;
			if (s->development > RENT) {
				cout << a->getShortName() << ": " << a->getName() << "("
						<< s->development - 2 << ")" << endl;
				developedStreets.push_back(a->getShortName());
			}
		}
	}
	return developedStreets;
}

list<string> Player::getMortgaged() {
	list<string> mortgagedAssets;
	for (Asset* a : assets) {
		if (a->isMortgaged) {
			cout << a->getShortName() << " :" << a->getName() << endl;
			mortgagedAssets.push_back(a->getShortName());
		}
	}
	return mortgagedAssets;
}

list<string> Player::getUnmortgaged() {
	list<string> unmortgagedAssets;
	for (Asset* a : assets) {
		if (!(a->isMortgaged)) {
			cout << a->getShortName() << " :" << a->getName() << endl;
			unmortgagedAssets.push_back(a->getShortName());
		}
	}
	return unmortgagedAssets;
}
list<string> Player::getSeries() {
	list<string> seriesStreets;
	for (Asset* a : assets) {
		if (!(a->isMortgaged) && a->data[TYPE] == 'S') {
			Street* s = (Street*) a;
			if (s->getSeries() && s->development < HOTEL) {
				cout << a->getShortName() << ": " << a->getName() << "("
						<< s->development - 2 << ")" << endl;

				seriesStreets.push_back(a->getShortName());

			}
		}
	}
	return seriesStreets;
}

list<string> Player::getAssetsForSale() {
	list<string> assetsForSale;
	for (Asset* a : assets) {
		if (a->data[TYPE] == 'S') {
			Street* s = (Street*) a;
			if (s->development == RENT) {
				cout << a->getShortName() << ": " << a->getName() << "("
						<< s->development - 2 << ")" << endl;
				assetsForSale.push_back(a->getShortName());
			}
		} else {
			cout << a->getShortName() << ": " << a->getName() << endl;
			assetsForSale.push_back(a->getShortName());
		}
	}
	return assetsForSale;
}

void Player::transferAssets(Player* payee) {
	cout << "Player " << getName() << " has LOST to player " << payee->getName()
			<< endl;
	for (Asset* a : assets) {
		cout << a->getName() << " is transferred to " << payee->getName()
				<< endl;
		a->owner = payee;
		payee->assets.push_back(a);

	}
	payee->money += money;
	money = 0;
}

ostream& operator<<(ostream& os, const Street& s) {
	os << s.getName() << ": isSeries: " << s.series << ", Series: "
			<< s.data[SERIES] << ", type: " << s.data[TYPE];
	return os;
}

