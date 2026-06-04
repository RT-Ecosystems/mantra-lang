#include "async_system.h"
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>

namespace mantra {
namespace core {

struct AsyncTask {
    std::string id;
    std::function<MantraValue()> work;
    std::promise<MantraValue> promise;
    bool is_completed = false;
    MantraValue result;
    std::string error;
};

class EventLoop {
public:
    EventLoop() : running_(false) {}

    void start() {
        running_ = true;
        worker_thread_ = std::thread([this]() {
            while (running_) {
                std::unique_ptr<AsyncTask> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_.wait(lock, [this]() { return !task_queue_.empty() || !running_; });
                    if (!running_) break;
                    if (!task_queue_.empty()) {
                        task = std::move(task_queue_.front());
                        task_queue_.pop();
                    }
                }
                if (task) {
                    try {
                        MantraValue result = task->work();
                        task->promise.set_value(result);
                        task->result = result;
                    } catch (const std::exception& e) {
                        task->promise.set_exception(std::make_exception_ptr(e));
                        task->error = e.what();
                    }
                    task->is_completed = true;
                }
            }
        });
    }

    void stop() {
        running_ = false;
        condition_.notify_all();
        if (worker_thread_.joinable()) worker_thread_.join();
    }

    std::string submit(std::function<MantraValue()> work) {
        auto task = std::make_unique<AsyncTask>();
        task->id = generateTaskId();
        task->work = work;
        std::future<MantraValue> future = task->promise.get_future();
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            task_queue_.push(std::move(task));
        }
        condition_.notify_one();
        futures_[task->id] = std::move(future);
        return task->id;
    }

    MantraValue await(const std::string& task_id) {
        auto it = futures_.find(task_id);
        if (it == futures_.end()) throw std::runtime_error("Task not found: " + task_id);
        try {
            MantraValue result = it->second.get();
            futures_.erase(it);
            return result;
        } catch (const std::exception& e) {
            futures_.erase(it);
            throw std::runtime_error(std::string("Await error: ") + e.what());
        }
    }

    bool isComplete(const std::string& task_id) {
        auto it = futures_.find(task_id);
        if (it == futures_.end()) return true;
        return it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    void sleep(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

private:
    std::queue<std::unique_ptr<AsyncTask>> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::thread worker_thread_;
    bool running_;
    std::unordered_map<std::string, std::future<MantraValue>> futures_;

    static std::string generateTaskId() {
        static int counter = 0;
        return "task_" + std::to_string(++counter) + "_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    }
};

static std::unique_ptr<EventLoop> g_event_loop;

EventLoop* getEventLoop() {
    if (!g_event_loop) {
        g_event_loop = std::make_unique<EventLoop>();
        g_event_loop->start();
    }
    return g_event_loop.get();
}

MantraValue builtinAsync(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::FUNCTION) throw std::runtime_error("async: function required");
    MantraValue func = args[0];
    std::string task_id = getEventLoop()->submit([func]() -> MantraValue {
        return MantraValue::string("Async task started");
    });
    return MantraValue::string(task_id);
}

MantraValue builtinAwait(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("await: task id required");
    return getEventLoop()->await(args[0].string_value);
}

MantraValue builtinSleep(const std::vector<MantraValue>& args) {
    int ms = 1000;
    if (!args.empty() && args[0].type == MantraValue::Type::NUMBER) ms = static_cast<int>(args[0].number_value);
    getEventLoop()->sleep(ms);
    return MantraValue::nil();
}

MantraValue builtinIsComplete(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) return MantraValue::boolean(false);
    return MantraValue::boolean(getEventLoop()->isComplete(args[0].string_value));
}

} // namespace core
} // namespace mantra