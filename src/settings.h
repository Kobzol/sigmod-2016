#pragma once

/*
 * TODO:
 * Bulk loading
 * Časová razítka u hran - základ hotov
 * Komponenty (Union Find) - základní implementace hotova, přidat restrukturalizaci po mazání
 * */


#include <stdint.h>

#define DEBUG
#define DEALLOC

typedef uint32_t sigint;


// použít Union-Find pro kontrolu slabě propojených komponent
//#define USE_UNION_FIND

// bude při přidávání a mazání hran kontrolovat, jestli daná hrana už náhodou neexistuje
// asi zbytečné, pro init testovacich dat je tam jen jedna taková hrana
//#define CHECK_UNIQUE_EDGES

// povolí použití thread poolu
//#define USE_THREADS

// od kolikati hran se přepne na předpočítání cest u BFS při tvorbě indexu (dobrá hodnota 40-50)
#define HEURISTIC_FULL_QUERY_COUNT (40)

// počet vláken v thread poolu pro grafové operace
#define THREAD_POOL_THREAD_COUNT (4)

// co kolik dávek se mají normalizovat časová razítka
#define TIMESTAMP_NORMALIZE_RATE (10)

// polovina vláken bude provádět dotazy od začátku seznamu, druhá polovina od konce seznamu
//#define LOAD_BALANCER_TWO_SIDE

// zaznamena a vypise statistiky prubehu programu (zatím pouze pro singlethreaded verzi)
//#define COLLECT_STATS

// otevře test/test-data.txt a přesměruje jej na standardní vstup aplikace
#define REDIRECT_TEST_FILE_TO_INPUT
