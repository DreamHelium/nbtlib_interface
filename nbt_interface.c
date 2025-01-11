/*  nbt_interface - NBT Lib Interface
    Copyright (C) 2025 Dream Helium
    This file is part of nbtlib_interface.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include "glib.h"
#define LIBNBT_CODE_SPECIFIC

#ifdef LIBNBT_CODE_SPECIFIC
#define _RealNbt NBT 
/* idk */
#endif

#include "libnbt/nbt.h"
#include "nbt_interface.h"
#include <gio/gio.h>

typedef struct _NbtInstance
{
    /* Root NBT */
    RealNbt* original_nbt;
    /* The current position of NBT */
    RealNbt* current_nbt;
    /* This can be NULL, based on the implement */
    TreeStruct* tree_struct;
    /* Free onbt? */
    gboolean duped;
} _NbtInstance;

/* I have forgotten the implement of this, thus copied from nbt_pos */
/* And this is for the design of the libnbt because it's simple
 * I want to add parent support */
typedef struct _TreeStruct
{
    // /** The level of the pos in the NBT */
    // int level;
    // /** The pos of child in the NBT, len: level */
    // int* child;
    // /** NBT tree, len: level + 1 (include root as the first when level is 0) */
    // RealNbt** tree;
    // /** Represent the item in the latest tree */
    // int item;
    /* Maybe I do not need the functions above */
    GPtrArray* tree_array;
} _TreeStruct;


static char *dh_strdup(const char *o_str)
{
#if (defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L) || _POSIX_C_SOURCE >= 200809L
    return strdup(o_str); // use strdup if provided
#else
    char* str = malloc( (strlen(o_str) + sizeof("") ) * sizeof(char));
    if(str)
    {
        strcpy(str, o_str);
        return str;
    }
    else return NULL;
#endif
}


static TreeStruct* get_new_tree(RealNbt* nbt)
{
#ifdef LIBNBT_CODE_SPECIFIC
    TreeStruct* ts = g_new0(TreeStruct, 1);
    ts->tree_array = g_ptr_array_new();
    g_ptr_array_add(ts->tree_array, nbt);
    return ts;
#endif
}

static NbtInstance* parse_nbt_real(RealNbt* nbt)
{
    NbtInstance* instance = g_new(NbtInstance, 1);
    instance->original_nbt = nbt;
    instance->current_nbt = nbt;
    instance->tree_struct = get_new_tree(nbt);
    instance->duped = 0;
    return instance;
}

NbtInstance* dh_nbt_if_parse(const char* filename)
{
    size_t len = 0;
    guint8* content = NULL;
    GError* err = NULL;
    if(g_file_get_contents(filename, (char**)&content, &len, &err))
    {
#ifdef LIBNBT_CODE_SPECIFIC
        NBT* nbt = NBT_Parse(content, len);
        if(nbt)
        {
            return parse_nbt_real(nbt);
        }
        else return NULL;
#endif
    }
    else return NULL;
}

NbtInstance* dh_nbt_instance_new_from_real_nbt(RealNbt* nbt)
{
    return parse_nbt_real(nbt);
}

NbtInstance* dh_nbt_instance_dup(NbtInstance* instance)
{
    NbtInstance* ret = g_new0(NbtInstance, 1);
    ret->duped = 1;
    ret->current_nbt = instance->current_nbt;
    ret->original_nbt = instance->original_nbt;
    ret->tree_struct = g_new0(TreeStruct, 1);
    ret->tree_struct->tree_array = g_ptr_array_copy(instance->tree_struct->tree_array, NULL, NULL);
    return ret;
}

RealNbt* dh_nbt_instance_get_real_original_nbt(NbtInstance* instance)
{
    return instance->original_nbt;
}

RealNbt* dh_nbt_instance_get_real_current_nbt(NbtInstance* instance)
{
    return instance->current_nbt;
}

void dh_nbt_instance_free(NbtInstance* instance)
{
#ifdef LIBNBT_CODE_SPECIFIC
    if(!instance->duped) NBT_Free(instance->original_nbt);
    if(instance->tree_struct) g_ptr_array_free(instance->tree_struct->tree_array, FALSE);
    g_free(instance->tree_struct);
    g_free(instance);
#endif
}

int dh_nbt_instance_next(NbtInstance* instance)
{
#ifdef LIBNBT_CODE_SPECIFIC
    RealNbt* c_nbt = instance->current_nbt;
    if(c_nbt)
    {
        instance->current_nbt = c_nbt->next;
        return TRUE;
    }
    else return FALSE;
#endif
}

int dh_nbt_instance_parent(NbtInstance* instance)
{
#ifdef LIBNBT_CODE_SPECIFIC
    int len = instance->tree_struct->tree_array->len;
    if(len == 1)
        return FALSE;
    else
    {
        /*  Move last*/
        g_ptr_array_remove_index(instance->tree_struct->tree_array, --len);
        /* Get last */
        instance->current_nbt = instance->tree_struct->tree_array->pdata[len - 1];
        return TRUE;
    }
#endif
}

int dh_nbt_instance_child(NbtInstance *instance)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    RealNbt* c_nbt = instance->current_nbt;
    if(c_nbt && 
    (dh_nbt_instance_is_type(instance, DH_TYPE_Compound) || 
     dh_nbt_instance_is_type(instance, DH_TYPE_List)))
    {
        instance->current_nbt = c_nbt->child;
        if(instance->current_nbt != instance->original_nbt->child)
            g_ptr_array_add(instance->tree_struct->tree_array, instance->current_nbt);
        return TRUE;
    }
    else return FALSE;
    #endif
}

DhNbtType dh_nbt_get_type(NbtInstance* instance)
{
    if(instance->current_nbt)
        return instance->current_nbt->type + 1;
    else return 0;
}

int dh_nbt_instance_is_non_null(NbtInstance* instance)
{
    return ((instance != NULL) && (instance->current_nbt != NULL));
}

void dh_nbt_instance_goto_root(NbtInstance* instance)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    instance->current_nbt = instance->original_nbt;
    g_ptr_array_free(instance->tree_struct->tree_array, FALSE);
    instance->tree_struct->tree_array = g_ptr_array_new();
    g_ptr_array_add(instance->tree_struct->tree_array, instance->original_nbt);
    #endif
}

int dh_nbt_instance_is_type(NbtInstance* instance, DhNbtType type)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    if(!instance->current_nbt)
    {
        if(type == DH_TYPE_INVALID)
            return TRUE;
        else
            return FALSE;
    }
    int o_type = instance->current_nbt->type;
    if(o_type == (type - 1)) return TRUE;
    else
    {
        return FALSE;
    }
    #endif
}

const char* dh_nbt_instance_get_key(NbtInstance* instance)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    if(instance->current_nbt)
        return instance->current_nbt->key;
    else return NULL;
    #endif
}

int8_t dh_nbt_instance_get_byte(NbtInstance* instance)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Byte))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        return instance->current_nbt->value_i;
        #endif
    }
    else return -1;
}

int16_t dh_nbt_instance_get_short(NbtInstance* instance)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Short))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        return instance->current_nbt->value_i;
        #endif
    }
    else return -1;
}

int32_t dh_nbt_instance_get_int(NbtInstance* instance)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Int))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        return instance->current_nbt->value_i;
        #endif
    }
    else return -1;
}

int64_t dh_nbt_instance_get_long(NbtInstance* instance)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Long))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        return instance->current_nbt->value_i;
        #endif
    }
    else return -1;
}

int64_t dh_nbt_instance_get_integer(NbtInstance* instance)
{
    DhNbtType type = dh_nbt_get_type(instance);
    if(type >= DH_TYPE_Byte && type <= DH_TYPE_Long)
    #ifdef LIBNBT_CODE_SPECIFIC
        return instance->current_nbt->value_i;
    #endif
    else return -1;
}

float dh_nbt_instance_get_float(NbtInstance* instance)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Float))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        return instance->current_nbt->value_d;
        #endif
    }
    else return -1;
}

double dh_nbt_instance_get_double(NbtInstance* instance)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Double))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        return instance->current_nbt->value_d;
        #endif
    }
    else return -1;
}

/* The array type should not be freed! */
const char* dh_nbt_instance_get_string(NbtInstance* instance)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_String))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        return dh_strdup(instance->current_nbt->value_a.value);
        #endif
    }
    else return NULL;
}

const int8_t* dh_nbt_instance_get_byte_array(NbtInstance* instance, int* len)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Byte_Array))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        *len = instance->current_nbt->value_a.len;
        int byte = *len * sizeof(int8_t);
        int8_t* ret = malloc(byte);
        memcpy(ret, instance->current_nbt->value_a.value, byte);
        return ret;
        #endif
    }
    else return NULL;
}

const int32_t* dh_nbt_instance_get_int_array(NbtInstance* instance, int* len)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Int_Array))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        *len = instance->current_nbt->value_a.len;
        int byte = *len * sizeof(int32_t);
        int32_t* ret = malloc(byte);
        memcpy(ret, instance->current_nbt->value_a.value, byte);
        return ret;
        #endif
    }
    else return NULL;
}

const int64_t* dh_nbt_instance_get_long_array(NbtInstance* instance, int* len)
{
    if(dh_nbt_instance_is_type(instance, DH_TYPE_Long_Array))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        *len = instance->current_nbt->value_a.len;
        int byte = *len * sizeof(int64_t);
        int64_t* ret = malloc(byte);
        memcpy(ret, instance->current_nbt->value_a.value, byte);
        return ret;
        #endif
    }
    else return NULL;
}