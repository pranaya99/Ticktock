#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>
#include <string>

// Structure representing a task with its attributes.
struct Task {
  char id;
  unsigned start_time;
  unsigned duration;
  unsigned executed;
  unsigned vruntime;
  unsigned last_run;  // Tick when the task last ran.
  Task(char i, unsigned st, unsigned d)
      : id(i),
        start_time(st),
        duration(d),
        executed(0),
        vruntime(0),
        last_run(0) {}
  bool finished() const { return executed >= duration; }
};

// Custom comparator that orders tasks as follows:
// 1. By virtual runtime (lower comes first)
// 2. If equal, by last_run (the one that ran earlier gets priority)
// 3. Finally, by task identifier (lexicographical order)
struct TaskComparator {
  bool operator()(const Task *a, const Task *b) const {
    if (a->vruntime != b->vruntime) return a->vruntime < b->vruntime;
    if (a->last_run != b->last_run) return a->last_run < b->last_run;
    return a->id < b->id;
  }
};

int main(int argc, char *argv[]) {
  // Check for correct command-line usage.
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <task_file.dat>" << std::endl;
    return 1;
  }

  std::ifstream infile(argv[1]);
  if (!infile) {
    std::cerr << "Error: cannot open file " << argv[1] << std::endl;
    return 1;
  }

  // Read tasks from the input file.
  std::vector<Task *> tasks;
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty()) continue;
    std::istringstream iss(line);
    char id;
    unsigned st, dur;
    if (!(iss >> id >> st >> dur)) continue;
    tasks.push_back(new Task(id, st, dur));
  }

  // Sort tasks by start time.
  std::sort(tasks.begin(), tasks.end(), [](const Task *a, const Task *b) {
    return a->start_time < b->start_time;
  });

  unsigned tick = 0;
  unsigned global_min_vruntime =
      0;  // Initialize global minimum virtual runtime.
  std::set<Task *, TaskComparator> ready;
  Task *current = nullptr;
  size_t next_task_index = 0;

  // Main scheduling loop: runs until all tasks have been processed.
  while (next_task_index < tasks.size() || !ready.empty() ||
         current != nullptr) {
    // Add tasks that arrive at the current tick.
    while (next_task_index < tasks.size() &&
           tasks[next_task_index]->start_time == tick) {
      Task *t = tasks[next_task_index];
      // Initialize the new task's virtual runtime to the global minimum.
      t->vruntime = global_min_vruntime;
      ready.insert(t);
      next_task_index++;
    }

    // If there is a running task and a ready task with a lower virtual runtime
    // exists, preempt the current task.
    if (current && !ready.empty()) {
      Task *top = *ready.begin();
      if (top->vruntime < current->vruntime) {
        ready.insert(current);
        current = nullptr;
      }
    }

    // If no task is currently running, select the next task.
    if (!current) {
      if (!ready.empty()) {
        current = *ready.begin();
        ready.erase(ready.begin());
        // Update global minimum to the current task's virtual runtime.
        global_min_vruntime = current->vruntime;
      }
    }

    // Total runnable tasks: ready tasks plus the current task if one is
    // running.
    size_t total_tasks = ready.size() + (current ? 1 : 0);
    char print_id = current ? current->id : '_';

    // Print the scheduling status.
    std::cout << tick << " [" << total_tasks << "]: " << print_id;

    bool finished = false;
    if (current) {
      // Run the current task for one tick.
      current->executed++;
      current->vruntime++;
      current->last_run = tick;  // Update the last run tick.
      if (current->finished()) finished = true;
    }

    // If the task finishes during this tick, mark it and deallocate.
    if (finished) {
      std::cout << "*";
      delete current;
      current = nullptr;
    }
    std::cout << std::endl;

    tick++;  // Increment tick for the next iteration.
  }

  return 0;
}
