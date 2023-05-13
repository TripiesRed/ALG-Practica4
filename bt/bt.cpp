#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> 
#include <fstream>
#include <sstream>
#include <chrono>


using namespace std;
using namespace chrono;

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

    void push_back(Aviso a){
        v.push_back(a);

        totalDuration += a.duration;
        totalPenalty += a.penalty;
        totalPenaltyPerDay += a.penalty_per_day;
    }

    int size() const {return v.size();}

    vector<Aviso>::iterator begin(){return v.begin();}

    vector<Aviso>::iterator end(){return v.end();}

    Aviso& operator[](int i) {return v[i];}

    const Aviso& operator[](int i) const {return v[i];}

    bool empty() {return v.empty();}

    void clear() {
        v.clear();

        totalDuration = 0;
        totalPenalty = 0;
        totalPenaltyPerDay = 0;
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
bool Factible(Grafo v, Aviso nodo){
    double total_duration = 0;

    if(!v.empty())
        total_duration = v.getTotalDuration() + nodo.duration;

    return nodo.deadline >= total_duration;
}

// Función que calcula el total de nodos factibles que aún quedan por explorar
// en la rama actual
int nNodosFactibles(Grafo partial_sol, Grafo grafo, Aviso a, int k=0){
    int size = grafo.size();
    int n = 0;

    for(int i = k; i < size; i++){
        if(Factible(partial_sol,grafo[i]) && a != grafo[i])
            n++;
    }

    return n;
}

/*****************************************************************************/
//FUNCIONES DE CÁLCULO DE COTA

// Función de Cota 1:
// Calcula la penalización actual de la solución parcial más la penalización
// recién añadida, por el número de nodos que quedan por visitar que verifican
// la función de factibilidad
double Cota1(Grafo partial_sol, Grafo grafo, Aviso a, int k){
    int nAvisos = nNodosFactibles(partial_sol, grafo,a, k);

    return partial_sol.getTotalPenalty() + grafo[k].penalty * nAvisos;
}

// Función de Cota 2:
// Calcula la penalización actual de la solución parcial más la penalización
// de los nodos que quedan por visitar que verifican la función de factibilidad
double Cota2(Grafo partial_sol, Grafo grafo, Aviso a, int k){
    double total = partial_sol.getTotalPenalty();
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
double Cota3(Grafo partial_sol, Grafo grafo, Aviso a, int k){
    int nAvisos = nNodosFactibles(partial_sol, grafo,a, k);
    int size = grafo.size();
   

    return partial_sol.getTotalPenaltyPerDay() + grafo[k].penalty_per_day * nAvisos;
}


/*****************************************************************************/
// Implementación del algoritmo Backtracking
Grafo BackTracking(Grafo grafo, int cota){

    //Definimos variables
    Grafo greedy, partial_sol, final_sol;
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
        cota_global = greedy.getTotalPenaltyPerDay();
        break;
    
    default:
        cota_global = greedy.getTotalPenalty();
        break;
    }

    cout << "COTA GLOB: " << cota_global << endl;
    

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
                
                cout << "COTA LOCAL: " << cota_local << endl;

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

            switch (cota) //Actualizamos la cota global
            {
            case 3:
                cota_global = final_sol.getTotalPenaltyPerDay();
                break;
            
            default:
                cota_global = final_sol.getTotalPenalty();
                break;
            } 

            cout << "COTA GLOB (ACT): " << cota_global << endl;
        }

    }   

    //Esto significa que la mejor solución es la voraz
    if(final_sol.empty())
        return greedy;

    return final_sol;
}


/*****************************************************************************/
/*****************************************************************************/

int main(int argc, char** argv){

    //vector<Aviso> g, s;

    /*for(int i = 0; i < SIZE; i++){
        g.push_back(Aviso(1,9,i));
    }

    //Ejemplo 
    // Resultado correcto: 199 1000 150
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

    //Muestra resultado
    cout << "RESULTADO: " << endl;
    for(int i = 0; i < size; i++){
        cout << s[i].penalty << " ";
    }
    cout << endl;*/

     // Comprobamos que se ha pasado un argumento con la ruta del fichero
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
    sol = BackTracking(grafo, fcota);
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

    


    return 0;

    return 0;
}