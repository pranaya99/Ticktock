#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

class Task {
public:
    char id;
    unsigned int start_time;
    unsigned int duration;
    unsigned int vruntime;

    Task(char i, unsigned int s, unsigned int d)
        : id(i), start_time(s), duration(d), vruntime(0) {}
};

struct CompareStart {
    bool operator()(const Task* a, const Task* b) {
        if (a->start_time == b->start_time) {
            return a->id > b->id;
        }
        return a->start_time > b->start_time;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <task_file.dat>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: cannot open file " << argv[1] << '\n';
        return 1;
    }

    std::priority_queue<Task*, std::vector<Task*>, CompareStart> pending_tasks;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char id;
        unsigned int start, duration;
        if (!(iss >> id >> start >> duration)) {
            std::cerr << "Error: invalid task line\n";
            continue;
        }
        pending_tasks.push(new Task(id, start, duration));
    }

    std::multimap<std::pair<unsigned int, char>, Task*> timeline;
    Task* current_task = nullptr;
    unsigned int current_tick = 0;
    unsigned int min_vruntime = 0;

    while (true) {
        // Add new tasks starting at current_tick
        while (!pending_tasks.empty()) {
            Task* task = pending_tasks.top();
            if (task->start_time == current_tick) {
                pending_tasks.pop();
                task->vruntime = min_vruntime;
                timeline.insert({{task->vruntime, task->id}, task});
            } else {
                break;
            }
        }

        // Check if current task should yield
        if (current_task && current_task->vruntime > min_vruntime) {
            timeline.insert({{current_task->vruntime, current_task->id}, current_task});
            current_task = nullptr;
        }

        // Select new current task
        if (!current_task && !timeline.empty()) {
            auto it = timeline.begin();
            current_task = it->second;
            min_vruntime = current_task->vruntime;
            timeline.erase(it);
        }

        // Run current task and output
        bool done = false;
        if (current_task) {
            current_task->vruntime += 1;
            done = (current_task->vruntime >= current_task->duration);
        }

        int active_count = timeline.size() + (current_task ? 1 : 0);
        std::cout << current_tick << " [" << active_count << "]: ";
        if (current_task) {
            std::cout << current_task->id;
            if (done) {
                std::cout << '*';
                delete current_task;
                current_task = nullptr;
            }
        } else {
            std::cout << '_';
        }
        std::cout << '\n';

        // Increment tick and check exit condition
        current_tick++;
        if (pending_tasks.empty() && timeline.empty() && !current_task) {
            break;
        }
    }

    // Cleanup
    while (!pending_tasks.empty()) {
        delete pending_tasks.top();
        pending_tasks.pop();
    }
    for (auto& entry : timeline) {
        delete entry.second;
    }
    if (current_task) {
        delete current_task;
    }

    return 0;
}