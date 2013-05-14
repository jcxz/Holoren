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
 * Contains program's entry point
 */

#include "global.h"
#include "Utils.h"
#include "CPointCloudRWFactory.h"
#include "CBasePointCloudRW.h"
#include "CPointCloud.h"
#include "COpticalField.h"
#include "CSimpleRenderer.h"
#include "COpenCLRenderer.h"
#include "CTimer.h"

#include <iostream>
#include <ctime>



/** a usage message */
static const char * const g_usage = "USAGE:\n"
                                    "./holoren -i input_file\n"
                                    "          -o output_file\n"
                                    "          -h optical_field_num_rows\n"
                                    "          -w optical_field_num_cols\n"
                                    "          -s optical field sampling\n"
                                    "          -l lambda\n"
                                    "          -z hologram_z\n"
                                    "          -r simple|opencl|none\n"
                                    "          -a SinglePass|MultiPass|MultiPassCPU|MultiPassNative|MultiPassAligned\n"
                                    "          -c chunk_size\n"
                                    "          -f cl_source_file\n"
                                    "          -p performace_log\n"
                                    "          -t point_cloud_converter_sampling_step (in metres)\n";

/** a timer to meassure execution time of different sections of code */
static CTimer g_timer;


/** Defines the type of renderer that shall be used */
enum ERendererType
{
  REN_SIMPLE = 0,  /// use simple SW renderer
  REN_OPENCL,      /// use OpenCL renderer
  REN_NONE         /// do not render
};


/* a structure of command line arguments */
typedef struct {
  const char *ifilename;                     /// input file
  const char *ofilename;                     /// output file
  unsigned int of_rows;                      /// optical field number of rows
  unsigned int of_cols;                      /// optical field number of columns
  const char *cl_source;                     /// the location of OpenCL source file
  ERendererType renderer;                    /// the type of rendering engine
  COpenCLRenderer::EAlgorithmType alg_type;  /// the type of rendering algorithm that will be used
  unsigned long chunk_size;                  /// the size of processed chunk
  double sampling;                           /// pitch between individual samples
  double lambda;                             /// light wavelength
  double hologram_z;                         /// hologram placement
  const char *perf_log;                      /// a pointer to performance log file
  double pc_sampling_step;                   /// point cloud converter sampling step (size of a pixel of image)
                                             /// or the distance between sampled points on a line from XML
} tParams;


/**
 * A function to convert ERendererType enum to string
 */
static const char *renToStr(ERendererType type)
{
  static const char *strings[] = {
    "Simple",
    "OpenCL",
    "None"
  };

  if ((type < REN_SIMPLE) || (type > REN_NONE))
  {
    return "Unknown";
  }

  return strings[type];
}


/**
 * A function to print command line argument (for debugging purposes)
 */
std::ostream & operator<<(std::ostream & os, const tParams & params)
{
  os << "input file                : " << ((params.ifilename == NULL) ? "NULL" : params.ifilename) << std::endl;
  os << "output file               : " << ((params.ofilename == NULL) ? "NULL" : params.ofilename) << std::endl;
  os << "optical field n. rows     : " << params.of_rows                                           << std::endl;
  os << "optical field n. cols     : " << params.of_cols                                           << std::endl;
  os << "sampling                  : " << params.sampling                                          << std::endl;
  os << "light wavelength          : " << params.lambda                                            << std::endl;
  os << "hologram z                : " << params.hologram_z                                        << std::endl;
  os << "Rendering engine          : " << renToStr(params.renderer)                                << std::endl;
  os << "Rendering algorithm       : " << COpenCLRenderer::algToStr(params.alg_type)               << std::endl;
  os << "Chunk size                : " << params.chunk_size                                        << std::endl;
  os << "OpenCL source file        : " << ((params.cl_source == NULL) ? "NULL" : params.cl_source) << std::endl;
  os << "Performance log file      : " << ((params.perf_log == NULL) ?  "NULL" : params.perf_log)  << std::endl;
  os << "Point cloud sampling step : " << params.pc_sampling_step                                  << std::endl;

  return os;
}


/**
 * A method to reset command line arguments to their default values
 */
static void resetParams(tParams *params)
{
  HOLOREN_ASSERT(params != NULL);

  params->ifilename = NULL;
  params->ofilename = NULL;
  params->of_rows = 200;
  params->of_cols = 200;
  params->sampling = 20e-6;
  params->lambda = 630e-9;
  params->hologram_z = 0.0f;
  params->renderer = REN_SIMPLE;
  params->alg_type = COpenCLRenderer::ALGORITHM_MULTIPASS_NATIVE;
  params->chunk_size = 0;    // 0 means unspecified
  params->cl_source = NULL;
  params->perf_log = NULL;   // no logging by default
  params->pc_sampling_step = 0.000025;

  return;
}


/**
 * A function to parse command line arguments
 */
static bool parseArgs(int argc, char *argv[], tParams *params)
{
  HOLOREN_ASSERT(params != NULL);

  #define CMP_SHORT_OPT(arg, opt) \
    (((arg)[0] == '-') && ((arg)[1] == (opt)) && ((arg)[2] == 0))

  /* reset arguments */
  resetParams(params);

  /* run through the arguments and process them */
  int i = 1;
  while (i < argc)
  {
    if (CMP_SHORT_OPT(argv[i], 'i'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -i requires an argument" << std::endl;
        return false;
      }

      if (params->ifilename != NULL)
      {
        std::cerr << "Input file already given" << std::endl;
        return false;
      }

      params->ifilename = argv[i];
    }
    else if (CMP_SHORT_OPT(argv[i], 'o'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -o requires an argument" << std::endl;
        return false;
      }

      if (params->ofilename != NULL)
      {
        std::cerr << "Output file already given" << std::endl;
        return false;
      }

      params->ofilename = argv[i];
    }
    else if (CMP_SHORT_OPT(argv[i], 'r'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -r requires an argument" << std::endl;
        return false;
      }

      if (Utils::strCaseCmp(argv[i], "simple") == 0)
      {
        params->renderer = REN_SIMPLE;
      }
      else if (Utils::strCaseCmp(argv[i], "opencl") == 0)
      {
        params->renderer = REN_OPENCL;
      }
      else if (Utils::strCaseCmp(argv[i], "none") == 0)
      {
        params->renderer = REN_NONE;
      }
      else
      {
        std::cerr << "Uknown rendering engine: \"" << argv[i] << "\"" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 'a'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -a requires an argument" << std::endl;
        return false;
      }

      if (Utils::strCaseCmp(argv[i], "SinglePass") == 0)
      {
        params->alg_type = COpenCLRenderer::ALGORITHM_SINGLEPASS;
      }
      else if (Utils::strCaseCmp(argv[i], "MultiPass") == 0)
      {
        params->alg_type = COpenCLRenderer::ALGORITHM_MULTIPASS;
      }
      else if (Utils::strCaseCmp(argv[i], "MultiPassCPU") == 0)
      {
        params->alg_type = COpenCLRenderer::ALGORITHM_MULTIPASS_CPU;
      }
      else if (Utils::strCaseCmp(argv[i], "MultiPassNative") == 0)
      {
        params->alg_type = COpenCLRenderer::ALGORITHM_MULTIPASS_NATIVE;
      }
      else if (Utils::strCaseCmp(argv[i], "MultiPassAligned") == 0)
      {
        params->alg_type = COpenCLRenderer::ALGORITHM_MULTIPASS_ALIGNED;
      }
      else
      {
        std::cerr << "Unknown rendering algorithm: \"" << argv[i] << "\"" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 'c'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -c requires an argument" << std::endl;
        return false;
      }

      if (!Utils::strToULong(argv[i], &params->chunk_size))
      {
        std::cerr << "Failed to convert argument " << argv[i] << " of option -c to number" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 'h'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -h requires an argument" << std::endl;
        return false;
      }

      if (!Utils::strToUInt(argv[i], &params->of_rows))
      {
        std::cerr << "Failed to convert argument " << argv[i] << " of option -h to number" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 'w'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -w requires an argument" << std::endl;
        return false;
      }

      if (!Utils::strToUInt(argv[i], &params->of_cols))
      {
        std::cerr << "Failed to convert argument " << argv[i] << " of option -w to number" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 's'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -s requires an argument" << std::endl;
        return false;
      }

      if (!Utils::strToDbl(argv[i], &params->sampling))
      {
        std::cerr << "Failed to convert argument " << argv[i] << " of option -s to number" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 'l'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -l requires an argument" << std::endl;
        return false;
      }

      if (!Utils::strToDbl(argv[i], &params->lambda))
      {
        std::cerr << "Failed to convert argument " << argv[i] << " of option -l to number" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 'z'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -z requires an argument" << std::endl;
        return false;
      }

      if (!Utils::strToDbl(argv[i], &params->hologram_z))
      {
        std::cerr << "Failed to convert argument " << argv[i] << " of option -z to number" << std::endl;
        return false;
      }
    }
    else if (CMP_SHORT_OPT(argv[i], 'f'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -f requires an argument" << std::endl;
        return false;
      }

      if (params->cl_source != NULL)
      {
        std::cerr << "OpenCL source file already given" << std::endl;
        return false;
      }

      params->cl_source = argv[i];
    }
    else if (CMP_SHORT_OPT(argv[i], 'p'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -p requires an argument" << std::endl;
        return false;
      }

      if (params->perf_log != NULL)
      {
        std::cerr << "Performance log file already given" << std::endl;
        return false;
      }

      params->perf_log = argv[i];
    }
    else if (CMP_SHORT_OPT(argv[i], 't'))
    {
      if (++i >= argc)
      {
        std::cerr << "Option -t requires an argument" << std::endl;
        return false;
      }

      if (!Utils::strToDbl(argv[i], &params->pc_sampling_step))
      {
        std::cerr << "Failed to convert argument " << argv[i] << " of option -t to number" << std::endl;
        return false;
      }
    }
    else
    {
      std::cerr << "Unrecognized option: " << argv[i] << std::endl;
      return false;
    }

    ++i;
  }

  /* check on required arguments */
  if (params->ifilename == NULL)
  {
    std::cerr << "Missing input file name" << std::endl;
    return false;
  }

  if (params->ofilename == NULL)
  {
    std::cerr << "Missing output file name" << std::endl;
    return false;
  }

  #undef CMP_SHORT_OPT

  return true;
}


/**
 * A debug function to display current platform data type sizes
 */
#ifdef HOLOREN_DEBUG
static void testPlatform(void)
{
  DBG("Platform sizes:");
  DBG("sizeof(tFPType) == " << sizeof(tFPType) << " B");
  DBG("sizeof(COpticalField::CComplex) == " << sizeof(COpticalField::CComplex) << " B");
  DBG("sizeof(SPointSource) == " << sizeof(SPointSource) << " B");
  DBG("");

  return;
}
#else
#  define testPlatform()  // else undefine the function
#endif


/**
 * A function to initialize a correct renderer
 */
static CBaseRenderer *createRenderer(const tParams & params)
{
  switch (params.renderer)
  {
    case REN_SIMPLE:
      {
        std::cout << "Renderer engine: Simple" << std::endl;
        return new CSimpleRenderer(params.hologram_z);
      }
      break;

    case REN_OPENCL:
      {
        std::cout << "Renderer engine: OpenCL" << std::endl;
        std::cout << "Rendering algorithm: " << COpenCLRenderer::algToStr(params.alg_type) << std::endl;
        COpenCLRenderer *ren = new COpenCLRenderer(params.hologram_z);
        ren->setAlgorithmType(params.alg_type);
        ren->setChunkSize(params.chunk_size);
        return ren;
      }
      break;

    case REN_NONE:
      std::cout << "Renderer engine: None" << std::endl;
      return NULL;
  }

  return NULL;
}


/**
 * A function to log information about application's performance
 */
static void writePerfLog(const tParams & params, const CPointCloud & pc, const COpticalField & of, const CBaseRenderer *ren)
{
  /* check if creating a log was actually requested and
     that the input parameters are valid */
  if ((params.perf_log == NULL) || (ren == NULL))
  {
    return;
  }

  std::cout << "Writing performance log" << std::endl;
  
  std::ofstream perf_log(params.perf_log, std::ofstream::app);
  if (!perf_log)
  {
    std::cerr << "Failed to create performance log" << std::endl;
    return;
  }

  perf_log << "================================================================================" << std::endl;
  perf_log << "date                        : "   << Utils::fmtDateTime("%H:%M:%S %d.%m.%Y", time(NULL)) << std::endl;
#ifdef HOLOREN_DEBUG
  perf_log << "build type                  : debug" << std::endl;
#else
  perf_log << "build type                  : release" << std::endl;
#endif
  perf_log << "input file                  : \"" << params.ifilename << '\"'                     << std::endl;
  perf_log << "point sources               : "   << pc.size()                                    << std::endl;
  perf_log << "optical field (rows x cols) : "   << of.getNumRows()  << 'x' << of.getNumCols()   << std::endl;

  if (params.renderer == REN_OPENCL)
  {
    const COpenCLRenderer *ocl_ren = static_cast<const COpenCLRenderer *>(ren);
    perf_log << "algorithm                   : " << COpenCLRenderer::algToStr(params.alg_type)   << std::endl;
    perf_log << "chunk size                  : " << ocl_ren->getChunkSize()      << std::endl;
  }

  perf_log << "rendering time              : " << g_timer                                        << std::endl;
  perf_log << "================================================================================" << std::endl;

  perf_log.close();

  return;
}


/////////////////////
//// Main Program
int main(int argc, char *argv[])
{
  tParams params;
  if (!parseArgs(argc, argv, &params))
  {
    std::cerr << "Failed to parse command line arguments" << std::endl;
    std::cerr << g_usage << std::endl;
    return 1;
  }

  DBG(params);

  testPlatform();

  DBG_T_START(g_timer, "** Starting point cloud conversion:\n");

  /* instantiate the class that can convert the given file to point cloud */
  CBasePointCloudRW *rw = g_pcrw_factory.getReaderWriter(params.ifilename);
  if (rw == NULL)
  {
    std::cerr << "The conversion from given file type to point cloud is not supported" << std::endl;
    return 1;
  }

  //rw->setSamplingStep(10e-4);
  rw->setSamplingStep(params.pc_sampling_step);

  /* convert to point cloud from a given file type */
  CPointCloud pc;
  if (!rw->read(&pc))
  {
    std::cerr << "Failed to convert the file \""
              << params.ifilename
              << "\" to point cloud: "
              << rw->getErrorMsg()
              << std::endl;
    delete rw;
    return 1;
  }

  delete rw;

  DBG_T_END(g_timer, "** Point cloud conversion finished: ");

  std::cout << "Point cloud was loaded successfully" << std::endl;
  std::cout << "Total number of point sources that have to be rendered: " << pc.size() << std::endl;
#ifdef HOLOREN_HEAVY_DEBUG
  DBG("Loaded point sources:");
  DBG(pc);
#endif

  /* set up renderer */
  std::cout << "Setting up renderer" << std::endl;

  DBG_T_START(g_timer, "** Starting renderer setup:\n");

  CBaseRenderer *p_ren = createRenderer(params);
  if (p_ren == NULL)
  {
    std::cerr << "Failed to create renderer. No rendering done." << std::endl;
    return 1;
  }

  COpticalField of(params.of_rows, params.of_cols, params.lambda, params.sampling);

  if (!p_ren->open(params.cl_source))
  {
    std::cerr << "Failed to initialize " << renToStr(params.renderer) << " renderer" << std::endl;
    std::cerr << p_ren->getError() << std::endl;
    delete p_ren;
    return 1;
  }
  
  DBG_T_END(g_timer, "** Finished renderer setup: ");
  
  /* render the object wave from scene */
  std::cout << "Rendering object wave" << std::endl;
  
  DBG_T_START(g_timer, "** Starting rendering:\n");

  if (!p_ren->renderObjectWave(pc, &of))
  {
    std::cerr << "Failed to render object wave: " << p_ren->getError() << std::endl;
    p_ren->close();
    delete p_ren;
    return 1;
  }
  
  DBG_T_END(g_timer, "** Rendering finished: ");

  /* create a performance log before closing the renderer */
  writePerfLog(params, pc, of, p_ren);

  p_ren->close();

  delete p_ren;

  std::cout << "Point cloud rendered successfully" << std::endl;

  DBG_T_START(g_timer, "** Starting saving:\n");

  /* try to guess the name of the output file if not given */
  std::string out_file(params.ofilename);
  if (Utils::getFileName(out_file) == NULL)
  {
    const char *filename = Utils::getFileName(params.ifilename);
    HOLOREN_ASSERT(filename != NULL);
    out_file += filename;
  }
  
  /* save computed results */
  if (!of.save(params.ofilename))
  {
    std::cerr << "Failed to save the computed results" << std::endl;
    return 1;
  }

  DBG_T_END(g_timer, "** Saving finished: ");

  std::cout << "Point cloud saved successfully to \"" << params.ofilename << "\"" << std::endl;

  return 0;
}
