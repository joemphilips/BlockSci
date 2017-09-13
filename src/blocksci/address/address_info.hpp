//
//  address_info.hpp
//  blocksci
//
//  Created by Harry Kalodner on 3/18/17.
//
//

#ifndef address_info_hpp
#define address_info_hpp

#include "address_types.hpp"

#include <blocksci/util.hpp>
#include <blocksci/scripts/script_type.hpp>

#include <boost/variant/variant_fwd.hpp>

#include <tuple>
#include <sstream>
#include <stdio.h>

namespace blocksci {
    template <AddressType::Enum>
    struct AddressInfo;
    
    template <>
    struct AddressInfo<AddressType::Enum::PUBKEY> {
        static constexpr char typeName[] = "pubkey";
        static constexpr int addressType = 0;
        static constexpr bool spendable = true;
        static constexpr bool hasNestedAddresses = false;
        static constexpr bool deduped = true;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::PUBKEY;
    };
    
    template <>
    struct AddressInfo<AddressType::Enum::PUBKEYHASH> {
        static constexpr char typeName[] = "pubkeyhash";
        static constexpr int addressType = 0;
        static constexpr bool spendable = true;
        static constexpr bool hasNestedAddresses = false;
        static constexpr bool deduped = true;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::PUBKEY;
    };
    
    template <>
    struct AddressInfo<AddressType::Enum::WITNESS_PUBKEYHASH> {
        static constexpr char typeName[] = "witness_pubkeyhash";
        static constexpr int addressType = 0;
        static constexpr bool spendable = true;
        static constexpr bool hasNestedAddresses = false;
        static constexpr bool deduped = true;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::PUBKEY;
    };
    
    template <>
    struct AddressInfo<AddressType::Enum::SCRIPTHASH> {
        static constexpr char typeName[] = "scripthash";
        static constexpr int addressType = 1;
        static constexpr bool spendable = true;
        static constexpr bool hasNestedAddresses = true;
        static constexpr bool deduped = true;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::SCRIPTHASH;
    };
    
    template <>
    struct AddressInfo<AddressType::Enum::WITNESS_SCRIPTHASH> {
        static constexpr char typeName[] = "witness_scripthash";
        static constexpr int addressType = 1;
        static constexpr bool spendable = true;
        static constexpr bool hasNestedAddresses = true;
        static constexpr bool deduped = true;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::SCRIPTHASH;
    };
    
    template <>
    struct AddressInfo<AddressType::Enum::MULTISIG> {
        static constexpr char typeName[] = "multisig";
        static constexpr int addressType = 2;
        static constexpr bool spendable = true;
        static constexpr bool hasNestedAddresses = true;
        static constexpr bool deduped = true;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::MULTISIG;
    };
    
    template <>
    struct AddressInfo<AddressType::Enum::NONSTANDARD> {
        static constexpr char typeName[] = "nonstandard";
        static constexpr int addressType = -1;
        static constexpr bool spendable = true;
        static constexpr bool hasNestedAddresses = false;
        static constexpr bool deduped = false;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::NONSTANDARD;
    };
    
    template <>
    struct AddressInfo<AddressType::Enum::NULL_DATA> {
        static constexpr char typeName[] = "nulldata";
        static constexpr int addressType = -1;
        static constexpr bool spendable = false;
        static constexpr bool hasNestedAddresses = false;
        static constexpr bool deduped = false;
        static constexpr ScriptType::Enum scriptType = ScriptType::Enum::NULL_DATA;
    };
    
    std::string GetTxnOutputType(AddressType::Enum t);
    
    template<AddressType::Enum AddressType>
    struct AddressTag {
        static constexpr AddressType::Enum type = AddressType;
    };
    
    namespace internal {
        template<typename T>
        struct to_variant;
        
        template<typename... Ts>
        struct to_variant<std::tuple<Ts...>>
        {
            using type = boost::variant<Ts...>;
        };
        
        template<template<AddressType::Enum> class K, typename T>
        struct to_address_type;
        
        template<template<AddressType::Enum> class K, AddressType::Enum Type>
        struct to_address_type<K, AddressTag<Type>> {
            using type = K<Type>;
        };
        
        template <template<AddressType::Enum> class K, typename... Types>
        struct to_address_type<K, std::tuple<Types...>> {
            using type = std::tuple<typename to_address_type<K, Types>::type...>;
        };
    }
    
    template <template<AddressType::Enum> class K, typename... Types>
    using to_script_variant_t = typename internal::to_variant<typename internal::to_address_type<K, Types...>::type>::type;

    
    template<AddressType::Enum type>
    struct SpendableFunctor {
        static constexpr bool f() {
            return AddressInfo<type>::spendable;
        }
    };
    
    template<AddressType::Enum type>
    struct ScriptTypeFunctor {
        static constexpr ScriptType::Enum f() {
            return AddressInfo<type>::scriptType;
        }
    };
    
    template<AddressType::Enum type>
    struct HasNestedAddressesFunctor {
        static constexpr bool f() {
            return AddressInfo<type>::hasNestedAddresses;
        }
    };
    
    template<AddressType::Enum type>
    struct DBTypeFunctor {
        static constexpr int f() {
            return AddressInfo<type>::addressType;
        }
    };
    
    inline auto getAddressTypes() {
        return internal::index_apply<AddressType::all.size()>([](auto... Is) {
            return std::make_tuple(AddressTag<std::get<Is>(AddressType::all)>{}...);
        });
    }
    
    using AddressInfoList = decltype(getAddressTypes());
    
    static constexpr auto spendableTable = blocksci::make_static_table<AddressType, SpendableFunctor>();
    static constexpr auto scriptTypeTable = blocksci::make_static_table<AddressType, ScriptTypeFunctor>();
    static constexpr auto nestedAddressTable = blocksci::make_static_table<AddressType, HasNestedAddressesFunctor>();
    static constexpr auto dbTypeTable = blocksci::make_static_table<AddressType, DBTypeFunctor>();
    
    constexpr bool isSpendable(AddressType::Enum t) {
        
        auto index = static_cast<size_t>(t);
        if (index >= AddressType::size)
        {
            throw std::invalid_argument("combination of enum values is not valid");
        }
        return spendableTable[index];
    }
    
    constexpr ScriptType::Enum scriptType(AddressType::Enum t) {
        
        auto index = static_cast<size_t>(t);
        if (index >= AddressType::size)
        {
            throw std::invalid_argument("combination of enum values is not valid");
        }
        return scriptTypeTable[index];
    }
    
    constexpr bool hasNestedAddresses(AddressType::Enum t) {
        
        auto index = static_cast<size_t>(t);
        if (index >= AddressType::size)
        {
            throw std::invalid_argument("combination of enum values is not valid");
        }
        return nestedAddressTable[index];
    }
    
    constexpr int getDBType(AddressType::Enum t) {
        
        auto index = static_cast<size_t>(t);
        if (index >= AddressType::size)
        {
            throw std::invalid_argument("combination of enum values is not valid");
        }
        return dbTypeTable[index];
    }
}

#endif /* address_info_hpp */