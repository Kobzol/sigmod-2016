#pragma once

/*
 * Bulk loading
 * Řazení výsledků
 * Conditional variables místo sleepu
 * */



#include <stdint.h>

#define DEBUG
#define DEALLOC

typedef uint32_t sigint;

// počet vláken v thread poolu pro grafové operace
#define THREAD_POOL_THREAD_COUNT (2)

// velikost fronty na joby
#define JOB_QUEUE_SIZE (10000)

// velikost bufferu na bariéry pro joby
#define BARRIER_BUFFER_COUNT JOB_QUEUE_SIZE

// otevře test/test-data.txt a přesměruje jej na standardní vstup aplikace
//#define REDIRECT_TEST_FILE_TO_INPUT
