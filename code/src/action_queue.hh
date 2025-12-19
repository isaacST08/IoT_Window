#ifndef ACTION_QUEUE_HH
#define ACTION_QUEUE_HH

#include <pico/critical_section.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

// ============================================================================
// Action Queue Configuration Macros
// ============================================================================

// // Maximum size of argument buffer for each queued action.
// #ifndef AQ_ARG_BUFFER_SIZE
// #define AQ_ARG_BUFFER_SIZE 256
// #endif

// Queue capacity (number of actions that can be queued).
#ifndef AQ_CAPACITY
#define AQ_CAPACITY 8
#endif

// ============================================================================
// Action Queue Data Structures
// ============================================================================

namespace stepper_motor::action {

// Action type enum - must be defined here since action_queue.hh is included
// first
enum class ActionType { NONE, OPEN, CLOSE, MOVE_TO_PERCENT, MOVE_TO_STEP };

union ActionData {
  float percent;
  int64_t step;
  int null = 0;
};

/**
 * Represents a single queued action with its argument.
 */
struct Action {
  ActionType action_type;
  union ActionData data;
};

/**
 * A FIFO (First-In-First-Out) queue for stepper motor actions.
 * Uses a circular buffer implementation.
 */
class ActionQueue {
 private:
  Action buffer[AQ_CAPACITY];   // Queue members buffer.
  uint8_t head;                 // Index of front element.
  uint8_t tail;                 // Index of next empty slot.
  uint8_t count;                // Number of elements in queue.
  critical_section_t crit_sec;  // Critical section for the queue.

 public:
  /**
   * Initializes an action queue.
   */
  ActionQueue();

  /**
   * De-initializes this action queue.
   */
  void deinit();

  /** Gets whether the queue is empty. */
  bool isEmpty();

  /**
   * Gets whether the queue is full.
   */
  bool isFull();

  /**
   * Gets the number of members currently in the queue.
   */
  int getCount();

  /**
   * Gets the maximum number of members the queue can hold.
   */
  int getCapacity();

  /**
   * Gets the action at the head of the queue without removing it.
   *
   * @return The pointer to the action at the head of the queue at the time of
   * the function call.
   */
  Action* peek();

  /**
   * Clears the queue.
   */
  void clear();

  /**
   * Enqueues an action.
   *
   * @param action The action to enqueue.
   *
   * @returns TRUE if the action was successfully enqueued, FALSE if an error
   * occurred such as the queue already being full.
   */
  bool enqueue(Action action);

  /**
   * Enqueues an action.
   *
   * @param action_type The type of action to enqueue.
   * @param action_data The data for the action that is being enqueued.
   *
   * @returns TRUE if the action was successfully enqueued, FALSE if an error
   * occurred such as the queue already being full.
   */
  bool enqueue(ActionType action_type, ActionData action_data);

  /**
   * Enqueues an action with no data.
   *
   * @param action_type The type of action to enqueue.
   *
   * @returns TRUE if the action was successfully enqueued, FALSE if an error
   * occurred such as the queue already being full.
   */
  bool enqueue(ActionType action_type);

  /**
   * Dequeues and returns the element from the head of the queue.
   */
  Action dequeue();
};

// ============================================================================
// Action Queue Macros
// ============================================================================

// /**
//  * Initialize an action queue to empty state.
//  * @param q Pointer to ActionQueue struct
//  */
// #define AQ_INIT(q)  \
//   do {              \
//     (q)->head = 0;  \
//     (q)->tail = 0;  \
//     (q)->count = 0; \
//   } while (0)
//
// /**
//  * Check if the queue is empty.
//  * @param q Pointer to ActionQueue struct
//  * @return true if empty
//  */
// #define AQ_IS_EMPTY(q) ((q)->count == 0)
//
// /**
//  * Check if the queue is full.
//  * @param q Pointer to ActionQueue struct
//  * @return true if full
//  */
// #define AQ_IS_FULL(q) ((q)->count >= AQ_CAPACITY)
//
// /**
//  * Get the current size of the queue.
//  * @param q Pointer to ActionQueue struct
//  * @return Number of elements in queue
//  */
// #define AQ_SIZE(q) ((q)->count)
//
// /**
//  * Get the capacity of the queue.
//  * @return Maximum number of elements
//  */
// #define AQ_GET_CAPACITY() (AQ_CAPACITY)
//
// /**
//  * Peek at the front element without removing it.
//  * @param q Pointer to ActionQueue struct
//  * @return Pointer to front QueuedAction, or NULL if empty
//  */
// #define AQ_PEEK(q) (AQ_IS_EMPTY(q) ? NULL : &((q)->buffer[(q)->head]))
//
// /**
//  * Clear all elements from the queue.
//  * @param q Pointer to ActionQueue struct
//  */
// #define AQ_CLEAR(q) \
//   do {              \
//     (q)->head = 0;  \
//     (q)->tail = 0;  \
//     (q)->count = 0; \
//   } while (0)

/**
 * Advance an index with wrap-around.
 * @param idx The index to advance
 */
#define AQ_ADVANCE_INDEX(idx) (((idx) + 1) % AQ_CAPACITY)

// /**
//  * Enqueue an action without an argument.
//  * @param q Pointer to ActionQueue struct
//  * @param act The Action to enqueue
//  * @param result_var Variable to store result (true if success)
//  */
// #define AQ_ENQUEUE(q, act, result_var)         \
//   do {                                         \
//     if (AQ_IS_FULL(q)) {                       \
//       (result_var) = false;                    \
//     } else {                                   \
//       (q)->buffer[(q)->tail].action = (act);   \
//       (q)->buffer[(q)->tail].arg[0] = '\0';    \
//       (q)->buffer[(q)->tail].has_arg = false;  \
//       (q)->tail = AQ_ADVANCE_INDEX((q)->tail); \
//       (q)->count++;                            \
//       (result_var) = true;                     \
//     }                                          \
//   } while (0)
//
// /**
//  * Enqueue an action with an argument.
//  * @param q Pointer to ActionQueue struct
//  * @param act The Action to enqueue
//  * @param arg_ptr Pointer to argument string (can be NULL)
//  * @param arg_len Length of argument string
//  * @param result_var Variable to store result (true if success)
//  */
// #define AQ_ENQUEUE_WITH_ARG(q, act, arg_ptr, arg_len, result_var) \
//   do {                                                            \
//     if (AQ_IS_FULL(q)) {                                          \
//       (result_var) = false;                                       \
//     } else {                                                      \
//       (q)->buffer[(q)->tail].action = (act);                      \
//       if ((arg_ptr) != NULL && (arg_len) > 0) {                   \
//         int _aq_len = ((arg_len) < AQ_ARG_BUFFER_SIZE)            \
//                           ? (arg_len)                             \
//                           : (AQ_ARG_BUFFER_SIZE - 1);             \
//         memcpy((q)->buffer[(q)->tail].arg, (arg_ptr), _aq_len);   \
//         (q)->buffer[(q)->tail].arg[_aq_len] = '\0';               \
//         (q)->buffer[(q)->tail].has_arg = true;                    \
//       } else {                                                    \
//         (q)->buffer[(q)->tail].arg[0] = '\0';                     \
//         (q)->buffer[(q)->tail].has_arg = false;                   \
//       }                                                           \
//       (q)->tail = AQ_ADVANCE_INDEX((q)->tail);                    \
//       (q)->count++;                                               \
//       (result_var) = true;                                        \
//     }                                                             \
//   } while (0)
//
// /**
//  * Dequeue an action from the front of the queue.
//  * @param q Pointer to ActionQueue struct
//  * @param out_action Pointer to store the dequeued Action
//  * @param out_arg Buffer to store argument (can be NULL)
//  * @param out_arg_len Size of output argument buffer
//  * @param result_var Variable to store result (true if success)
//  */
// #define AQ_DEQUEUE(q, out_action, out_arg, out_arg_len, result_var) \
//   do {                                                              \
//     if (AQ_IS_EMPTY(q)) {                                           \
//       (result_var) = false;                                         \
//     } else {                                                        \
//       if ((out_action) != NULL) {                                   \
//         *(out_action) = (q)->buffer[(q)->head].action;              \
//       }                                                             \
//       if ((out_arg) != NULL && (out_arg_len) > 0) {                 \
//         if ((q)->buffer[(q)->head].has_arg) {                       \
//           int _aq_len = ((out_arg_len) < AQ_ARG_BUFFER_SIZE)        \
//                             ? ((out_arg_len) - 1)                   \
//                             : (AQ_ARG_BUFFER_SIZE - 1);             \
//           memcpy((out_arg), (q)->buffer[(q)->head].arg, _aq_len);   \
//           ((char*)(out_arg))[_aq_len] = '\0';                       \
//         } else {                                                    \
//           ((char*)(out_arg))[0] = '\0';                             \
//         }                                                           \
//       }                                                             \
//       (q)->head = AQ_ADVANCE_INDEX((q)->head);                      \
//       (q)->count--;                                                 \
//       (result_var) = true;                                          \
//     }                                                               \
//   } while (0)

}  // namespace stepper_motor::action

#endif
