// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide..

#ifndef __VF_RANGE_VFHEADER__
#define __VF_RANGE_VFHEADER__

// Represents the half-open interval [begin, end)
template <class Ty>
struct RangeOld
{
  Ty begin;
  Ty end;

public:
  typedef Ty value_type;

  static RangeOld EmptySet;

  RangeOld()
    : begin (Ty())
    , end (Ty())
  {
  }

  RangeOld (Ty const& begin0, Ty const& end0)
  {
    begin = begin0;
    end = end0;
  }

  bool operator== (RangeOld const& rhs) const
  {
    return begin == rhs.begin && end == rhs.end;
  }

  bool operator!= (RangeOld const& rhs ) const
  {
    return begin != rhs.begin || end == rhs.end;
  }

  Ty const& Start() const
  {
    jassert (begin <= end);
    return begin;
  }

  Ty const& End() const
  {
    jassert( begin<=end );
    return end;
  }

  // Lebesque measure
  Ty Count() const
  {
    return Empty() ? 0 : (end-begin);
  }

  // Sugar for the case where Ty is a non integer type
  Ty Distance() const
  {
    return Empty() ? 0 : (end-begin);
  }

  Ty Length() const
  {
    return Distance();
  }

  bool Empty() const
  {
    return begin>=end;
  }

  bool NotEmpty() const
  {
    return !Empty();
  }

  bool IsEmpty() const // alias
  {
    return Empty();
  }

  bool IsNotEmpty() const
  {
    return !Empty();
  }

  bool Contains( const Ty& value ) const
  {
    return NotEmpty() && value>=begin && value<end;
  }

  bool Intersects( const RangeOld& range ) const
  {
    return NotEmpty() && range.NotEmpty() && end>range.begin && begin<range.end;
  }

  bool Disjoint( const RangeOld& range ) const
  {
    return !Intersects( range );
  }

  // Contiguous iff the union is a single non empty half-open interval
  bool Contiguous( const RangeOld& range ) const
  {
    return ( Empty()!=range.Empty() ) || ( NotEmpty() && end>=range.begin && begin<=range.end );
  }

  bool NotContiguous( const RangeOld& range ) const
  {
    return !Contiguous( range );
  }

  // A is a superset of B if B is empty or if A fully contains B
  bool SuperSetOf( const RangeOld& range ) const
  {
    return range.Empty() || ( NotEmpty() && begin<=range.begin && end>=range.end );
  }

  // A is a proper superset of B if A is not empty, and B is empty or A is larger than B
  bool ProperSuperSetOf( const RangeOld& range ) const
  {
    return NotEmpty() && ( range.Empty() || (
                             ( begin<=range.begin && end>range.end ) || ( begin<range.begin && end>=range.end ) ) );
  }

  bool SubSetOf( const RangeOld& range ) const
  {
    return range.SuperSetOf( *this );
  }

  bool ProperSubSetOf( const RangeOld& range ) const
  {
    return range.ProperSuperSetOf( *this );
  }

  RangeOld Intersect( const RangeOld& range ) const
  {
    //if( !Empty() && !range.Empty() )
    // this may return an irregular interval
    return RangeOld( std::max( begin, range.begin ), std::min( end, range.end ) );
    //else
    //	return EmptySet;
  }

  // Returns the smallest range that contains both intervals
  RangeOld SingleUnion( const RangeOld& range ) const
  {
    return RangeOld(
             std::min( range.Normalized().begin, Normalized().begin ),
             std::max( range.Normalized().end, Normalized().end ) );
  }

  // Empty if the union cannot be represented as a single half-open interval
  RangeOld ContiguousUnion( const RangeOld& range ) const
  {
    if( Empty() ) return range;
    else if( range.Empty() ) return *this;
    else if( end<range.begin || begin>range.end ) return EmptySet;
    else return RangeOld( std::min( begin, range.begin ), std::max( end, range.end ) );
  }

  // Returns true if the RangeOld is correctly ordererd
  bool IsNormalized() const
  {
    return end >= begin;
  }

  // Returns a correctly ordered range
  RangeOld Normalized() const
  {
    if( begin>end )
      return RangeOld( end, begin );
    else
      return *this;
  }

  // Clips the value to lie within the range
  template<typename Tv>
  const Ty Clip( Tv value )
  {
    // These conditionals are carefully ordered so
    // that if begin==end, value is assigned begin.
    if( value > end )
      value = end - ( std::numeric_limits<Tv>::is_integer ?
                      1 : std::numeric_limits<Tv>::epsilon() );

    if( value < begin )
      value = begin;

    return value;
  }

  //----------------------------------------------------------------------
};

template<typename Ty>
RangeOld<Ty> RangeOld<Ty>::EmptySet;

#endif
