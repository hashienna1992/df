#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-SHT");
    BOOST_CHECK(GetBoolArg("-SHT"));
    BOOST_CHECK(GetBoolArg("-SHT", false));
    BOOST_CHECK(GetBoolArg("-SHT", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BARo"));
    BOOST_CHECK(!GetBoolArg("-BARo", false));
    BOOST_CHECK(GetBoolArg("-BARo", true));

    ResetArgs("-SHT=0");
    BOOST_CHECK(!GetBoolArg("-SHT"));
    BOOST_CHECK(!GetBoolArg("-SHT", false));
    BOOST_CHECK(!GetBoolArg("-SHT", true));

    ResetArgs("-SHT=1");
    BOOST_CHECK(GetBoolArg("-SHT"));
    BOOST_CHECK(GetBoolArg("-SHT", false));
    BOOST_CHECK(GetBoolArg("-SHT", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-SHT"));
    BOOST_CHECK(!GetBoolArg("-SHT", false));
    BOOST_CHECK(!GetBoolArg("-SHT", true));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-SHT"));
    BOOST_CHECK(!GetBoolArg("-SHT", false));
    BOOST_CHECK(!GetBoolArg("-SHT", true));

    ResetArgs("-SHT -noBAR");  // -SHT should win
    BOOST_CHECK(GetBoolArg("-SHT"));
    BOOST_CHECK(GetBoolArg("-SHT", false));
    BOOST_CHECK(GetBoolArg("-SHT", true));

    ResetArgs("-SHT=1 -noBAR=1");  // -SHT should win
    BOOST_CHECK(GetBoolArg("-SHT"));
    BOOST_CHECK(GetBoolArg("-SHT", false));
    BOOST_CHECK(GetBoolArg("-SHT", true));

    ResetArgs("-SHT=0 -noBAR=0");  // -SHT should win
    BOOST_CHECK(!GetBoolArg("-SHT"));
    BOOST_CHECK(!GetBoolArg("-SHT", false));
    BOOST_CHECK(!GetBoolArg("-SHT", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--SHT=1");
    BOOST_CHECK(GetBoolArg("-SHT"));
    BOOST_CHECK(GetBoolArg("-SHT", false));
    BOOST_CHECK(GetBoolArg("-SHT", true));

    ResetArgs("--noBAR=1");
    BOOST_CHECK(!GetBoolArg("-SHT"));
    BOOST_CHECK(!GetBoolArg("-SHT", false));
    BOOST_CHECK(!GetBoolArg("-SHT", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-SHT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SHT", "eleven"), "eleven");

    ResetArgs("-SHT -bar");
    BOOST_CHECK_EQUAL(GetArg("-SHT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SHT", "eleven"), "");

    ResetArgs("-SHT=");
    BOOST_CHECK_EQUAL(GetArg("-SHT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SHT", "eleven"), "");

    ResetArgs("-SHT=11");
    BOOST_CHECK_EQUAL(GetArg("-SHT", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-SHT", "eleven"), "11");

    ResetArgs("-SHT=eleven");
    BOOST_CHECK_EQUAL(GetArg("-SHT", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-SHT", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-SHT", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-SHT", 0), 0);

    ResetArgs("-SHT -bar");
    BOOST_CHECK_EQUAL(GetArg("-SHT", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-SHT=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-SHT", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-SHT=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-SHT", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--SHT");
    BOOST_CHECK_EQUAL(GetBoolArg("-SHT"), true);

    ResetArgs("--SHT=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-SHT", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-SHT"));
    BOOST_CHECK(!GetBoolArg("-SHT", true));
    BOOST_CHECK(!GetBoolArg("-SHT", false));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-SHT"));
    BOOST_CHECK(!GetBoolArg("-SHT", true));
    BOOST_CHECK(!GetBoolArg("-SHT", false));

    ResetArgs("-noBAR=0");
    BOOST_CHECK(GetBoolArg("-SHT"));
    BOOST_CHECK(GetBoolArg("-SHT", true));
    BOOST_CHECK(GetBoolArg("-SHT", false));

    ResetArgs("-SHT --noBAR");
    BOOST_CHECK(GetBoolArg("-SHT"));

    ResetArgs("-noBAR -SHT"); // SHT always wins:
    BOOST_CHECK(GetBoolArg("-SHT"));
}

BOOST_AUTO_TEST_SUITE_END()
