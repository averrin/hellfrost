#ifndef __CREAL_JSON_H_
#define __CREAL_JSON_H_

// #include <cereal/types/utility.hpp>
// #include <cereal/types/array.hpp>
#include <cereal/types/string.hpp>
// #include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>

namespace cereal
{
template <typename Archive, typename T, size_t S>
void serialize(Archive& archive, std::array<T, S>& m) {
    cereal::size_type s = S;
    archive(cereal::make_size_tag(s));
    if (s != S) {
        throw std::runtime_error("array has incorrect length");
    }
    for (auto& i : m) { archive(i); }
}
    template<class Archive, class F, class S>
    void save(Archive& ar, const std::pair<F, S>& pair)
    {
        ar(cereal::make_size_tag(2));
        ar(pair.first);
        ar(pair.second);
    }

    template<class Archive, class F, class S>
    void load(Archive& ar, std::pair<F, S>& pair)
    {
        // ar(cereal::make_size_tag(2));
        F first;
        S second;
        ar(first);
        ar(second);
        pair.first = first;
        pair.second = second;
    }

    template <class Archive, class F, class S>
    struct specialize<Archive, std::pair<F, S>, cereal::specialization::non_member_load_save> {};

  template <class Archive, class V, class C, class A> inline
  void save( Archive & ar, std::map<std::string, V, C, A> const & map )
  {
    for( const auto & i : map )
      ar( cereal::make_nvp( i.first, i.second ) );
  }

  template <class Archive, class V, class C, class A> inline
  void load( Archive & ar, std::map<std::string, V, C, A> & map )
  {
    map.clear();

    auto hint = map.begin();
    while( true )
    {
      const auto namePtr = ar.getNodeName();

      if( !namePtr )
        break;

      std::string key = namePtr;
      V value; ar( value );
      hint = map.emplace_hint( hint, std::move( key ), std::move( value ) );
    }
  }
}

#endif // __CREAL_JSON_H_
