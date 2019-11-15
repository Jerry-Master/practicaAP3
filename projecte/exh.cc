#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <cassert>
using namespace std;

struct player{
  string name, pos, club;
  int price, points;
};

/*
  Functions for ordering players
*/
bool operator <(const player& a, const player& b){
  return a.points > b.points;
}


/*
  Struct for storing the player of the team
*/
struct football_team{
  vector<vector<player>> pos = vector<vector<player>>(4);
  int price = 0;
  int points = 0;
};

/*
  Global variables
*/
int n1, n2, n3, T, J;
vector<int> lim;
vector<vector<player>> db(4);
vector<vector<bool>> used(4);
football_team team, dream_team;
string outputFile;

// Returns actual time
double now(){
  return clock()/double(CLOCKS_PER_SEC);
}

// Reads the database from argv[1] and stores it in db
// Exception: Player with price 0 aren't taken into account
void read(int argc, char** argv){
  if (argc != 4) {
    cout << "Syntax: " << argv[0] << " data_base.txt input.txt output.txt" << endl;
    exit(1);
  }

  ifstream in(argv[1]);
  while (not in.eof()) {
    string name, pos, club;
    int price, points;

    getline(in,name,';');  if (name == "") break;
    getline(in,pos,';');
    in >> price;
    char aux; in >> aux;
    getline(in,club,';');
    in >> points;
    string aux2;
    getline(in,aux2);

    int p = 0;
    if (pos == "def") p = 1;
    else if (pos == "mig") p = 2;
    else if (pos == "dav") p = 3;
    db[p].push_back({name, pos, club, price, points});
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
  for (int j = 0; j < 4;j++){
    f << zone[j];
    for (int i = 0; i < team.pos[j].size(); i++) {
      f <<  (i != 0 ? ";":"") << team.pos[j][i].name;
    }
    f << endl;
  }
  f <<  "Punts: " << team.points << endl;
  f <<  "Preu: " << team.price << endl;
  f.close();
}

// Deletes players that costs more than J
void clean(){
  vector<vector<player>> copy = db;
  db = vector<vector<player>>(4);
  for (int j = 0; j < 4; j++){
    for (int i = 0; i < copy[j].size(); i++){
      if (copy[j][i].price <= J) db[j].push_back(copy[j][i]);
    }
  }
}

// Tells if the team is filled;
bool filled(){
  return 11 == team.pos[0].size() + team.pos[1].size() + team.pos[2].size()
    + team.pos[3].size();
}

// Returns the maximum number of points that can be obtained
// with the players remaining in each position
int best_points(int idx, int p, int insd){
  int points = 0;
  for (int i = 0; i < lim[p]-insd; i++) points += db[p][idx+i].points;
  for (int i = p+1; i < 4; i++){
    for (int j = 0; j < lim[i]; j++) points += db[i][j].points;
  }
  return points;
}

/*
  Looks for the best configuration of team with
  allineation n1, n2, n3 and price no higher than T
  idx: current player index
  p: position, 0: por, 1: def, 2: mig, 3: dav
  t1: time when the program started
*/
void find(int idx, int p, double& t1){
  // Base case
  if (filled() or (p == 3 and idx == db[p].size())){
    if(team.points >= dream_team.points) {
      double t2 = now();
      print(t2 - t1);
      dream_team = team;
    }
  // The position p is already filled
  } else if (team.pos[p].size() == lim[p]) find(0, p+1, t1);
  // There are players remaining
  else if (not (p < 3 and idx == db[p].size())){
    player pl = db[p][idx];
    // There aren't enough players to fill position p
    if (db[p].size() - idx < lim[p] - team.pos[p].size()) return;
    // The points can't be improved
    if (best_points(idx, p, team.pos[p].size()) + team.points <= dream_team.points) return;
    int n = dream_team.pos[p].size();
    bool useless = false;
    if (n>0) useless = (pl.points <= dream_team.pos[p][n-1].points) and (pl.price > dream_team.pos[p][n-1].price);
    // Player is cheap enough
    if (not useless and (pl.price + team.price <= T) and (not used[p][idx])){
      used[p][idx] = true;
      team.pos[p].push_back(pl);
      team.price += pl.price; team.points += pl.points;
      find(idx+1, p, t1);

      used[p][idx] = false;
      team.pos[p].pop_back();
      team.price -= pl.price; team.points -= pl.points;
    }
    find(idx+1, p, t1);
  }
}

int main(int argc, char** argv) {
  double t1 = now();
  read(argc, argv);
  clean();
  for (int i = 0; i < 4; i++) used[i] = vector<bool>(db[i].size(), false);
  for (int i = 0; i < 4; i++) sort(db[i].begin(), db[i].end());
  outputFile = argv[3];
  find(0, 0, t1);
}

