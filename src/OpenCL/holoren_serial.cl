/*
 * Copyright (C) 2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of Holoren.
 *
 * Holoren is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Holoren is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Holoren.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * A function to compute spherical wave equation for a single point source
 */
__kernel void compObjWave(__global float *pc,    /// a point cloud with point sources
                          uint pc_size,          /// the number of elements in point cloud
                          __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                          float hologram_z,      /// the z depth of hologram
                          float k,               /// wavenumber
                          float pitch,           /// pitch between samples
                          float corner_x,        /// x corner of optical field
                          float corner_y)        /// y corner of optical field
{
  int row = get_global_id(0);
  int col = get_global_id(1);
  uint of_index = row * get_global_size(1) + col;
  
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i * 3], pc[i * 3 + 1], pc[i * 3 + 2], 0.0f);
    float x = (((col - 1) * pitch) + corner_x) - ps.x;
    float y = (((row - 1) * pitch) + corner_y) - ps.y;
    float z = hologram_z - ps.z;
    float r = sqrt(x * x + y * y + z * z);

    float re = cos(k * r);
    float im = sin(k * r);
    of[of_index] += (float2) (re, im);
  }
}
