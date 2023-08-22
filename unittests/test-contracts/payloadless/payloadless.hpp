#pragma once

#include <eosio/eosio.hpp>

class [[eosio::contract]] payloadless : public eosio::contract {
public:
   using eosio::contract::contract;

   [[eosio::action]]
   void doit();

   [[eosio::action]]
   void setcode( eosio::name account, uint8_t vmtype, uint8_t vmversion, const std::vector<char>& code ) {}
};
