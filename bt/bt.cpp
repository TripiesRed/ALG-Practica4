#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> 

using namespace std;

/*****************************************************************************/
//Definimos el struct Aviso

struct Aviso{

    int duration, deadline, penalty;

    //Constructores

    Aviso(int d, int dl, int p){
        duration = d;
        deadline = dl;
        penalty = p;
    }

    Aviso (const Aviso &other){
        duration = other.duration;
        deadline = other.deadline;
        penalty = other.penalty;
    }

    //Operadores

    bool operator<(const Aviso& other){
        return penalty < other.penalty;
    }

};
/*****************************************************************************/

vector<Aviso> BackTracking(vector<Aviso> grafo){

    vector<Aviso> solucion;
    
    sort(grafo.begin(), grafo.end());

    for(auto it = grafo.begin(); it != grafo.end(); it++)
        solucion.push_back(*it);

    return solucion;
}

/*****************************************************************************/

int main(){

    vector<Aviso> g, s;
    const int SIZE = 10;

    for(int i = 0; i < SIZE; i++){
        g.push_back(Aviso(i,i,i));
    }

    s = BackTracking(g);
    cout << "RESULTADO: " << endl;
    for(int i = 0; i < SIZE; i++){
        cout << s[i].penalty << " ";
    }
    cout << endl;

    return 0;
}