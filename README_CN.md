1. ## 联盟链模板节点配置说明

2. ### 节点程序相关的配置

   ```shell
   #在编译软件时，根据品牌需求修改相应配置文件，包括公钥前缀、系统账户
   ```

3. #### 节点账户相关的配置文件

   ```c++
   //这个配置文件负责配置节点核心模块和单元测试模块的系统账户，以及action处理句柄标签system_config.hpp
   system_config.hpp
   //相关配置项，默认值
   #define SYSTEM_ACCOUNT_NAME "uni"_n
   #define SYSTEM_TOKEN_ACCOUNT_NAME "uni.token"_n
   #define SYSTEM_MULTISIG_ACCOUNT_NAME "uni.msig"_n
   #define SYSTEM_WRAP_ACCOUNT_NAME "uni.wrap"_n
   #define SYSTEM_CONTRACT_NAME "uni.system"_n
   #define SYSTEM_NULL_ACCOUNT_NAME "uni.null"_n
   #define SYSTEM_PRODUCERS_ACCOUNT_NAME "uni.prods"_n
   #define SYSTEM_AUTH_SCOPE_NAME "uni.auth"_n
   #define SYSTEM_ALL_SCOPE_NAME "uni.all"_n
   #define SYSTEM_ANY_ACCOUNT_NAME "uni.any"_n
   #define SYSTEM_CODE_ACCOUNT_NAME "uni.code"_n
   #define SYSTEM_RAM_ACCOUNT_NAME  "uni.ram"_n
   #define SYSTEM_RAMFEE_ACCOUNT_NAME "uni.ramfee"_n
   #define SYSTEM_STAKE_ACCOUNT_NAME "uni.stake"_n
   #define SYSTEM_BPAY_ACCOUNT_NAME "uni.bpay"_n
   #define SYSTEM_VPAY_ACCOUNT_NAME "uni.vpay"_n
   #define SYSTEM_SAVING_ACCOUNT_NAME "uni.saving"_n
   #define SYSTEM_NAMES_ACCOUNT_NAME "uni.names"_n
   #define SYSTEM_REX_ACCOUNT_NAME "uni.rex"_n
   #define DEFAULT_WALLET_DIR "uni-wallet"
   //本机方法处理句柄标签
   #define NATIVE_HANDLER_TAG uni
   ```

4. #### 节点密码学模块公钥前缀配置文件

   ```c++
   //这个配置文件模板化指定公钥的前缀,比如uni、EOS、UNION
   system_config.hpp
   //相关默认配置
   #define PUBLIC_KEY_LEGACY_PREFIX "uni"
   ```

5. #### 整体测试框架python配置文件

   ```python
   #这个配置文件负责配置系统测试框架的系统账户、钱包目录等
   system_config.py
   #相关默认配置
   SYSTEM_ACCOUNT_NAME = "uni"
   SYSTEM_TOKEN_ACCOUNT_NAME = "uni.token"
   SYSTEM_MULTISIG_ACCOUNT_NAME = "uni.msig"
   SYSTEM_BIOS_ACCOUNT_NAME = "uni.bios"
   SYSTEM_WRAP_ACCOUNT_NAME = "uni.wrap"
   SYSTEM_BPAY_ACCOUNT_NAME = "uni.bpay"
   SYSTEM_VPAY_ACCOUNT_NAME = "uni.vpay"
   SYSTEM_NAMES_ACCOUNT_NAME = "uni.names"
   SYSTEM_RAM_ACCOUNT_NAME   = "uni.ram"
   SYSTEM_RAMFEE_ACCOUNT_NAME = "uni.ramfee"
   SYSTEM_SAVING_ACCOUNT_NAME = "uni.saving"
   SYSTEM_STAKE_ACCOUNT_NAME  = "uni.stake"
   SYSTEM_CONTRACT_NAME = "uni.system"
   SYSTEM_NULL_ACCOUNT_NAME = "uni.null"
   SYSTEM_PRODUCERS_ACCOUNT_NAME = "uni.prods"
   SYSTEM_AUTH_SCOPE_NAME = "uni.auth"
   SYSTEM_ALL_SCOPE_NAME = "uni.all"
   SYSTEM_ANY_ACCOUNT_NAME = "uni.any"
   SYSTEM_CODE_ACCOUNT_NAME = "uni.code"
   DEFAULT_WALLET_DIR = "uni-wallet"
   ```

6. #### CMake编译模块的配置文件

   ```cmake
   #这个配置文件用来配置系统核心代币符号，生成的应用名称，默认根密钥
   config.cmake
   #默认配置项,制定编译后应用程序名称
   set( CLI_CLIENT_EXECUTABLE_NAME unicli )
   set( NODE_EXECUTABLE_NAME uninod )
   set( KEY_STORE_EXECUTABLE_NAME unikey )
   set( LEAP_UTIL_EXECUTABLE_NAME uni-util )
   set( ROOT_EOSIO_ROOT_KEY "UNI6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV")
   set( ROOT_CORE_SYMBOL_NAME "UN" )
   ```

7. #### 单元测试合约的配置

   ```c++
   //这个配置文件用来配置延时交易测试合约
   deferred_test_config.hpp.in
   //默认配置项
   #define NOTIFY_ACTION "uni::onerror"
   //代理合约配置文件
   proxy_config.hpp.in
   //默认配置项
   #define TOKEN_ACCOUNT "uni.token"_n
   //api合约配置文件
   test_api_config.hpp.in
   //默认配置项
   #define SYSTEM_ACCOUNT "uni"_n
   //失败处理合约配置文件
   reject_all_config.hpp.in
   //默认配置项
   #define SYSTEM_ACCOUNT "uni"_n
   ```

8. #### 配置文件的一致性

   ```shell
   #在对上述3，4，5，6项配置文件进行编译前配置的时候需要确保系统账户前缀保持一致，公钥前缀保持一致。
   ```

9. ### 系统合约和CDT相关配置

   ```c++
   //系统合约eosio.system里面有静态指定的系统账户名称，需要修改成跟上述名称一致
   eosio.system.hpp
   rex.cpp
   //CDT需要配置相应的应用程序名称
   ```

   

