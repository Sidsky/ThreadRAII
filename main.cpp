#include <iostream>
#include <vector>
#include <thread>

class ThreadRAII {
public:
    enum class DtorAction {join, detach};

    ThreadRAII (std::thread&& t, DtorAction a) :
                action(a), t(std::move(t)){}

    ~ThreadRAII() {
        if (t.joinable()) {
            if (action == DtorAction::join){
                t.join();
            } else {
                t.detach();
            }
        }
    }

    std::thread& get() { return t; }

    ThreadRAII (ThreadRAII&&) = default;
    ThreadRAII& operator=(ThreadRAII&&) = default;

private:
    DtorAction action;
    std::thread t;
};

constexpr auto tenMillion = 10'00'000;

bool conditionsAreSatisfied() {
    return true;
}

bool doWork(std::function<bool(int)> filter,
            int maxVal = tenMillion)
{
    std::vector<int> goodVals;

    ThreadRAII t (std::thread([&filter, maxVal, &goodVals](){
        for (int i=0; i<maxVal; i++) {
            if (filter(i)) goodVals.emplace_back(i);
        }
    }), ThreadRAII::DtorAction::join);

    auto nh = t.get().native_handle();

    if (conditionsAreSatisfied()) {
        t.get().join();
        return true;
    } else {
        return false;
    }
}

int main() {
    return 0;
}
