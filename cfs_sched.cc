#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "multimap.h"
#include <algorithm>

// A class representing a task in the system
class Task {
 public:
  Task(char id, unsigned int start_time, unsigned int duration)
      : id_(id), start_time_(start_time), duration_(duration), vruntime_(0) {}

  char GetId() const { return id_; }
  unsigned int GetStartTime() const { return start_time_; }
  unsigned int GetDuration() const { return duration_; }
  unsigned int GetVruntime() const { return vruntime_; }

  void IncrementVruntime() { vruntime_++; }
  void SetVruntime(unsigned int vruntime) { vruntime_ = vruntime; }
  bool IsCompleted() const { return duration_ == 0; }
  void Run() { duration_--; }

  // For debugging
  friend std::ostream& operator<<(std::ostream& os, const Task& task) {
    os << "Task " << task.id_ << " (start: " << task.start_time_
       << ", duration: " << task.duration_ << ", vruntime: " << task.vruntime_
       << ")";
    return os;
  }

 private:
  char id_;
  unsigned int start_time_;
  unsigned int duration_;
  unsigned int vruntime_;
};

// A custom comparator for tasks based on their vruntime and ID
struct TaskComparator {
  bool operator()(const Task* a, const Task* b) const {
    if (a->GetVruntime() == b->GetVruntime()) {
      return a->GetId() < b->GetId();
    }
    return a->GetVruntime() < b->GetVruntime();
  }
};

// The CFS scheduler class
class CFSScheduler {
 public:
  // Add a task to be scheduled
  void AddTask(char id, unsigned int start_time, unsigned int duration) {
    tasks_.push_back(
        std::make_unique<Task>(id, start_time, duration));
  }

  // Run the scheduler until all tasks are complete
  void Run() {
    unsigned int current_tick = 0;
    unsigned int min_vruntime = 0;
    
    // Set to track ready tasks, ordered by vruntime and then by ID
    std::set<Task*, TaskComparator> ready_queue;
    Task* current_task = nullptr;

    while (true) {
      // Check for new tasks starting at this tick
      for (auto& task : tasks_) {
        if (task->GetStartTime() == current_tick && !task->IsCompleted()) {
          task->SetVruntime(min_vruntime);
          ready_queue.insert(task.get());
        }
      }

      // If we have a current task that's not completed, put it back in the queue
      if (current_task && !current_task->IsCompleted()) {
        ready_queue.insert(current_task);
        current_task = nullptr;
      }

      // Select the next task with the lowest vruntime
      if (!current_task && !ready_queue.empty()) {
        current_task = *ready_queue.begin();
        ready_queue.erase(ready_queue.begin());
        min_vruntime = current_task->GetVruntime();
      }

      // Display queue size - count current task as part of the total for display
      int display_size = ready_queue.size();
      if (current_task) {
        display_size += 1;
      }

      // Output the current tick state
      std::cout << current_tick << " [" << display_size << "]: ";

      // Execute the current task for one tick if available
      if (current_task) {
        std::cout << current_task->GetId();
        current_task->Run();
        current_task->IncrementVruntime();
        
        // Mark completed tasks
        if (current_task->IsCompleted()) {
          std::cout << "*";
          current_task = nullptr;
        }
      } else {
        std::cout << "_";
      }
      std::cout << std::endl;

      // Increment time
      current_tick++;

      // Check if we're done (no current task, empty queue, and no future tasks)
      if (!current_task && ready_queue.empty()) {
        bool any_future_tasks = false;
        for (auto& task : tasks_) {
          if (task->GetStartTime() > current_tick || !task->IsCompleted()) {
            any_future_tasks = true;
            break;
          }
        }
        if (!any_future_tasks) {
          break;
        }
      }
    }
  }

 private:
  std::vector<std::unique_ptr<Task>> tasks_;
};

int main(int argc, char* argv[]) {
  // Check if a file name was provided
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <task_file.dat>" << std::endl;
    return 1;
  }

  // Open the file
  std::ifstream file(argv[1]);
  if (!file) {
    std::cerr << "Error: cannot open file " << argv[1] << std::endl;
    return 1;
  }

  // Create scheduler
  CFSScheduler scheduler;

  // Read tasks from file
  char id;
  unsigned int start_time, duration;
  while (file >> id >> start_time >> duration) {
    scheduler.AddTask(id, start_time, duration);
  }

  // Run the scheduler
  scheduler.Run();

  return 0;
}