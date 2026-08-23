#include "Events.hpp"

#include "EventDispatcherImpl.hpp"

std::unordered_set<EventDispatcherBase*>* zknt::EventDispatcherRegistry::g_Dispatchers = nullptr;

zknt::Events::Events() {}

zknt::Events::~Events() {}
