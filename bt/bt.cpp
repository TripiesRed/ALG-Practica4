#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> 

using namespace std;

//Definimos el tipo de objeto Aviso
struct Aviso{

    double duration, deadline, penalty;
    double penalty_per_day;

    //Constructores

    Aviso(int d, int dl, int p){
        duration = d;
        deadline = dl;
        penalty = p;
        penalty_per_day = penalty/duration;
    }

    Aviso (const Aviso &other){
        duration = other.duration;
        deadline = other.deadline;
        penalty = other.penalty;
        penalty_per_day = penalty/duration;
    }

    //Operadores

    bool operator<(const Aviso& other){
        return penalty < other.penalty;
    }

    bool operator==(const Aviso& other){
        return (duration == other.duration) && (deadline == other.deadline)
                && (penalty == other.penalty);
    }

    bool operator!=(const Aviso& other){
        return !(*this == other);
    }

};

/*****************************************************************************/
//FUNCIONES AUXILIARES

static const int INVALID = -1;

// Función de comparación entre dos objetos de tipo Aviso, que usaremos para
// ordenar el vector de Avisos de mayor a menor según la penalización
bool comp(Aviso &l, Aviso &r){
        return l.penalty > r.penalty;
}

// Calcula la duración total de realizar todos los Avisos de un vector
double totalDuration(vector<Aviso> v){
    double total = 0;

    for(auto it : v) total += it.duration;

    return total;
}

// Calcula la penalización total de todos los Avisos de un vector
double totalPenalty(vector<Aviso> v){
    double total = 0;

    for(auto it : v) total += it.penalty;

    return total;
}

// Calcula la penalización media por día total de todos los Avisos de un vector
double totalPenaltyPerDay(vector<Aviso> v){
    double total = 0;

    for(auto it : v) total += it.penalty_per_day;

    return total;
}

// Función que devuelve 'true' si el objeto Aviso ya está en el vector v,
// 'false' en otro caso
bool alreadyIn(vector<Aviso> v, Aviso a){
    bool already_in = false;
    for(auto it : v){
        if(it == a)already_in = true;
    }

    return already_in;
}

// Función de factibilidad de un nodo. Indica si el nodo cumple los requisitos
// para ser tenido en cuenta para la solucion
bool Factible(vector<Aviso> v, Aviso nodo){
    double total_duration = 0;

    if(!v.empty())
        total_duration = totalDuration(v) + nodo.duration;

    return nodo.deadline >= total_duration;
}

// Función que calcula el total de nodos factibles que aún quedan por explorar
// en la rama actual
int nNodosFactibles(vector<Aviso> partial_sol, vector<Aviso> grafo, Aviso a, int k=0){
    int size = grafo.size();
    int n = 0;

    for(int i = k; i < size; i++){
        if(Factible(partial_sol,grafo[i]) && a != grafo[i])
            n++;
    }

    return n;
}

// Función que dado un vector de Avisos, busca la posición del Aviso con 
// mayor penalización factible a partir de una posición dada k
int findMayorPenaltyFactible(vector<Aviso> partial_sol,vector<Aviso> grafo,int k=0){
    double max = grafo[k].penalty, pos = INVALID;
    int size = grafo.size();

    for(int i = k; i < size; i++){
        if(grafo[i].penalty > max && Factible(partial_sol,grafo[i]) && 
            !alreadyIn(partial_sol,grafo[i])){
            max = grafo[i].penalty;
            pos = i;
        }
    }

    return pos;
}

/*****************************************************************************/
//FUNCIONES DE CÁLCULO DE COTA

// Función de Cota 1:
// Calcula la penalización actual de la solución parcial más la penalización
// recién añadida, por el número de nodos que quedan por visitar que verifican
// la función de factibilidad
double Cota1(vector<Aviso> partial_sol, vector<Aviso> grafo, Aviso a, int k){
    int nAvisos = nNodosFactibles(partial_sol, grafo,a, k);

    return totalPenalty(partial_sol) + grafo[k].penalty * nAvisos;
}

// Función de Cota 2:
// Calcula la penalización actual de la solución parcial más la penalización
// de los nodos que quedan por visitar que verifican la función de factibilidad
double Cota2(vector<Aviso> partial_sol, vector<Aviso> grafo, Aviso a, int k){
    double total = totalPenalty(partial_sol);
    int size = grafo.size();

    for(int i = k+1; i < size; i++){
        if(Factible(partial_sol,grafo[i]) && grafo[i] != a)
            total += grafo[i].penalty;
    }

    return total;
}

// Función de Cota 3:
// Calcula la media de penalización por día de trabajo del vector con la 
// solución parcial, más la penalización por día de los nodos que quedan por
// visitar que verifican la función de factibilidad
double Cota3(vector<Aviso> partial_sol, vector<Aviso> grafo, Aviso a, int k){
    double total = totalPenaltyPerDay(partial_sol);
    int size = grafo.size();
   
    for(int i = k+1; i < size; i++){
        if(Factible(partial_sol,grafo[i]) && a != grafo[i]){
            total += grafo[i].penalty;
            
        }
    }

    return total;
}


/*****************************************************************************/
// Implementación del algoritmo Backtracking
vector<Aviso> BackTracking(vector<Aviso> grafo, int cota){

    //Definimos variables
    vector<Aviso> greedy, partial_sol, final_sol;
    int size = grafo.size();
    double cota_global, cota_local, aux = 0;
    bool sigue;
    
    sort(grafo.begin(), grafo.end(), comp); //Ordenamos vector

    //Calculamos la Cota Global aplicando greedy sobre el vector ordenado
    for(int i = 0; i < size; i++)
        if(Factible(greedy,grafo[i]))
            greedy.push_back(grafo[i]);
    
    //Definimos cotas según el tipo introducido
    switch (cota)
    {
    case 3:
        cota_global =totalPenaltyPerDay(greedy);
        break;
    
    default:
        cota_global = totalPenalty(greedy);
        break;
    }
    

    //Recorremos el grafo en busca de la solución óptima
    for (int i = 0; i < size; i++){
        Aviso a = grafo[i];
        sigue = true;
        partial_sol.push_back(a);

        for(int j = 0; j < size && sigue; j++){

            //Función de factibilildad
            if(a!=grafo[j] && Factible(partial_sol, grafo[j])){ 
                partial_sol.push_back(grafo[j]);

                //Seleccionamos función de cota
                switch (cota)
                {
                case 2:
                    cota_local = Cota2(partial_sol, grafo, a, j);
                    break;
                
                case 3:
                    cota_local = Cota3(partial_sol, grafo, a, j);
                    break;
                
                default:
                    cota_local = Cota1(partial_sol, grafo, a, j);
                    break;
                }
                

                if(cota_local <= cota_global){ //Podamos 
                    sigue = false;
                    partial_sol.clear();
                }
                
            }
            
        }

        //En caso de haber encontrado un posible solución la guardamos 
        if(!partial_sol.empty()){
            final_sol = partial_sol;
            partial_sol.clear();

            cota_global = totalPenalty(final_sol); //Actualizamos la cota global
        }

    }   

    //Esto significa que la mejor solución es la voraz
    if(final_sol.empty())
        return greedy;

    return final_sol;
}


/*****************************************************************************/
/*****************************************************************************/

int main(int narg, char** arg){

    vector<Aviso> g, s;

    /*for(int i = 0; i < SIZE; i++){
        g.push_back(Aviso(1,9,i));
    }*/

    g.push_back(Aviso(2,5,50));
    g.push_back(Aviso(1,4,150));
    g.push_back(Aviso(2,4,13));
    g.push_back(Aviso(3,3,10));
    g.push_back(Aviso(1,4,199));
    g.push_back(Aviso(2,4,200));
    g.push_back(Aviso(1,4,33));
    g.push_back(Aviso(2,4,1000));
    g.push_back(Aviso(1,4,133));

    s = BackTracking(g, 1);

    int size = s.size();
    cout << "RESULTADO: " << endl;
    for(int i = 0; i < size; i++){
        cout << s[i].penalty << " ";
    }
    cout << endl;

    return 0;
}