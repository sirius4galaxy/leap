#include <trx_provider.hpp>
#include <trx_generator.cpp>
#include <eosio/chain/system_config.hpp>
#define BOOST_TEST_MODULE trx_generator_tests
#include <boost/test/included/unit_test.hpp>

using namespace eosio::testing;
using namespace eosio::chain;
struct simple_tps_monitor {
   std::vector<tps_test_stats> _calls;
   bool monitor_test(const tps_test_stats& stats) {
      _calls.push_back(stats);
      return true;
   }

   simple_tps_monitor(size_t expected_num_calls) { _calls.reserve(expected_num_calls); }
};

struct mock_trx_generator {
   std::vector<fc::time_point> _calls;
   std::chrono::microseconds _delay;

   bool setup() {return true;}
   bool tear_down() {return true;}

   bool generate_and_send() {
      _calls.push_back(fc::time_point::now());
      if (_delay.count() > 0) {
         std::this_thread::sleep_for(_delay);
      }
      return true;
   }

   bool stop_on_trx_fail() {
      return false;
   }

   mock_trx_generator(size_t expected_num_calls, uint32_t delay=0) :_calls(), _delay(delay) {
      _calls.reserve(expected_num_calls);
   }
};

BOOST_AUTO_TEST_SUITE(trx_generator_tests)

BOOST_AUTO_TEST_CASE(tps_short_run_low_tps)
{
   constexpr uint32_t test_duration_s = 5;
   constexpr uint32_t test_tps = 5;
   constexpr uint32_t expected_trxs = test_duration_s * test_tps;
   constexpr uint64_t expected_runtime_us = test_duration_s * 1000000;
   constexpr uint64_t allowable_runtime_deviation_per = 20;
   constexpr uint64_t allowable_runtime_deviation_us = expected_runtime_us / allowable_runtime_deviation_per;
   constexpr uint64_t minimum_runtime_us = expected_runtime_us - allowable_runtime_deviation_us;
   constexpr uint64_t maximum_runtime_us = expected_runtime_us + allowable_runtime_deviation_us;

   std::shared_ptr<mock_trx_generator> generator = std::make_shared<mock_trx_generator>(expected_trxs);
   std::shared_ptr<simple_tps_monitor> monitor = std::make_shared<simple_tps_monitor>(expected_trxs);

   trx_tps_tester<mock_trx_generator, simple_tps_monitor> t1(generator, monitor, {test_duration_s, test_tps});

   fc::time_point start = fc::time_point::now();
   t1.run();
   fc::time_point end = fc::time_point::now();
   fc::microseconds runtime_us = end.time_since_epoch() - start.time_since_epoch();

   BOOST_REQUIRE_EQUAL(generator->_calls.size(), expected_trxs);
   BOOST_REQUIRE_GT(runtime_us.count(), minimum_runtime_us);
   BOOST_REQUIRE_LT(runtime_us.count(), maximum_runtime_us);
}

BOOST_AUTO_TEST_CASE(tps_short_run_high_tps)
{
   constexpr uint32_t test_duration_s = 5;
   constexpr uint32_t test_tps = 50000;
   constexpr uint32_t expected_trxs = test_duration_s * test_tps;
   constexpr uint64_t expected_runtime_us = test_duration_s * 1000000;
   constexpr uint64_t allowable_runtime_deviation_per = 20;
   constexpr uint64_t allowable_runtime_deviation_us = expected_runtime_us / allowable_runtime_deviation_per;
   constexpr int64_t minimum_runtime_us = expected_runtime_us - allowable_runtime_deviation_us;
   constexpr int64_t maximum_runtime_us = expected_runtime_us + allowable_runtime_deviation_us;

   std::shared_ptr<mock_trx_generator> generator = std::make_shared<mock_trx_generator>(expected_trxs);
   std::shared_ptr<simple_tps_monitor> monitor = std::make_shared<simple_tps_monitor>(expected_trxs);

   trx_tps_tester<mock_trx_generator, simple_tps_monitor> t1(generator, monitor, {test_duration_s, test_tps});

   fc::time_point start = fc::time_point::now();
   t1.run();
   fc::time_point end = fc::time_point::now();
   fc::microseconds runtime_us = end.time_since_epoch() - start.time_since_epoch();

   BOOST_REQUIRE_EQUAL(generator->_calls.size(), expected_trxs);
   BOOST_REQUIRE_GT(runtime_us.count(), minimum_runtime_us);

   if (runtime_us.count() > maximum_runtime_us) {
      ilog("couldn't sustain transaction rate.  ran ${rt}us vs expected max ${mx}us",
           ("rt", runtime_us.count())("mx", maximum_runtime_us));
      BOOST_REQUIRE_LT(monitor->_calls.back().time_to_next_trx_us, 0);
   }
}

BOOST_AUTO_TEST_CASE(tps_short_run_med_tps_med_delay)
{
   constexpr uint32_t test_duration_s = 5;
   constexpr uint32_t test_tps = 10000;
   constexpr uint32_t trx_delay_us = 10;
   constexpr uint32_t expected_trxs = test_duration_s * test_tps;
   constexpr uint64_t expected_runtime_us = test_duration_s * 1000000;
   constexpr uint64_t allowable_runtime_deviation_per = 20;
   constexpr uint64_t allowable_runtime_deviation_us = expected_runtime_us / allowable_runtime_deviation_per;
   constexpr int64_t minimum_runtime_us = expected_runtime_us - allowable_runtime_deviation_us;
   constexpr int64_t maximum_runtime_us = expected_runtime_us + allowable_runtime_deviation_us;

   std::shared_ptr<mock_trx_generator> generator = std::make_shared<mock_trx_generator>(expected_trxs, trx_delay_us);
   std::shared_ptr<simple_tps_monitor> monitor = std::make_shared<simple_tps_monitor>(expected_trxs);

   trx_tps_tester<mock_trx_generator, simple_tps_monitor> t1(generator, monitor, {test_duration_s, test_tps});

   fc::time_point start = fc::time_point::now();
   t1.run();
   fc::time_point end = fc::time_point::now();
   fc::microseconds runtime_us = end.time_since_epoch() - start.time_since_epoch();

   BOOST_REQUIRE_EQUAL(generator->_calls.size(), expected_trxs);
   BOOST_REQUIRE_GT(runtime_us.count(), minimum_runtime_us);

   if (runtime_us.count() > maximum_runtime_us) {
      ilog("couldn't sustain transaction rate.  ran ${rt}us vs expected max ${mx}us",
           ("rt", runtime_us.count())("mx", maximum_runtime_us));
      BOOST_REQUIRE_LT(monitor->_calls.back().time_to_next_trx_us, 0);
   }
}

BOOST_AUTO_TEST_CASE(tps_med_run_med_tps_med_delay)
{
   constexpr uint32_t test_duration_s = 30;
   constexpr uint32_t test_tps = 10000;
   constexpr uint32_t trx_delay_us = 10;
   constexpr uint32_t expected_trxs = test_duration_s * test_tps;
   constexpr uint64_t expected_runtime_us = test_duration_s * 1000000;
   constexpr uint64_t allowable_runtime_deviation_per = 20;
   constexpr uint64_t allowable_runtime_deviation_us = expected_runtime_us / allowable_runtime_deviation_per;
   constexpr int64_t minimum_runtime_us = expected_runtime_us - allowable_runtime_deviation_us;
   constexpr int64_t maximum_runtime_us = expected_runtime_us + allowable_runtime_deviation_us;

   std::shared_ptr<mock_trx_generator> generator = std::make_shared<mock_trx_generator>(expected_trxs, trx_delay_us);
   std::shared_ptr<simple_tps_monitor> monitor = std::make_shared<simple_tps_monitor>(expected_trxs);

   trx_tps_tester<mock_trx_generator, simple_tps_monitor> t1(generator, monitor, {test_duration_s, test_tps});

   fc::time_point start = fc::time_point::now();
   t1.run();
   fc::time_point end = fc::time_point::now();
   fc::microseconds runtime_us = end.time_since_epoch() - start.time_since_epoch();

   BOOST_REQUIRE_EQUAL(generator->_calls.size(), expected_trxs);
   BOOST_REQUIRE_GT(runtime_us.count(), minimum_runtime_us);

   if (runtime_us.count() > maximum_runtime_us) {
      ilog("couldn't sustain transaction rate.  ran ${rt}us vs expected max ${mx}us",
           ("rt", runtime_us.count())("mx", maximum_runtime_us));
      BOOST_REQUIRE_LT(monitor->_calls.back().time_to_next_trx_us, 0);
   }
}

BOOST_AUTO_TEST_CASE(tps_cant_keep_up)
{
   constexpr uint32_t test_duration_s = 5;
   constexpr uint32_t test_tps = 100000;
   constexpr uint32_t trx_delay_us = 10;
   constexpr uint32_t expected_trxs = test_duration_s * test_tps;
   constexpr uint64_t expected_runtime_us = test_duration_s * 1000000;
   constexpr uint64_t allowable_runtime_deviation_per = 20;
   constexpr uint64_t allowable_runtime_deviation_us = expected_runtime_us / allowable_runtime_deviation_per;
   constexpr int64_t minimum_runtime_us = expected_runtime_us - allowable_runtime_deviation_us;
   constexpr int64_t maximum_runtime_us = expected_runtime_us + allowable_runtime_deviation_us;

   std::shared_ptr<mock_trx_generator> generator = std::make_shared<mock_trx_generator>(expected_trxs, trx_delay_us);
   std::shared_ptr<simple_tps_monitor> monitor = std::make_shared<simple_tps_monitor>(expected_trxs);

   trx_tps_tester<mock_trx_generator, simple_tps_monitor> t1(generator, monitor, {test_duration_s, test_tps});

   fc::time_point start = fc::time_point::now();
   t1.run();
   fc::time_point end = fc::time_point::now();
   fc::microseconds runtime_us = end.time_since_epoch() - start.time_since_epoch();

   BOOST_REQUIRE_EQUAL(generator->_calls.size(), expected_trxs);
   BOOST_REQUIRE_GT(runtime_us.count(), minimum_runtime_us);

   if (runtime_us.count() > maximum_runtime_us) {
      ilog("couldn't sustain transaction rate.  ran ${rt}us vs expected max ${mx}us",
           ("rt", runtime_us.count())("mx", maximum_runtime_us));
      BOOST_REQUIRE_LT(monitor->_calls.back().time_to_next_trx_us, 0);
   }
}

BOOST_AUTO_TEST_CASE(tps_med_run_med_tps_30us_delay)
{
   constexpr uint32_t test_duration_s = 15;
   constexpr uint32_t test_tps = 3000;
   constexpr uint32_t trx_delay_us = 30;
   constexpr uint32_t expected_trxs = test_duration_s * test_tps;
   constexpr uint64_t expected_runtime_us = test_duration_s * 1000000;
   constexpr uint64_t allowable_runtime_deviation_per = 20;
   constexpr uint64_t allowable_runtime_deviation_us = expected_runtime_us / allowable_runtime_deviation_per;
   constexpr int64_t minimum_runtime_us = expected_runtime_us - allowable_runtime_deviation_us;
   constexpr int64_t maximum_runtime_us = expected_runtime_us + allowable_runtime_deviation_us;

   std::shared_ptr<mock_trx_generator> generator = std::make_shared<mock_trx_generator>(expected_trxs, trx_delay_us);
   std::shared_ptr<simple_tps_monitor> monitor = std::make_shared<simple_tps_monitor>(expected_trxs);

   trx_tps_tester<mock_trx_generator, simple_tps_monitor> t1(generator, monitor, {test_duration_s, test_tps});

   fc::time_point start = fc::time_point::now();
   t1.run();
   fc::time_point end = fc::time_point::now();
   fc::microseconds runtime_us = end.time_since_epoch() - start.time_since_epoch();

   BOOST_REQUIRE_EQUAL(generator->_calls.size(), expected_trxs);
   BOOST_REQUIRE_GT(runtime_us.count(), minimum_runtime_us);

   if (runtime_us.count() > maximum_runtime_us) {
      ilog("couldn't sustain transaction rate.  ran ${rt}us vs expected max ${mx}us",
           ("rt", runtime_us.count())("mx", maximum_runtime_us));
      BOOST_REQUIRE_LT(monitor->_calls.back().time_to_next_trx_us, 0);
   }
}

BOOST_AUTO_TEST_CASE(tps_performance_monitor_during_spin_up)
{
   tps_test_stats stats;
   tps_performance_monitor monitor{std::chrono::microseconds(5s).count()};
   stats.total_trxs = 1000;
   stats.start_time = fc::time_point{fc::microseconds{0}};
   stats.expected_sent = 100;
   stats.trxs_sent = 90;

   // behind, but still within spin up window
   stats.last_run = fc::time_point{fc::microseconds{100000}};
   BOOST_REQUIRE(monitor.monitor_test(stats));

   // violation, but still within spin up window
   stats.last_run = fc::time_point{fc::microseconds{1100000}};
   BOOST_REQUIRE(monitor.monitor_test(stats));
}

BOOST_AUTO_TEST_CASE(tps_performance_monitor_outside_spin_up)
{
   tps_test_stats stats;
   tps_performance_monitor monitor{std::chrono::microseconds(5s).count()};
   stats.total_trxs = 1000;
   stats.start_time = fc::time_point{fc::microseconds{0}};
   stats.expected_sent = 100;
   stats.trxs_sent = 90;

   // behind, out of spin up window
   stats.last_run = fc::time_point{fc::microseconds{5500000}};
   BOOST_REQUIRE(monitor.monitor_test(stats));

   // violation, out of spin up window
   stats.last_run = fc::time_point{fc::microseconds{6600000}};
   BOOST_REQUIRE(!monitor.monitor_test(stats));
}

BOOST_AUTO_TEST_CASE(tps_performance_monitor_outside_spin_up_within_limit)
{
   tps_test_stats stats;
   tps_performance_monitor monitor{std::chrono::microseconds(5s).count()};
   stats.total_trxs = 1000;
   stats.start_time = fc::time_point{fc::microseconds{0}};
   stats.expected_sent = 100;
   stats.trxs_sent = 90;

   // outside of limit,  out of spin up window
   stats.last_run = fc::time_point{fc::microseconds{5500000}};
   BOOST_REQUIRE(monitor.monitor_test(stats));

   // outside of limit, less than max violation duration
   stats.last_run = fc::time_point{fc::microseconds{6000000}};
   BOOST_REQUIRE(monitor.monitor_test(stats));

   stats.trxs_sent = 98;
   // behind, but within limit, out of spin up window
   stats.last_run = fc::time_point{fc::microseconds{6600000}};
   BOOST_REQUIRE(monitor.monitor_test(stats));

   stats.expected_sent = 150;
   // outside of limit again, out of spin up window
   stats.last_run = fc::time_point{fc::microseconds{7000000}};
   BOOST_REQUIRE(monitor.monitor_test(stats));

   // outside of limit for too long
   stats.last_run = fc::time_point{fc::microseconds{8100000}};
   BOOST_REQUIRE(!monitor.monitor_test(stats));
}

BOOST_AUTO_TEST_CASE(tps_cant_keep_up_monitored)
{
   constexpr uint32_t test_duration_s = 5;
   constexpr uint32_t test_tps = 100000;
   constexpr uint32_t trx_delay_us = 10;
   constexpr uint32_t expected_trxs = test_duration_s * test_tps;
   constexpr uint64_t expected_runtime_us = test_duration_s * 1000000;

   std::shared_ptr<mock_trx_generator> generator = std::make_shared<mock_trx_generator>(expected_trxs, trx_delay_us);
   std::shared_ptr<tps_performance_monitor> monitor = std::make_shared<tps_performance_monitor>();

   trx_tps_tester<mock_trx_generator, tps_performance_monitor> t1(generator, monitor, {test_duration_s, test_tps});

   fc::time_point start = fc::time_point::now();
   t1.run();
   fc::time_point end = fc::time_point::now();
   fc::microseconds runtime_us = end.time_since_epoch() - start.time_since_epoch();

   BOOST_REQUIRE_LT(runtime_us.count(), expected_runtime_us);
   BOOST_REQUIRE_LT(generator->_calls.size(), expected_trxs);
}

BOOST_AUTO_TEST_CASE(trx_generator_constructor)
{
   trx_generator_base_config tg_config{1, chain::chain_id_type("999"), SYSTEM_ACCOUNT_NAME, fc::seconds(3600),
                                       fc::variant("00000062989f69fd251df3e0b274c3364ffc2f4fce73de3f1c7b5e11a4c92f21").as<chain::block_id_type>(), ".", true};
   provider_base_config p_config{"127.0.0.1", 9876};
   name tname(SYSTEM_TOKEN_ACCOUNT_NAME);
   const std::string abi_file = "../../unittests/contracts/"+tname.to_string()+"/"+tname.to_string()+".abi";
   const std::string actions_data = "[{\"actionAuthAcct\": \"testacct1\",\"actionName\": \"transfer\",\"authorization\": {\"actor\": \"testacct1\",\"permission\": \"active\"},"
                                    "\"actionData\": {\"from\": \"testacct1\",\"to\": \"testacct2\",\"quantity\": \"0.0001 CUR\",\"memo\": \"transaction specified\"}}]";
   name sname(SYSTEM_ACCOUNT_NAME);
   const std::string action_auths = "{\"testacct1\":\"5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3\",\"testacct2\":\"5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3\","
                                    +sname.to_string()+":"+"\"5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3\"}";
   user_specified_trx_config trx_config{abi_file, actions_data, action_auths};

   auto generator = trx_generator(tg_config, p_config, trx_config);
}

BOOST_AUTO_TEST_CASE(account_name_generator_tests)
{
   auto acct_gen = account_name_generator();
   BOOST_REQUIRE_EQUAL(acct_gen.calc_name(), "111111111111");

   //Test account name prefixes for differentiating between transaction generator instances
   acct_gen.setPrefix(1);
   BOOST_REQUIRE_EQUAL(acct_gen.calc_name(), "121111111111");
   acct_gen.setPrefix(30);
   BOOST_REQUIRE_EQUAL(acct_gen.calc_name(), "1z1111111111");
   acct_gen.setPrefix(31);
   BOOST_REQUIRE_EQUAL(acct_gen.calc_name(), "211111111111");
   acct_gen.setPrefix(960);
   BOOST_REQUIRE_EQUAL(acct_gen.calc_name(), "zz1111111111");

   //Test account name generation
   std::vector<std::string> expected = {
         "zz1111111111",
         "zz1111111112",
         "zz1111111113",
         "zz1111111114",
         "zz1111111115",
         "zz111111111a",
         "zz111111111b",
         "zz111111111c",
         "zz111111111d",
         "zz111111111e",
         "zz111111111f",
         "zz111111111g",
         "zz111111111h",
         "zz111111111i",
         "zz111111111j",
         "zz111111111k",
         "zz111111111l",
         "zz111111111m",
         "zz111111111n",
         "zz111111111o",
         "zz111111111p",
         "zz111111111q",
         "zz111111111r",
         "zz111111111s",
         "zz111111111t",
         "zz111111111u",
         "zz111111111v",
         "zz111111111w",
         "zz111111111x",
         "zz111111111y",
         "zz111111111z",
         "zz1111111121",
         "zz1111111122"};
   for(size_t i = 0; i < expected.size(); ++i) {
      BOOST_REQUIRE_EQUAL(acct_gen.calc_name(), expected.at(i));
      acct_gen.increment();
   }


   //Test account name generation starting at 31 ^ 5 - 1 = 28629150
   std::vector<std::string> expected2 = {
         "1211111zzzzz",
         "121111211111",
         "121111211112",
         "121111211113",
         "121111211114",
         "121111211115",
         "12111121111a",
         "12111121111b",
         "12111121111c",
         "12111121111d",
         "12111121111e",
         "12111121111f",
         "12111121111g",
         "12111121111h",
         "12111121111i",
         "12111121111j",
         "12111121111k",
         "12111121111l",
         "12111121111m",
         "12111121111n",
         "12111121111o",
         "12111121111p",
         "12111121111q",
         "12111121111r",
         "12111121111s",
         "12111121111t",
         "12111121111u",
         "12111121111v",
         "12111121111w",
         "12111121111x",
         "12111121111y",
         "12111121111z",
         "121111211121",
         "121111211122"};
   auto acct_gen2 = account_name_generator();
   acct_gen2.setPrefix(1);
   int initialVal = 28629150;
   for(int i = 0; i < initialVal; ++i) {
      acct_gen2.increment();
   }
   for(size_t i = 0; i < expected2.size(); ++i) {
      BOOST_REQUIRE_EQUAL(acct_gen2.calc_name(), expected2.at(i));
      acct_gen2.increment();
   }
}

BOOST_AUTO_TEST_SUITE_END()
