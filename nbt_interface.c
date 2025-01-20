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

#include <string.h>
#define LIBNBT_CODE_SPECIFIC

#ifdef LIBNBT_CODE_SPECIFIC
#define _RealNbt NBT 
/* idk */
#endif

#include "libnbt/nbt.h"
#include "nbt_interface.h"
#include <gio/gio.h>

/* The real Tree structure */
typedef struct _TreeStruct TreeStruct;

typedef struct _NbtInstance
{
    /* Root NBT */
    RealNbt* original_nbt;
    /* The current position of NBT */
    RealNbt* current_nbt;
    /* This can be NULL, based on the implement */
    TreeStruct* tree_struct;
    /* Free onbt? */
    gboolean free_only_instance;
    /* Reference counting */
    int count;
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

static gboolean dh_file_create_gfile(GFile* file, gboolean is_file)
{
    if(!file)
        return FALSE;
    GError* error = NULL;
    if(file)
    {
        if(g_file_query_exists(file, NULL)) /* If file exists, delete file */
        {
            if(!g_file_delete(file, NULL, &error)) /* Couldn't delete file */
                goto error_handle;
        }
        /* I'll use a dummy way to create file
         * First, I'll mkdir */
        if(!g_file_make_directory_with_parents(file, NULL, &error))
            goto error_handle;
        if(is_file)
        {
            if(!g_file_delete(file, NULL, &error))
                goto error_handle;
            GFileOutputStream* gfos = g_file_create(file, G_FILE_CREATE_NONE, NULL, &error);
            if(error)
                goto error_handle;
            g_object_unref(gfos);
            return TRUE;
        }
        else return TRUE; /* It's directory */
    }
    else return FALSE;

error_handle:
    g_error_free(error);
    return FALSE;
}


static gboolean dh_write_file_gfile(GFile* file, char* content, gsize count)
{
    if(!file)
        return FALSE;
    if(!dh_file_create_gfile(file, TRUE))
        return FALSE;
    GFileIOStream* fios = g_file_open_readwrite(file, NULL, NULL);
    if(fios)
    {
        GOutputStream* os = g_io_stream_get_output_stream(G_IO_STREAM(fios));
        int ret_d = g_output_stream_write(os, content, count, NULL, NULL);
        gboolean ret = (ret_d == -1? FALSE : TRUE);
        g_object_unref(fios);
        return ret;
    }
    else
    {
        return FALSE;
    }
}


static gboolean dh_write_file(const char* filepos, char* content, gsize count)
{
    GFile* file = g_file_new_for_path(filepos);
    gboolean ret = dh_write_file_gfile(file, content, count);
    g_object_unref(file);
    return ret;
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
    instance->free_only_instance = 0;
    instance->count = 1;
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

void dh_nbt_instance_set_free_only_instance(NbtInstance* instance, int val)
{
    instance->free_only_instance = val;
}

NbtInstance* dh_nbt_instance_dup(NbtInstance* instance)
{
    NbtInstance* ret = g_new0(NbtInstance, 1);
    if(instance)
    {
        ret->free_only_instance = 1;
        ret->current_nbt = instance->current_nbt;
        ret->original_nbt = instance->original_nbt;
        ret->tree_struct = g_new0(TreeStruct, 1);
        ret->tree_struct->tree_array = g_ptr_array_copy(instance->tree_struct->tree_array, NULL, NULL);
        ret->count = 1;
    }
    else
    {
        ret->free_only_instance = 1;
        ret->current_nbt = NULL;
        ret->original_nbt = NULL;
        ret->tree_struct = g_new0(TreeStruct, 1);
        ret->tree_struct->tree_array = g_ptr_array_new();
        g_ptr_array_add(ret->tree_struct->tree_array, NULL);
        ret->count = 1;
    }
    return ret;
}

NbtInstance* dh_nbt_instance_dup_full(NbtInstance* instance)
{
    /* TODO */
    return instance;
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
    if(instance)
    {
#ifdef LIBNBT_CODE_SPECIFIC
        if(!instance->free_only_instance && instance->original_nbt) NBT_Free(instance->original_nbt);
        if(instance->tree_struct) g_ptr_array_free(instance->tree_struct->tree_array, FALSE);
        g_free(instance->tree_struct);
        g_free(instance);
#endif
    }
}

void dh_nbt_instance_free_only_instance(NbtInstance* instance)
{
    if(instance)
    {
#ifdef LIBNBT_CODE_SPECIFIC
        if(instance->tree_struct) g_ptr_array_free(instance->tree_struct->tree_array, FALSE);
        g_free(instance->tree_struct);
        g_free(instance);
#endif
    }
}

int dh_nbt_instance_prev(NbtInstance* instance)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    if(dh_nbt_instance_is_non_null(instance))
    {
        RealNbt* nbt = instance->current_nbt;
        instance->current_nbt = nbt->prev;
        return TRUE;
    }
    else return FALSE;
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
        instance->current_nbt = instance->tree_struct->tree_array->pdata[len -1];
        /*  Move last*/
        g_ptr_array_remove_index(instance->tree_struct->tree_array, --len);
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
        if(c_nbt != instance->original_nbt)
            g_ptr_array_add(instance->tree_struct->tree_array, c_nbt);
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

static NBT* ret_non_filled_nbt()
{
    NBT* new_nbt = malloc(sizeof(NBT));
    memset(new_nbt, 0, sizeof(NBT));
    return new_nbt;
}

NbtInstance*    dh_nbt_instance_new_byte(int8_t value, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Byte;
    new_nbt->value_i = value;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_short(int16_t value, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Short;
    new_nbt->value_i = value;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_int(int32_t value, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Int;
    new_nbt->value_i = value;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_long(int64_t value, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Long;
    new_nbt->value_i = value;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_float(float value, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Float;
    new_nbt->value_d = value;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_double(double value, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Double;
    new_nbt->value_d = value;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_string(const char* str, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_String;
    new_nbt->value_a.value = dh_strdup(str);
    new_nbt->value_a.len = strlen(str);
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_byte_array(int8_t* value, int len, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Byte_Array;
    int byte = len * sizeof(int8_t);
    int8_t* new_array = malloc(byte);
    memcpy(new_array, value, byte);
    new_nbt->value_a.value = new_array;
    new_nbt->value_a.len = len;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_int_array(int32_t* value, int len, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Byte_Array;
    int byte = len * sizeof(int32_t);
    int32_t* new_array = malloc(byte);
    memcpy(new_array, value, byte);
    new_nbt->value_a.value = new_array;
    new_nbt->value_a.len = len;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_long_array(int64_t* value, int len, const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Byte_Array;
    int byte = len * sizeof(int64_t);
    int64_t* new_array = malloc(byte);
    memcpy(new_array, value, byte);
    new_nbt->value_a.value = new_array;
    new_nbt->value_a.len = len;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_list(const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_List;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

NbtInstance*    dh_nbt_instance_new_compound(const char* key)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* new_nbt = ret_non_filled_nbt();
    new_nbt->type = TAG_Compound;
    new_nbt->key = key ? dh_strdup(key) : NULL;
    return dh_nbt_instance_new_from_real_nbt(new_nbt);
    #endif
}

int dh_nbt_instance_fill_child(NbtInstance* src, NbtInstance* child)
{
    if(dh_nbt_instance_is_non_null(src) && dh_nbt_instance_is_non_null(child))
    {
        if(dh_nbt_instance_is_type(src, DH_TYPE_List) ||
           dh_nbt_instance_is_type(src, DH_TYPE_Compound))
        {
            #ifdef LIBNBT_CODE_SPECIFIC
            RealNbt* nbt = src->current_nbt;
            nbt->child = child->current_nbt;
            return TRUE;
            #endif
        }
        else return FALSE;
    }
    else return FALSE;
}

G_DEPRECATED
int dh_nbt_instance_fill_prev(NbtInstance* src, NbtInstance* prev)
{
    if(dh_nbt_instance_is_non_null(src) && dh_nbt_instance_is_non_null(prev))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        RealNbt* nbt = src->current_nbt;
        nbt->prev = prev->current_nbt;
        return TRUE;
        #endif
    }
    else return FALSE;
}
int dh_nbt_instance_fill_next(NbtInstance* src, NbtInstance* next)
{
    if(dh_nbt_instance_is_non_null(src) && dh_nbt_instance_is_non_null(next))
    {
        #ifdef LIBNBT_CODE_SPECIFIC
        RealNbt* nbt = src->current_nbt;
        nbt->next = next->current_nbt;
        return TRUE;
        #endif
    }
    else return FALSE;
}

#ifdef LIBNBT_CODE_SPECIFIC
static gboolean nbt_instance_has_child(NbtInstance* parent, NbtInstance* child)
{
    NBT* parent_node = parent->current_nbt;
    if(parent_node->child && child)
    {
        NBT* child_node = parent_node->child;
        for(; child_node ; child_node = child_node->next)
        {
            if(child_node == child->current_nbt)
                return TRUE;
        }
        return FALSE;
    }
    else if(child != NULL) 
        return FALSE;
    else return TRUE;
}
#endif

int dh_nbt_instance_insert_after(NbtInstance* parent, NbtInstance* sibling, NbtInstance* node)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    if(nbt_instance_has_child(parent, sibling))
    {
        if(sibling->current_nbt && sibling)
        {
            NBT* sibling_node = sibling->current_nbt;
            if(sibling_node->next)
            {
                sibling_node->next->prev = node->current_nbt;
            }
            node->current_nbt->next = sibling_node->next;
            node->current_nbt->prev = sibling_node;
            sibling_node->next = node->current_nbt;
        }
        else
        {
            if(parent->current_nbt->child)
            {
                node->current_nbt->next = parent->current_nbt;
                parent->current_nbt->child->prev = node->current_nbt;
            }
            parent->current_nbt->child = node->current_nbt;
        }
        return TRUE;
    }
    else return FALSE;
    #endif
}

int dh_nbt_instance_insert_before(NbtInstance *parent, NbtInstance *sibling, NbtInstance *node)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    if(nbt_instance_has_child(parent, sibling))
    {
        if(sibling && sibling->current_nbt)
        {
            NBT* sibling_node = sibling->current_nbt;
            if(sibling_node->prev)
            {
                node->current_nbt->prev = sibling_node->prev;
                node->current_nbt->prev->next = node->current_nbt;
                node->current_nbt->next = sibling_node;
                sibling_node->prev = node->current_nbt;
            }
            else
            {
                parent->current_nbt->child = node->current_nbt;
                node->current_nbt->next = sibling_node;
                sibling_node->prev = node->current_nbt;
            }
        }
        else 
        {
            if(parent->current_nbt->child)
            {
                NBT* sibling_node = parent->current_nbt->child;
                while(sibling_node->next)
                    sibling_node = sibling_node->next;
                node->current_nbt->prev = sibling_node;
                sibling_node->next = node->current_nbt;
            }
            else
                parent->current_nbt->child = node->current_nbt;
        }
        return TRUE;
    }
    else return FALSE;
    #endif
}

int dh_nbt_instance_save_to_file(NbtInstance* instance, const char* pos)
{
    #ifdef LIBNBT_CODE_SPECIFIC
    NBT* root = dh_nbt_instance_get_real_original_nbt(instance);
    int bit = 1;
    size_t len = 0;
#ifndef LIBNBT_USE_LIBDEFLATE
    size_t old_len = 0;
#endif
    uint8_t* data = NULL;
    while(1)
    {
        len = 1 << bit;
        data = (uint8_t*)malloc(len * sizeof(uint8_t));
        int ret = NBT_Pack(root, data, &len);
        if(ret == 0)
        {
#ifndef LIBNBT_USE_LIBDEFLATE
            if(old_len != len) // compress not finish due to a bug in old libnbt (in submodule)
            {
                old_len = len;
                free(data);
                bit++;
                continue;
            }
#endif
            if(pos)
            {
                dh_write_file(pos, (char*)data, len);
                free(data);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if(bit < 25)
        {
            free(data);
            bit++; // It might be not enough space
        }
        else
        {
            free(data);
            return 0;
        }
    }
    return 0;
    #endif
}