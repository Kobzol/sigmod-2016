#pragma once

/*
 * TODO:
 * Bulk loading
 * Řazení výsledků - hash mapa
 * Conditional variables místo sleepu - hotovo (THREAD_USE_JOB_SYNC)
 * Časová razítka u hran
 * Komponenty (Union Find) - základní implementace hotova, přidat restrukturalizaci po mazání
 * */


#include <stdint.h>

#define DEBUG
#define DEALLOC

typedef uint32_t sigint;


// použít Union-Find pro slabě kontrolu slabě propojených komponent
#define USE_UNION_FIND

// bude při přidávání a mazání hran kontrolovat, jestli daná hrana už náhodou neexistuje
// asi zbytečné, pro init testovacich dat je tam jen jedna taková hrana
//#define CHECK_UNIQUE_EDGES

// povolí použití thread poolu
//#define USE_THREADS

// počet vláken v thread poolu pro grafové operace
#define THREAD_POOL_THREAD_COUNT (4)

// použít synchronizaci místo čekání pevnou dobu
#define THREAD_USE_JOB_SYNC

// velikost fronty na joby
#define JOB_QUEUE_SIZE (200000)

// velikost bufferu na bariéry pro joby
#define BARRIER_BUFFER_COUNT JOB_QUEUE_SIZE

// zaznamena a vypise statistiky prubehu programu (zatím pouze pro singlethreaded verzi)
//#define COLLECT_STATS

// otevře test/test-data.txt a přesměruje jej na standardní vstup aplikace
#define REDIRECT_TEST_FILE_TO_INPUT
