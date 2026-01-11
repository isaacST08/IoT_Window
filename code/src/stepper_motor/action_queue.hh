#ifndef ACTION_QUEUE_HH
#define ACTION_QUEUE_HH

#include <pico/critical_section.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

// **====================================================**
// ||          <<<<< Configuration Macros >>>>>          ||
// **====================================================**

// Queue capacity (number of actions that can be queued).
#ifndef AQ_CAPACITY
#define AQ_CAPACITY 8
#endif

// **===================================================**
// ||          <<<<< Action Queue Macros >>>>>          ||
// **===================================================**

/**
 * Advance an index with wrap-around.
 * @param idx The index to advance
 */
#define AQ_ADVANCE_INDEX(idx) (((idx) + 1) % AQ_CAPACITY)

// **==================================================**
// ||          <<<<< Action Definitions >>>>>          ||
// **==================================================**

namespace stepper_motor::action {

// Action type enum - must be defined here since action_queue.hh is included
// first
enum class ActionType {
  NONE,
  OPEN,
  CLOSE,
  MOVE_TO_PERCENT,
  MOVE_TO_STEP,
  HOME,
  CALIBRATE
};

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

}  // namespace stepper_motor::action

#endif
