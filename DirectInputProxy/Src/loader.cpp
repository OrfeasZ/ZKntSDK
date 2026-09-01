#define CR_HOST CR_DISABLE
#include <cr.h>

#include "loader.hpp"
#include "HostServices.hpp"
#include "Rendering/RenderingHost.hpp"

#include <atomic>
#include <cstdio>
#include <filesystem>
#include <string_view>
#include <thread>
#include <Windows.h>
#include <io.h>

namespace fs = std::filesystem;

namespace zknt::loader {
    namespace {
        constexpr auto c_GuestDll = L"ZKntSdk.dll";

        std::atomic<HANDLE> g_GameDir{nullptr};
        std::atomic g_Stopping{false};

#if _DEBUG
        int g_OriginalStdin;
        int g_OriginalStdout;
        int g_OriginalStderr;
#endif

        void log(const std::string& s) {
            const std::string line = s + '\n';
            OutputDebugStringA(line.c_str());
            printf("%s", line.c_str());
        }

        bool WaitReleased(const fs::path& dll) {
            for (int i = 0; i < 100; ++i) {
                const auto s_Handle = CreateFileW(dll.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

                if (s_Handle != INVALID_HANDLE_VALUE) {
                    CloseHandle(s_Handle);
                    return true;
                }

                Sleep(100);
            }

            return false;
        }

        void ReloadSdk(const fs::path& p_SdkDllPath, cr_plugin& p_CrCtx) {
            log("[dinput8] Reloading SDK DLL...");

            if (!WaitReleased(p_SdkDllPath)) {
                log("[dinput8] Timed out waiting for DLL to be released...");
                return;
            }

            const auto s_Result = cr_plugin_update(p_CrCtx, true);
            if (s_Result < 0 || p_CrCtx.failure != CR_NONE) {
                log(std::format("[dinput8] reload failed: rc={} failure={} (kept previous)", s_Result, static_cast<int>(p_CrCtx.failure)));
            }
        }

        bool IsSdkDll(const FILE_NOTIFY_INFORMATION* p_Info) {
            for (const auto* i = p_Info;;
                 i = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(reinterpret_cast<const BYTE*>(i) + i->NextEntryOffset)) {
                if (std::wstring_view(i->FileName, i->FileNameLength / sizeof(WCHAR)) == c_GuestDll) {
                    return true;
                }

                if (!i->NextEntryOffset) {
                    return false;
                }
            }
        }

        void WatchForNewSdk(const fs::path& p_GameDir, const fs::path& p_SdkDllPath, cr_plugin& p_CrCtx) {
            alignas(DWORD) BYTE s_NotifyBuffer[4096];
            auto* s_NotifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(s_NotifyBuffer);
            while (!g_Stopping) {
                auto s_GameDir = CreateFileW(
                    p_GameDir.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS, nullptr
                );

                if (s_GameDir == INVALID_HANDLE_VALUE) {
                    log(std::format("[dinput8] CreateFileW(watch dir) failed: {}", GetLastError()));
                    Sleep(250);
                    continue;
                }

                g_GameDir = s_GameDir;

                DWORD n;
                while (!g_Stopping
                       && ReadDirectoryChangesW(
                           s_GameDir, s_NotifyInfo, sizeof(s_NotifyBuffer), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, &n,
                           nullptr, nullptr
                       )) {
                    if (IsSdkDll(s_NotifyInfo)) {
                        ReloadSdk(p_SdkDllPath, p_CrCtx);
                    }
                }

                // If we're not yet stopping, and we got a notification error, just reload.
                if (!g_Stopping) {
                    if (const auto s_Err = GetLastError(); s_Err == ERROR_NOTIFY_ENUM_DIR) {
                        ReloadSdk(p_SdkDllPath, p_CrCtx);
                    }
                }

                g_GameDir = nullptr;
                CloseHandle(s_GameDir);

                // Not stopping, so wait for a bit before we try registering a watcher again.
                if (!g_Stopping) {
                    Sleep(250);
                }
            }
        }

        void Run(fs::path p_GameDir, fs::path p_SdkDllPath, fs::path p_CrTempDir, cr_plugin p_CrCtx) {
            WatchForNewSdk(p_GameDir, p_SdkDllPath, p_CrCtx);
            cr_plugin_close(p_CrCtx);

            if (!p_CrTempDir.empty()) {
                std::error_code s_Ec;
                fs::remove_all(p_CrTempDir, s_Ec);
            }
        }
    }

    void Start(HMODULE p_Self) {
        // Allocate console and start redirection in debug builds.
#if _DEBUG
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        SetConsoleTitleA("Knt SDK - Debug Console");

        g_OriginalStdin = _dup(0);
        g_OriginalStdout = _dup(1);
        g_OriginalStderr = _dup(2);

        FILE* s_Con;
        freopen_s(&s_Con, "CONIN$", "r", stdin);
        freopen_s(&s_Con, "CONOUT$", "w", stderr);
        freopen_s(&s_Con, "CONOUT$", "w", stdout);

        SetConsoleOutputCP(CP_UTF8);
#endif

        wchar_t s_GameExe[65535];
        GetModuleFileNameW(p_Self, s_GameExe, _countof(s_GameExe));
        const auto s_GameDir = fs::path(s_GameExe).parent_path();
        const auto s_SdkDllPath = s_GameDir / c_GuestDll;

        cr_plugin s_CrCtx{};
        s_CrCtx.userdata = knt::rendering::RenderingHost::Instance().Services();
        if (!cr_plugin_open(s_CrCtx, s_SdkDllPath.string().c_str())) {
            log("[dinput8] cr_plugin_open failed -- is ZKntSdk.dll present?");
            return;
        }

        // Route cr's working copies through a process-private temp dir so
        // they (a) don't pollute the game install and (b) don't sit in a
        // directory that the SDK / mods watch. Without this, cr drops
        // `ZKntSdk_NN.dll` next to the original which can be picked up by
        // dependents (mods importing `ZKntSdk.dll`) as a second module
        // instance.
        fs::path s_CrTempDir;
        wchar_t s_TempBuf[MAX_PATH];
        const auto s_TempLen = GetTempPathW(_countof(s_TempBuf), s_TempBuf);
        if (s_TempLen > 0 && s_TempLen < _countof(s_TempBuf)) {
            s_CrTempDir = fs::path(s_TempBuf) / std::format(L"ZKntSdk-{}", GetCurrentProcessId());
            std::error_code s_Ec;
            fs::create_directories(s_CrTempDir, s_Ec);
            if (!s_Ec) {
                cr_set_temporary_path(s_CrCtx, s_CrTempDir.string());
            }
            else {
                s_CrTempDir.clear();
            }
        }

        // Load the SDK.
        const auto s_LoadResult = cr_plugin_update(s_CrCtx, true);
        if (s_LoadResult < 0 || s_CrCtx.failure != CR_NONE) {
            log(std::format("[dinput8] initial SDK load failed: rc={} failure={}", s_LoadResult, static_cast<int>(s_CrCtx.failure)));
        }

        std::thread(Run, s_GameDir, s_SdkDllPath, s_CrTempDir, s_CrCtx).detach();
    }

    void Stop() {
        g_Stopping = true;

        if (const auto s_Handle = g_GameDir.exchange(nullptr); s_Handle && s_Handle != INVALID_HANDLE_VALUE) {
            CancelIoEx(s_Handle, nullptr);
        }

        // De-allocate console.
#if _DEBUG
        _dup2(g_OriginalStdin, 0);
        _dup2(g_OriginalStdout, 1);
        _dup2(g_OriginalStderr, 2);

        FreeConsole();
#endif
    }
}
