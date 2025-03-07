#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

// A class representing a task in the system
class Task
{
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



private:
  char id_;
  unsigned int start_time_;
  unsigned int duration_;
  unsigned int vruntime_;
};

// The CFS scheduler class
class CFSScheduler
{
public:
  // Add a task to be scheduled
  void AddTask(char id, unsigned int start_time, unsigned int duration)
  {
    tasks_.push_back(std::unique_ptr<Task>(new Task(id, start_time, duration)));
  }

  // Run the scheduler until all tasks are complete
  void Run()
  {
    unsigned int current_tick = 0;
    unsigned int min_vruntime = 0;

    // Maintain a list of active tasks (not yet completed)
    std::vector<Task *> active_tasks;
    Task *current_task = nullptr;

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
          active_tasks.push_back(task.get());
        }
      }

      // Sort active tasks by vruntime and then by ID for tie-breaking
      std::sort(active_tasks.begin(), active_tasks.end(),
                [](Task *a, Task *b)
                {
                  if (a->GetVruntime() == b->GetVruntime())
                  {
                    return a->GetStartTime() > b->GetStartTime();
                  }
                  return a->GetVruntime() > b->GetVruntime();
                });

      // Select the task with lowest vruntime
      // After running a task, if it's not completed, it should be added back.
      if (current_task && !current_task->IsCompleted())
      {
        active_tasks.push_back(current_task);
      }
      current_task = nullptr; // Reset current task after execution

      if (!active_tasks.empty())
      {
        current_task = active_tasks.front();
        active_tasks.erase(active_tasks.begin());
      }

      // Display queue size - count current task as part of the total for display
      int display_size = active_tasks.size();
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
        current_task->IncrementVruntime();

        // Update min_vruntime if this is the smallest in all active tasks
        if (current_task->IsCompleted())
        {
          if (!active_tasks.empty())
          {
            min_vruntime = active_tasks.front()->GetVruntime();
          }
          else
          {
            min_vruntime = 0;
          }
        }

        // Mark completed tasks
        if (current_task->IsCompleted())
        {
          std::cout << "*";
          current_task = nullptr;
        }
      }
      else
      {
        std::cout << "_";
      }
      std::cout << std::endl;

      // Increment time
      current_tick++;

      // Check if we're done (no current task, empty queue, and no future tasks)
      if (!current_task && active_tasks.empty())
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