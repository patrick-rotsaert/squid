//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/detail/always_false.h"

#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include <cstddef>

namespace squid {

template<class BinderType>
class bind_iarchive
{
	BinderType binder_;

	template<class Archive>
	struct load_primitive
	{
		template<class T>
		static void invoke(Archive&, T&)
		{
			static_assert(always_false_v<T>, "Only named value pairs allowed");
		}
	};

	template<class Archive>
	struct load_only
	{
		template<class T>
		static void invoke(Archive& ar, T& t)
		{
			boost::serialization::serialize_adl(ar, t, ::boost::serialization::version<T>::value);
		}
	};

	template<class T>
	void load(T& t)
	{
		using typex = BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
		    // if its primitive
		    boost::mpl::equal_to<boost::serialization::implementation_level<T>, boost::mpl::int_<boost::serialization::primitive_type>>,
		    boost::mpl::identity<load_primitive<bind_iarchive>>,
		    // else
		    boost::mpl::identity<load_only<bind_iarchive>>>::type;
		typex::invoke(*this, t);
	}

public:
	using is_loading = boost::mpl::bool_<true>;
	using is_saving  = boost::mpl::bool_<false>;

	template<class T>
	void register_type(const T* = nullptr)
	{
	}

	unsigned int get_library_version()
	{
		return 0;
	}

	void load_binary(const void* /*address*/, std::size_t /*count*/)
	{
		// load_binary not implemented;
	}

	template<class T>
	bind_iarchive& operator>>(T& t)
	{
		load(t);
		return *this;
	}

	template<class T>
	bind_iarchive& operator>>(boost::serialization::nvp<T>& t)
	{
		this->binder_.bind(t.name(), t.value());
		return *this;
	}

	template<class T>
	bind_iarchive& operator>>(const boost::serialization::nvp<T>& t)
	{
		return operator>>(const_cast<boost::serialization::nvp<T>&>(t));
	}

	template<class T>
	bind_iarchive& operator&(T& t)
	{
		return *this >> t;
	}

	template<class StatementType>
	explicit bind_iarchive(StatementType& st)
	    : binder_{ st }
	{
	}
};

} // namespace squid
