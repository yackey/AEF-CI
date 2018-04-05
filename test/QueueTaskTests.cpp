#include <stdio.h>
#include <string>
#include <string.h>
#include "aefQueueTask.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;
using namespace std;

namespace {

std::string sNameSet("testtaskqueue");
int nPrioritySet = 22;
int nOptionSet = 55;
int nStackSizeSet = 10000;

class qtt : public CAefQueueTask
{
public:
    qtt() {};
    qtt(const char *name, int priority, int options, int stack) 
    { 
        SetTaskName(sNameSet.c_str()) ;
        SetTaskPriority(nPrioritySet) ;
        SetTaskOptions(nOptionSet) ;
        SetTaskStackSize(nStackSizeSet) ;
    };
};

// The fixture for testing class qtt.
class QueueTaskTest : public ::testing::Test {

 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  QueueTaskTest() 
    : qtWithArgs(sNameSet.c_str(), nPrioritySet, nOptionSet, nStackSizeSet)
  {
    // You can do set-up work for each test here.
  }

  virtual ~QueueTaskTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
  qtt qtNoArgs;
  qtt qtWithArgs;
};

TEST_F(QueueTaskTest, Create_WithNoArgs_PopulateWithDefaults) {
  EXPECT_EQ(0, qtNoArgs.GetQueueOptions().mq_flags);
  EXPECT_EQ(MAX_MQ_QUEUE_MESSAGE_SIZE, qtNoArgs.GetQueueOptions().mq_msgsize);
  EXPECT_EQ(MAX_MQ_QUEUE_MESSAGES, qtNoArgs.GetQueueOptions().mq_maxmsg);
  EXPECT_EQ(MSG_Q_ID_INVALID, qtNoArgs.GetQueueID());
  EXPECT_STREQ("", qtNoArgs.GetName().c_str());
}

TEST_F(QueueTaskTest, Create_WithArgs_PopulateTaskWithValues) {
  EXPECT_EQ(0, qtWithArgs.GetQueueOptions().mq_flags);
  EXPECT_EQ(MAX_MQ_QUEUE_MESSAGE_SIZE, qtWithArgs.GetQueueOptions().mq_msgsize);
  EXPECT_EQ(MAX_MQ_QUEUE_MESSAGES, qtWithArgs.GetQueueOptions().mq_maxmsg);
  EXPECT_EQ(MSG_Q_ID_INVALID, qtWithArgs.GetQueueID());
  EXPECT_STREQ("", qtWithArgs.GetName().c_str());

  EXPECT_STREQ(sNameSet.c_str(), qtWithArgs.GetTaskName());
  EXPECT_EQ(nPrioritySet, qtWithArgs.GetTaskPriority());
  EXPECT_EQ(nOptionSet, qtWithArgs.GetTaskOptions());
  EXPECT_EQ(nStackSizeSet, qtWithArgs.GetTaskStackSize());

}


}  // namespace
