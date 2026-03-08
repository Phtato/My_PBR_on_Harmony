//
// Created by 新垣つつ on 2026/3/6.
//

#ifndef VKNDKEXAMPLE_COMMON_H
#define VKNDKEXAMPLE_COMMON_H

namespace gli
{
	/// Load a texture (DDS, KTX or KMG) from memory
	inline texture load(char const * Data, std::size_t Size)
	{
		{
			texture Texture = load_dds(Data, Size);
			if(!Texture.empty())
				return Texture;
		}
		{
			texture Texture = load_kmg(Data, Size);
			if(!Texture.empty())
				return Texture;
		}
		{
			texture Texture = load_ktx(Data, Size);
			if(!Texture.empty())
				return Texture;
		}

		return texture();
	}

    inline void tujiaming_printFiles(const char * dirPath){
        DIR* __d = (DIR*)opendir(dirPath);
        if (!__d)
        {

        }else
        {
            struct dirent* en;
            while ((en = readdir(__d)) != NULL)
            {
                // Platform-specific logging: use HiLog on OHOS, fallback to stderr elsewhere
                OH_LOG_ERROR(LOG_APP, "filename %{public}s", en->d_name);
            }
            closedir(__d);
        }
    }

	/// Load a texture (DDS, KTX or KMG) from file
	inline texture load(char const * Filename)
	{
		FILE* File = detail::open_file(Filename, "rb");
		if(!File)
			return texture();

		long Beg = std::ftell(File);
		std::fseek(File, 0, SEEK_END);
		long End = std::ftell(File);
		std::fseek(File, 0, SEEK_SET);

		std::vector<char> Data(static_cast<std::size_t>(End - Beg));

		std::fread(&Data[0], 1, Data.size(), File);
		std::fclose(File);

		return load(&Data[0], Data.size());
	}

	/// Load a texture (DDS, KTX or KMG) from file
	inline texture load(std::string const & Filename)
	{
		return load(Filename.c_str());
	}
}//namespace gli


#endif //VKNDKEXAMPLE_COMMON_H