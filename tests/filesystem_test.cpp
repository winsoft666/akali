#include "gtest/gtest.h"
#include "akali/filesystem.hpp"
#include <iostream>

TEST(FileSystemTest, Dir) {
  std::error_code ec;
  akali::filesystem::path p;
  EXPECT_TRUE(akali::filesystem::create_directories("d:\\test123\\test456", ec));
  std::cout << ec.message() << std::endl;
}

TEST(FileSystemTest, DirItr) {
  akali::filesystem::remove_all("D:\\__filesystem_test");
  akali::filesystem::create_directories("D:\\__filesystem_test\\test01");

  std::ofstream test1file("D:\\__filesystem_test\\test1.txt");
  EXPECT_TRUE(test1file.good());
  test1file << "test1 file";
  test1file.flush();
  test1file.close();

  std::ofstream test2file("D:\\__filesystem_test\\test2.txt");
  EXPECT_TRUE(test2file.good());
  test2file << "test2 file";
  test2file.flush();
  test2file.close();

  std::ofstream test3file("D:\\__filesystem_test\\test01\\test3.txt");
  EXPECT_TRUE(test3file.good());
  test3file << "test3 file";
  test3file.flush();
  test3file.close();

  akali::filesystem::recursive_directory_iterator itr("D:\\__filesystem_test");
  for (itr = akali::filesystem::begin(itr); itr != akali::filesystem::end(itr); itr++) {
    std::cout << itr->path().generic_string() << std::endl;
  }


  akali::filesystem::remove_all("D:\\__filesystem_test");
}