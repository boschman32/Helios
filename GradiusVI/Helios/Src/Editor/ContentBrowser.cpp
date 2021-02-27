#include "hepch.h"
#include "Editor/ContentBrowser.h"
#include "Editor/pfd.h"
#include "Core/Core.h"
#include "Core/TFD.h"
#include "Utils/FileUtils.h"
#include "Core/ResourceManager.h"

namespace Helios
{
    ContentBrowser::ContentBrowser(const std::string& a_windowName, float a_xPos, float a_yPos, float xSize, float a_ySize)
        : EditorWindow(a_windowName, a_xPos, a_yPos, xSize, a_ySize)
    {
        m_rootFolder = ".\\";
        m_assetFolder = m_rootFolder.string() + "Assets";

        //Check if the asset folder exists if so we create a one.
        if (!exists(m_assetFolder))
        {
            create_directory(m_assetFolder);
        }

        //Find all the files inside the asset folder and store it.
        for (const auto& entry : std::filesystem::recursive_directory_iterator(m_assetFolder))
        {
            FileData data;
            data.m_fileName = entry.path().filename().string();
            data.m_fileExtension = entry.path().extension().string();
            data.m_timeLastEdited = entry.last_write_time();
            data.m_filePath = entry.path().string();

            if (entry.is_directory())
            {
                data.m_isFolder = true;
                m_fileDatas[entry.path().parent_path().string()].push_back(data);
                m_fileDatas[data.m_filePath] = std::vector<FileData>();
            }
            else
            {
                data.m_isFolder = false;
                const auto found = std::find(m_allowedExtensions.begin(), m_allowedExtensions.end(), data.m_fileExtension);
                if (found != m_allowedExtensions.end())
                {
                    m_fileDatas[entry.path().parent_path().string()].push_back(data);
                }
            }
        }
        SortContentBrowser();

        m_currentFolder = m_assetFolder;
    }

    void ContentBrowser::ShowContext()
    {
        ImGui::BeginChild("Container");
        {
            ImGui::Columns(2);
            ImGui::BeginChild("FileBrowser");
            {
                ImGui::BeginChild("FileBrowserTop", ImVec2(ImGui::GetWindowContentRegionWidth(), 20));
                {
                    ImGui::Text("Current Folder:");
                    ImGui::SameLine();
                    if (ImGui::Button(m_currentFolder.string().c_str()))
                    {
                      
                    }
                }
                ImGui::EndChild();

                ImGui::Separator();
                ImGui::BeginChild("FileBrowserBottom");
                {
                    if (!equivalent(m_currentFolder, m_assetFolder) && m_currentFolder.has_parent_path())
                    {
                        if (ImGui::Button("Go Folder Up"))
                        {
                            m_currentFolder = m_currentFolder.parent_path().string();
                            DidFolderChange(m_currentFolder.string());
                        }
                    }

                    //Show the content for this folder.
                    for (auto& f : m_fileDatas.at(m_currentFolder.string()))
                    {
                        if (f.m_isFolder)
                        {
                            if (!equivalent(f.m_filePath, m_currentFolder))
                            {
                                const std::string folderName = "[Folder]" + f.m_fileName;
                                if (ImGui::Button(folderName.c_str()))
                                {
                                    m_currentFolder = f.m_filePath;
                                    DidFolderChange(m_currentFolder.string());
                                    break;
                                }
                            }
                        }
                        else
                        {
                            ImGui::Selectable(f.m_fileName.c_str());
                        }
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();
            ImGui::NextColumn();

            ImGui::BeginChild("LoggerTop", ImVec2(ImGui::GetContentRegionAvailWidth(), 25));
            {
                static const ImVec2 ButtonSize = ImVec2(50.f, 20.f);

                ImGui::Text("Logger:");
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ButtonSize.x);
                if (ImGui::Button("Clear", ButtonSize))
                {
                    m_log.clear();
                }
                ImGui::Separator();
            }
            ImGui::EndChild();

            ImGui::BeginChild("LoggerBottom");
            {
                for (auto& t : m_log)
                {
                    ImGui::TextColored(m_loggingColors[t.first], "%s", t.second.c_str());
                }
                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
    }

    bool ContentBrowser::DidFolderChange(const std::filesystem::path& a_folderPath)
    {
        std::vector<FileData> folderFiles;

        //Go through the files in the current folder.
        for (const auto& entry : std::filesystem::directory_iterator(a_folderPath))
        {
            FileData data;
            data.m_fileName = entry.path().filename().string();
            data.m_fileExtension = entry.path().extension().string();
            data.m_timeLastEdited = entry.last_write_time();
            data.m_filePath = entry.path().string();

            if (entry.is_directory())
            {
                data.m_isFolder = true;
                folderFiles.push_back(data);
            }
            else
            {
                data.m_isFolder = false;
                const auto found = std::find(m_allowedExtensions.begin(), m_allowedExtensions.end(), data.m_fileExtension);
                if (found != m_allowedExtensions.end())
                {
                    folderFiles.push_back(data);
                }
            }
        }

        std::sort(folderFiles.begin(), folderFiles.end(), [](const auto& lhs, const auto& rhs)
            {
                return lhs.m_isFolder && !rhs.m_isFolder;
            });

        std::vector<FileData>& currentFiles = m_fileDatas.at(a_folderPath.string());
        if (currentFiles.size() != folderFiles.size())
        {
            m_fileDatas[a_folderPath.string()] = folderFiles;
            HE_CORE_INFO("[ContentBrowser] Number of files has changed in folder: {0}", a_folderPath.string());

            SortContentBrowser();
            return true;
        }

        for (std::size_t i = 0; i < folderFiles.size(); ++i)
        {
            if (folderFiles[i] != currentFiles[i])
            {
                HE_CORE_INFO("[ContentBrowser] One or more files has been updated in folder: {0}", a_folderPath.string());
                m_fileDatas[a_folderPath.string()] = folderFiles;

                SortContentBrowser();
                return true;
            }
        }

        //No changes
        return false;
    }

    std::vector<FileData> ContentBrowser::GetAllFilesOfExtensions(std::vector<std::string> a_extensions)
    {
        std::vector<FileData> files;
        for (auto& folder : m_fileDatas)
        {
            for (auto& file : folder.second)
            {
                if (!file.m_isFolder)
                {
                    const auto it = std::find_if(a_extensions.begin(), a_extensions.end(), [&file](const auto& ext)
                        {
                            return ext == file.m_fileExtension;
                        });

                    if (it != a_extensions.end())
                    {
                        files.push_back(file);
                    }
                }
            }
        }
        return files;
    }

    void ContentBrowser::SortContentBrowser()
    {
        for (auto& folder : m_fileDatas)
        {
            std::sort(folder.second.begin(), folder.second.end(), [](const auto& lhs, const auto& rhs)
                {
                    return lhs.m_isFolder && !rhs.m_isFolder;
                });
        }
    }
}
