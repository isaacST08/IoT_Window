#include "action_queue.hh"

#include <pico/critical_section.h>

using namespace stepper_motor::action;

ActionQueue::ActionQueue() {
  this->head = 0;
  this->tail = 0;
  this->count = 0;

  critical_section_init(&this->crit_sec);
}

void ActionQueue::deinit() { critical_section_deinit(&this->crit_sec); }

bool ActionQueue::isEmpty() { return (this->count == 0); }

bool ActionQueue::isFull() { return (this->count >= AQ_CAPACITY); }

int ActionQueue::getCount() { return this->count; }

int ActionQueue::getCapacity() { return AQ_CAPACITY; }

Action* ActionQueue::peek() {
  Action* result = NULL;

  // Acquire the lock.
  critical_section_enter_blocking(&this->crit_sec);

  // If the queue is not empty, get the head of the queue.
  if (!this->isEmpty()) result = &this->buffer[this->head];

  // Release the lock and return the result.
  critical_section_exit(&this->crit_sec);
  return result;
}

void ActionQueue::clear() {
  critical_section_enter_blocking(&this->crit_sec);

  this->head = 0;
  this->tail = 0;
  this->count = 0;

  critical_section_exit(&this->crit_sec);
}

bool ActionQueue::enqueue(Action action) {
  // Acquire the lock for performing operations.
  critical_section_enter_blocking(&this->crit_sec);

  // Return false if the queue is already full.
  if (this->isFull()) {
    critical_section_exit(&this->crit_sec);
    return false;
  }

  // If the queue is not full, add the action to the queue.
  else {
    switch (action.action_type) {
      // None actions are never queued.
      case ActionType::NONE:
        break;

      default:
        // Add the action to the queue buffer.
        this->buffer[this->tail] = action;

        // Record the addition of another action to the queue.
        this->tail = AQ_ADVANCE_INDEX(this->tail);
        this->count++;

        break;
    };

    // Release the lock on the CS.
    critical_section_exit(&this->crit_sec);

    // Return that the enqueue operation succeeded.
    return true;
  }
}

bool ActionQueue::enqueue(ActionType action_type, ActionData action_data) {
  Action action;
  action.action_type = action_type;
  action.data = action_data;

  return this->enqueue(action);
}

bool ActionQueue::enqueue(ActionType action_type) {
  Action action;
  action.action_type = action_type;
  action.data.null = 0;

  return this->enqueue(action);
}

Action ActionQueue::dequeue() {
  // Acquire the lock for performing operations.
  critical_section_enter_blocking(&this->crit_sec);

  // If the queue is empty, return a "NONE" action with no data.
  if (this->isEmpty()) {
    critical_section_exit(&this->crit_sec);

    Action action;
    action.action_type = ActionType::NONE;
    action.data.null = 0;

    return action;
  }

  // If the queue is not empty, pop off and return the action at the head of the
  // queue.
  else {
    // Get the head action.
    Action action = this->buffer[this->head];

    // Remove the action from the queue.
    this->head = AQ_ADVANCE_INDEX(this->head);
    this->count--;

    // Release the lock.
    critical_section_exit(&this->crit_sec);

    return action;
  }
}
