// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_ATOMICPOINTER_VFHEADER
#define VF_ATOMICPOINTER_VFHEADER

template <class P>
class AtomicPointer
{
public:
  explicit AtomicPointer (P* const p = nullptr) noexcept
	: m_value (p)
  {
  }

  inline P* get () const noexcept
  {
	return m_value.get();
  }

  inline operator P* () const noexcept
  {
	return get ();
  }

  inline operator P& () const noexcept
  {
	return &get();
  }

  inline P* operator-> () const noexcept
  {
	return get();
  }

  inline void set (P* p)
  {
	m_value.set (p);
  }

  inline void operator= (P* p) noexcept
  {
	set (p);
  }

  inline void operator= (P& p) noexcept
  {
	set (&p);
  }

  // returns the previous value
  inline P* exchange (P* p)
  {
	return m_value.exchange (p);
  }

  inline bool compareAndSet (P* newValue, P* oldValue)
  {
    return m_value.compareAndSetBool (newValue, oldValue);
  }

private:
  VF_JUCE::Atomic <P*> m_value;
};

#endif
