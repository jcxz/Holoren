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
 * Optical field interface and implementation
 */

#ifndef COPTICALFIELD_H
#define COPTICALFIELD_H

#include "config.h"
#include "global.h"
#include "CSharedDataPtr.h"

#include <cstring>
#include <fstream>
#include <complex>


/**
 * A class to manage the data of optical field
 */
class COpticalField
{
  public:
    /** the type of a single item in optical field */
    typedef std::complex<tFPType> CComplex;

  private:
    /* a data storage class for the optical field */
    struct SOpticalFieldPrivate : public CSharedData
    {
      public:
        /**
         * Constructor
         */
        SOpticalFieldPrivate(unsigned int rows, unsigned int cols, double lambda, double pitch)
          : CSharedData(),
            m_lambda(lambda),
            m_pitch(pitch),
            m_rows(rows),
            m_cols(cols),
            m_samples(new CComplex[rows * cols])
        {
        }

        /**
         * Copy constructor
         */
        SOpticalFieldPrivate(const SOpticalFieldPrivate & other)
          : CSharedData(other),
            m_lambda(other.m_lambda),
            m_pitch(other.m_pitch),
            m_rows(other.m_rows),
            m_cols(other.m_cols),
            m_samples(new CComplex[other.m_rows * other.m_cols])
        {
          std::memcpy(m_samples, other.m_samples, other.m_rows * other.m_cols * sizeof(CComplex));
        }

        /**
         * Destructor
         */
        ~SOpticalFieldPrivate(void)
        {
          delete [] m_samples;
        }

        /**
         * Assignment operator
         */
        SOpticalFieldPrivate & operator=(const SOpticalFieldPrivate & other)
        {
          // first allocate new data
          CComplex *samples = new CComplex[other.m_rows * other.m_cols];
          std::memcpy(samples, other.m_samples, other.m_rows * other.m_cols * sizeof(CComplex));

          // and only then delete the old data
          // note that this also handles self-assignment
          // without needing an if conditional
          delete [] m_samples;

          // now reinitialize all remaining members (including samples pointer)
          m_samples = samples;
          m_lambda = other.m_lambda;
          m_pitch = other.m_pitch;
          m_cols = other.m_cols;
          m_rows = other.m_rows;

          return *this;
        }

      public:
        double m_lambda;                       /// light wavelength
        double m_pitch;                        /// pitch between samples
        unsigned int m_rows;                   /// the number of rows
        unsigned int m_cols;                   /// the number of columns
        CComplex *m_samples;   /// an array of samples
    };

  public:
    /**
     * Default constructor
     */
    COpticalField(void)
      : m_data(new SOpticalFieldPrivate(0, 0, 0.0f, 0.0f))
    {
    }

    /**
     * Constructor
     */
    COpticalField(unsigned int rows, unsigned int cols, double lambda, double pitch)
      : m_data(new SOpticalFieldPrivate(rows, cols, lambda, pitch))
    {
    }

    /**
     * Copy constructor
     */
    COpticalField(const COpticalField & other)
      : m_data(other.m_data)
    {
    }

    /**
     */
    CComplex *data(void)
    {
      return m_data->m_samples;
    }

    /**
     */
    const CComplex *data(void) const
    {
      return m_data->m_samples;
    }

    /**
     */
    const CComplex *constData(void) const
    {
      return m_data.constData().m_samples;
    }

    /**
     */
    unsigned int getSize(void) const
    {
      return m_data->m_rows * m_data->m_cols;
    }

    /**
     * Get the size of data in bytes held by this container
     */
    size_t getByteSize(void) const
    {
      return m_data->m_rows * m_data->m_cols * sizeof(CComplex);
    }

    /**
     */
    unsigned int getNumRows(void) const
    {
      return m_data->m_rows;
    }

    /**
     */
    unsigned int getNumCols(void) const
    {
      return m_data->m_cols;
    }

    /**
     */
    double getWaveLength(void) const
    {
      return m_data->m_lambda;
    }

    /**
     */
    void setWaveLength(double lambda)
    {
      m_data->m_lambda = lambda;
      return;
    }

    /**
     */
    double getPitch(void) const
    {
      return m_data->m_pitch;
    }

    /**
     */
    void setPitch(double pitch)
    {
      m_data->m_pitch = pitch;
      return;
    }

    /**
     */
    CComplex & getSample(unsigned int row, unsigned int col)
    {
      return m_data->m_samples[row * m_data->m_cols + col];
    }

    /**
     */
    const CComplex & getSample(unsigned int row, unsigned int col) const
    {
      return m_data->m_samples[row * m_data->m_cols + col];
    }

    /**
     */
    void setSample(unsigned int row, unsigned int col, const CComplex & sample)
    {
      m_data->m_samples[row * m_data->m_cols + col] = sample;
      return;
    }

    /**
     * clear all data and reset dimensions
     */
    void reset(unsigned int rows, unsigned int cols)
    {
      delete [] m_data->m_samples;

      m_data->m_rows = rows;
      m_data->m_cols = cols;
      m_data->m_samples = new CComplex[rows * cols];

      return;
    }

    /**
     * clear all data
     */
    void clear(void)
    {
      m_data->m_rows = 0;
      m_data->m_cols = 0;
      return;
    }

    /**
     * Set all samples in optical field to zero
     */
    void zero(void)
    {
      std::memset(m_data->m_samples, 0, m_data->m_rows * m_data->m_cols * sizeof(tFPType));
      return;
    }

    /**
     * Save the optical field to a file
     */
    bool save(const char *filename)
    {
      std::ofstream os(filename, std::ios::out | std::ios::binary);
      if (!os.is_open())
      {
        DBG("Failed to create output file");
        return false;
      }

      return save(os);
    }

    /**
     * A method to save the optical field of floats
     */
    bool save(std::ostream & os);

  private:
    CSharedDataPtr<SOpticalFieldPrivate> m_data;  /// the actual data
};

#endif
