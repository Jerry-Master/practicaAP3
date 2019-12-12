#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <cassert>
#include <stdio.h>
#include <sys/resource.h>
using namespace std;

using ll = long long int;

static char * _topOfStack;
static int _maxAllowedStackUsage;

int GetCurrentStackSize()
{
   char localVar;
   int curStackSize = (&localVar)-_topOfStack;
   if (curStackSize < 0) curStackSize = -curStackSize;  // in case the stack is growing down
   return curStackSize;
}

struct player{
  string name, pos, club;
  ll price, points;
};

bool operator <(const player& a, const player& b){
  return a.points > b.points;
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
  ll total = 0;
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

    // We split by positions
    ll p = 0;
    if (pos == "def") p = 1;
    else if (pos == "mig") p = 2;
    else if (pos == "dav") p = 3;
    db[p].push_back({name, pos, club, price, points});
    total++;
  }
  cout << total << endl;
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
    for (ll i = 0; i < team.pos[j].size(); i++) {
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
  for (ll j = 0; j < 4; j++){
    for (ll i = 0; i < copy[j].size(); i++){
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
ll best_points(ll idx, ll p, ll insd){
  ll points = 0;
  for (ll i = 0; i < lim[p]-insd; i++) {
    assert(p < db.size());
    assert(idx+i < db[p].size());
    points += db[p][idx+i].points;
  }
  for (ll i = p+1; i < 4; i++){
    for (ll j = 0; j < lim[i]; j++) {
      assert(i < db.size());
      assert(j < db[i].size());
      points += db[i][j].points;
    }
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
void find(ll idx, ll p, double t1){
  int curStackSize = GetCurrentStackSize();
   //printf("MyRecursiveFunction:  curStackSize=%i\n", curStackSize);

   /*if (curStackSize >= _maxAllowedStackUsage){
      printf("    Can't recurse any more, the stack is too big!\n");
      exit(1);
   }*/
  assert(p < team.pos.size());
  assert(p < lim.size());
  assert(p < db.size());
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
    assert(idx < db[p].size());
    player pl = db[p][idx];
    // There aren't enough players to fill position p
    if (db[p].size() - idx < lim[p] - team.pos[p].size()) return;
    // The points can't be improved
    if (best_points(idx, p, team.pos[p].size()) + team.points <= dream_team.points) return;
    assert(p < dream_team.pos.size());
    ll n = dream_team.pos[p].size();
    bool useless = false;
    assert(n-1 < (ll)dream_team.pos[p].size());
    if (n>0) useless = (pl.points <= dream_team.pos[p][n-1].points) 
                        and (pl.price > dream_team.pos[p][n-1].price);
    // Player is cheap enough
    assert(p < used.size());
    assert(idx < used[p].size());
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

  char topOfStack;
   _topOfStack = &topOfStack;
   _maxAllowedStackUsage = 838311100;

   const rlim_t kStackSize = 40 * 1024 * 1024;   // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    // ...

   
  double t1 = now();
  read(argc, argv);
  clean();
  for (ll i = 0; i < 4; i++) used[i] = vector<bool>(db[i].size(), false);
  for (ll i = 0; i < 4; i++) sort(db[i].begin(), db[i].end());
  outputFile = argv[3];
  find((ll)0, (ll)0, t1);
}

