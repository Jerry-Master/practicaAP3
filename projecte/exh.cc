#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
using namespace std;

struct player{
  string name, pos, club;
  int price, points;
};

struct football_team{
  vector<player> por, def, mig, dav;
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
  f <<  "POR: " << team.por[0].name << endl;
  f <<  "DEF: ";
  for (int i = 0; i < team.def.size(); i++) {
    f <<  (i != 0 ? ";":"") << team.def[i].name;
  } 
  f <<  endl;
  f <<  "MIG: ";
  for (int i = 0; i < team.mig.size(); i++) {
    f <<  (i != 0 ? ";":"") << team.mig[i].name;
  } 
  f <<  endl;
  f <<  "DAV: ";
  for (int i = 0; i < team.dav.size(); i++) {
    f <<  (i != 0 ? ";":"") << team.dav[i].name;
  } 
  f <<  endl;
  //f <<  "Punts: " << team.points << endl;
  f <<  "Preu: " << team.price << endl;
  f.close();
  cout << "printed" << endl;
}

bool filled(){
  return 11 == team.por.size() + team.def.size() + team.mig.size()
    + team.dav.size();
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
      if (p.pos == "por"){
        if (team.por.size() < 1){
          team.por.push_back(p);
          team.price += p.price;
          team.points += p.points;
          find(idx+1);
          team.por.pop_back();
          team.price -= p.price;
          team.points -= p.points;
          find(idx+1);
        }
      } else if (p.pos == "def"){
        if (team.def.size() < n1){
          team.def.push_back(p);
          team.price += p.price;
          team.points += p.points;
          find(idx+1);
          team.def.pop_back();
          team.price -= p.price;
          team.points -= p.points;
          find(idx+1);
        }
      } else if (p.pos == "mig"){
        if (team.mig.size() < n2){
          team.mig.push_back(p);
          team.price += p.price;
          team.points += p.points;
          find(idx+1);
          team.mig.pop_back();
          team.price -= p.price;
          team.points -= p.points;
          find(idx+1);
        }
      } else if (p.pos == "dav"){
        if (team.dav.size() < n3){
          team.dav.push_back(p);
          team.price += p.price;
          team.points += p.points;
          find(idx+1);
          team.dav.pop_back();
          team.price -= p.price;
          team.points -= p.points;
          find(idx+1);
        }
      }
    } else find(idx+1);
  }
}

int main(int argc, char** argv) {
  read_database(argc, argv);
  read_parameters(argc, argv);
  outputFile = argv[3];
  used = vector<bool>(db.size());
  find(0);
}
