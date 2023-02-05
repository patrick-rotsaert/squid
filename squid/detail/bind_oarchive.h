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
class bind_oarchive
{
	BinderType binder_;

	template<class Archive>
	struct save_primitive
	{
		template<class T>
		static void invoke(Archive&, const T&)
		{
			static_assert(always_false_v<T>, "Only named value pairs allowed");
		}
	};

	template<class Archive>
	struct save_only
	{
		template<class T>
		static void invoke(Archive& ar, const T& t)
		{
			boost::serialization::serialize_adl(ar, const_cast<T&>(t), ::boost::serialization::version<T>::value);
		}
	};

	template<class T>
	void save(const T& t)
	{
		using typex = BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
		    // if its primitive
		    boost::mpl::equal_to<boost::serialization::implementation_level<T>, boost::mpl::int_<boost::serialization::primitive_type>>,
		    boost::mpl::identity<save_primitive<bind_oarchive>>,
		    // else
		    boost::mpl::identity<save_only<bind_oarchive>>>::type;
		typex::invoke(*this, t);
	}

public:
	using is_loading = boost::mpl::bool_<false>;
	using is_saving  = boost::mpl::bool_<true>;

	template<class T>
	void register_type(const T* = nullptr)
	{
	}

	unsigned int get_library_version()
	{
		return 0;
	}

	void save_binary(const void* /*address*/, std::size_t /*count*/)
	{
		// save_binary not implemented;
	}

	template<class T>
	bind_oarchive& operator<<(T const& t)
	{
		save(t);
		return *this;
	}

	template<class T>
	bind_oarchive& operator<<(const boost::serialization::nvp<T>& t)
	{
		this->binder_.bind(t.name(), t.const_value());
		return *this;
	}

	template<class T>
	bind_oarchive& operator&(const T& t)
	{
		return *this << t;
	}

	template<class StatementType>
	explicit bind_oarchive(StatementType& st)
	    : binder_{ st }
	{
	}
};

} // namespace squid
