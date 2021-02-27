#include "hepch.h"
#include "Utils/FileUtils.h"
#include <fstream>

namespace Helios
{
    bool SaveFile(const std::string& a_filename, const std::string& a_toSave)
    {
        std::ofstream os(a_filename, std::ofstream::out);

        if (!os.is_open())
        {
            HE_CORE_WARN("Couldn't open file for writing: {0}", a_filename);
            return false;
        }

        //Write to file.
        os.write(a_toSave.c_str(), a_toSave.size());

        return true;
    }

    bool ReadFromFileToString(const std::string& a_filename, std::string& a_data)
    {
        std::ifstream is(a_filename);

        if (!is.is_open())
        {
            HE_CORE_WARN("Can't open file for reading: {0}", a_filename);
            a_data = std::string();
            return false;
        }

        std::string data;
        is.seekg(0, std::ios::end);
        data.reserve(is.tellg());
        is.seekg(0, std::ios::beg);

        data.assign((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        a_data = data;
        return true;
    }

    std::string ExtractFilenameFromPath(const std::string& a_path)
    {
        std::size_t lastOf = a_path.find_last_of('\\');
        if (lastOf == std::string::npos)
        {
            lastOf = a_path.find_last_of("//");
            if (lastOf == std::string::npos)
            {
                HE_CORE_CRITICAL("Couldn't extract filename from path: {0}", a_path);
                return "";
            }
        }

        return a_path.substr(lastOf + 1);
    }

    std::string ExtractFolderFromFilePath(const std::string& a_filePath)
    {
        std::size_t lastOf = a_filePath.find_last_of("\\");
        if (lastOf == std::string::npos)
        {
            lastOf = a_filePath.find_last_of("//");
            if (lastOf == std::string::npos)
            {
                HE_CORE_CRITICAL("Couldn't extract filename from path: {0}", a_filePath);
                return "";
            }
        }
        return a_filePath.substr(0, a_filePath.size() - a_filePath.substr(lastOf).size());
    }

    std::string GetExePath(const std::string& a_exeName)
    {
        wchar_t buffer[MAX_PATH];

        GetModuleFileName(NULL, buffer, MAX_PATH);

        char s[MAX_PATH];
        sprintf(s, "%ls", buffer);

        std::string temp = std::string(s);

        const std::string exeName = a_exeName + ".exe";
        const size_t pos = temp.find(exeName);

        if (pos != std::string::npos)
        {
            // If found then erase it from string
            temp.erase(pos, exeName.length());
        }

        return temp;
    }

}