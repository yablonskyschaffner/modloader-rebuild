/*
 * Copyright (C) 2013-2014  LINK/2012 <dma_2012@hotmail.com>
 * Licensed under GNU GPL v3, see LICENSE at top level directory.
 *
 *  std-rdm -- Standard Replaceable Disk iMage Plugin for San Andreas Mod Loader
 *
 *      Takes .img files from the mod folders and, whenever the game owns an image file with the
 *      very same name (that is, a file with that name exists at the "models/img" folder), it makes
 *      the game read our file instead of the original one.
 *
 *      Nothing is written to the disk and no image content gets merged, the whole image file is
 *      replaced by redirecting the paths the game opens.
 *
 */
#include "rdm.h"
#include <modloader_util.hpp>
#include <modloader_util_path.hpp>
#include <modloader_util_container.hpp>
#include <modloader_util_injector.hpp>
#include "CdStreamInfo.h"

using namespace modloader;

// The folder, relative to the game directory, that tells which image files the game owns
static const char* szGameImgFolder = "models\\img";

CThePlugin* rdmPlugin;
static CThePlugin plugin;

/*
 *  Export plugin object data
 */
extern "C" __declspec(dllexport)
void GetPluginData(modloader_plugin_t* data)
{
    rdmPlugin = &plugin;
    modloader::RegisterPluginData(plugin, data, plugin.default_priority);
}



/*
 *  Basic plugin informations
 */
const char* CThePlugin::GetName()
{
    return "std-rdm";
}

const char* CThePlugin::GetAuthor()
{
    return "LINK/2012";
}

const char* CThePlugin::GetVersion()
{
    return "RC1";
}

const char** CThePlugin::GetExtensionTable()
{
    /* Put the extensions this plugin handles on @table */
    static const char* table[] = { "img", nullptr };
    return table;
}


/*
 *  Reads the image files the game owns, they're the only ones we're able to replace
 */
bool CThePlugin::OnStartup()
{
    std::string dir = std::string(this->modloader->gamepath) + szGameImgFolder;

    if(!IsDirectoryA(dir.c_str()))
    {
        Log("Folder \"%s\" doesn't exist, no image file will be replaced by this plugin", szGameImgFolder);
        return true;
    }

    ForeachFile(dir, "*.img", false, [this](ModLoaderFile& file)
    {
        if(file.is_dir == false)
        {
            std::string name = file.filename;
            this->gameImgs.emplace(tolower(name));
            this->Log("Found game image file \"%s\\%s\"", szGameImgFolder, file.filename);
        }
        return true;
    });

    Log("Found %d image file(s) at \"%s\"", (int)(this->gameImgs.size()), szGameImgFolder);
    return true;
}

/*
 *  Checks if @filename names an image file the game owns
 */
bool CThePlugin::IsGameImg(const char* filename)
{
    std::string name = filename;
    return this->gameImgs.find(tolower(name)) != this->gameImgs.end();
}

/*
 *  Check if the file is the one we're looking for
 */
bool CThePlugin::CheckFile(modloader::ModLoaderFile& file)
{
    // Take actual image files only. A ".img" directory is a std-img thing (it imports the files
    // inside of it into the streaming), it doesn't replace any image file at all.
    if(!file.is_dir && IsFileExtension(file.filext, "img") && IsGameImg(file.filename))
        return true;
    return false;
}

/*
 * Process the replacement
 */
bool CThePlugin::ProcessFile(const modloader::ModLoaderFile& file)
{
    std::string key = file.filename;
    std::string filepath = GetFilePath(file);

    // Following the modloader overriding rule, the last file we receive is the one to be used
    auto it = this->replacements.emplace(tolower(key), std::string()).first;
    return RegisterReplacementFile(*this, file.filename, it->second, filepath.c_str());
}

/*
 *  Finds the replacement registered for the image file named @filename
 */
const char* CThePlugin::FindReplacement(const char* filename)
{
    std::string key = filename;
    auto it = this->replacements.find(tolower(key));
    return it == this->replacements.end()? nullptr : it->second.c_str();
}

/*
 * Called after all files have been processed
 */
bool CThePlugin::PosProcess()
{
    if(this->replacements.size())
    {
        Log("Replacing %d image file(s)", (int)(this->replacements.size()));
        this->RedirectStandardImg();
        this->RedirectLevelImg();
    }
    return true;
}


/*
 *  Redirects the image files the game opens from a hardcoded path, such as "models\gta3.img"
 */
void CThePlugin::RedirectStandardImg()
{
    struct TableItem
    {
        const char* name;       // Image file name
        const char* path;       // Replacement path, null when there's no replacement for it
        uintptr_t pushes[5];    // Addresses of the 'push' instructions that reference the image path
    };

    TableItem table[] =
    {
        { "gta3.img",       nullptr, { 0x408430, 0x406C2A, 0x40844C }                       },
        { "gta_int.img",    nullptr, { 0x40846E, 0x40848C }                                 },
        { "player.img",     nullptr, { 0x5A41A4, 0x5A69F7, 0x5A80F9 }                       },
        { "cuts.img",       nullptr, { 0x4D5EB9, 0x5AFBCB, 0x5AFC98, 0x5B07DA, 0x5B1423 }   }
    };

    bool bGtaImg = false;   // Replacing gta3.img or gta_int.img?

    for(TableItem& item : table)
    {
        // The strings we send to the game must be alive for it's entire lifetime,
        // that's why we take the pointer from our replacements map and never touch it again.
        if((item.path = this->FindReplacement(item.name)) != nullptr)
        {
            Log("Redirecting \"%s\" to \"%s\"", item.name, item.path);

            for(uintptr_t p : item.pushes)
            {
                if(p == 0) break;
                WriteMemory<const char*>(p + 1, item.path, true);
            }

            if(!compare(item.name, "gta3.img", false) || !compare(item.name, "gta_int.img", false))
                bGtaImg = true;
        }
    }

    // The game doesn't open gta3.img and gta_int.img by simply reading the pushes we've patched
    // above, it opens them in a paired sequence that assumes both paths have a limited length.
    // Let's reimplement that piece of code, the same way std-img does.
    //
    // Note std-img may replace this very same piece of code when it has a gta3/gta_int replacement
    // of it's own. That's not a problem because both implementations read the paths back from the
    // pushes at 0x40844C and 0x40848C, which is where our replacement paths are already stored.
    if(bGtaImg)
    {
        void(*OpenGtaImg)(void) = []()  // mid replacement for CStreaming::OpenGtaImg
        {
            int(*CStreaming__AddImageToList)(const char* filename, char notPlayerImg)
                = memory_pointer(0x407610).get();

            CdStreamInfo& cdinfo = *memory_pointer(0x8E3FEC).get<CdStreamInfo>();
            char* gta3Path = ReadMemory<char*>(0x40844C + 1, true);
            char* gtaIntPath = ReadMemory<char*>(0x40848C + 1, true);

            rdmPlugin->Log("Opening gta3 img: %s\nOpening gta_int img: %s", gta3Path, gtaIntPath);

            cdinfo.gta3_id = CStreaming__AddImageToList(gta3Path, true);
            cdinfo.gtaint_id = CStreaming__AddImageToList(gtaIntPath, true);
        };

        // We need to do this hook to not hook too much code
        MakeNOP(0x4083E4 + 5, 4);
        MakeJMP(0x4083E4, raw_ptr(OpenGtaImg));
    }
}


/*
 *  Redirects the image files the game opens because of an "IMG" line at the gta.dat/default.dat
 *  files, such as "data\script\script.img"
 */
void CThePlugin::RedirectLevelImg()
{
    typedef function_hooker<0x5B915B, int(const char*, char)> datcd_hook;

    // Hook the CStreaming::AddImageToList call made from CFileLoader::LoadLevel (the gta.dat reader).
    // Note std-img hooks this very same call site to load it's own image files. Both hooks do chain
    // properly, no matter which one gets installed first, because MakeCALL gives us back the previous
    // destination of the call (that is, the other plugin hook) as the function to continue into.
    make_function_hook<datcd_hook>([](datcd_hook::func_type AddImageToList, const char*& path, char& bIsNotClothesImage)
    {
        // Take the file name out of the path the game is about to open...
        std::string normalized = NormalizePath(path);
        const char* filename = normalized.c_str() + GetLastPathComponent<char>(normalized);

        // ...and see if we got a replacement for it
        if(const char* replacement = rdmPlugin->FindReplacement(filename))
        {
            rdmPlugin->Log("Redirecting level image file \"%s\" to \"%s\"", path, replacement);
            path = replacement;
        }

        return AddImageToList(path, bIsNotClothesImage);
    });
}
