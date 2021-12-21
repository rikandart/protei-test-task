// local
#include "parser.h"
// gtest
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define ANSI_TXT_GRN "\033[0;32m"
#define ANSI_TXT_MGT "\033[0;35m" //Magenta
#define ANSI_TXT_DFT "\033[0;0m" //Console default
#define GTEST_BOX "[     cout ] "
#define COUT_GTEST ANSI_TXT_GRN << GTEST_BOX //You could add the Default
#define COUT_GTEST_MGT COUT_GTEST << ANSI_TXT_MGT

TEST(ParserTest, SplitTestTrue1){
	std::string str = "21 first, 25 second, 67 third, 87 fourth";
	std::string output;
	Parser p;
	bool res = p.parse(str, ' ', output);
	EXPECT_TRUE(res);
	std::cout << COUT_GTEST_MGT << "output = " << output << ANSI_TXT_DFT << std::endl;
}

TEST(ParserTest, SplitTestTrue2){
	std::string str = "first, 25 second, 67 third, 87";
	std::string output;
	Parser p;
	bool res = p.parse(str, ' ', output);
	EXPECT_TRUE(res);
	std::cout << COUT_GTEST_MGT << "output = " << output << ANSI_TXT_DFT << std::endl;
}

TEST(ParserTest, SplitTestFalse1){
	std::string str = "no number message";
	std::string output;
	Parser p;
	bool res = p.parse(str, ' ', output);
	EXPECT_FALSE(res);
	std::cout << COUT_GTEST_MGT << "output = " << output << ANSI_TXT_DFT << std::endl;
}

TEST(ParserTest, SplitTestFalse2){
	std::string str = "message";
	std::string output;
	Parser p;
	bool res = p.parse(str, ' ', output);
	EXPECT_FALSE(res);
	std::cout << COUT_GTEST_MGT << "output = " << output << ANSI_TXT_DFT << std::endl;
}

TEST(ParserTest, ParseTwoStrings){
	std::string str = "21 first, 25 second, 67 third, 87 fourth";
	std::string output;
	Parser p;
	bool res = p.parse(str, ' ', output);
	EXPECT_TRUE(res);
	std::cout << COUT_GTEST_MGT << "output = " << output << ANSI_TXT_DFT << std::endl;
	str = "first, 25 second, 67 third, 87 fourth";
	res = p.parse(str, ' ', output);
	EXPECT_TRUE(res);
	std::cout << COUT_GTEST_MGT << "output = " << output << ANSI_TXT_DFT << std::endl;
}

int main(int argc, char** argv){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}