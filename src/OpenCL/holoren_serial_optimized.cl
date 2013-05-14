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

  float2 sample = (float2) (
    0.0f,
    0.0f
  );

  float4 dist = (float4) (
    ((col - 1) * pitch) + corner_x,
    ((row - 1) * pitch) + corner_y,
    hologram_z,
    0.0f
  );
  
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i * 3], pc[i * 3 + 1], pc[i * 3 + 2], 0.0f);    // TODO: optimize this
                                                                             // maybe by passing a float4 aligned array to kernel

    ps = dist - ps;
    ps.w = sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += cos(ps.w);
    sample.s1 += sin(ps.w);
  }

  of[row * get_global_size(1) + col] = sample;
}
