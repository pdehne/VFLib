// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide..

#ifndef VF_PIPE_H
#define VF_PIPE_H

//
// JUCE DOES THIS ALREADY SO DONT USE THIS
//

namespace Pipe {

//--------------------------------------------------------------------------

class In : Uncopyable
{
public:
  virtual Error Read( void* dest, size_t destBytes )=0;

public:
  // read in native endianness

  template<typename Ty>
  Error ReadBlob( Ty* pValue )
  {
    return Read( pValue, sizeof(*pValue) );
  }
};

//--------------------------------------------------------------------------

class Out : Uncopyable
{
public:
  virtual Error Write( const void* src, size_t srcBytes )=0;

public:
  // write in native endianness

  template<typename Ty>
  Error WriteBlob( const Ty& value )
  {
    return Write( &value, sizeof(value) );
  }

};

//--------------------------------------------------------------------------

class MemOut : public Out
{
public:
  MemOut( void* buffer, size_t bufferBytes );

  virtual Error Write( const void* src, size_t srcBytes );

  uint8* 	GetBuffer();
  size_t	GetBytesRemaining();

private:
  uint8* 	m_buffer;
  size_t	m_remain;
};

//--------------------------------------------------------------------------

class MemIn : public In
{
public:
  MemIn( const void* buffer, size_t bufferBytes );

  virtual Error Read( void* dest, size_t destBytes );

  const uint8* GetBuffer();
  size_t	GetBytesRemaining();

private:
  const uint8* m_buffer;
  size_t	m_remain;
};

}

#endif
