#include "YTE/StandardLibrary/FileSystem.hpp"
#include "YTE/StandardLibrary/File.hpp"

namespace YTE
{
  FileWriter::FileWriter(std::string const& aFile)
    : mFile{ aFile, std::ios::binary }
  {
    if (false == mFile.bad())
    {
      mOpened = true;
    }
  }
    
  FileWriter::~FileWriter()
  {
    mFile.write(mData.data(), mData.size());
    mFile.close();
  }

    
  FileReader::FileReader(std::string const& aFile)
  {
    if (!std::filesystem::exists(aFile))
    {
      return;
    }

    std::ifstream file(aFile, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
      std::streamsize size = file.tellg();
      file.seekg(0, std::ios::beg);

      mData.resize(size);
      file.read(mData.data(), size);
      assert(false == file.bad());

      if (false == file.bad())
      {
        mOpened = true;
      }
    }
  }
}
