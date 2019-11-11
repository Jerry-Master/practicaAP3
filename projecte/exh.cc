#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cassert>
using namespace std;

struct player{
  string name, pos, club;
  int price, points;
};

bool operator <(const player& a, const player& b){
  if (a.pos != b.pos) return a.pos < b.pos;
  return a.points > b.points;
}

struct football_team{
  vector<vector<player>> pos = vector<vector<player>>(4);
  int price = 0;
  int points = 0;
};

int n1, n2, n3, T, J;
int max_points = 0;
vector<player> db;
vector<bool> used;
football_team team;
string outputFile;


void read_database(int argc, char** argv){
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
    
    db.push_back({name, pos, club, price, points});
  }
  in.close(); 
}

void read_parameters(int argc, char** argv){
  ifstream in(argv[2]);
  in >> n1 >> n2 >> n3 >> T >> J;
  in.close(); 
}

void print(){
  ofstream f(outputFile);
  vector<string> zone = {"POR: ", "DEF: ", "MIG: ", "DAV: "};
  for (int j = 0; j < 4;j++){
    f << zone[j];
    for (int i = 0; i < team.pos[j].size(); i++) {
      f <<  (i != 0 ? ";":"") << team.pos[j][i].name;
    }
    f << endl;
  } 
  f <<  "Preu: " << team.price << endl;
  f <<  "Punts: " << team.points << endl;
  f.close();
  cout << "printed" << endl;
}

bool filled(){
  return 11 == team.pos[0].size() + team.pos[1].size() + team.pos[2].size()
    + team.pos[3].size();
}

void find(int idx){
  if (filled() or idx == db.size()){
    if(team.points >= max_points) {
      print();
      max_points = team.points;
    }
  } else {
    player p = db[idx];
    if (p.price <= J and p.price + team.price <= T){
      vector<string> zone = {"por", "def", "mig", "dav"};
      vector<int> lim = {1, n1, n2, n3};
      for (int i = 0; i < 4; i++){
        if (p.pos == zone[i]){
          if (team.pos[i].size() < lim[i]){
            team.pos[i].push_back(p);
            team.price += p.price;
            team.points += p.points;
            find(idx+1);
            team.pos[i].pop_back();
            team.price -= p.price;
            team.points -= p.points;
            find(idx+1);
          }
        }
      } 
    } else find(idx+1);
  }
}

int main(int argc, char** argv) {
  read_database(argc, argv);
  //sort(db.begin(), db.end());
  read_parameters(argc, argv);
  outputFile = argv[3];
  used = vector<bool>(db.size());
  find(0);
}
