/**
 * Algorithm 1
 *
 * A kernel to compute spherical wave equation of a single point in optical field.
 * This kernel is meant to be used with `renderAlgorithm_SinglePass`.
 */
__kernel void compObjWave_SinglePass(__global float *pc,    /// a point cloud with point sources
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Algorithm 3
 *
 * A function to compute spherical wave equation for a single point of optical field
 * This function has two additional offset arguments, which allow consequent enqueues
 * of the kernel for the same optical field. This way even a optical field larger than
 * the available GPU memory can be computed.
 */
__kernel void compObjWave_big(__global float *pc,    /// a point cloud with point sources
                              uint pc_size,          /// the number of elements in point cloud
                              __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                              int of_row_offset,     /// offset in row of the resulting optical field
                              int of_col_offset,     /// offset in column of the resulting optical field
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
    ((col + of_col_offset - 1) * pitch) + corner_x,
    ((row + of_row_offset - 1) * pitch) + corner_y,
    hologram_z,
    0.0f
  );
  
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i * 3], pc[i * 3 + 1], pc[i * 3 + 2], 0.0f);  // TODO: optimize this
                                                                           // maybe by passing a float4 aligned array to kernel
    ps = dist - ps;
    ps.w = sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += cos(ps.w);
    sample.s1 += sin(ps.w);
  }

  // put the result to a correct place in global memory
  of[row * get_global_size(1) + col] = sample;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Algorithm 4
 *
 * A function to compute spherical wave equation for a single point of optical field
 * This function has two additional offset arguments, which allow consequent enqueues
 * of the kernel for the same optical field. This way even a optical field larger than
 * the available GPU memory can be computed.
 */
__kernel void compObjWave_big_flat(__global float *pc,    /// a point cloud with point sources
                                   uint pc_size,          /// the number of elements in point cloud
                                   __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                                   int offset,            /// offset in the resulting optical field
                                   int num_cols,          /// number of pixels in a column of optical field
                                   float hologram_z,      /// the z depth of hologram
                                   float k,               /// wavenumber
                                   float pitch,           /// pitch between samples
                                   float corner_x,        /// x corner of optical field
                                   float corner_y)        /// y corner of optical field
{
  size_t gid = get_global_id(0);
  int row = (gid + offset) / num_cols;
  //int row = native_divide((gid + offset), num_cols);
  int col = (gid + offset) - (row * num_cols);

  /* cumulative sum of complex amplitudes from point sources */
  float2 sample = (float2) (
    0.0f,
    0.0f
  );

  /* target position on the optical field */
  float4 of_pos = (float4) (
    ((col - 1) * pitch) + corner_x,
    ((row - 1) * pitch) + corner_y,
    hologram_z,
    0.0f
  );
  
  /* loop through the point sources and sum complex amplitudes
     from each point source to the target location in optical field  */
  for (uint i = 0; i < pc_size; ++i)
  {
    float4 ps = (float4) (pc[i * 3], pc[i * 3 + 1], pc[i * 3 + 2], 0.0f);  // TODO: optimize this
                                                                           // maybe by passing a float4 aligned array to kernel
    ps = of_pos - ps;
	ps.w = native_sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += native_cos(ps.w);
    sample.s1 += native_sin(ps.w);

	/*
    ps.w = sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += cos(ps.w);
    sample.s1 += sin(ps.w);
	*/
  }

  // put the result to a correct place in global memory
  of[gid] = sample;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Algorithm 5
 *
 * A function to compute spherical wave equation for a single point of optical field
 * This function has two additional offset arguments, which allow consequent enqueues
 * of the kernel for the same optical field. This way even a optical field larger than
 * the available GPU memory can be computed.
 */
__kernel void compObjWave_big_aligned(__global float4 *pc,   /// a point cloud with point sources
                                      uint pc_size,          /// the number of elements in point cloud
                                      __global float2 *of,   /// an output optical field (float2 because this is an array of complex numbers)
                                      int offset,            /// offset in the resulting optical field
                                      int num_cols,          /// number of pixels in a column of optical field
                                      float hologram_z,      /// the z depth of hologram
                                      float k,               /// wavenumber
                                      float pitch,           /// pitch between samples
                                      float corner_x,        /// x corner of optical field
                                      float corner_y)        /// y corner of optical field
{
  size_t gid = get_global_id(0);
  int row = (gid + offset) / num_cols;
  int col = (gid + offset) - (row * num_cols);

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
    float4 ps = (float4) (pc[i]);
    ps = dist - ps;
    ps.w = sqrt(ps.x * ps.x + ps.y * ps.y + ps.z * ps.z) * k;

    sample.s0 += cos(ps.w);
    sample.s1 += sin(ps.w);
  }

  // put the result to a correct place in global memory
  of[gid] = sample;
}
