/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <filesystem>
#include <optional>

namespace Gris
{

class DirectoryRegistry
{
public:
    static void AddResolvePath(std::filesystem::path path);

    [[nodiscard]] static const std::filesystem::path & ExecutableLocation();
    [[nodiscard]] static std::filesystem::path WorkingDirectory();
    [[nodiscard]] static std::optional<std::filesystem::path> TryResolvePath(const std::filesystem::path & path);

private:
    [[nodiscard]] static DirectoryRegistry & Instance();

    DirectoryRegistry();

    [[nodiscard]] std::optional<std::filesystem::path> TryResolvePathFromExecutableLocation(const std::filesystem::path & path) const;
    [[nodiscard]] std::optional<std::filesystem::path> TryResolvePathFromWorkingDirectory(const std::filesystem::path & path) const;
    [[nodiscard]] std::optional<std::filesystem::path> TryResolvePathInAdditionalDirectories(const std::filesystem::path & path) const;

    std::filesystem::path m_executableLocation = {};
    std::vector<std::filesystem::path> m_additionalPaths = {};
};

}  // namespace Gris
