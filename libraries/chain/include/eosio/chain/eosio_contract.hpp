#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/contract_types.hpp>
#include <eosio/chain/system_config.hpp>

namespace eosio { namespace chain {

   class apply_context;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{
   #define NATIVE_ACTION_HANDLER(contract, action) \
    void BOOST_PP_CAT(apply_, BOOST_PP_CAT(contract, BOOST_PP_CAT(_,action) ) )(apply_context&)
   
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, newaccount );
   // void apply_gax_newaccount(apply_context&);
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, updateauth );
   //void apply_gax_updateauth(apply_context&);
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, deleteauth );
   //void apply_gax_deleteauth(apply_context&);
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, linkauth );
   // void apply_gax_linkauth(apply_context&);
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, unlinkauth );
   // void apply_gax_unlinkauth(apply_context&);

   /*
   void apply_eosio_postrecovery(apply_context&);
   void apply_eosio_passrecovery(apply_context&);
   void apply_eosio_vetorecovery(apply_context&);
   */
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, setcode );
   //void apply_gax_setcode(apply_context&);
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, setabi );
   //void apply_gax_setabi(apply_context&);
   NATIVE_ACTION_HANDLER( NATIVE_HANDLER_TAG, canceldelay );
   //void apply_gax_canceldelay(apply_context&);
   ///@}  end action handlers

} } /// namespace eosio::chain
