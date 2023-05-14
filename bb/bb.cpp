#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm> 
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>


using namespace std;
using namespace chrono;

//Definimos el tipo de objeto Aviso
struct Aviso{

    int duration, deadline, penalty;
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

string to_s(Aviso A){
    
    string s = "{" + to_string(A.duration) + ", " + to_string(A.deadline) + ", " + to_string(A.penalty) + "}";
    return s;
}

//Clase grafo de Avisos
class Grafo{
private:
    
    vector<Aviso> v;
    double totalDuration, totalPenalty, totalPenaltyPerDay;

public:

    //Constructores
    Grafo(){
        totalDuration = 0;
        totalPenalty = 0;
        totalPenaltyPerDay = 0;
    }
    

    Grafo(const Grafo& other){
        v = other.v;

        totalDuration = other.totalDuration;
        totalPenalty = other.totalPenalty;
        totalPenaltyPerDay = other.totalPenaltyPerDay;
    }
    

    //Consultores
    double getTotalDuration() {return totalDuration;}

    double getTotalPenalty() {return totalPenalty;}

    double getTotalPenaltyPerDay() {return totalPenaltyPerDay;}

    void push_back(Aviso a, double extra_pen){
        v.push_back(a);

        totalDuration += a.duration;
        totalPenalty += extra_pen;
       // totalPenaltyPerDay += a.penalty_per_day;
    }


    double plusDuration (Aviso a){
        return totalDuration + a.duration;
    }

    int size() const {return v.size();}

    vector<Aviso>::iterator begin(){return v.begin();}

    vector<Aviso>::iterator end(){return v.end();}

    Aviso& operator[](int i) {return v[i];}

    const Aviso& operator[](int i) const {return v[i];}

    bool empty() {return v.empty();}

    void erase(int pos) {
        v.erase(v.begin() + pos);
    }

    void clear() {
        v.clear();

        totalDuration = 0;
        totalPenalty = 0;
        totalPenaltyPerDay = 0;
    }

    void print(){
        for(int i = 0; i < v.size(); i++){
            cout << to_s(v[i]) << endl;
        }
    }
};

/*****************************************************************************/
static const int GLOBAL_LIMIT = 0;
static const int INVALID = -1;

bool Feasible(int current_day, Aviso A){

    return ((current_day + A.duration) <= A.deadline);
}

int GetPositionLowerLimit(vector<double> limits){
    
    auto min_it = min_element(limits.begin(), limits.end());

    if(*min_it == INFINITY){
        return INVALID;
    }
    else{
        return distance(limits.begin(), min_it);
    }
}
void Cota2(Grafo partial_sol,vector<Aviso> v, Aviso A){}
/*****************************************************************************/
 
double Cota1(Grafo partial_sol, vector<Aviso> v, Aviso A){  //Funcion de prueba
      
    if(!Feasible(partial_sol.getTotalDuration(), A)){
        return INFINITY;
    }
    else{
        double estimated_penalty = 0;
        double duration = partial_sol.plusDuration(A);

        for(int i = 0; i < v.size(); i++){

            if(v[i] != A){
                
                if(!Feasible(duration, v[i])){
                    estimated_penalty += v[i].penalty;
                }
            }  
        }

        return partial_sol.getTotalPenalty() + estimated_penalty;
    }

}

/*****************************************************************************/

Grafo BandB(vector<Aviso> v, int cota){

    double (*function)(Grafo, vector<Aviso>, Aviso) = nullptr;
    
    switch (cota) {
        case 1:
            function = Cota1;
            break;
        case 2:
            function = Cota1;
            break;
        case 3:
            function = Cota1;
            break;
        default:
            cerr << "Error: la función de cota ingresada no es válida" << endl;
            exit(-1);
    }

    Grafo solution;
    double current_limit = GLOBAL_LIMIT;
    bool all_nodes_unfeasible = false;

    do{

        vector<double> limits;
        for(int i = 0; i < v.size(); i++){
            limits.push_back(function(solution, v, v[i]));
        }

        int min_pos = GetPositionLowerLimit(limits);

        if(min_pos != INVALID){
            solution.push_back(v[min_pos], limits[min_pos]);
            current_limit += limits[min_pos];
            v.erase(v.begin() + min_pos);

            for(int i = 0; i < v.size(); i++){
                if(!Feasible(solution.getTotalDuration(), v[i])){
                    v.erase(v.begin() + i);
                }
            }
        }
        else{
            all_nodes_unfeasible = true;
        }

    }while(!v.empty() && !all_nodes_unfeasible);

    return solution;
}


int main(int argc, char** argv){

/*
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <funcion_de_cota {1,2,3}> "<< " <fichero_entrada> " 
             << " <fichero_salida> " << endl;
        return 1;
    }
    
    Grafo grafo, sol;
    string line;
    int fcota = 0;

    //ENTRADA DEL PROBLEMA
    fcota = stoi(argv[1]);

    // Abrimos el fichero
    ifstream file(argv[2]);
    if (!file.is_open()) {
        cerr << "No se pudo abrir el fichero" << endl;
        return 1;
    }

    double v1, v2, v3;
    int k = 0;

    // Leemos el fichero línea a línea 
    while (getline(file, line)) {

        istringstream iss(line);
        
        if (!(iss >> v1 >> v2 >> v3)) {
            std::cerr << "Error al leer el archivo en la línea: " << k << std::endl;
            return 1;
        }

        k++;
        // Lo añadimos al vector
        grafo.push_back(Aviso(v1,v2,v3));
    }
	
	// Cerramos el fichero
    file.close();

    
    //CÁLCULO DEL TIEMPO EMPÍRICO

    //Inicializamos las variables para calcular el tiempo que tarda el algoritmo
    high_resolution_clock::time_point t_antes, t_despues;
	duration<double> transcurrido;
    
    //Pasamos los datos del vector y calculamos el tiempo que tarda
    t_antes = high_resolution_clock::now();
  //  sol = BackTracking(grafo, fcota);
    t_despues = high_resolution_clock::now();
    
    transcurrido = duration_cast<duration<double>>(t_despues - t_antes);
    cout << grafo.size() << "\t" << transcurrido.count() << endl;
    

    //SALIDA DEL PROGRAMA (Solución al Problema)

    //Abrimos el fichero de salida
    ofstream ofile(argv[3]);
    
    if (!ofile.is_open()) {
        cerr << "No se pudo abrir el fichero de salida" << endl;
        return 1;
    }
    
    for(int i = 0; i < sol.size(); i++){
        line = to_string((int)sol[i].penalty) + "\n";
        ofile << line;
    }

    
    //Cerramos el fichero
    ofile.close();
*/

    vector<Aviso> v;

    v.push_back({2, 3, 50});
    v.push_back({1, 4, 150});
    v.push_back({2, 4, 13});
    v.push_back({3, 3, 10});

    Grafo g;
    g = BandB(v, 1);

    g.print();

    return 0;
}