// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "event_queue.h"
#include "pinmap.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================

// =============================================================================
// Private constants
// =============================================================================
#define QUEUE_SIZE (16u)

// =============================================================================
// Private variables
// =============================================================================
static event_t low_prio_queue[QUEUE_SIZE];
static event_t high_prio_queue[QUEUE_SIZE];

static uint16_t low_prio_first = 0;
static uint16_t low_prio_last = 0;
static uint16_t low_prio_size = 0;

static uint16_t high_prio_first = 0;
static uint16_t high_prio_last = 0;
static uint16_t high_prio_size = 0;

// =============================================================================
// Private function declarations
// =============================================================================

// =============================================================================
// Public function definitions
// =============================================================================

void event_queue_push_event(event_t* e)
{
    switch (e->priority)
    {
    case EVENT_PRIO_LOW:
#ifdef DEBUG
        if (QUEUE_SIZE == low_prio_size)
        {
            RED_LED_ON;

            while (1);
        }
#elif !defined(NDEBUG)
#error DEBUG or NDEBUG should be defined as project macros.
#endif

        if (low_prio_size++)
        {
            ++low_prio_last;
        }
        
        if (QUEUE_SIZE == low_prio_last)
        {
            low_prio_last = 0;
        }

        low_prio_queue[low_prio_last].callback = e->callback;
        low_prio_queue[low_prio_last].argument = e->argument;
        break;

    case EVENT_PRIO_HIGH:
#ifdef DEBUG
        if (QUEUE_SIZE == high_prio_size)
        {
            RED_LED_ON;

            while (1);
        }
#elif !defined(NDEBUG)
#error DEBUG or NDEBUG should be defined as project macros.
#endif

        if (high_prio_size++)
        {
            ++high_prio_last;
        }

        if (QUEUE_SIZE == high_prio_last)
        {
            high_prio_last = 0;
        }

        high_prio_queue[high_prio_last].callback = e->callback;
        high_prio_queue[high_prio_last].argument = e->argument;
        break;

    default:
        // If no priority was given, assume low priority.
#ifdef DEBUG
        if (QUEUE_SIZE == low_prio_size)
        {
            RED_LED_ON;

            while (1);
        }
#elif !defined(NDEBUG)
#error DEBUG or NDEBUG should be defined as project macros.
#endif
        ++low_prio_last;
        ++low_prio_size;

        if (QUEUE_SIZE == low_prio_last)
        {
            low_prio_last = 0;
        }

        low_prio_queue[low_prio_last].callback = e->callback;
        low_prio_queue[low_prio_last].argument = e->argument;
        break;
    }
}

void event_queue_push_callback(event_callback_t callback,
                      int16_t arg,
                      event_priority_t priority)
{
    switch (priority)
    {
    case EVENT_PRIO_LOW:
#ifdef DEBUG
        if (QUEUE_SIZE == low_prio_size)
        {
            RED_LED_ON;

            while (1);
        }
#elif !defined(NDEBUG)
#error DEBUG or NDEBUG should be defined as project macros.
#endif

        if (low_prio_size++)
        {
            ++low_prio_last;
        }

        if (QUEUE_SIZE == low_prio_last)
        {
            low_prio_last = 0;
        }

        low_prio_queue[low_prio_last].callback = callback;
        low_prio_queue[low_prio_last].argument = arg;
        break;

    case EVENT_PRIO_HIGH:
#ifdef DEBUG
        if (QUEUE_SIZE == high_prio_size)
        {
            RED_LED_ON;

            while (1);
        }
#elif !defined(NDEBUG)
#error DEBUG or NDEBUG should be defined as project macros.
#endif

        if (high_prio_size++)
        {
            ++high_prio_last;
        }

        if (QUEUE_SIZE == high_prio_last)
        {
            high_prio_last = 0;
        }

        high_prio_queue[high_prio_last].callback = callback;
        high_prio_queue[high_prio_last].argument = arg;
        break;

    default:
        // If no priority was given, assume low priority.
#ifdef DEBUG
        if (QUEUE_SIZE == low_prio_size)
        {
            RED_LED_ON;

            while (1);
        }
#elif !defined(NDEBUG)
#error DEBUG or NDEBUG should be defined as project macros.
#endif
        ++low_prio_last;
        ++low_prio_size;

        if (QUEUE_SIZE == low_prio_last)
        {
            low_prio_last = 0;
        }

        low_prio_queue[low_prio_last].callback = callback;
        low_prio_queue[low_prio_last].argument = arg;
        break;
    }
}

int16_t event_queue_run_next(void)
{
    int16_t arg;
    int16_t ret_val = 0;

    if (high_prio_size)
    {
        arg = high_prio_queue[high_prio_first].argument;
        ret_val = high_prio_queue[high_prio_first].callback(arg);

        if (0 != --high_prio_size)
        {
            ++high_prio_first;
        }   

        if (QUEUE_SIZE == high_prio_first)
        {
            high_prio_first = 0;
        }
    }
    else if (low_prio_size)
    {
        arg = low_prio_queue[low_prio_first].argument;
        ret_val = low_prio_queue[low_prio_first].callback(arg);

        if (0 != --low_prio_size)
        {
            ++low_prio_first;
        }
        
        if (QUEUE_SIZE == low_prio_first)
        {
            low_prio_first = 0;
        }
    }

    return ret_val;
}

event_t* event_queue_peek(void)
{
    event_t* e = NULL;

    if (high_prio_size)
    {
        e = &high_prio_queue[high_prio_first];
    }
    else if (low_prio_size)
    {
        e = &low_prio_queue[high_prio_first];
    }

    return e;
}

bool event_queue_is_empty(void)
{
    return (0 == (low_prio_size | high_prio_size));
}

uint16_t event_queue_size(void)
{
    return low_prio_size + high_prio_size;
}

// =============================================================================
// Private function definitions
// =============================================================================



