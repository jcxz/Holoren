/**
 * CSharedDataPtr and CSharedData implementation and interface
 */

#ifndef CSHAREDDATAPTR_H
#define CSHAREDDATAPTR_H

/**
 * The class representing a base class for all
 * classes to be used with shared data pointer
 */
class CSharedData
{
  private:
    mutable unsigned int m_ref_cnt;  /// reference count

  private:
    /*
     * we only want to prevent reference count from being copied
     * and because this class contains only the reference count
     * it does not make sense to copy it
     */
    CSharedData & operator=(const CSharedData &);

  public:
    /**
     * Default constructor
     */
    CSharedData(void)
      : m_ref_cnt(0)
    {
    }

    /**
     * Copy constructor
     */
    CSharedData(const CSharedData & )
      : m_ref_cnt(0)   // when I make a copy of shared data, then it is a new
                       // object and basically nobody points to it yet, so that
                       // is the reason to setting the reference count to 0
    {
    }

    template<class T>
    friend class CSharedDataPtr;
};


/**
 * A pointer to shared data class
 * It encapsulates all operations with shared classes.
 */
template <class T>
class CSharedDataPtr
{
  private:
    T *m_ptr;

  public:
    explicit inline CSharedDataPtr(T *ptr = 0)
      : m_ptr(ptr)
    {
      if (m_ptr) m_ptr->m_ref_cnt++;
    }

    inline CSharedDataPtr(const CSharedDataPtr & other)
      : m_ptr(other.m_ptr)
    {
      if (m_ptr) m_ptr->m_ref_cnt++;
    }

    inline ~CSharedDataPtr(void)
    {
      detach();
    }

    /**
     * This method returns a constant reference to data
     * pointed to by this pointer.
     * This method might be usefull, when used for instance
     * in a function, which takes this class as a non-constant
     * parameter and needs to access the underlying data for
     * reading without having to pay the penalty of detaching
     * from them first.
     * 
     */
    inline const T & constData(void) const
    {
      return *m_ptr;
    }

    inline CSharedDataPtr & operator=(const CSharedDataPtr & other)
    {
      T *const old = m_ptr;

      m_ptr = other.m_ptr;

      /* increasing the reference count and then decreasing it,
         prevents from self assignment */
      if (m_ptr) m_ptr->m_ref_cnt++;

      /* having (--old->m_ref_cnt < 1) here probably would not be a good idea.
         What if a CSharedData subclass had a CSharedDataPtr as a member pointing
         to this CSharedDataPtr ? Then there would be a double free */
      if ((old) && (--old->m_ref_cnt == 0)) delete old;

      return *this;
    }

    inline CSharedDataPtr & operator=(T *ptr)
    {
     T *const old = m_ptr;

      m_ptr = ptr;
      if (m_ptr) m_ptr->m_ref_cnt++;
      if ((old) && (--old->m_ref_cnt == 0)) delete old;

      return *this;
    }

    inline T & operator*(void)
    {
      copy();
      return *m_ptr;
    }

    inline const T & operator*(void) const
    {
      return *m_ptr;
    }

    inline T *operator->(void)
    {
      copy();
      return m_ptr;
    }

    inline const T *operator->(void) const
    {
      return m_ptr;
    }

    /**
     * A function to detach from old data and to free them if necessary
     */
    inline void detach(void)
    {
      if ((m_ptr) && (--m_ptr->m_ref_cnt == 0))
      {
        delete m_ptr;
      }

      return;
    }

    /**
     * Detach from data previously pointed to if necessary
     */
    inline void copy(void)
    {
      if ((m_ptr) && (m_ptr->m_ref_cnt > 1))
      {
        DBG("CSharedDataPtr: making a copy of data");

        // make a copy of the data
        // (do this first in case new would have thrown an exception)
        T *tmp = new T(*m_ptr);
 
        // reset the reference count
        tmp->m_ref_cnt = 1;

        // decrease the old reference counter
        m_ptr->m_ref_cnt--;
 
        // start pointing to new data
        m_ptr = tmp;
      }

      return;
    }
};

#endif
