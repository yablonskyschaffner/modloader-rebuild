/*
 * Copyright (C) 2013-2014  LINK/2012 <dma_2012@hotmail.com>
 * Licensed under GNU GPL v3, see LICENSE at top level directory.
 *
 *  std-rdm -- Standard Replaceable Disk iMage Plugin for San Andreas Mod Loader
 *      Replaces whole image files, no content merging happens here.
 *
 */
#ifndef RDM_H
#define	RDM_H

#include <modloader.hpp>
#include <string>
#include <map>
#include <set>

/*
 *  The plugin object
 */
extern class CThePlugin* rdmPlugin;
class CThePlugin : public modloader::CPlugin
{
    public:
        // Must be higher than the std-img priority (48), otherwise std-img would be the
        // handler for the .img files (it accepts any of them) and we'd never see a single one.
        static const int default_priority = 52;

        const char* GetName() override;
        const char* GetAuthor() override;
        const char* GetVersion() override;

        bool OnStartup() override;
        bool CheckFile(modloader::ModLoaderFile& file) override;
        bool ProcessFile(const modloader::ModLoaderFile& file) override;
        bool PosProcess() override;

        const char** GetExtensionTable() override;

        // Finds the replacement path registered for the image file named @filename (case insensitive)
        // Returns a null pointer when there's no replacement for it.
        // The returned pointer is valid for the entire lifetime of the plugin.
        const char* FindReplacement(const char* filename);

    private:
        std::set<std::string> gameImgs;                 // Lowercased names of the image files found at the game folder
        std::map<std::string, std::string> replacements;// Lowercased image name -> replacement path (relative to the game dir)

        // Checks if @filename names an image file the game owns and so we're able to replace
        bool IsGameImg(const char* filename);

        void RedirectStandardImg();     // Redirects gta3.img, gta_int.img, player.img and cuts.img
        void RedirectLevelImg();        // Redirects the image files declared at the gta.dat/default.dat files
};

#endif	/* RDM_H */
