#include "Events.hpp"

#include "EventDispatcherImpl.hpp"

std::unordered_set<EventDispatcherBase*>* zknt::EventDispatcherRegistry::g_Dispatchers = nullptr;

zknt::Events::Events() {
    DEFINE_EVENT(OnConsoleCommand, TArray<ZString>&);
}

zknt::Events::~Events() {
    delete OnConsoleCommand;
}
