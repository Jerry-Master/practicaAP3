#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <cassert>
#include <cmath>
#include <cstdlib>
using namespace std;

using ll = long long int;

struct player{
  string name, pos, club;
  ll price, points, idx;
};

/*
  Struct for storing the players of the team
*/
struct football_team {
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
vector<vector<player>> db(4);
vector<vector<bool>> used(4);
football_team team, dream_team;
string outputFile;

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
  ifstream in(argv[2]);
  in >> n1 >> n2 >> n3 >> T >> J;
  if (J > T) J = T;

  in = ifstream(argv[1]);
  while (not in.eof()) {
    string name, pos, club;
    ll price, points;

    getline(in,name,';');  if (name == "") break;
    getline(in,pos,';');
    in >> price;
    char aux; in >> aux;
    getline(in,club,';');
    in >> points;
    if (price > J) continue;
    string aux2;
    getline(in,aux2);

    // We split by positions
    ll p = 0;
    if (pos == "def") p = 1;
    else if (pos == "mig") p = 2;
    else if (pos == "dav") p = 3;
    db[p].push_back({name, pos, club, price, points, 0});
  }
  
  
  in.close();
  lim = {1, n1, n2, n3};

  //clean();
  for (ll i = 0; i < 4; ++i) used[i] = vector<bool>(db[i].size(), false);
  //for (ll i = 0; i < 4; ++i) sort(db[i].begin(), db[i].end(), comp);
  for (ll i = 0; i < 4; ++i) for (ll j = 0; j < db[i].size(); ++j) db[i][j].idx = j;
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

// Tells if the team is filled;
bool filled(){
  return 11 == dream_team.pos[0].size() + dream_team.pos[1].size() 
    + dream_team.pos[2].size() + dream_team.pos[3].size();
}

// Fills the team randomly
void fill_random(double t1) {
  while (not filled()) {
    int pos;
    pos = rand()%4;
    if (dream_team.pos[pos].size() < lim[pos]) {
      int index;
      do {
        index = rand() % (int)db[pos].size();
      } while (used[pos][index] or dream_team.price + db[pos][index].price > T);
      dream_team.pos[pos].push_back(db[pos][index]);
      dream_team.points += db[pos][index].points;
      dream_team.price += db[pos][index].price;
      used[pos][index] = true;
    }
  }
  print(now()-t1);
}

// Computes a simulated annealing to fill the dream team, trying to obtain the 
// maximum number of points as possible without exceeding the price constraints.
void simulated_annealing(double temp, double alpha, double zero) {
  double t1 = now();
  fill_random(t1);
  football_team copy = dream_team;
  while (temp > zero) {
    football_team aux = copy; // aux team will be updated trying to improve copy
    
    int i = rand()%11; // player of the current team that will be replaced
    int pos = 0;
    if (i > n1 + n2) {pos = 3; i -= n1+n2+1;}
    else if (i > n1) {pos = 2; i -= n1+1;}
    else if (i > 0) {pos = 1; i -= 1;}
    player p = aux.pos[pos][i];
    used[pos][p.idx] = false;

    int j;
    do {
      j = rand() % db[pos].size(); // neighborhood
    } while (db[pos][j].price - p.price + aux.price > T or used[pos][j]);

    aux.pos[pos][i] = db[pos][j];
    aux.points += db[pos][j].points - p.points;
    aux.price += db[pos][j].price - p.price;
    
    // check whether the team should be updated or not
    if (aux.points > copy.points) {
      copy = aux; used[pos][j] = true;
      if (copy.points > dream_team.points) {
        dream_team = copy;
        print(now()-t1);
      }
    } else {
      double prob = exp(-(copy.points - aux.points)/temp); // Boltzamnn distribution
      if (rand() < prob*RAND_MAX) {
        copy = aux; used[pos][j] = true;
      } else used[pos][p.idx] = true;
    }
    temp *= alpha;
  }
}

int main(int argc, char** argv) {
  read(argc, argv);
  outputFile = argv[3];
  // the parameters may be modified as needed
  simulated_annealing(1e15, 0.9999, 1e-15);
}
