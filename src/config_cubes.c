/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file config_cubes.c
 *     Terrain cubes configuration loading functions.
 * @par Purpose:
 *     Support of configuration files for terrain cubes, which make up columns.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     11 Jun 2012 - 24 Nov 2012
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "pre_inc.h"
#include "bflib_basics.h"
#include "bflib_fileio.h"
#include "bflib_dernc.h"
#include "globals.h"
#include "game_legacy.h"
#include "config.h"
#include "config_cubes.h"
#include "post_inc.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
static void assign_owner(const struct NamedField* named_field, int64_t value, const struct NamedFieldSet* named_fields_set, int idx, const char* src_str, unsigned char flags);
/******************************************************************************/
struct NamedCommand cube_desc[CUBE_ITEMS_MAX];
/******************************************************************************/
static TbBool load_cubes_config_file(const char *fname, unsigned short flags);

const struct ConfigFileData keeper_cubes_file_data = {
    .filename = "cubes.cfg",
    .load_func = load_cubes_config_file,
    .pre_load_func = NULL,
    .post_load_func = NULL,
};

static const struct NamedCommand cubes_properties_flags[] = {
    {"LAVA",           CPF_IsLava},
    {"WATER",          CPF_IsWater},
    {"SACRIFICIAL",    CPF_IsSacrificial},
    {"UNCLAIMED_PATH", CPF_IsUnclaimedPath},
    {NULL,             0},
};

static const struct NamedField cubes_named_fields[] = {
    //name           //pos    //field                                               //default //min     //max           //NamedCommand
    {"Name",            0, field(game.conf.cube_conf.cube_cfgstats[0].code_name),         0,  0,                     0, cube_desc,                 value_name,      assign_null},
    {"Textures",        0, field(game.conf.cube_conf.cube_cfgstats[0].texture_id[0]),     0,  0,             USHRT_MAX, NULL,                      value_default,   assign_default},
    {"Textures",        1, field(game.conf.cube_conf.cube_cfgstats[0].texture_id[1]),     0,  0,             USHRT_MAX, NULL,                      value_default,   assign_default},
    {"Textures",        2, field(game.conf.cube_conf.cube_cfgstats[0].texture_id[2]),     0,  0,             USHRT_MAX, NULL,                      value_default,   assign_default},
    {"Textures",        3, field(game.conf.cube_conf.cube_cfgstats[0].texture_id[3]),     0,  0,             USHRT_MAX, NULL,                      value_default,   assign_default},
    {"Textures",        4, field(game.conf.cube_conf.cube_cfgstats[0].texture_id[4]),     0,  0,             USHRT_MAX, NULL,                      value_default,   assign_default},
    {"Textures",        5, field(game.conf.cube_conf.cube_cfgstats[0].texture_id[5]),     0,  0,             USHRT_MAX, NULL,                      value_default,   assign_default},
    {"OwnershipGroup",  0, field(game.conf.cube_conf.cube_cfgstats[0].ownershipGroup),    0,  0, CUBE_OWNERSHIP_GROUPS, NULL,                      value_default,   assign_default},
    {"Owner",           0, field(game.conf.cube_conf.cube_cfgstats[0].owner),             0,  0,         PLAYERS_COUNT, cmpgn_human_player_options,value_default,   assign_owner},
    {"Properties",     -1, field(game.conf.cube_conf.cube_cfgstats[0].properties_flags),  0,  0,             UCHAR_MAX, cubes_properties_flags,    value_flagsfield,assign_default},
    {NULL},
};

const struct NamedFieldSet cubes_named_fields_set = {
    &game.conf.cube_conf.cube_types_count,
    "cube",
    cubes_named_fields,
    cube_desc,
    CUBE_ITEMS_MAX,
    sizeof(game.conf.cube_conf.cube_cfgstats[0]),
    game.conf.cube_conf.cube_cfgstats,
};


/******************************************************************************/
#ifdef __cplusplus
}
#endif
/******************************************************************************/

static void assign_owner(const struct NamedField* named_field, int64_t value, const struct NamedFieldSet* named_fields_set, int idx, const char* src_str, unsigned char flags)
{
    struct CubeConfigStats *cubed = get_cube_model_stats(idx);
    if (cubed->ownershipGroup <= 0)
    {
        NAMFIELDWRNLOG("Owner without OwnershipGroup for [%s%d].", named_fields_set->block_basename, idx);
        return;
    }

    game.conf.cube_conf.cube_bits[cubed->ownershipGroup][value] = idx;
    assign_default(named_field,value,named_fields_set,idx,src_str,flags);
}

struct CubeConfigStats *get_cube_model_stats(long cumodel)
{
    if ((cumodel < 0) || (cumodel >= CUBE_ITEMS_MAX))
    {
        return &game.conf.cube_conf.cube_cfgstats[0];
    }
    return &game.conf.cube_conf.cube_cfgstats[cumodel];
}

static TbBool load_cubes_config_file(const char *fname, unsigned short flags)
{
    SYNCDBG(0, "%s file \"%s\".", ((flags & CnfLd_ListOnly) == 0) ? "Reading" : "Parsing", fname);
    long len = LbFileLengthRnc(fname);
    if (len < MIN_CONFIG_FILE_SIZE)
    {
        if ((flags & CnfLd_IgnoreErrors) == 0)
        {
            WARNMSG("file \"%s\" doesn't exist or is too small.", fname);
        }
        return false;
    }
    char *buf = (char *)calloc(len + 256, 1);
    if (buf == NULL)
    {
        return false;
    }
    // Loading file data.
    len = LbFileLoadAt(fname, buf);
    TbBool result = (len > 0);
    // Parse blocks of the config file.
    parse_named_field_blocks(buf, len, fname, flags, &cubes_named_fields_set);
    // Freeing and exiting.
    free(buf);
    return result;
}

/* Returns Code Name (name to use in script file) of given cube model. */
const char *cube_code_name(long model)
{
    const char *name = get_conf_parameter_text(cube_desc, model);
    if (name[0] != '\0')
    {
        return name;
    }
    return "INVALID";
}

/*
 * Returns the cube model identifier for a given code name (found in script file). Linear running time.
 * @param code_name
 * @return A positive integer for the cube model if found, otherwise -1.
 */
ThingModel cube_model_id(const char *code_name)
{
    for (int i = 0; i < game.conf.cube_conf.cube_types_count; ++i)
    {
        if (strncasecmp(game.conf.cube_conf.cube_cfgstats[i].code_name, code_name, COMMAND_WORD_LEN) == 0)
        {
            return i;
        }
    }
    return -1;
}

void clear_cubes(void)
{
    memset(&game.conf.cube_conf, 0, sizeof(game.conf.cube_conf));
}

/******************************************************************************/
