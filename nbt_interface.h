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

#ifndef NBT_INTERFACE_H
#define NBT_INTERFACE_H

/* From glibconfig.h */
#ifdef __GNUC__
#define G_GNUC_CHECK_VERSION(major, minor) \
    ((__GNUC__ > (major)) || \
     ((__GNUC__ == (major)) && \
      (__GNUC_MINOR__ >= (minor))))
#else
#define G_GNUC_CHECK_VERSION(major, minor) 0
#endif

#if G_GNUC_CHECK_VERSION(3, 1) || defined(__clang__)
#define G_DEPRECATED __attribute__((__deprecated__))
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define G_DEPRECATED __declspec(deprecated)
#else
#define G_DEPRECATED
#endif

#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif

/* The base instance of NBT */
typedef struct _NbtInstance NbtInstance;
/* The real NBT structure */
typedef struct _RealNbt RealNbt;
/* The enum type for recognize */
typedef enum {
    DH_TYPE_INVALID, DH_TYPE_End, DH_TYPE_Byte, DH_TYPE_Short, DH_TYPE_Int, DH_TYPE_Long, DH_TYPE_Float, DH_TYPE_Double, DH_TYPE_Byte_Array, DH_TYPE_String, DH_TYPE_List, DH_TYPE_Compound, DH_TYPE_Int_Array, DH_TYPE_Long_Array} DhNbtType;

#define      dh_nbt_instance_parse(filename) dh_nbt_if_parse(filename)
NbtInstance* dh_nbt_if_parse(const char* filename);
NbtInstance* dh_nbt_instance_new_from_real_nbt(RealNbt* nbt);
void         dh_nbt_instance_set_free_only_instance(NbtInstance* instance, int val);
/* Free only instance */
NbtInstance* dh_nbt_instance_dup(NbtInstance* instance);
/* To be implemented. */
// NbtInstance* dh_nbt_instance_dup_full(NbtInstance* instance);
void         dh_nbt_instance_free(NbtInstance* instance);
/* Not free the RealNbt Struct. */
void         dh_nbt_instance_free_only_instance(NbtInstance* instance);
RealNbt*     dh_nbt_instance_get_real_original_nbt(NbtInstance* instance);
RealNbt*     dh_nbt_instance_get_real_current_nbt(NbtInstance* instance);
DhNbtType    dh_nbt_get_type(NbtInstance* instance);
int          dh_nbt_instance_is_non_null(NbtInstance* instance);
int          dh_nbt_instance_prev(NbtInstance* instance);
int          dh_nbt_instance_next(NbtInstance* instance);
int          dh_nbt_instance_parent(NbtInstance* instance);
int          dh_nbt_instance_child(NbtInstance* instance);
void         dh_nbt_instance_goto_root(NbtInstance* instance);
int          dh_nbt_instance_is_type(NbtInstance* instance, DhNbtType type);
const char*  dh_nbt_instance_get_key(NbtInstance* instance);

int8_t       dh_nbt_instance_get_byte(NbtInstance* instance);
int16_t      dh_nbt_instance_get_short(NbtInstance* instance);
int32_t      dh_nbt_instance_get_int(NbtInstance* instance);
int64_t      dh_nbt_instance_get_long(NbtInstance* instance);
int64_t      dh_nbt_instance_get_integer(NbtInstance* instance);

float        dh_nbt_instance_get_float(NbtInstance* instance);
double       dh_nbt_instance_get_double(NbtInstance* instance);

/* The array type should be freed! */
const char*     dh_nbt_instance_get_string(NbtInstance* instance);
const int8_t*   dh_nbt_instance_get_byte_array(NbtInstance* instance, int* len);
const int32_t*  dh_nbt_instance_get_int_array(NbtInstance* instance, int* len);
const int64_t*  dh_nbt_instance_get_long_array(NbtInstance* instance, int* len);

NbtInstance*    dh_nbt_instance_new_byte(int8_t value, const char* key);
NbtInstance*    dh_nbt_instance_new_short(int16_t value, const char* key);
NbtInstance*    dh_nbt_instance_new_int(int32_t value, const char* key);
NbtInstance*    dh_nbt_instance_new_long(int64_t value, const char* key);
NbtInstance*    dh_nbt_instance_new_float(float value, const char* key);
NbtInstance*    dh_nbt_instance_new_double(double value, const char* key);
NbtInstance*    dh_nbt_instance_new_string(const char* str, const char* key);
NbtInstance*    dh_nbt_instance_new_byte_array(int8_t* value, int len, const char* key);
NbtInstance*    dh_nbt_instance_new_int_array(int32_t* value, int len, const char* key);
NbtInstance*    dh_nbt_instance_new_long_array(int64_t* value, int len, const char* key);
NbtInstance*    dh_nbt_instance_new_list(const char* key);
NbtInstance*    dh_nbt_instance_new_compound(const char* key);
#define         dh_nbt_instance_prepend(parent, child) dh_nbt_instance_fill_child(parent, child)
int             dh_nbt_instance_fill_child(NbtInstance* src, NbtInstance* child);
G_DEPRECATED
int             dh_nbt_instance_fill_prev(NbtInstance* src, NbtInstance* prev);
G_DEPRECATED
int             dh_nbt_instance_fill_next(NbtInstance* src, NbtInstance* next);
int             dh_nbt_instance_insert_after(NbtInstance* parent, NbtInstance* sibling, NbtInstance* node);
int             dh_nbt_instance_insert_before(NbtInstance* parent, NbtInstance* sibling, NbtInstance* node);

int             dh_nbt_instance_save_to_file(NbtInstance* instance, const char* pos);

#ifdef __cplusplus
}
#endif

#endif /* NBT_INTERFACE_H */