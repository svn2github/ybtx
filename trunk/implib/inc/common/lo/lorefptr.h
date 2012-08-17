#ifndef __lo_REFPTR_H__
#define __lo_REFPTR_H__

#include <common/lo/lodefine.h>

DEFINE_NAMESPACE(locom)

/** CloRefPtrT<> is a reference-counting shared smartpointer.
 *
 * Reference counting means that a shared reference count is incremented each
 * time a CloRefPtrT is copied, and decremented each time a CloRefPtrT is destroyed,
 * for instance when it leaves its scope. When the reference count reaches
 * zero, the contained object is deleted
 *
 * cairomm uses CloRefPtrT so that you don't need to remember
 * to delete the object explicitly, or know when a method expects you to delete 
 * the object that it returns, and to prevent any need to manually  reference 
 * and unreference() cairo objects.
 */
template <class T_CppObject>
class CloRefPtrT
{
public:
  /** Default constructor
   *
   * Afterwards it will be null and use of -> will cause a segmentation fault.
   */
  inline CloRefPtrT();
  
  /// Destructor - decrements reference count.
  inline ~CloRefPtrT();

  /** For use only in the internal implementation of cairomm, gtkmm, etc.
   *
   * This takes ownership of @a pCppObject, so it will be deleted when the 
   * last CloRefPtrT is deleted, for instance when it goes out of scope.
   *
   * This assumes that @a pCppObject already has a starting reference for its underlying cairo object,
   * so that destruction of @a @pCppObject will cause a corresponding unreference of its underlying 
   * cairo object. For instance, a cairo_*_create() function usually provides a starting reference, 
   * but a cairo_*_get_*() function requires the caller to manually reference the returned object.
   * In this case, you should call reference() on @a pCppObject before passing it to this constructor.
   */
  explicit inline CloRefPtrT(T_CppObject* pCppObject);

  ///  For use only in the internal implementation of sharedptr.
  explicit inline CloRefPtrT(T_CppObject* pCppObject, int* refcount);

  /** Copy constructor
   *
   * This increments the shared reference count.
   */
  inline CloRefPtrT(const CloRefPtrT<T_CppObject>& src);

  /** Copy constructor (from different, but castable type).
   *
   * Increments the reference count.
   */
  template <class T_CastFrom>
  inline CloRefPtrT(const CloRefPtrT<T_CastFrom>& src);

  /** Swap the contents of two CloRefPtrT<>.
   * This method swaps the internal pointers to T_CppObject.  This can be
   * done safely without involving a reference/unreference cycle and is
   * therefore highly efficient.
   */
  inline void swap(CloRefPtrT<T_CppObject>& other);

  /// Copy from another CloRefPtrT:
  inline CloRefPtrT<T_CppObject>& operator=(const CloRefPtrT<T_CppObject>& src);

  /** Copy from different, but castable type).
   *
   * Increments the reference count.
   */
  template <class T_CastFrom>
  inline CloRefPtrT<T_CppObject>& operator=(const CloRefPtrT<T_CastFrom>& src);

  /// Tests whether the CloRefPtrT<> point to the same underlying instance.
  inline bool operator==(const CloRefPtrT<T_CppObject>& src) const;
  
  /// See operator==().
  inline bool operator!=(const CloRefPtrT<T_CppObject>& src) const;

  /** Dereferencing.
   *
   * Use the methods of the underlying instance like so:
   * <code>refptr->memberfun()</code>.
   */
  inline T_CppObject* operator->() const;

  /** Test whether the CloRefPtrT<> points to any underlying instance.
   *
   * Mimics usage of ordinary pointers:
   * @code
   *   if (ptr)
   *     do_something();
   * @endcode
   */
  inline operator bool() const;

  /// Set underlying instance to 0, decrementing reference count of existing instance appropriately.
  inline void clear();


  /** Dynamic cast to derived class.
   *
   * The CloRefPtrT can't be cast with the usual notation so instead you can use
   * @code
   *   ptr_derived = CloRefPtrT<Derived>::cast_dynamic(ptr_base);
   * @endcode
   */
  template <class T_CastFrom>
  static inline CloRefPtrT<T_CppObject> cast_dynamic(const CloRefPtrT<T_CastFrom>& src);

  /** Static cast to derived class.
   *
   * Like the dynamic cast; the notation is 
   * @code
   *   ptr_derived = CloRefPtrT<Derived>::cast_static(ptr_base);
   * @endcode
   */
  template <class T_CastFrom>
  static inline CloRefPtrT<T_CppObject> cast_static(const CloRefPtrT<T_CastFrom>& src);

  /** Cast to non-const.
   *
   * The CloRefPtrT can't be cast with the usual notation so instead you can use
   * @code
   *   ptr_unconst = CloRefPtrT<UnConstType>::cast_const(ptr_const);
   * @endcode
   */
  template <class T_CastFrom>
  static inline CloRefPtrT<T_CppObject> cast_const(const CloRefPtrT<T_CastFrom>& src);


  // Warning: This is for internal use only.  Do not manually modify the
  // reference count with this pointer.
  inline int* refcount_() const { return pCppRefcount_; }

private:
  void unref();

  T_CppObject* pCppObject_;
  mutable int* pCppRefcount_;
};


// CloRefPtrT<>::operator->() comes first here since it's used by other methods.
// If it would come after them it wouldn't be inlined.

template <class T_CppObject> inline
T_CppObject* CloRefPtrT<T_CppObject>::operator->() const
{
  return pCppObject_;
}

template <class T_CppObject> inline
CloRefPtrT<T_CppObject>::CloRefPtrT()
:
  pCppObject_(0),
  pCppRefcount_(0)
{}

template <class T_CppObject> inline
CloRefPtrT<T_CppObject>::~CloRefPtrT()
{
  unref();
}

template <class T_CppObject> inline
void CloRefPtrT<T_CppObject>::unref()
{
  if(pCppRefcount_)
  {
    --(*pCppRefcount_);

    if(*pCppRefcount_ == 0)
    {
      if(pCppObject_)
      {
        delete pCppObject_;
        pCppObject_ = 0;
      }

      delete pCppRefcount_;
      pCppRefcount_ = 0;
    }
  }
}


template <class T_CppObject> inline
CloRefPtrT<T_CppObject>::CloRefPtrT(T_CppObject* pCppObject)
:
  pCppObject_(pCppObject),
  pCppRefcount_(0)
{
  if(pCppObject)
  {
    pCppRefcount_ = new int;
    *pCppRefcount_ = 1; //This will be decremented in the destructor.
  }
}

//Used by cast_*() implementations:
template <class T_CppObject> inline
CloRefPtrT<T_CppObject>::CloRefPtrT(T_CppObject* pCppObject, int* refcount)
:
  pCppObject_(pCppObject),
  pCppRefcount_(refcount)
{
  if(pCppObject_ && pCppRefcount_)
    ++(*pCppRefcount_);
}

template <class T_CppObject> inline
CloRefPtrT<T_CppObject>::CloRefPtrT(const CloRefPtrT<T_CppObject>& src)
:
  pCppObject_ (src.pCppObject_),
  pCppRefcount_(src.pCppRefcount_)
{
  if(pCppObject_ && pCppRefcount_)
    ++(*pCppRefcount_);
}

// The templated ctor allows copy construction from any object that's
// castable.  Thus, it does downcasts:
//   base_ref = derived_ref
template <class T_CppObject>
  template <class T_CastFrom>
inline
CloRefPtrT<T_CppObject>::CloRefPtrT(const CloRefPtrT<T_CastFrom>& src)
:
  // A different CloRefPtrT<> will not allow us access to pCppObject_.  We need
  // to add a get_underlying() for this, but that would encourage incorrect
  // use, so we use the less well-known operator->() accessor:
  pCppObject_ (src.operator->()),
  pCppRefcount_(src.refcount_())
{
  if(pCppObject_ && pCppRefcount_)
    ++(*pCppRefcount_);
}

template <class T_CppObject> inline
void CloRefPtrT<T_CppObject>::swap(CloRefPtrT<T_CppObject>& other)
{
  T_CppObject *const temp = pCppObject_;
  int* temp_count = pCppRefcount_; 

  pCppObject_ = other.pCppObject_;
  pCppRefcount_ = other.pCppRefcount_;

  other.pCppObject_ = temp;
  other.pCppRefcount_ = temp_count;
}

template <class T_CppObject> inline
CloRefPtrT<T_CppObject>& CloRefPtrT<T_CppObject>::operator=(const CloRefPtrT<T_CppObject>& src)
{
  // In case you haven't seen the swap() technique to implement copy
  // assignment before, here's what it does:
  //
  // 1) Create a temporary CloRefPtrT<> instance via the copy ctor, thereby
  //    increasing the reference count of the source object.
  //
  // 2) Swap the internal object pointers of *this and the temporary
  //    CloRefPtrT<>.  After this step, *this already contains the new pointer,
  //    and the old pointer is now managed by temp.
  //
  // 3) The destructor of temp is executed, thereby unreferencing the
  //    old object pointer.
  //
  // This technique is described in Herb Sutter's "Exceptional C++", and
  // has a number of advantages over conventional approaches:
  //
  // - Code reuse by calling the copy ctor.
  // - Strong exception safety for free.
  // - Self assignment is handled implicitely.
  // - Simplicity.
  // - It just works and is hard to get wrong; i.e. you can use it without
  //   even thinking about it to implement copy assignment whereever the
  //   object data is managed indirectly via a pointer, which is very common.

  CloRefPtrT<T_CppObject> temp (src);
  this->swap(temp);
  return *this;
}

template <class T_CppObject>
  template <class T_CastFrom>
inline
CloRefPtrT<T_CppObject>& CloRefPtrT<T_CppObject>::operator=(const CloRefPtrT<T_CastFrom>& src)
{
  CloRefPtrT<T_CppObject> temp (src);
  this->swap(temp);
  return *this;
}

template <class T_CppObject> inline
bool CloRefPtrT<T_CppObject>::operator==(const CloRefPtrT<T_CppObject>& src) const
{
  return (pCppObject_ == src.pCppObject_);
}

template <class T_CppObject> inline
bool CloRefPtrT<T_CppObject>::operator!=(const CloRefPtrT<T_CppObject>& src) const
{
  return (pCppObject_ != src.pCppObject_);
}

template <class T_CppObject> inline
CloRefPtrT<T_CppObject>::operator bool() const
{
  return (pCppObject_ != 0);
}

template <class T_CppObject> inline
void CloRefPtrT<T_CppObject>::clear()
{
  CloRefPtrT<T_CppObject> temp; // swap with an empty CloRefPtrT<> to clear *this
  this->swap(temp);
}

template <class T_CppObject>
  template <class T_CastFrom>
inline
CloRefPtrT<T_CppObject> CloRefPtrT<T_CppObject>::cast_dynamic(const CloRefPtrT<T_CastFrom>& src)
{
  T_CppObject *const pCppObject = dynamic_cast<T_CppObject*>(src.operator->());

  if(pCppObject) //Check whether dynamic_cast<> succeeded so we don't pass a null object with a used refcount:
    return CloRefPtrT<T_CppObject>(pCppObject, src.refcount_());
  else
    return CloRefPtrT<T_CppObject>();
}

template <class T_CppObject>
  template <class T_CastFrom>
inline
CloRefPtrT<T_CppObject> CloRefPtrT<T_CppObject>::cast_static(const CloRefPtrT<T_CastFrom>& src)
{
  T_CppObject *const pCppObject = static_cast<T_CppObject*>(src.operator->());

  return CloRefPtrT<T_CppObject>(pCppObject, src.refcount_());
}

template <class T_CppObject>
  template <class T_CastFrom>
inline
CloRefPtrT<T_CppObject> CloRefPtrT<T_CppObject>::cast_const(const CloRefPtrT<T_CastFrom>& src)
{
  T_CppObject *const pCppObject = const_cast<T_CppObject*>(src.operator->());

  return CloRefPtrT<T_CppObject>(pCppObject, src.refcount_());
}

/** @relates Glib::CloRefPtrT */
template <class T_CppObject> inline
void swap(CloRefPtrT<T_CppObject>& lhs, CloRefPtrT<T_CppObject>& rhs)
{
  lhs.swap(rhs);
}

END_NAMESPACE(locom)

#endif /* __lo_REFPTR_H__ */
