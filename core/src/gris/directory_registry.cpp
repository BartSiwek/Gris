#include <gris/directory_registry.h>

#include <gris/engine_exception.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <cerrno>
#include <cstring>
#include <unistd.h>
#endif

namespace
{

#ifdef _MSC_VER

std::string ErrorToString(DWORD error)
{
    LPTSTR lpMsgBuf = nullptr;
    auto const bufLen = FormatMessage(
             FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
             nullptr,
             error,
             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
             reinterpret_cast<LPTSTR>(&lpMsgBuf),
             0,
             nullptr);
    if (bufLen > 0)
    {
        auto const lpMsgStr = static_cast<LPCSTR>(lpMsgBuf);
        std::string result(lpMsgStr, lpMsgStr + bufLen);

        LocalFree(lpMsgBuf);

        return result;
    }

    return "Unknown error";
}

#endif

}  // namespace

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const std::filesystem::path & Gris::DirectoryRegistry::ExecutableLocation()
{
    return Instance().m_executableLocation;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::filesystem::path Gris::DirectoryRegistry::WorkingDirectory()
{
    return std::filesystem::current_path();
}

// -------------------------------------------------------------------------------------------------

void Gris::DirectoryRegistry::AddResolvePath(std::filesystem::path path)
{
    if (!path.is_absolute())
    {
        throw EngineException("Argument to DirectoryRegistry::AddPath must be an absolute path");
    }

    Instance().m_additionalPaths.emplace_back(std::move(path));
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::filesystem::path> Gris::DirectoryRegistry::TryResolvePath(const std::filesystem::path & path)
{
    if (path.is_absolute())
    {
        return path;
    }

    auto pathFromExecutableLocation = Instance().TryResolvePathFromExecutableLocation(path);
    if (pathFromExecutableLocation)
    {
        return pathFromExecutableLocation;
    }

    auto pathFromWorkingDirectory = TryResolvePathFromWorkingDirectory(path);
    if (pathFromWorkingDirectory)
    {
        return pathFromWorkingDirectory;
    }

    return Instance().TryResolvePathInAdditionalDirectories(path);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::DirectoryRegistry & Gris::DirectoryRegistry::Instance()
{
    static DirectoryRegistry s_instance = {};
    return s_instance;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::filesystem::path> Gris::DirectoryRegistry::TryResolvePathFromWorkingDirectory(const std::filesystem::path & path)
{
    auto pathFromWorkingDirectory = std::filesystem::current_path() / path;
    if (exists(pathFromWorkingDirectory))
    {
        return pathFromWorkingDirectory;
    }

    return {};
}

// -------------------------------------------------------------------------------------------------

Gris::DirectoryRegistry::DirectoryRegistry()
{
#ifdef _MSC_VER
    auto buffer = std::vector<char>(MAX_PATH, '\0');

    auto resolved = false;
    while (!resolved)
    {
        GetModuleFileName(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        auto const lastError = GetLastError();
        if (lastError == ERROR_INSUFFICIENT_BUFFER)
            buffer.resize(2 * buffer.size(), '\0');
        else if (lastError != ERROR_SUCCESS)
            throw EngineException("Error resolving executable location", ErrorToString(lastError));
        else
            resolved = true;
    }

    m_executableLocation = std::filesystem::path(std::string(buffer.data()));
    m_executableLocation.remove_filename();
#else
    constexpr size_t BUFFER_STARTING_SIZE = 260;

    auto currentBufferSize = BUFFER_STARTING_SIZE;
    auto buffer = std::vector<char>(currentBufferSize + 1, '\0');

    auto resolved = false;
    while (!resolved)
    {
        auto readlinkResult = readlink("/proc/self/exe", buffer.data(), buffer.size());
        if (readlinkResult == -1)
        {
            throw EngineException("Error resolving executable location", strerror(errno));
        }
        if (static_cast<size_t>(readlinkResult) == currentBufferSize)
        {
            currentBufferSize *= 2;
            buffer.resize(currentBufferSize + 1, '\0');
        }
        else
        {
            buffer[static_cast<size_t>(readlinkResult)] = '\0';
            resolved = true;
        }
    }

    m_executableLocation = std::filesystem::path(std::string(buffer.data()));
    m_executableLocation.remove_filename();
#endif
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::filesystem::path> Gris::DirectoryRegistry::TryResolvePathFromExecutableLocation(const std::filesystem::path & path) const
{
    auto pathFromExecutableLocation = m_executableLocation / path;
    if (exists(pathFromExecutableLocation))
    {
        return pathFromExecutableLocation;
    }

    return {};
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::filesystem::path> Gris::DirectoryRegistry::TryResolvePathInAdditionalDirectories(const std::filesystem::path & path) const
{
    for (const auto & additionalPath : m_additionalPaths)
    {
        auto currentPath = additionalPath / path;
        if (exists(currentPath))
        {
            return currentPath;
        }
    }

    return {};
}
