// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#ifndef TYPE_ID_DWA2002517_HPP
# define TYPE_ID_DWA2002517_HPP

# include <boost/python/detail/config.hpp>
# include <boost/python/detail/msvc_typeinfo.hpp>
# include <boost/operators.hpp>
# include <typeinfo>

namespace boost { namespace python { 

// for this compiler at least, cross-shared-library type_info
// comparisons don't work, so use typeid(x).name() instead. It's not
// yet clear what the best default strategy is.
# if defined(__GNUC__) && __GNUC__ >= 3
#  define BOOST_PYTHON_TYPE_ID_NAME
# endif 

// type ids which represent the same information as std::type_info
// (i.e. the top-level reference and cv-qualifiers are stripped), but
// which works across shared libraries.
struct type_info : private totally_ordered<type_info>
{
    type_info(std::type_info const& = typeid(void));
    
    bool operator<(type_info const& rhs) const;
    bool operator==(type_info const& rhs) const;

    char const* name() const;
    friend BOOST_PYTHON_DECL std::ostream& operator<<(
        std::ostream&, type_info const&);
    
 private: // data members
#  ifdef BOOST_PYTHON_TYPE_ID_NAME
    typedef char const* base_id_t;
#  else
    typedef std::type_info const* base_id_t;
#  endif
    
    base_id_t m_base_type;
};

template <class T>
inline type_info type_id(boost::type<T>* = 0)
{
    return type_info(
#  if (!defined(BOOST_MSVC) || BOOST_MSVC > 1300) && (!defined(BOOST_INTEL_CXX_VERSION) || BOOST_INTEL_CXX_VERSION > 600)
        typeid(T)
#  else // strip the decoration which msvc and Intel mistakenly leave in
        python::detail::msvc_typeid<T>()
#  endif 
        );
}

inline type_info::type_info(std::type_info const& id)
    : m_base_type(
#  ifdef BOOST_PYTHON_TYPE_ID_NAME
        id.name()
#  else
        &id
#  endif
        )
{
}

inline bool type_info::operator<(type_info const& rhs) const
{
#  ifdef BOOST_PYTHON_TYPE_ID_NAME
    return std::strcmp(m_base_type, rhs.m_base_type) < 0;
#  else
    return m_base_type->before(*rhs.m_base_type);
#  endif 
}

inline bool type_info::operator==(type_info const& rhs) const
{
#  ifdef BOOST_PYTHON_TYPE_ID_NAME
    return !std::strcmp(m_base_type, rhs.m_base_type);
#  else
    return *m_base_type == *rhs.m_base_type;
#  endif 
}

inline char const* type_info::name() const
{
#  ifdef BOOST_PYTHON_TYPE_ID_NAME
    return m_base_type;
#  else
    return m_base_type->name();
#  endif 
}


BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream&, type_info const&);

}} // namespace boost::python

#endif // TYPE_ID_DWA2002517_HPP