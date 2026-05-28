#pragma once

#include <type_traits>

namespace zknt {
    template<class T> class EngineFunction;

    template<class ReturnType, class... Args> class EngineFunction<ReturnType(Args...)> {
      protected:
        explicit EngineFunction(void* p_Address) : m_Address(p_Address) {}

      public:
        ReturnType Call(Args... p_Args) {
            if (m_Address == nullptr) {
                if constexpr (std::is_pointer_v<ReturnType>) {
                    return nullptr;
                }
                else {
                    return ReturnType();
                }
            }
            return reinterpret_cast<ReturnType (*)(Args...)>(m_Address)(p_Args...);
        }

        [[nodiscard]] bool Exists() const {
            return m_Address != nullptr;
        }

      protected:
        void* m_Address = nullptr;
    };

    template<class... Args> class EngineFunction<void(Args...)> {
      protected:
        explicit EngineFunction(void* p_Address) : m_Address(p_Address) {}

      public:
        void Call(Args... p_Args) {
            if (m_Address == nullptr) {
                return;
            }
            reinterpret_cast<void (*)(Args...)>(m_Address)(p_Args...);
        }

        [[nodiscard]] bool Exists() const {
            return m_Address != nullptr;
        }

      protected:
        void* m_Address = nullptr;
    };
}
