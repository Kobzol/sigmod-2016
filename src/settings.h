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

// použít index pro heuristiku konektivity vrcholů
#define USE_INDEX

// bude při přidávání a mazání hran kontrolovat, jestli daná hrana už náhodou neexistuje
// asi zbytečné, pro init testovacich dat je tam jen jedna taková hrana
//#define CHECK_UNIQUE_EDGES

// povolí použití thread poolu
#define USE_THREADS

// počet vláken v thread poolu pro grafové operace
#define THREAD_POOL_THREAD_COUNT (4)

// co kolik dávek se mají normalizovat časová razítka
#define TIMESTAMP_NORMALIZE_RATE (10)

#define THREAD_SCHEDULE_DYNAMIC (0)
#define THREAD_SCHEDULE_STATIC (1)

// jak se budou rozdelovat joby mezi vlakny
#define THREAD_SCHEDULE THREAD_SCHEDULE_DYNAMIC

// otevře test/test-data.txt a přesměruje jej na standardní vstup aplikace
//#define REDIRECT_TEST_FILE_TO_INPUT
