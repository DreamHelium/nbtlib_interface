/*  nbt_if_common - NBT Lib Common Part
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

#include "nbt_if_common.h"
#include "nbt_interface.h"
#include <glib.h>

int dh_nbt_instance_child_to_node(NbtInstance* instance, const char* key)
{
    if(dh_nbt_instance_child(instance))
    {
        for(; dh_nbt_instance_is_non_null(instance) ; dh_nbt_instance_next(instance))
        {
            if(dh_nbt_instance_get_key(instance) && !strcmp(dh_nbt_instance_get_key(instance), key))
                return TRUE;
        }
        dh_nbt_instance_parent(instance);
        return FALSE;
    }
    else return FALSE;
}