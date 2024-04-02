/**
 * @file   shp.h
 * @author Dennis Sitelew
 * @date   Apr. 17, 2022
 */
#ifndef SIMPLE_HEX_PRINTER_INCLUDE_SHP_SHP_H
#define SIMPLE_HEX_PRINTER_INCLUDE_SHP_SHP_H

#include <array>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace shp {

////////////////////////////////////////////////////////////////////////////////
/// Class: format_backup
////////////////////////////////////////////////////////////////////////////////

//! Backs up the stream formatting and restores it upon destruction.
class format_backup {
public:
   /**
    * Constructor - stores stream formatting
    * @param os Stream reference
    */
   explicit format_backup(std::ostream &os)
      : os_(&os) {
      old_ios_.copyfmt(os);
   }

   /**
    * Destructor - restores formatting
    */
   ~format_backup() { os_->copyfmt(old_ios_); }

public:
   format_backup(const format_backup &) = delete;
   format_backup(format_backup &&) = delete;

   format_backup &operator=(const format_backup &) = delete;
   format_backup &operator=(format_backup &&) = delete;

private:
   //! Output stream reference
   std::ostream *os_;

   //! State backup
   std::ios old_ios_{nullptr};
};

////////////////////////////////////////////////////////////////////////////////
/// HEX Format Specifiers
////////////////////////////////////////////////////////////////////////////////
//! Add 0x prefix
template <bool V>
struct PrefixType : std::integral_constant<bool, V> {};
struct Prefix : PrefixType<true> {};
struct NoPrefix : PrefixType<false> {};

//! Fill value with zeroes
template <bool V>
struct FillType : std::integral_constant<bool, V> {};
struct Fill : FillType<true> {};
struct NoFill : FillType<false> {};

//! Print in Upper Case
template <bool V>
struct UpperCaseType : std::integral_constant<bool, V> {};
struct UpperCase : UpperCaseType<true> {};
struct LowerCase : UpperCaseType<false> {};

//! Print byte offsets (address field)
template <bool V>
struct PrintOffsetsType : std::integral_constant<bool, V> {};
struct PrintOffsets : PrintOffsetsType<true> {};
struct NoOffsets : PrintOffsetsType<false> {};

//! Separate nibbles
template <bool V>
struct SeparateNibblesType : std::integral_constant<bool, V> {};
struct SeparateNibbles : SeparateNibblesType<true> {};
struct NoNibbleSeparation : SeparateNibblesType<false> {};

//! Row width
template <std::size_t Sz>
struct RowWidth : std::integral_constant<std::size_t, Sz> {};
struct SingleRow : std::integral_constant<std::size_t, std::numeric_limits<std::size_t>::max()> {};

//! Print ASCII values
template <bool V>
struct PrintASCIIType : std::integral_constant<bool, V> {};
struct PrintASCII : PrintASCIIType<true> {};
struct NoASCII : PrintASCIIType<false> {};

////////////////////////////////////////////////////////////////////////////////
/// Class: integral_hex_writer
////////////////////////////////////////////////////////////////////////////////
//! Helper class for writing integral types in the hex form into an output stream.
template <typename T, typename WithPrefix = Prefix, typename DoFill = Fill, typename InUpperCase = UpperCase>
class integral_hex_writer {
private:
   static_assert(std::is_same<WithPrefix, Prefix>::value || std::is_same<WithPrefix, NoPrefix>::value,
                 "Valid prefix type expected");

   static_assert(std::is_same<DoFill, Fill>::value || std::is_same<DoFill, NoFill>::value, "Valid Fill type expected");

   static_assert(std::is_same<InUpperCase, UpperCase>::value || std::is_same<InUpperCase, LowerCase>::value,
                 "Valid case type expected");

   static_assert(std::is_integral<T>::value, "T should be an integral type");

public:
   explicit integral_hex_writer(T value)
      : value_{value} {
      // Nothing to do here
   }

public:
   template <typename OT, typename OWithPrefix, typename ODoFill, typename OInUpperCase>
   friend std::ostream &operator<<(std::ostream &os,
                                   const integral_hex_writer<OT, OWithPrefix, ODoFill, OInUpperCase> &v);

private:
   T value_; //!< Value to be printed
};

namespace detail {

template <typename T>
inline void print_one_byte(std::ostream &os, T v) {
   os << static_cast<int>(static_cast<typename std::make_unsigned<T>::type>(v));
}

template <>
inline void print_one_byte(std::ostream &os, bool v) {
   os << static_cast<unsigned>(v);
}

} // namespace detail

template <typename T, typename WithPrefix, typename DoFill, typename InUpperCase>
std::ostream &operator<<(std::ostream &os, const integral_hex_writer<T, WithPrefix, DoFill, InUpperCase> &v) {
   format_backup backup(os);
   os << std::hex;

   if (WithPrefix::value) {
      os << "0x";
   }

   if (DoFill::value) {
      os << std::setw(2U * sizeof(T));
      os << std::setfill('0');
   }

   if (InUpperCase::value) {
      os << std::uppercase;
   }

   if (sizeof(T) > 1U) {
      // If size of T is bigger than one byte it can be printed without extra
      // casting without a risk of being printed out as an ASCII character
      os << v.value_;
   } else {
      detail::print_one_byte(os, v.value_);
   }
   return os;
}

////////////////////////////////////////////////////////////////////////////////
/// Class:
////////////////////////////////////////////////////////////////////////////////
//! Helper class for writing iterator ranges in the hex form into an output stream.
template <typename Iterator,
          typename WithOffsets = PrintOffsets,
          typename WithNibbleSeparation = SeparateNibbles,
          typename RowWidthValue = RowWidth<16>,
          typename WithASCII = PrintASCII,
          typename InUpperCase = UpperCase>
class iterator_hex_writer {
private:
   static_assert(std::is_same<WithOffsets, PrintOffsets>::value || std::is_same<WithOffsets, NoOffsets>::value,
                 "Valid offset type expected");

   static_assert(std::is_same<WithNibbleSeparation, SeparateNibbles>::value
                    || std::is_same<WithNibbleSeparation, NoNibbleSeparation>::value,
                 "Valid nibble separation type expected");

   static_assert(std::is_same<WithASCII, PrintASCII>::value || std::is_same<WithASCII, NoASCII>::value,
                 "Valid ASCII type expected");

   static_assert(std::is_same<InUpperCase, UpperCase>::value || std::is_same<InUpperCase, LowerCase>::value,
                 "Valid case type expected");

   static_assert(!std::is_same<RowWidthValue, SingleRow>::value
                    || (std::is_same<RowWidthValue, SingleRow>::value
                        && !(std::is_same<WithOffsets, PrintOffsets>::value
                             || std::is_same<WithASCII, PrintASCII>::value)),
                 "Single row printer should exclude offsets and ASCII");

   static_assert(RowWidthValue::value != 0, "Row width cannot be 0");

   using self_t =
      iterator_hex_writer<Iterator, WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII, InUpperCase>;

   using iterator_t = Iterator;
   using iterator_value_t = typename std::iterator_traits<iterator_t>::value_type;
   using value_t = typename std::remove_cv<typename std::remove_reference<iterator_value_t>::type>::type;

   static_assert(std::is_integral<value_t>::value || std::is_standard_layout<value_t>::value,
                 "Iterator::value_type should either be an integral type or a POD type");

   struct dummy_ascii_cache_t {
      void add_byte(std::uint8_t) {
         // Nothing to do here
      }
      void print_cached(std::ostream &) {
         // Nothing to do here
      }
   };

   struct ascii_cache_t {
      void add_byte(std::uint8_t b) { cache[offset++] = b; }

      // Print a subset of bytes in ASCII cache
      void print_cached(std::ostream &os) {
         if (offset != RowWidthValue::value) {
            // We are printing the rest of the ASCII cache - add some padding
            auto missing_bytes = RowWidthValue::value - offset;
            auto characters_per_byte = 2;
            if (std::is_same<WithNibbleSeparation, SeparateNibbles>::value) {
               ++characters_per_byte;
            }

            os << std::setfill(' ') << std::setw(missing_bytes * characters_per_byte) << ' ';
         }

         os << "  ";
         for (std::size_t i = 0; i < offset; ++i) {
            print_ascii(os, cache[i]);
         }
         offset = 0;
      }

      std::size_t offset{0};
      std::array<std::uint8_t, RowWidthValue::value> cache{};
   };

   // Helper struct to keep track of printing state. We use it because we want to be able to print POD-types without
   // the byte printing boilerplate
   struct print_state {
      explicit print_state(const self_t &owner)
         : it{owner.begin_}
         , end{owner.end_}
         , address_width{with_offsets ? get_address_width() : 0} {
         // Nothing to do here
      }

      size_t get_address_width() const {
         // Calculate the number of HEX digits required to encode all address values in the iterator range
         const auto full_size = std::distance(it, end);
         std::size_t result = 0;
         for (std::size_t reminder = full_size - 1; reminder != 0; reminder /= 16, ++result) {
            // Nothing to do here
         }

         // Print at least two address characters
         return result < 2 ? 2 : result;
      }

      static const bool with_offsets = WithOffsets::value;
      static const bool separate_nibbles = WithNibbleSeparation::value;
      static const std::size_t row_width = RowWidthValue::value;
      static const bool upper_case = InUpperCase::value;

      iterator_t it;
      iterator_t end;

      const std::size_t address_width;

      std::size_t global_offset{};
      std::size_t row_offset{};
      typename std::conditional<std::is_same<RowWidthValue, SingleRow>::value, dummy_ascii_cache_t, ascii_cache_t>::type
         ascii_cache;
   };

public:
   iterator_hex_writer(iterator_t begin, iterator_t end)
      : begin_{begin}
      , end_{end} {
      // Nothing to do here
   }

   explicit iterator_hex_writer(std::pair<iterator_t, iterator_t> range)
      : begin_{range.first}
      , end_{range.second} {
      // Nothing to do here
   }

public:
   template <typename OIterator,
             typename OWithOffsets,
             typename OWithNibbleSeparation,
             typename ORowWidthValue,
             typename OWithASCII,
             typename OInUpperCase>
   friend std::ostream &operator<<(std::ostream &os,
                                   const iterator_hex_writer<OIterator,
                                                             OWithOffsets,
                                                             OWithNibbleSeparation,
                                                             ORowWidthValue,
                                                             OWithASCII,
                                                             OInUpperCase> &v);

private:
   static void print_ascii(std::ostream &os, std::uint8_t byte) {
      auto ascii_byte = static_cast<unsigned char>(byte);
      if (std::isprint(ascii_byte)) {
         os << static_cast<char>(ascii_byte);
      } else {
         os << '.';
      }
   }

   static void print_byte(std::ostream &os, print_state &s, uint8_t byte) {
      if (s.row_offset == 0 && s.global_offset != 0) {
         os << '\n';
      }

      if (s.with_offsets && s.row_offset == 0) {
         os << "0x" << std::hex << std::setw(s.address_width) << std::setfill('0');
         if (s.upper_case) {
            os << std::uppercase;
         }
         os << s.global_offset << ": ";
      }

      if (s.separate_nibbles && s.row_offset != 0) {
         os << ' ';
      }

      s.ascii_cache.add_byte(byte);

      os << std::setw(2) << std::setfill('0') << std::hex;
      if (InUpperCase::value) {
         os << std::uppercase;
      }

      os << static_cast<int>(byte);

      ++s.global_offset;
      ++s.row_offset;

      if (s.row_offset == s.row_width) {
         s.ascii_cache.print_cached(os);
         s.row_offset = 0;
      }
   }

   template <typename T>
   static void print_value_bytes(std::ostream &os, print_state &ps) {
      const value_t &value = *ps.it;
      auto value_byte = reinterpret_cast<const char *>(&value);
      const auto last_byte = value_byte + sizeof(value_t);
      for (; value_byte != last_byte; ++value_byte) {
         print_byte(os, ps, static_cast<uint8_t>(*value_byte));
      }
   }

   void do_print(std::ostream &os) const {
      format_backup backup(os);

      print_state ps{*this};
      for (; ps.it != ps.end; ++ps.it) {
         // Print out, depending on whether the object being printed is a POD-struct or just an integral value.
         print_value_bytes<value_t>(os, ps);
      }

      if (ps.row_offset != 0) {
         ps.ascii_cache.print_cached(os);
      }
   }

private:
   //! Range begin iterator
   iterator_t begin_;

   //! Range end iterator
   iterator_t end_;
};

template <typename Iterator,
          typename WithOffsets,
          typename WithNibbleSeparation,
          typename RowWidthValue,
          typename WithASCII,
          typename InUpperCase>
std::ostream &operator<<(
   std::ostream &os,
   const iterator_hex_writer<Iterator, WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII, InUpperCase> &v) {
   v.do_print(os);
   return os;
}

template <typename T>
struct is_container : std::false_type {};

template <typename T>
struct is_container<std::vector<T>> : std::true_type {
   using element_type = T;
};

template <typename T, std::size_t N>
struct is_container<std::array<T, N>> : std::true_type {
   using element_type = T;
};

template <typename T, std::size_t N>
struct is_container<T[N]> : std::true_type {
   using element_type = T;
};

template <>
struct is_container<std::string> : std::true_type {
   using element_type = std::string::value_type;
};

template <typename T>
struct is_container<std::initializer_list<T>> : std::true_type {
   using element_type = T;
};

////////////////////////////////////////////////////////////////////////////////
/// Helper functions for constructing a streamable object
////////////////////////////////////////////////////////////////////////////////
/**
 * Construct a streamable object for printing out an integral constant in HEX.
 *
 * @example std::cout << shp::hex(0xBEEF) << std::endl;
 *
 * @tparam T Integral type.
 * @tparam WithPrefix Controls whether the 0x prefix should be printed or not.
 * @tparam DoFill Controls whether the printed out value should be filled (padded) with zeroes or not.
 * @tparam InUpperCase Controls whether the HEX value should be printed in upper case or not.
 * @param value Value to construct a streamable object from.
 * @return A streamable object.
 */
template <typename T, typename WithPrefix = Prefix, typename DoFill = Fill, typename InUpperCase = UpperCase>
inline
   typename std::enable_if<std::is_integral<T>::value, integral_hex_writer<T, WithPrefix, DoFill, InUpperCase>>::type
   hex(const T &value, const WithPrefix = WithPrefix{}, const DoFill = DoFill{}, const InUpperCase = InUpperCase{}) {
   return integral_hex_writer<T, WithPrefix, DoFill, InUpperCase>{value};
}

/**
 * Construct a streamable object for printing out a collection of POD-objects in HEX.
 *
 * @example struct { int a; int b; } q[2] = {{1, 2}, {3, 4}}; std::cout << shp::hex(q) << std::endl;
 *
 * @tparam ContainerT Container type.
 * @tparam WithOffsets Controls whether the row offsets should be printed out or not.
 * @tparam WithNibbleSeparation Controls whether nibbles should be separated or not.
 * @tparam RowWidthValue Maximal of a single row (in bytes).
 * @tparam WithASCII Controls whether ASCII values should be printed out or not.
 * @tparam InUpperCase Controls whether HEX values should be printed out in upper-case or not.
 * @param cont Container to construct a streamable object for.
 * @return A streamable object.
 */
template <typename ContainerT,
          typename WithOffsets = PrintOffsets,
          typename WithNibbleSeparation = SeparateNibbles,
          typename RowWidthValue = RowWidth<16>,
          typename WithASCII = PrintASCII,
          typename InUpperCase = UpperCase>
inline typename std::enable_if<is_container<ContainerT>::value
                                  && std::is_standard_layout<typename is_container<ContainerT>::element_type>::value,
                               iterator_hex_writer<decltype(std::cbegin(std::declval<ContainerT>())),
                                                   WithOffsets,
                                                   WithNibbleSeparation,
                                                   RowWidthValue,
                                                   WithASCII,
                                                   InUpperCase>>::type
hex(const ContainerT &cont,
    const WithOffsets = WithOffsets{},
    const WithNibbleSeparation = WithNibbleSeparation{},
    const RowWidthValue = RowWidthValue{},
    const WithASCII = WithASCII{},
    const InUpperCase = InUpperCase{}) {
   return iterator_hex_writer<decltype(std::cbegin(cont)), WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII,
                              InUpperCase>{std::cbegin(cont), std::cend(cont)};
}

/**
 * Construct a streamable object for printing out a single POD-object in HEX.
 *
 * @example struct { int a; int b; } q = {1, 2}; std::cout << shp::hex(q) << std::endl;
 *
 * @tparam T Object type.
 * @tparam WithOffsets Controls whether the row offsets should be printed out or not.
 * @tparam WithNibbleSeparation Controls whether nibbles should be separated or not.
 * @tparam RowWidthValue Maximal of a single row (in bytes).
 * @tparam WithASCII Controls whether ASCII values should be printed out or not.
 * @tparam InUpperCase Controls whether HEX values should be printed out in upper-case or not.
 * @param cont Container to construct a streamable object for.
 * @return A streamable object.
 */
template <typename T,
          typename WithOffsets = PrintOffsets,
          typename WithNibbleSeparation = SeparateNibbles,
          typename RowWidthValue = RowWidth<16>,
          typename WithASCII = PrintASCII,
          typename InUpperCase = UpperCase>
inline typename std::enable_if<
   !is_container<T>::value && std::is_standard_layout<T>::value && !std::is_integral<T>::value,
   iterator_hex_writer<const T *, WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII, InUpperCase>>::type
hex(const T &v,
    const WithOffsets = WithOffsets{},
    const WithNibbleSeparation = WithNibbleSeparation{},
    const RowWidthValue = RowWidthValue{},
    const WithASCII = WithASCII{},
    const InUpperCase = InUpperCase{}) {
   auto start = std::addressof(v);
   auto end = start + 1;
   return iterator_hex_writer<const T *, WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII, InUpperCase>{
      start, end};
}

////////////////////////////////////////////////////////////////////////////////
/// HEX-Strings from objects
////////////////////////////////////////////////////////////////////////////////

/**
 * Convert an integral type to a HEX-string.
 *
 * @example auto str = shp::hex_str(0xBEEF);
 *
 * @tparam T Integral type.
 * @tparam WithPrefix Controls whether the 0x prefix should be printed or not.
 * @tparam DoFill Controls whether the printed out value should be filled (padded) with zeroes or not.
 * @tparam InUpperCase Controls whether the HEX value should be printed in upper case or not.
 * @param value Value to construct a streamable object from.
 * @return A HEX string representation of a value.
 */
template <typename T, typename WithPrefix = Prefix, typename DoFill = Fill, typename InUpperCase = UpperCase>
inline typename std::enable_if<std::is_integral<T>::value, std::string>::type
hex_str(const T &value, const WithPrefix = WithPrefix{}, const DoFill = DoFill{}, const InUpperCase = InUpperCase{}) {
   std::ostringstream os;
   os << integral_hex_writer<T, WithPrefix, DoFill, InUpperCase>{value};
   return os.str();
}

/**
 * Convert an collection of POD-objects into a HEX-string.
 *
 * @example struct { int a; int b; } q[2] = {{1, 2}, {3, 4}}; auto str = shp::hex_str(q);
 *
 * @tparam ContainerT Container type.
 * @tparam WithOffsets Controls whether the row offsets should be printed out or not.
 * @tparam WithNibbleSeparation Controls whether nibbles should be separated or not.
 * @tparam RowWidthValue Maximal of a single row (in bytes).
 * @tparam WithASCII Controls whether ASCII values should be printed out or not.
 * @tparam InUpperCase Controls whether HEX values should be printed out in upper-case or not.
 * @param cont Container to construct a streamable object for.
 * @return A HEX string representation of the collection.
 */
template <typename ContainerT,
          typename WithOffsets = PrintOffsets,
          typename WithNibbleSeparation = SeparateNibbles,
          typename RowWidthValue = RowWidth<16>,
          typename WithASCII = PrintASCII,
          typename InUpperCase = UpperCase>
inline typename std::enable_if<is_container<ContainerT>::value
                                  && std::is_standard_layout<typename std::iterator_traits<
                                     decltype(std::cbegin(std::declval<ContainerT>()))>::value_type>::value,
                               std::string>::type
hex_str(const ContainerT &cont,
        const WithOffsets = WithOffsets{},
        const WithNibbleSeparation = WithNibbleSeparation{},
        const RowWidthValue = RowWidthValue{},
        const WithASCII = WithASCII{},
        const InUpperCase = InUpperCase{}) {
   std::ostringstream os;
   os << iterator_hex_writer<decltype(std::cbegin(cont)), WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII,
                             InUpperCase>{std::cbegin(cont), std::cend(cont)};
   return os.str();
}

template <typename ValueT,
   typename WithOffsets = PrintOffsets,
   typename WithNibbleSeparation = SeparateNibbles,
   typename RowWidthValue = RowWidth<16>,
   typename WithASCII = PrintASCII,
   typename InUpperCase = UpperCase>
inline typename std::enable_if<std::is_standard_layout<ValueT>::value, std::string>::type
hex_str(std::initializer_list<ValueT> cont,
        const WithOffsets = WithOffsets{},
        const WithNibbleSeparation = WithNibbleSeparation{},
        const RowWidthValue = RowWidthValue{},
        const WithASCII = WithASCII{},
        const InUpperCase = InUpperCase{}) {
   std::ostringstream os;
   os << iterator_hex_writer<decltype(std::cbegin(cont)), WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII,
                             InUpperCase>{std::cbegin(cont), std::cend(cont)};
   return os.str();
}

/**
 * Convert an single POD-object into a HEX-string.
 *
 * @example struct { int a; int b; } q = {1, 2}; auto str = shp::hex_str(q);
 *
 * @tparam T Object type.
 * @tparam WithOffsets Controls whether the row offsets should be printed out or not.
 * @tparam WithNibbleSeparation Controls whether nibbles should be separated or not.
 * @tparam RowWidthValue Maximal of a single row (in bytes).
 * @tparam WithASCII Controls whether ASCII values should be printed out or not.
 * @tparam InUpperCase Controls whether HEX values should be printed out in upper-case or not.
 * @param cont Container to construct a streamable object for.
 * @return A HEX string representation of the collection.
 */
template <typename T,
          typename WithOffsets = PrintOffsets,
          typename WithNibbleSeparation = SeparateNibbles,
          typename RowWidthValue = RowWidth<16>,
          typename WithASCII = PrintASCII,
          typename InUpperCase = UpperCase>
inline
   typename std::enable_if<!is_container<T>::value && std::is_standard_layout<T>::value && !std::is_integral<T>::value,
                           std::string>::type
   hex_str(const T &v,
           const WithOffsets = WithOffsets{},
           const WithNibbleSeparation = WithNibbleSeparation{},
           const RowWidthValue = RowWidthValue{},
           const WithASCII = WithASCII{},
           const InUpperCase = InUpperCase{}) {
   std::ostringstream os;
   auto start = std::addressof(v);
   auto end = start + 1;
   os << iterator_hex_writer<const T *, WithOffsets, WithNibbleSeparation, RowWidthValue, WithASCII, InUpperCase>{start,
                                                                                                                  end};
   return os.str();
}

} // namespace shp

#endif /* SIMPLE_HEX_PRINTER_INCLUDE_SHP_SHP_H */
