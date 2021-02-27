#pragma once

namespace Helios
{
	bool SaveFile(const std::string& a_filename, const std::string& a_toSave);
    bool ReadFromFileToString(const std::string& a_filename, std::string& a_data);
	std::string ExtractFilenameFromPath(const std::string& a_path);
    std::string ExtractFolderFromFilePath(const std::string& a_filePath);
    std::string GetExePath(const std::string& a_exeName);
}