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

#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif

/* The base instance of NBT */
typedef struct _NbtInstance NbtInstance;
/* The real NBT structure */
typedef struct _RealNbt RealNbt;
/* The real Tree structure */
typedef struct _TreeStruct TreeStruct;
/* The enum type for recognize */
typedef enum {
    DH_TYPE_INVALID, DH_TYPE_End, DH_TYPE_Byte, DH_TYPE_Short, DH_TYPE_Int, DH_TYPE_Long, DH_TYPE_Float, DH_TYPE_Double, DH_TYPE_Byte_Array, DH_TYPE_String, DH_TYPE_List, DH_TYPE_Compound, DH_TYPE_Int_Array, DH_TYPE_Long_Array} DhNbtType;

NbtInstance* dh_nbt_if_parse(const char* filename);
NbtInstance* dh_nbt_instance_new_from_real_nbt(RealNbt* nbt);
NbtInstance* dh_nbt_instance_dup(NbtInstance* instance);
void         dh_nbt_instance_free(NbtInstance* instance);
RealNbt*     dh_nbt_instance_get_real_original_nbt(NbtInstance* instance);
RealNbt*     dh_nbt_instance_get_real_current_nbt(NbtInstance* instance);
DhNbtType    dh_nbt_get_type(NbtInstance* instance);
int          dh_nbt_instance_is_non_null(NbtInstance* instance);
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
const char*  dh_nbt_instance_get_string(NbtInstance* instance);
const int8_t*   dh_nbt_instance_get_byte_array(NbtInstance* instance, int* len);
const int32_t*  dh_nbt_instance_get_int_array(NbtInstance* instance, int* len);
const int64_t*  dh_nbt_instance_get_long_array(NbtInstance* instance, int* len);



#ifdef __cplusplus
}
#endif

#endif /* NBT_INTERFACE_H */