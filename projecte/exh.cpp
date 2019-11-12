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

bool operator <(const player& a, const player& b){
  return a.points > b.points;
}

struct football_team{
  vector<vector<player>> pos = vector<vector<player>>(4);
  int price = 0;
  int points = 0;
};

int n1, n2, n3, T, J;
int max_points = 0;
vector<vector<player>> db(4);
vector<vector<bool>> used(4);
football_team team;
string outputFile;

double now(){
  return clock()/double(CLOCKS_PER_SEC);
}

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
  in.close();
}

void print(double t){
  ofstream f(outputFile);
  f.precision(1);
  f.setf(ios::fixed);
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
  cout << "printed" << endl;
}

bool filled(){
  return 11 == team.pos[0].size() + team.pos[1].size() + team.pos[2].size()
    + team.pos[3].size();
}

// p means position, 0: por, 1: def, 2: mig, 3: dav
void find(int idx, int p, double& t1){
  vector<int> lim = {1, n1, n2, n3};
  if (filled() or (p == 3 and idx == db[p].size())){
    if(team.points >= max_points) {
      double t2 = now();
      print(t2 - t1);
      max_points = team.points;
    }
  } else if (team.pos[p].size() == lim[p]) find(0, p+1, t1);
  else if (not (p < 3 and idx == db[p].size())){
    player pl = db[p][idx];
    if ((pl.price <= J and pl.price + team.price <= T) 
    and (not used[p][idx])){
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
  read(argc, argv);
  //for (int i = 0; i < 4; i++) sort(db[i].begin(), db[i].end());
  outputFile = argv[3];
  for (int i = 0; i < 4; i++) used[i] = vector<bool>(db[i].size(), false);
  double t1 = now();
  find(0, 0, t1);
}
