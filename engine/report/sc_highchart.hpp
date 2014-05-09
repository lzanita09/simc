// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================
#ifndef SC_HIGHCHART_HPP
#define SC_HIGHCHART_HPP

#include "simulationcraft.hpp"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

namespace highchart
{

struct chart_t
{
  std::string id_str_;
  size_t height_, width_;
  rapidjson::Document js_;

  chart_t( const std::string& id_str );
  virtual ~chart_t() { }

  void set_title( const std::string& title )
  { this -> set( "title.text", title.c_str() ); }

  virtual std::string to_string() const;

  // Set the value of JSON object indicated by path to value_
  template <typename T>
  chart_t& set( const std::string& path, const T& value_ );
  // Set the value of JSON object indicated by path to an array of values_
  template <typename T>
  chart_t& set( const std::string& path, const std::vector<T>& values_ );
  // Set the JSON object property name_ to value_
  template<typename T>
  chart_t& set( rapidjson::Value& obj, const std::string& name_, const T& value_ );
  // Set the JSON object property name_ to a JSON array value_
  template<typename T>
  chart_t& set( rapidjson::Value& obj, const std::string& name_, const std::vector<T>& value_ );

  // Specializations for const char* (we need copies), and std::string
  chart_t& set( const std::string& path, const char* value );
  chart_t& set( const std::string& path, const std::string& value );
  chart_t& set( rapidjson::Value& obj, const std::string& name, const char* value_ );
  chart_t& set( rapidjson::Value& obj, const std::string& name, const std::string& value_ );

  // Add elements to a JSON array indicated by path by appending value_
  template <typename T>
  chart_t& add( const std::string& path, const T& value_ );
  // Add elements to a JSON array indicated by path by appending data
  template <typename T>
  chart_t& add( const std::string& path, const std::vector<T>& data );

  // Specializations for adding elements to a JSON array for various types
  chart_t& add( const std::string& path, rapidjson::Value& obj );
  chart_t& add( const std::string& path, const std::string& value_ );
  chart_t& add( const std::string& path, const char* value_ );
  chart_t& add( const std::string& path, double x, double low, double high );
  chart_t& add( const std::string& path, double x, double y );

protected:
  // Find the Value object given by a path, and construct any missing objects along the way
  rapidjson::Value* path_value( const std::string& path );
  rapidjson::Value& do_set( rapidjson::Value& obj, const char* name_, rapidjson::Value& value_ )
  {
    assert( obj.GetType() == rapidjson::kObjectType );

    return obj.AddMember( name_, js_.GetAllocator(), value_, js_.GetAllocator() );
  }

  template <typename T>
  rapidjson::Value& do_insert( rapidjson::Value& obj, const std::vector<T>& values )
  {
    assert( obj.GetType() == rapidjson::kArrayType );

    for ( size_t i = 0, end = values.size(); i < end; i++ )
      obj.PushBack( values[ i ], js_.GetAllocator() );

    return obj;
  }
};

struct time_series_t : public chart_t
{
  const stats_t* stats_;

  time_series_t( const stats_t* stats );

  void add_series( const std::string& color, const std::string& name, const std::vector<double>& series );

  void set_mean( double value_ );
  std::string build_id( const stats_t* stats );
};

template <typename T>
chart_t& chart_t::set( const std::string& path, const T& value_ )
{
  if ( rapidjson::Value* obj = path_value( path ) )
    *obj = value_;
  return *this;
}

template<typename T>
chart_t& chart_t::set( const std::string& path, const std::vector<T>& values )
{
  if ( rapidjson::Value* obj = path_value( path ) )
  {
    if ( obj -> GetType() != rapidjson::kArrayType )
      obj -> SetArray();

    obj -> Clear();

    do_insert( *obj, values );
  }
  return *this;
}

template <typename T>
chart_t& chart_t::add( const std::string& path, const T& value_ )
{
  if ( rapidjson::Value* obj = path_value( path ) )
  {
    if ( obj -> GetType() != rapidjson::kArrayType )
      obj -> SetArray();

    obj -> PushBack( value_, js_.GetAllocator() );
  }

  return *this;
}

template<typename T>
chart_t& chart_t::add( const std::string& path, const std::vector<T>& values )
{
  if ( rapidjson::Value* obj = path_value( path ) )
  {
    if ( obj -> GetType() != rapidjson::kArrayType )
      obj -> SetArray();

    do_insert( *obj, values );
  }
  return *this;
}

template<typename T>
chart_t& chart_t::set( rapidjson::Value& obj, const std::string& name_, const T& value_ )
{
  assert( obj.GetType() == rapidjson::kObjectType );

  rapidjson::Value value_obj( value_ );

  do_set( obj, name_.c_str(), value_obj );
  return *this;
}

template<typename T>
chart_t& chart_t::set( rapidjson::Value& obj, const std::string& name_, const std::vector<T>& value_ )
{
  assert( obj.GetType() == rapidjson::kObjectType );

  rapidjson::Value value_obj( rapidjson::kArrayType );

  do_set( obj, name_.c_str(), do_insert( value_obj, value_ ) );
  return *this;
}
}

#endif
