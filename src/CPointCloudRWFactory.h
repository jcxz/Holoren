/**
 * Point cloud reader-writer creation functionality
 */

#ifndef CPOINTCLOUDRWFACTORY_H
#define CPOINTCLOUDRWFACTORY_H

#include "CBasePointCloudRW.h"


/**
 * A class that takes care of instantiation
 * and creation of point cloud readers/writers
 */
class CPointCloudRWFactory
{
  public:
    /* known reader-writer types */
    enum EReaderWriterType {
      RW_XML   = 0,    /// xml file
      RW_IMAGE = 1,    /// image file
      RW_PC    = 2,    /// binary point cloud file
      RW_NONE  = 3     /// no default reader-writer
    };


  public:
    /**
     * A method to obtain factory instance
     */
    static CPointCloudRWFactory & getInstance(void)
    {
      return m_instance;
    }

    /**
     * A method to create a reader
     */
    CBasePointCloudRW *getReaderWriter(const std::string & filename, EReaderWriterType type = RW_NONE);

  private:
    static const int PC_RW_CNT = RW_NONE + 1;

  private:
    /* Factory instance */
    static CPointCloudRWFactory m_instance;

  private:
    /**
     * Default constructor
     */
    CPointCloudRWFactory(void);

    /**
     * Copy constructor
     */
    CPointCloudRWFactory(const CPointCloudRWFactory & other);

    /**
     * Destructor
     */
    ~CPointCloudRWFactory(void);

    /**
     * Assignment operator
     */
    CPointCloudRWFactory & operator=(const CPointCloudRWFactory & other);

  private:
    struct SReaderWriter
    {
      CBasePointCloudRW::tCleanUpHandler cleanup_fn;  /// a pointer to clean-up handler
      bool created;                                   /// whether this reader-writer was already created
    };

  private:
    SReaderWriter m_rws[PC_RW_CNT];  /// an array of readers-writer types that were already constructed
};


/** A macro to get the point cloud reader-writer factory instance */
#define g_pcrw_factory CPointCloudRWFactory::getInstance()

#endif
