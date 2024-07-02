#include <bits/stdc++.h>

using namespace std;

struct Exm {
    int id;
    int cant;
};

struct Stu {
    string id;
    int idExm;
};

// Leer archivo de exámenes
vector<Exm> ReadFileExm(string name) {
    ifstream file;
    file.open(name);
    if (!file) {
        cerr << "Unable to open file " << name;
        exit(1);
    }
    int id, cant;
    vector<Exm> vec;
    while (file >> id >> cant) {
        Exm exm;
        exm.id = id;
        exm.cant = cant;
        vec.push_back(exm);
    }
    file.close();
    return vec;
}

// Leer archivo de estudiantes
vector<Stu> ReadFileStu(string name) {
    ifstream file;
    file.open(name);
    if (!file) {
        cerr << "Unable to open file " << name;
        exit(1);
    }
    int idExm;
    string id;
    vector<Stu> vec;
    while (file >> id >> idExm) {
        Stu stu;
        stu.id = id;
        stu.idExm = idExm;
        vec.push_back(stu);
    }
    file.close();
    return vec;
}

// Crear matriz de conflictos
vector<vector<int>> CrearMatrizDeConflictos(vector<Exm> exm, vector<Stu> stu) {
    int n = exm.size();
    vector<vector<int>> matriz(n, vector<int>(n, 0));
    unordered_map<int, unordered_set<string>> examenAlumnos;

    // Llenar el mapa con los alumnos que toman cada examen
    for (const auto& alumno : stu) {
        examenAlumnos[alumno.idExm].insert(alumno.id);
    }

    // Crear la matriz de conflictos
    for (int i = 0; i < n; ++i) {
        for (int j = i; j < n; ++j) {
            if (i == j) {
                matriz[i][j] = 0;
            } else {
                // Verificar si hay algún alumno en común entre los exámenes i y j
                const auto& alumnosI = examenAlumnos[exm[i].id];
                const auto& alumnosJ = examenAlumnos[exm[j].id];
                for (const auto& alumno : alumnosI) {
                    if (alumnosJ.find(alumno) != alumnosJ.end()) {
                        matriz[i][j] = 1;
                        matriz[j][i] = 1;
                        break;
                    }
                }
            }
        }
    }
    return matriz;
}

// Función para asignar exámenes a bloques de tiempo usando un algoritmo greedy
vector<int> AsignarExamenesAGreedy(vector<Exm>& exm, vector<vector<int>>& matrizDeConflictos) {
    int n = exm.size();
    vector<int> asignacionBloques(n, -1);
    vector<bool> bloquesUsados(n, false);

    for (int i = 0; i < n; ++i) {
        for (int t = 0; t < n; ++t) {
            bool conflicto = false;
            for (int j = 0; j < n; ++j) {
                if (matrizDeConflictos[i][j] == 1 && asignacionBloques[j] == t) {
                    conflicto = true;
                    break;
                }
            }
            if (!conflicto) {
                asignacionBloques[i] = t;
                bloquesUsados[t] = true;
                break;
            }
        }
    }

    return asignacionBloques;
}

// Función para calcular la penalización. Función de Evaluación.
double calcularPenaltyPromedio(const vector<int>& asignacion, const vector<Stu>& estudiantes, const unordered_map<int, int>& examenABloque) {
    unordered_map<string, vector<int>> alumnoExamenes;
    for (const auto& alumno : estudiantes) {
        alumnoExamenes[alumno.id].push_back(alumno.idExm);
    }

    int totalPenalty = 0;

    for (const auto& pair : alumnoExamenes) {
        const auto& examenes = pair.second;
        for (size_t i = 0; i < examenes.size(); ++i) {
            for (size_t j = i + 1; j < examenes.size(); ++j) {
                int diff = abs(examenABloque.at(examenes[i]) - examenABloque.at(examenes[j]));
                if (diff == 1) totalPenalty += 16;
                else if (diff == 2) totalPenalty += 8;
                else if (diff == 3) totalPenalty += 4;
                else if (diff == 4) totalPenalty += 2;
                else if (diff == 5) totalPenalty += 1;
            }
        }
    }
    unordered_set<string> uniqueStudents;
    for (const auto& stu : estudiantes) {
        uniqueStudents.insert(stu.id);
    }
    int totalAlumnos = uniqueStudents.size()+1;

    return static_cast<double>(totalPenalty) / totalAlumnos;
}

// El movimiento que se utilizará es un simple "swap" (intercambio) de dos exámenes asignados a diferentes bloques de tiempo.
// Este movimiento cambia la asignación de dos exámenes y evalúa si mejora la penalización promedio.
// Debe ir combinado con esFactible para verificar si la asignación resultante es factible.
void swapBloques(vector<int>& asignacion, int i, int j) {
    std::swap(asignacion[i], asignacion[j]);
}

// Función para verificar si una asignación es factible
bool esFactible(const vector<int>& asignacion, const vector<Stu>& estudiantes, const vector<Exm> exm) {
    unordered_map<int, int> examenABloque;
    for (size_t i = 0; i < exm.size(); ++i) {
        examenABloque[exm[i].id] = asignacion[i];
    }

    unordered_map<string, unordered_set<int>> alumnoBloques;
    for (const auto& alumno : estudiantes) {
        int bloque = examenABloque.at(alumno.idExm);
        if (alumnoBloques[alumno.id].count(bloque) > 0) {
            return false; // Conflicto encontrado
        }
        alumnoBloques[alumno.id].insert(bloque);
    }
    return true;
}


// Función para generar los archivos de salida
void OutputSolucion(std::vector<int>& asignacion, const std::vector<Exm>& exm, const std::vector<Stu>& estudiantes) {
    int total_timeslots = 0;
    std::unordered_set<int> unique_timeslots(asignacion.begin(), asignacion.end());
    total_timeslots = unique_timeslots.size();

    // Crear archivo problem.res
    std::ofstream res_file("problem.res");
    if (res_file.is_open()) {
        res_file << total_timeslots << std::endl;
        res_file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo problem.res" << std::endl;
    }

    // Crear archivo problem.sol
    std::ofstream sol_file("problem.sol");
    if (sol_file.is_open()) {
        for (size_t i = 0; i < asignacion.size(); ++i) {
            sol_file << exm[i].id << " " << asignacion[i] << std::endl;
        }
        sol_file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo problem.sol" << std::endl;
    }
    //swapBloques(asignacion, 0, 2);
    // Calcular penalización promedio y crear archivo problem.pen
    unordered_map<int, int> examenABloque;
    for (size_t i = 0; i < exm.size(); ++i) {
        examenABloque[exm[i].id] = asignacion[i];
    }
    double penalizacionPromedio = calcularPenaltyPromedio(asignacion, estudiantes, examenABloque);
    std::cout <<"Penalización Promedio: " <<penalizacionPromedio << std::endl;

    std::ofstream pen_file("problem.pen");
    if (pen_file.is_open()) {
        pen_file << penalizacionPromedio << std::endl;
        pen_file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo problem.pen" << std::endl;
    }
}
unordered_map<int, int> generarExamenABloque(const vector<int>& asignacion, const vector<Exm>& exm) {
    unordered_map<int, int> examenABloque;
    for (size_t i = 0; i < exm.size(); ++i) {
        examenABloque[exm[i].id] = asignacion[i];
    }
    return examenABloque;
}

// Tamaño máximo de la lista tabú
const int TAMANO_LISTA_TABU = 100;

// Estructura para la lista tabú (FIFO)
queue<string> tabuList;


// Función para verificar si un movimiento está en la lista tabú
// Función para verificar si un movimiento está en la lista tabú
bool esMovimientoTabu(int i, int j, const queue<string>& tabuList) {
    string movimiento = to_string(i) + "_" + to_string(j);
    
    // Creamos una copia de la cola para no modificar la original
    queue<string> tempQueue = tabuList;

    // Iteramos sobre la cola usando un bucle while
    while (!tempQueue.empty()) {
        if (tempQueue.front() == movimiento) {
            return true;  // Encontramos el movimiento en la lista tabú
        }
        tempQueue.pop();  // Pasamos al siguiente elemento en la cola
    }
    
    return false;  // No encontramos el movimiento en la lista tabú
}


// Función para actualizar la lista tabú después de aplicar un movimiento
void actualizarListaTabu(queue<string>& tabuList, int i, int j) {
    string movimiento = to_string(i) + "_" + to_string(j);
    tabuList.push(movimiento);
    if (tabuList.size() > TAMANO_LISTA_TABU) {
        tabuList.pop(); // Eliminar el movimiento más antiguo (FIFO)
    }
}

// Función para ejecutar el algoritmo Tabu Search
void tabuSearch(vector<int>& asignacionGreedy, const vector<Exm>& exm, const vector<Stu>& stu, const vector<vector<int>>& matrizDeConflictos, int maxIteraciones) {
    int n = exm.size();
    vector<int> mejorAsignacion = asignacionGreedy;
    vector<int> mejorVecindario(n);

    int iteracion = 0;
    while (iteracion < maxIteraciones) {
        double mejorPenalizacion = numeric_limits<double>::max();
        int mejorI = -1, mejorJ = -1;

        // Generar vecindario y encontrar el mejor movimiento no tabú
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                // Intercambiar exámenes i y j
                swapBloques(mejorAsignacion, i, j);

                // Calcular penalización promedio del nuevo estado si es factible y no está en la lista tabú
                if (esFactible(mejorAsignacion, stu, exm)) {
                    double penalizacion = calcularPenaltyPromedio(mejorAsignacion, stu, generarExamenABloque(mejorAsignacion, exm));

                    // Evaluar si es el mejor movimiento encontrado
                    if (penalizacion < mejorPenalizacion && !esMovimientoTabu(i, j, tabuList)) {
                        mejorPenalizacion = penalizacion;
                        mejorVecindario = mejorAsignacion;
                        mejorI = i;
                        mejorJ = j;
                    }
                }

                // Deshacer el intercambio para explorar otros movimientos
                swapBloques(mejorAsignacion, i, j);
            }
        }

        // Aplicar el mejor movimiento encontrado
        if (mejorI != -1 && mejorJ != -1) {
            swapBloques(mejorAsignacion, mejorI, mejorJ);
            actualizarListaTabu(tabuList, mejorI, mejorJ);
        }

        // Actualizar la mejor solución encontrada hasta ahora
        if (calcularPenaltyPromedio(mejorAsignacion, stu, generarExamenABloque(mejorAsignacion, exm)) < calcularPenaltyPromedio(mejorVecindario, stu, generarExamenABloque(mejorVecindario, exm))) {
            mejorVecindario = mejorAsignacion;
        }

        // Incrementar contador de iteraciones
        iteracion++;
        cout<<"Iteracion: "<<iteracion<<endl;
        cout<<"Penalizacion: "<< mejorPenalizacion <<endl;
    }

    // Generar archivos de salida con la mejor solución encontrada
    OutputSolucion(mejorVecindario, exm, stu);
}




int main() {
    std::string input;
    std::cout << "Nombre de los archivos a leer: ";
    std::cin >> input;
    int iter;
    cout << "Numero de iteraciones con largo de la lista tabú de 100: ";
    cin >> iter;
    vector<Exm> exm = ReadFileExm(input+".exm");
    vector<Stu> stu = ReadFileStu(input+".stu");
    vector<vector<int>> matrizDeConflictos = CrearMatrizDeConflictos(exm, stu);
    vector<int> asignacionGreedy = AsignarExamenesAGreedy(exm, matrizDeConflictos);
    tabuSearch(asignacionGreedy, exm, stu, matrizDeConflictos, iter);
    return 0;
}
