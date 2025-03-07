#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
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
    Task* current_task = nullptr;

    // Continue until all tasks are done
    while (true) {
      // Check if there are new tasks to add at this tick
      std::vector<Task*> new_tasks;
      for (auto& task : tasks_) {
        if (task->GetStartTime() == current_tick) {
          task->SetVruntime(min_vruntime);
          new_tasks.push_back(task.get());
        }
      }

      // Sort new tasks by ID if there are multiple with the same start time
      std::sort(new_tasks.begin(), new_tasks.end(),
                [](const Task* a, const Task* b) {
                  return a->GetId() < b->GetId();
                });

      // Add new tasks to the timeline
      for (Task* task : new_tasks) {
        timeline_.Insert(task->GetVruntime(), task);
      }

      // Check if the current task should yield to another task
      if (current_task && !current_task->IsCompleted()) {
        if (current_task->GetVruntime() > min_vruntime) {
          timeline_.Insert(current_task->GetVruntime(), current_task);
          current_task = nullptr;
        }
      }

      // If no current task, get the next task from the timeline
      if (!current_task) {
        if (timeline_.Size() > 0) {
          current_task = timeline_.Get(timeline_.Min())[0];
          timeline_.Remove(timeline_.Min());
          min_vruntime = current_task->GetVruntime();
        }
      }

      // Print the current status
      std::cout << current_tick << " [" 
                << (" 1 " ) << "]: ";

      // Run the current task for one tick
      if (current_task) {
        std::cout << current_task->GetId();
        current_task->Run();
        current_task->IncrementVruntime();

        // If the task is completed, mark it and set current_task to nullptr
        if (current_task->IsCompleted()) {
          std::cout << "*";
          current_task = nullptr;
        }
      } else {
        std::cout << "_";
      }
      std::cout << std::endl;

      // Increment tick
      current_tick++;

      // Check if we're done (no current task and no tasks in timeline)
      if (!current_task && timeline_.Size() == 0) {
        bool any_future_tasks = false;
        for (auto& task : tasks_) {
          if (task->GetStartTime() > current_tick || !task->IsCompleted()) {
            any_future_tasks = true;
            break;
          }
        }
        if (!any_future_tasks) break;
      }
    }
  }

 private:
  std::vector<std::unique_ptr<Task>> tasks_;
  Multimap<unsigned int, Task*> timeline_;
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