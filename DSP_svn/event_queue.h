/* 
 * File:   event_queue.h
 * Author: Erik
 *
 * Created on den 22 december 2015, 01:51
 */

#ifndef EVENT_QUEUE_H
#define	EVENT_QUEUE_H

#ifdef	__cplusplus
//extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// Public type definitions
// =============================================================================

typedef enum event_priority_t
{
    EVENT_PRIO_LOW,
    EVENT_PRIO_HIGH
} event_priority_t;

typedef int16_t (*event_callback_t)(int16_t);

typedef struct event_t
{
    event_callback_t callback;
    event_priority_t priority;
    int16_t argument;
} event_t;

// =============================================================================
// Global variable declarations
// =============================================================================

// =============================================================================
// Global constatants
// =============================================================================

// =============================================================================
// Public function declarations
// =============================================================================

/**
 * @brief Pushes an event into the event queue.
 * @param e - The event to push onto the queue.
 * @return void
 */
void event_queue_push_event(event_t* e);

/**
 * @brief Pushes an event inot the event queue.
 * @param callback - The callback of the event.
 * @param arg - The callback argument of the event.
 * @param priority - The event priority.
 * @return void
 */
void event_queue_push_callback(event_callback_t callback,
                      int16_t arg,
                      event_priority_t priority);

/**
 * @brief Runs the next event in the event queue.
 * @param void
 * @return The return value from the event.
 */
int16_t event_queue_run_next(void);

/**
 * @brief Peeks the next event in the event queue.
 * @param void
 * @return A pointer to the next event in the queue.
 */
event_t* event_queue_peek(void);

/**
 * @brief Checks if the queue is empty.
 * @param void
 * @return True if the queue is empty, false otherwise.
 */
bool event_queue_is_empty(void);

/**
 * @brief Checks the number of events in the event queue.
 * @param void
 * @return The number of events in the queue.
 */
uint16_t event_queue_size(void);

#ifdef	__cplusplus
}
#endif

#endif	/* EVENT_QUEUE_H */

