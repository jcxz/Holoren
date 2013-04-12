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



/** a usage message */
static const char * const g_usage = "USAGE:\n"
                                    "./holoren -i input_file\n"
                                    "          -o output_file\n"
                                    "          -h optical_field_num_rows\n"
                                    "          -w optical_field_num_cols\n"
                                    "          -s sampling\n"
                                    "          -l lambda\n"
                                    "          -z hologram_z\n"
                                    "          -r simple|opencl|none\n"
                                    "          -f cl_source_file\n";

/** a timer to meassure exection time of different sections of code */
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
  const char *ifilename;   /// input file
  const char *ofilename;   /// output file
  unsigned int of_rows;    /// optical field number of rows
  unsigned int of_cols;    /// optical field number of columns
  const char *cl_source;   /// the location of OpenCL source file
  ERendererType renderer;  /// the type of rendering engine
  double sampling;         /// pitch between individual samples
  double lambda;           /// light wavelength
  double hologram_z;       /// hologram placement
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
  os << "input file            : " << ((params.ifilename == NULL) ? "NULL" : params.ifilename) << std::endl;
  os << "output file           : " << ((params.ofilename == NULL) ? "NULL" : params.ofilename) << std::endl;
  os << "optical field n. rows : " << params.of_rows                                           << std::endl;
  os << "optical field n. cols : " << params.of_cols                                           << std::endl;
  os << "sampling              : " << params.sampling                                          << std::endl;
  os << "light wavelength      : " << params.lambda                                            << std::endl;
  os << "hologram z            : " << params.hologram_z                                        << std::endl;
  os << "Rendering engine      : " << renToStr(params.renderer)                                << std::endl;
  os << "OpenCL source file    : " << ((params.cl_source == NULL) ? "NULL" : params.cl_source) << std::endl;

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
  params->cl_source = NULL;

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

  DBG_T_START(g_timer, "** Starting point cloud conversion:\n");

  /* instantiate the class that can convert the given file to point cloud */
  CBasePointCloudRW *rw = g_pcrw_factory.getReaderWriter(params.ifilename);
  if (rw == NULL)
  {
    std::cerr << "The conversion from given file type to point cloud is not supported" << std::endl;
    return 1;
  }

  rw->setStep(10e-4);

  /* convert to point cloud from a given file type */
  CPointCloud pc;
  if (!rw->read(&pc))
  {
    std::cerr << "Failed to convert the file "
              << params.ifilename
              << "the point cloud: "
              << rw->getErrorMsg()
              << std::endl;
    delete rw;
    return 1;
  }

  delete rw;

  DBG_T_END(g_timer, "** Point cloud conversion finished: ");

  std::cout << "Point cloud was loaded successfully" << std::endl;
  std::cout << "Total number of point sources that have to be rendered: " << pc.size() << std::endl;

  /* set up renderer */
  std::cout << "Setting up renderer" << std::endl;

  CBaseRenderer *p_ren = NULL;

  switch (params.renderer)
  {
    case REN_SIMPLE:
      p_ren = new CSimpleRenderer(params.hologram_z);
      std::cout << "Renderer engine: Simple" << std::endl;
      break;

    case REN_OPENCL:
      p_ren = new COpenCLRenderer(params.hologram_z);
      std::cout << "Renderer engine: OpenCL" << std::endl;
      break;

    case REN_NONE:
      std::cout << "Renderer engine: None" << std::endl;
      std::cout << "No rendering done" << std::endl;
      return 0;
  }

  /* render the object wave from scene */
  std::cout << "Rendering object wave" << std::endl;
  
  DBG_T_START(g_timer, "** Starting rendering:\n");

  COpticalField of(params.of_rows, params.of_cols, params.lambda, params.sampling);

  if (!p_ren->open(params.cl_source))
  {
    std::cerr << "Failed to initialize " << renToStr(params.renderer) << " renderer" << std::endl;
    std::cerr << p_ren->getError() << std::endl;
    delete p_ren;
    return false;
  }

  p_ren->renderObjectWave(pc, &of);

  p_ren->close();

  delete p_ren;

  DBG_T_END(g_timer, "** Rendering finished: ");

  std::cout << "Point cloud rendered successfully" << std::endl;

  DBG_T_START(g_timer, "** Starting saving:\n");

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
