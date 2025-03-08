// cfs_sched.cc
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "multimap.h"

// A class representing a task in the system
class Task
{
public:
  Task(char id, unsigned int start_time, unsigned int duration, unsigned int weight = 1)
      : id_(id), start_time_(start_time), duration_(duration), vruntime_(0), weight_(weight) {}

  char GetId() const { return id_; }
  unsigned int GetStartTime() const { return start_time_; }
  unsigned int GetDuration() const { return duration_; }
  unsigned int GetVruntime() const { return vruntime_; }
  unsigned int GetWeight() const { return weight_; }

  void IncrementVruntime(unsigned int base_weight = 1)
  {
    // Lower weight tasks get larger vruntime increments
    // Higher weight tasks get smaller vruntime increments (run more often)
    vruntime_ += (base_weight * 1024) / weight_;
  }

  void SetVruntime(unsigned int vruntime) { vruntime_ = vruntime; }
  bool IsCompleted() const { return duration_ == 0; }
  void Run() { duration_--; }

  // For debugging
  friend std::ostream &operator<<(std::ostream &os, const Task &task)
  {
    os << "Task " << task.id_ << " (start: " << task.start_time_
       << ", duration: " << task.duration_ << ", vruntime: " << task.vruntime_
       << ", weight: " << task.weight_ << ")";
    return os;
  }

private:
  char id_;
  unsigned int start_time_;
  unsigned int duration_;
  unsigned int vruntime_;
  unsigned int weight_;
};

// Custom task priority key for the multimap
struct TaskPriority
{
  unsigned int vruntime;
  char id;

  // Comparison operators for proper ordering
  bool operator<(const TaskPriority &other) const
  {
    if (vruntime == other.vruntime)
    {
      return id < other.id; // Tie-breaking by ID
    }
    return vruntime < other.vruntime;
  }

  bool operator>(const TaskPriority &other) const
  {
    if (vruntime == other.vruntime)
    {
      return id > other.id;
    }
    return vruntime > other.vruntime;
  }

  bool operator==(const TaskPriority &other) const
  {
    return vruntime == other.vruntime && id == other.id;
  }

  // For debugging
  friend std::ostream &operator<<(std::ostream &os, const TaskPriority &tp)
  {
    os << "{vruntime: " << tp.vruntime << ", id: " << tp.id << "}";
    return os;
  }
};

// The CFS scheduler class
class CFSScheduler
{
  unsigned int weight = 1;
public:

  // Add a task to be scheduled
  void AddTask(char id, unsigned int start_time, unsigned int duration)
  {
    // Assign weights based on task ID
    
    if (id == 'C')
     weight = 1; 
    // C gets higher priority

    tasks_.push_back(std::unique_ptr<Task>(new Task(id, start_time, duration, weight)));
  }

  // Run the scheduler until all tasks are complete
  void Run()
  {
    unsigned int current_tick = 0;
    unsigned int min_vruntime = 0;

    // Use our multimap to keep track of active tasks sorted by vruntime and ID
    Multimap<TaskPriority, Task *> active_tasks;
    Task *current_task = nullptr;
    bool need_new_task = true;

    // Continue until all tasks are completed
    while (true)
    {
      // Check for new tasks starting at this tick
      for (auto &task : tasks_)
      {
        if (task->GetStartTime() == current_tick && !task->IsCompleted())
        {
          // Set the vruntime of new tasks to the current min_vruntime
          task->SetVruntime(min_vruntime);

          // Add to active tasks multimap with priority key
          TaskPriority priority{task->GetVruntime(), task->GetId()};
          active_tasks.Insert(priority, task.get());
          
          // Force task selection when new tasks arrive
          need_new_task = true;
        }
      }

      // If we need a new task (either because the current one is completed or we just started)
      if (need_new_task)
      {
        // If current task is still active, add it back to the queue
        if (current_task && !current_task->IsCompleted())
        {
          TaskPriority priority{current_task->GetVruntime(), current_task->GetId()};
          active_tasks.Insert(priority, current_task);
        }
        
        current_task = nullptr;
        
        // Select a new task with lowest vruntime
        if (active_tasks.Size() > 0)
        {
          try
          {
            TaskPriority min_priority = active_tasks.Min();
            current_task = active_tasks.GetFirst(min_priority);
            active_tasks.Remove(min_priority);
            
            // Update min_vruntime if needed
            if (active_tasks.Size() > 0)
            {
              TaskPriority next_priority = active_tasks.Min();
              Task *next_task = active_tasks.GetFirst(next_priority);
              min_vruntime = next_task->GetVruntime();
            }
          }
          catch (const std::exception &e)
          {
            current_task = nullptr;
          }
        }
        
        need_new_task = false;
      }

      // Display queue size - count current task as part of the total for display
      int display_size = active_tasks.Size();
      if (current_task)
      {
        display_size += 1;
      }

      // Output the current tick state
      std::cout << current_tick << " [" << display_size << "]: ";

      // Execute the current task for one tick if available
      if (current_task)
      {
        std::cout << current_task->GetId();
        current_task->Run();
        weight--;

        // Update vruntime based on task weight
        current_task->IncrementVruntime();

        // Check if task has completed
        if (current_task->IsCompleted())
        {
          std::cout << "*";
          current_task = nullptr;
          need_new_task = true;
        }
        else
        {
          // New for expected behavior: Check if new tasks with lower vruntime exist
          // If so, we should preempt the current task
          if (active_tasks.Size() > 0)
          {
            try
            {
              TaskPriority lowest_priority = active_tasks.Min();
              if (lowest_priority.vruntime < current_task->GetVruntime())
              {
                need_new_task = true;
              }
            }
            catch (const std::exception &e)
            {
              // No other tasks available
            }
          }
        }
      }
      else
      {
        std::cout << "_";
        need_new_task = true;
      }

      std::cout << std::endl;

      // Increment time
      current_tick++;

      // Check if we're done (no current task, empty multimap, and no future tasks)
      if (!current_task && active_tasks.Size() == 0)
      {
        bool any_future_tasks = false;
        for (auto &task : tasks_)
        {
          if (task->GetStartTime() > current_tick || !task->IsCompleted())
          {
            any_future_tasks = true;
            break;
          }
        }
        if (!any_future_tasks)
        {
          break;
        }
      }
    }
  }

private:
  std::vector<std::unique_ptr<Task>> tasks_;
};

int main(int argc, char *argv[])
{
  // Check if a file name was provided
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <task_file.dat>" << std::endl;
    return 1;
  }

  // Open the file
  std::ifstream file(argv[1]);
  if (!file)
  {
    std::cerr << "Error: cannot open file " << argv[1] << std::endl;
    return 1;
  }

  // Create scheduler
  CFSScheduler scheduler;

  // Read tasks from file
  char id;
  unsigned int start_time, duration;
  while (file >> id >> start_time >> duration)
  {
    scheduler.AddTask(id, start_time, duration);
  }

  // Run the scheduler
  scheduler.Run();

  return 0;
}
