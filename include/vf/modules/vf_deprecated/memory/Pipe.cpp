// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide..

namespace Pipe {

//******************************************************************************

MemOut::MemOut( void* buffer, size_t bufferBytes )
{
  m_buffer=static_cast<uint8*>(buffer);
  m_remain=bufferBytes;
}

//------------------------------------------------------------------------------

Error MemOut::Write( const void* src, size_t srcBytes )
{
  vf::Error error;

  if( m_remain>=srcBytes )
  {
    ::memcpy( m_buffer, src, srcBytes );
    m_buffer+=srcBytes;
    m_remain-=srcBytes;
  }
  else
  {
    error.fail( __FILE__, __LINE__, Error::fileNoSpace );
  }

  return error;
}

//------------------------------------------------------------------------------

uint8* MemOut::GetBuffer()
{
  return m_buffer;
}

//------------------------------------------------------------------------------

size_t MemOut::GetBytesRemaining()
{
  return m_remain;
}

//******************************************************************************

MemIn::MemIn( const void* buffer, size_t bufferBytes )
{
  m_buffer=static_cast<const uint8*>(buffer);
  m_remain=bufferBytes;
}

//------------------------------------------------------------------------------

Error MemIn::Read( void* dest, size_t destBytes )
{
  Error error;

  if( destBytes<=m_remain )
  {
    ::memcpy( dest, m_buffer, destBytes );

    m_buffer+=destBytes;
    m_remain-=destBytes;
  }
  else
  {
	error.fail (__FILE__, __LINE__, Error::noMoreData);
  }

  return error;
}

//------------------------------------------------------------------------------

const uint8* MemIn::GetBuffer()
{
  return m_buffer;
}

//------------------------------------------------------------------------------

size_t MemIn::GetBytesRemaining()
{
  return m_remain;
}

}
