#pragma once
#include "Editor/EditorWindow.h"
#include "GUILogger.h"

namespace Helios
{
    struct FileData
    {
        std::string m_fileName{ "NONE" };
        std::string m_fileExtension{ "" };
        std::filesystem::file_time_type m_timeLastEdited{ std::filesystem::file_time_type() };
        std::string m_filePath{ "" };
        bool m_isFolder{ false };
    };

    inline bool operator!=(const FileData& a_c1, const FileData& a_c2)
    {
        return(a_c1.m_fileName != a_c2.m_fileName || a_c1.m_timeLastEdited != a_c2.m_timeLastEdited);
    };

    class ContentBrowser : public EditorWindow
    {
    public:
        ContentBrowser(const std::string& a_windowName, float a_xPos, float a_yPos, float a_xSize, float a_ySize);
        void ShowContext() override;
        const std::filesystem::path& GetCurrentFolder() const { return m_currentFolder; }
        bool DidFolderChange(const std::filesystem::path& a_folderPath);
        std::vector<FileData> GetAllFilesOfExtensions(std::vector<std::string> a_extensions);

        std::vector<std::pair<GUILogger::ELevelType, std::string>>& GetLog() { return m_log; }
    private:
        std::unordered_map < std::string, std::vector<FileData>> m_fileDatas;

        std::filesystem::path m_assetFolder;
        std::filesystem::path m_currentFolder;
        std::filesystem::path m_rootFolder;

        std::vector<std::pair<GUILogger::ELevelType, std::string>> m_log;
        std::unordered_map<GUILogger::ELevelType, ImVec4> m_loggingColors
        {
            std::make_pair(GUILogger::ELevelType::LevelType_Trace, ImVec4(1.f,1.f,1.f,1.f)),
            std::make_pair(GUILogger::ELevelType::LevelType_Debug, ImVec4(0.26f,0.68f,1.f,1.f)),
            std::make_pair(GUILogger::ELevelType::LevelType_Info, ImVec4(0.3f,1.f,0.3f,1)),
            std::make_pair(GUILogger::ELevelType::LevelType_Warn, ImVec4(1.f, 0.6f, 0.f,1.f)),
            std::make_pair(GUILogger::ELevelType::LevelType_Error, ImVec4(1.f, 0.5f, 0.42f, 1.f)),
            std::make_pair(GUILogger::ELevelType::LevelType_Critical, ImVec4(1.f,0.f,0.f,1.f)),
            std::make_pair(GUILogger::ELevelType::LevelType_Off, ImVec4(0.f,0.f,1.f,1.f))
        };

        std::array<std::string, 7> m_allowedExtensions =
        {
            ".png",
            ".jpg",
            ".scene",
            ".wav",
            ".mp3",
            ".prefab",
        	".gltf"
        };

        void SortContentBrowser();
    };
}
