#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <cassert>
#include <cmath>
using namespace std;

using ll = long long int;

struct player{
  string name, pos, club;
  ll price, points;
};


// We give more importance to points but penalizing the big prices
// In general point^4 is more or less similar to prices, it is a way of normalizing
bool comp(player a, player b) {
  if (a.price*b.price == 0) return a.points > b.points;
  return pow(a.points, 4)/a.price > pow(b.points, 4)/b.price;
}

/*
  Struct for storing the players of the team
*/
struct football_team{
  vector<vector<player>> pos = vector<vector<player>>(4);
  ll price = 0;
  ll points = 0;
};

/*
  Global variables
  n1, n2, n3, T, J: input parameters
  lim: {1, n1, n2, n3} vector of limits
  db: database of players separated by position
  used: auxiliar vector to keep track of the used players in execution
  team, dream_team: current and best-so-far team
  outputFile: name of the output file
*/
ll n1, n2, n3, T, J;
vector<ll> lim;
vector<vector<bool>> used(4);
football_team dream_team;
string outputFile;
vector<player> db;

// Returns actual time
double now(){
  return clock()/double(CLOCKS_PER_SEC);
}

// Reads the database from argv[1] and stores it in db
void read(int argc, char** argv){
  if (argc != 4) {
    cout << "Syntax: " << argv[0] << " data_base.txt input.txt output.txt" << endl;
    exit(1);
  }
  
  ifstream in(argv[1]);
  while (not in.eof()) {
    string name, pos, club;
    ll price, points;

    getline(in,name,';');  if (name == "") break;
    getline(in,pos,';');
    in >> price;
    char aux; in >> aux;
    getline(in,club,';');
    in >> points;
    string aux2;
    getline(in,aux2);
    
    db.push_back({name, pos, club, price, points});

  }
  
  in = ifstream(argv[2]);
  in >> n1 >> n2 >> n3 >> T >> J;
  if (J > T) J = T;
  in.close();
  lim = {1, n1, n2, n3};
}

// Outputs the team in the format asked into the outputFile
void print(double t){
  ofstream f(outputFile); f.precision(1); f.setf(ios::fixed);
  f << t << endl;
  vector<string> zone = {"POR: ", "DEF: ", "MIG: ", "DAV: "};
  for (ll j = 0; j < 4;j++){
    f << zone[j];
    for (ll i = 0; i < dream_team.pos[j].size(); i++) {
      f <<  (i != 0 ? ";":"") << dream_team.pos[j][i].name;
    }
    f << endl;
  }
  f <<  "Punts: " << dream_team.points << endl;
  f <<  "Preu: " << dream_team.price << endl;
  f.close();
}

// Deletes players that costs more than J
void clean(){
  vector<player> copy = db;
  db = vector<player>();
  for (ll i = 0; i < copy.size(); i++){
      if (copy[i].price <= J) db.push_back(copy[i]);
  }
}

// fills the dream team
void fill(double t1){
  for (int i = 0; i < db.size(); i++){
    if (db[i].price + dream_team.price > T) continue;
    if (db[i].pos == "por" and dream_team.pos[0].size() < lim[0]){
      dream_team.pos[0].push_back(db[i]);
      dream_team.price += db[i].price;
      dream_team.points += db[i].points;
    } else if (db[i].pos == "def" and dream_team.pos[1].size() < lim[1]){
      dream_team.pos[1].push_back(db[i]);
      dream_team.price += db[i].price;
      dream_team.points += db[i].points;
    } else if (db[i].pos == "mig" and dream_team.pos[2].size() < lim[2]){
      dream_team.pos[2].push_back(db[i]);
      dream_team.price += db[i].price;
      dream_team.points += db[i].points;
    } else if (db[i].pos == "dav" and dream_team.pos[3].size() < lim[3]){
      dream_team.pos[3].push_back(db[i]);
      dream_team.price += db[i].price;
      dream_team.points += db[i].points;
    }
  }
  print(now()-t1);
}

int main(int argc, char** argv) {
  double t1 = now();
  read(argc, argv);
  clean();
  sort(db.begin(), db.end(), comp);
  outputFile = argv[3];
  fill(t1);
}
