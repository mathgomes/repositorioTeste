#include <iostream>
#include <string>
#include <cstdio>
#include <algorithm>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>
#define COLOR_NUM 4
using namespace std;

/**
Estrutura representando as informacoes de um vertice
    values : true ou false para cada valor do dominio
    remainingVal : quantidade de valroes remanescentes
    degree : grau do vertice
**/
typedef struct {

    bool *values;
    int remainingVal;
    int degree;

} Domain;

/**
Funcao booleana que checa se o char e um epaco ou ponto.
Usada para remover esses caracteres de um nome lido
**/
bool isCommaOrSpaceOrDot(char c) {

    switch(c) {
    //case ',':
    case '.':
    case ' ':
        return true;
    default:
        return false;
    }
}
pair<string,int> searchInVector(pair<string,int> state, vector<pair<string,int> > &states) {

    for( unsigned int j = 0; j < states.size(); j++) {

        if(get<0>(states[j]) == get<0>(state)) {
            return states[j];
        }
    }
    get<1>(state) = states.size();
    states.push_back(state);
    return state;
}
bool** allocateGraph(int vertexNum) {
    bool **Graph = new bool*[vertexNum];
    for (int i = 0; i < vertexNum ; ++i) {
        Graph[i] = new bool[vertexNum];
    }
    for( int i = 0; i < vertexNum; i ++) {
        for( int j = 0; j < vertexNum; j ++) {
            Graph[i][j] = false;
        }
    }
    return Graph;
}
void deleteMatrix (bool **Graph, int vertexNum) {
    for( int i = 0; i < vertexNum; i++ ){
        delete [] Graph[i];
    }
    delete [] Graph;
}
void addEdge(bool **Graph, int source, int destiny) {

    Graph[source][destiny] = true;
    Graph[destiny][source] = true;
}
void printfGraph(bool **Graph, int vertexNum) {
    cout << "\n";
    for (int i = 0; i < vertexNum; ++i) {
        for (int j = 0; j < vertexNum; ++j) {

           cout << Graph[i][j] << " ";
        }
        cout << "\n";
    }
}
void printSolution(int *solution, vector <  pair< string,int> > &colors, vector < pair < string, int > > &States,
                    int countriesNumber, int *attribution) {
    for( int i =0; i < countriesNumber; i ++) {
        cout << get<0>(States[i]) << ": " << get<0>(colors[solution[i]]) << ".\n";
    }
    cout << "\nAtribuicoes : " << *attribution;
}
bool isValidFC(bool **Country, int *subject, Domain *domain, int *countrieNumber, int *solution, int *color) {
    if(domain[*subject].values[*color] == false) {
        return false;
    }
    for(int i=0; i < *countrieNumber; i ++) {

        if(Country[*subject][i] && solution[i] == *color) {
            return false;
        }
    }
    return true;
}
int getNextFC(bool **Country, int *solution, int *countriesNumber) {
    for(int i = 0; i < *countriesNumber; i++) {
        if(solution[i] == -1) {
            return i;
        }
    }
    return -1;
}
bool FC_util(bool **Country, Domain *domain, int *solution, int *subject,
             int *countriesNumber, int *color) {

    for(int i = 0; i < *countriesNumber; i++) {
        if(Country[*subject][i] && solution[i] == -1) {
            if(domain[i].values[*color] == true) {
                domain[i].values[*color] = false;
                domain[i].remainingVal--;
            }
        }
    }

    for(int i =0; i < *countriesNumber; i ++) {
        if(Country[*subject][i] && solution[i] == -1 && domain[i].remainingVal == 0) {
            return false;
        }
    }
    return true;
}
void FC_util_reverse(bool **Country, Domain *domain, int *solution,
                     int *subject,int *countriesNumber, int *color) {
    for(int i = 0; i < *countriesNumber; i++) {
        if(Country[*subject][i] && solution[i] == -1) {
            if(domain[i].values[*color] == false) {
                domain[i].values[*color] = true;
                domain[i].remainingVal++;
            }
        }
    }
}

int getNextFC_MRV(bool **Country, Domain *domain, int *solution, int *countriesNumber) {
    int subject = 99;
    for(int i = 0; i < *countriesNumber; i++) {
        if(domain[i].remainingVal < subject && solution[i] == -1) {
            subject = i;
        }
    }
    if( subject == 99 ) {
        return -1;
    }
    return subject;
}

int getNextFC_MRV_grau(bool **Country, Domain *domain, int *solution, int *countriesNumber) {

    Domain subject;
    subject.remainingVal = 99;
    subject.degree = -99;
    int aux;

    for(int i = 0; i < *countriesNumber; i++) {
        if(domain[i].remainingVal <= subject.remainingVal && solution[i] == -1) {
            if(domain[i].degree > subject.degree) {
                subject.remainingVal = domain[i].remainingVal;
                subject.degree = domain[i].degree;
                aux = i;
            }
        }
    }
    if( subject.remainingVal == 99 ) {
        return -1;
    }
    return aux;
}
bool backTracking_forwardCheck_MRV_grau(bool **Country, int *solution,Domain *domain, int qtdColors,
                               int countriesNumber, int *attribution) {

    int subject = getNextFC_MRV_grau(Country,domain,solution,&countriesNumber);
    if(subject == -1) return true;

    for( int i =0; i < qtdColors; i ++) {

        if(isValidFC(Country,&subject,domain,&countriesNumber,solution,&i) ) {

            (*attribution)++;
            solution[subject] = i;
            if(FC_util(Country,domain,solution,&subject,&countriesNumber,&i) == false) {

                solution[subject] = -1;
                FC_util_reverse(Country,domain,solution,&subject,&countriesNumber,&i);
                continue;
            }
            if( backTracking_forwardCheck_MRV_grau(Country,solution,domain,qtdColors,countriesNumber,attribution)) {
                return true;
            }
            solution[subject] = -1;
            FC_util_reverse(Country,domain,solution,&subject,&countriesNumber,&i);
        }
    }

    return false;
}

bool backTracking_forwardCheck_MRV(bool **Country, int *solution,Domain *domain, int qtdColors,
                               int countriesNumber, int *attribution) {

    int subject = getNextFC_MRV(Country,domain,solution,&countriesNumber);
    if(subject == -1) return true;

    for( int i =0; i < qtdColors; i ++) {

        if(isValidFC(Country,&subject,domain,&countriesNumber,solution,&i) ) {

            (*attribution)++;
            solution[subject] = i;
            if(FC_util(Country,domain,solution,&subject,&countriesNumber,&i) == false) {

                solution[subject] = -1;
                FC_util_reverse(Country,domain,solution,&subject,&countriesNumber,&i);
                continue;
            }
            if( backTracking_forwardCheck_MRV(Country,solution,domain,qtdColors,countriesNumber,attribution)) {
                return true;
            }
            solution[subject] = -1;
            FC_util_reverse(Country,domain,solution,&subject,&countriesNumber,&i);
        }
    }

    return false;
}

bool backTracking_forwardCheck(bool **Country, int *solution,Domain *domain, int qtdColors,
                               int countriesNumber, int *attribution) {

    int subject = getNextFC(Country,solution,&countriesNumber);
    if(subject == -1) return true;

    for( int i =0; i < qtdColors; i ++) {

        if(isValidFC(Country,&subject,domain,&countriesNumber,solution,&i) ) {

            (*attribution)++;
            solution[subject] = i;
            if(FC_util(Country,domain,solution,&subject,&countriesNumber,&i) == false) {

                solution[subject] = -1;
                FC_util_reverse(Country,domain,solution,&subject,&countriesNumber,&i);
                continue;
            }
            if( backTracking_forwardCheck(Country,solution,domain,qtdColors,countriesNumber,attribution)) {
                return true;
            }
            solution[subject] = -1;
            FC_util_reverse(Country,domain,solution,&subject,&countriesNumber,&i);
        }
    }
    return false;
}

bool isValid(bool **Country, int *subject, int *countrieNumber, int *solution, int *color) {

    for(int i=0; i < *countrieNumber; i ++) {

        if(Country[*subject][i] && solution[i] == *color) {
            return false;
        }
    }
    return true;
}
bool backTracking(bool **Country, int *solution, int qtdColors,int countriesNumber, int *attribution) {

    int subject = getNextFC(Country,solution,&countriesNumber);
    if(subject == -1) return true;

    for( int i =0; i < qtdColors; i ++) {

        if(isValid(Country,&subject,&countriesNumber,solution,&i) ) {
            solution[subject] = i;
            (*attribution)++;
            if( backTracking(Country,solution,qtdColors,countriesNumber,attribution)) {
                return true;
            }
            solution[subject] = -1;
        }
    }
    return false;
}
int main() {

    clock_t begin = clock();

    /// variaveis de uso geral
    int countriesNumber;                    // numero de estados
    char heuristic;                         // heuristica a ser usada
    int numOfColors = 4;                    // numero de cores
    int attribution = 0;                    // quatidade de atribuicoes a variaveis

    //cin >> countriesNumber >> heuristic;    // le a primeira linha de entrada
    fscanf(arq,"%d  %c",countriesNumber,heuristic);

    /// variaveis usadas na leitura de entradas
    // armazena uma linha da entrada
    string line;
    // armazena o estado principal ( ate os dois pontos ) e o inteiro para o seu indice
    pair<string,int> mainState;
    // armazena os estados vizinhos ( separados por virgula ) e o inteiro para seus indices
    pair<string,int> neighbour;
    // armazena todos os estados e seus indices
    vector < pair < string, int > > States;
    // stream a ser usada para se ler o estado principal
    stringstream ss1;
    // stream a ser usada para se ler os estados auxiliares
    stringstream ss2;
    // string para amazenar tudo o que vem depois dos dois pontos na entrada por linha
    string neighbourLine;

    /// variaveis usadas na execucao do backtracking
    // Grafo representado por matriz
    bool **Country = allocateGraph(countriesNumber);
    // vetor com as solucoes
    int *solution = new int[countriesNumber];
    // cada cor possivel e seus indices
    vector < pair < string, int > > colors;
    colors.push_back(make_pair("Azul",0));
    colors.push_back(make_pair("Amarelo",1));
    colors.push_back(make_pair("Verde",2));
    colors.push_back(make_pair("Vermelho",3));

    /// variaveis usadas na verificacao adiante
    Domain *domain = new Domain[countriesNumber];
    // ignora uma linha para evitar erros
    cin.ignore();

    /// leitura de entradas
    //enquanto existir algo para ler
    while( getline(cin,line) ) {
        // armazena a linha lida na stream
        ss1 << line;
        // le o estado principal e armazena em mainState
        getline(ss1, get<0>(mainState), ':');
        // apaga caracteres espaço e ponto do nome lido
        get<0>(mainState).erase(
            remove_if(get<0>(mainState).begin(),get<0>(mainState).end(), &isCommaOrSpaceOrDot),get<0>(mainState).end()
        );
        // checa se esse par<nome,indice> ja esta no vetor de estados
        mainState = searchInVector(mainState,States);

        // le o restando da stream e armazena na linha de estados vizinhos
        while (getline(ss1, neighbourLine, '.')) {
            // transfere essa linha para a stream
            ss2 << neighbourLine;
            // le cada estado vizinho separado por virgula e armazena em neighbour
            while(getline(ss2, get<0>(neighbour), ',')) {

                // apaga caracteres espaço e ponto do nome lido
                get<0>(neighbour).erase(
                    remove_if(get<0>(neighbour).begin(),get<0>(neighbour).end(), &isCommaOrSpaceOrDot),get<0>(neighbour).end()
                );
                // checa se esse par<nome,indice> esta no vetor de estados
                neighbour = searchInVector(neighbour,States);
                // adiciona uma aresta entre o estado principal e seu vizinho
                addEdge(Country,get<1>(mainState),get<1>(neighbour));
            }
            // limpa a strea
            ss2.clear();
        }
        ss1.clear();
    }

    /*
    for ( unsigned int i = 0; i < States.size(); i ++) {
        cout << "\n" << get<0>(States[i]) << ", " << get<1>(States[i]) << "\n";
    }
    printfGraph(Country,countriesNumber);*/

    for( int i = 0; i < countriesNumber; i++) {
        solution[i] = -1;
    }
    for (int i = 0; i < countriesNumber ; ++i) {
        domain[i].values = new bool[numOfColors];
        domain[i].remainingVal = COLOR_NUM;
    }
    for( int i = 0; i < countriesNumber; i ++) {
        for( int j = 0; j < numOfColors; j ++) {
            domain[i].values[j] = true;
        }
    }
    for( int i = 0; i < countriesNumber; i ++) {
        for( int j = i+1; j < countriesNumber; j ++) {
            if(Country[i][j]) {
                domain[i].degree++;
            }
        }
    }
    switch(heuristic) {

    case 'a':
        // backtracking comum
        if(backTracking(Country,solution,numOfColors,countriesNumber,&attribution) ) {
            printSolution(solution,colors,States,countriesNumber,&attribution);
        }
        else {
            cout << "\nNão existe solucao\n ";
        }
        break;
    case 'b':
        // backtracking + forward checking
        if(backTracking_forwardCheck(Country,solution,domain,numOfColors,countriesNumber,&attribution) ) {
            printSolution(solution,colors,States,countriesNumber,&attribution);
        }
        else {
            cout << "\nNão existe solucao\n ";
        }
        break;
    case 'c':

        // backtracking + forward checking + mrv

        if(backTracking_forwardCheck_MRV(Country,solution,domain,numOfColors,countriesNumber,&attribution) ) {
            printSolution(solution,colors,States,countriesNumber,&attribution);
        }
        else {
            cout << "\nNão existe solucao\n ";
        }

        break;
    case 'd':
        // bt + fc + mrv + grau
        if(backTracking_forwardCheck_MRV_grau(Country,solution,domain,numOfColors,countriesNumber,&attribution) ) {
            printSolution(solution,colors,States,countriesNumber,&attribution);
        }
        else {
            cout << "\nNão existe solucao\n ";
        }

        break;

    }
    deleteMatrix(Country,countriesNumber);
    delete [] solution;
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    printf("\nTempo : %.4f",elapsed_secs);

    return 0;
}
