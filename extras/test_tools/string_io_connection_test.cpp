#include "extras/test_tools/string_io_connection.h"

#include "googletest/gtest.h"

namespace alpaca {
namespace test {
namespace {

TEST(StringIoConnectionTest, DoNothing) {
  StringIoConnection conn(3, "", false);
  EXPECT_TRUE(conn.connected());
  EXPECT_EQ(conn.sock_num(), 3);
  EXPECT_EQ(conn.available(), 0);
  EXPECT_EQ(conn.remaining_input(), "");
  EXPECT_EQ(conn.output(), "");
  EXPECT_FALSE(conn.peer_half_closed());
}

TEST(StringIoConnectionTest, WriteAndReadAndClose) {
  StringIoConnection conn(1, "DEFG", false);

  EXPECT_EQ(conn.print("ab"), 2);
  EXPECT_EQ(conn.print("cd"), 2);
  EXPECT_EQ(conn.output(), "abcd");

  EXPECT_EQ(conn.available(), 4);
  EXPECT_EQ(conn.read(), 'D');
  EXPECT_EQ(conn.available(), 3);
  uint8_t buf[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  EXPECT_EQ(conn.read(buf, 2), 2);
  EXPECT_EQ(buf[0], 'E');
  EXPECT_EQ(buf[1], 'F');
  EXPECT_EQ(buf[2], ' ');
  EXPECT_EQ(conn.available(), 1);

  // Make sure the connection doesn't do anything once closed.
  conn.close();
  EXPECT_FALSE(conn.connected());
  EXPECT_EQ(conn.available(), -1);
  EXPECT_EQ(conn.read(), -1);
  EXPECT_EQ(conn.read(buf, 10), -1);
  EXPECT_EQ(conn.print("xyz"), -1);

  EXPECT_EQ(conn.sock_num(), 1);
  EXPECT_EQ(conn.remaining_input(), "G");
  EXPECT_EQ(conn.output(), "abcd");
}

TEST(StringIoConnectionTest, ReadAll) {
  StringIoConnection conn(1, "DEFG", false);

  EXPECT_EQ(conn.available(), 4);
  uint8_t buf[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  EXPECT_EQ(conn.read(buf, 10), 4);
  EXPECT_EQ(buf[0], 'D');
  EXPECT_EQ(buf[1], 'E');
  EXPECT_EQ(buf[2], 'F');
  EXPECT_EQ(buf[3], 'G');
  EXPECT_EQ(buf[4], ' ');
  EXPECT_EQ(conn.available(), 0);
  EXPECT_FALSE(conn.peer_half_closed());
}

TEST(StringIoConnectionTest, ReadAllHalfClosed) {
  StringIoConnection conn(1, "DEFG", true);

  EXPECT_EQ(conn.available(), 4);
  uint8_t buf[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  EXPECT_EQ(conn.read(buf, 10), 4);
  EXPECT_EQ(buf[0], 'D');
  EXPECT_EQ(buf[1], 'E');
  EXPECT_EQ(buf[2], 'F');
  EXPECT_EQ(buf[3], 'G');
  EXPECT_EQ(buf[4], ' ');
  EXPECT_EQ(conn.available(), 0);
  EXPECT_TRUE(conn.peer_half_closed());
}

}  // namespace
}  // namespace test
}  // namespace alpaca
