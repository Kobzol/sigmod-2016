#pragma once

/*
 * TODO:
 * Bulk loading
 * Řazení výsledků - hash mapa
 * Conditional variables místo sleepu - hotovo (THREAD_USE_JOB_SYNC)
 * Časová razítka u hran
 * Komponenty (Union Find)
 * */



#include <stdint.h>

#define DEBUG
#define DEALLOC

typedef uint32_t sigint;

// počet vláken v thread poolu pro grafové operace
#define THREAD_POOL_THREAD_COUNT (23)

// použít synchronizaci místo čekání pevnou dobu
#define THREAD_USE_JOB_SYNC

// velikost fronty na joby
#define JOB_QUEUE_SIZE (200000)

// velikost bufferu na bariéry pro joby
#define BARRIER_BUFFER_COUNT JOB_QUEUE_SIZE

// otevře test/test-data.txt a přesměruje jej na standardní vstup aplikace
//#define REDIRECT_TEST_FILE_TO_INPUT
