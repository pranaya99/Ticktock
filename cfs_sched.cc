void Run()
{
  unsigned int current_tick = 0;
  unsigned int min_vruntime = 0;

  std::vector<Task *> active_tasks;
  Task *current_task = nullptr;

  while (true)
  {
    // Check for new tasks starting at this tick
    for (auto &task : tasks_)
    {
      if (task->GetStartTime() == current_tick && !task->IsCompleted())
      {
        task->SetVruntime(min_vruntime);
        active_tasks.push_back(task.get());
      }
    }

    // If current task is still active, consider it for rescheduling
    if (current_task && !current_task->IsCompleted())
    {
      active_tasks.push_back(current_task);
      current_task = nullptr;
    }

    // Sort active tasks by vruntime and then by ID for tie-breaking
    std::sort(active_tasks.begin(), active_tasks.end(),
              [](Task *a, Task *b)
              {
                if (a->GetVruntime() == b->GetVruntime())
                {
                  return a->GetId() < b->GetId();
                }
                return a->GetVruntime() < b->GetVruntime();
              });

    // Select the task with lowest vruntime
    if (!active_tasks.empty())
    {
      current_task = active_tasks.front();
      active_tasks.erase(active_tasks.begin());

      // Update min_vruntime to the current task's vruntime
      min_vruntime = current_task->GetVruntime();
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
      current_task->Run(); // Decrement duration

      // Mark completed tasks AFTER running
      if (current_task->IsCompleted())
      {
        std::cout << "*";
        // Don't increment vruntime for completed tasks
      }
      else
      {
        current_task->IncrementVruntime();
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
        if (!task->IsCompleted() && task->GetStartTime() >= current_tick)
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