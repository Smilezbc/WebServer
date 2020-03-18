#include "../../logging/LogFile.h"
#include "../../logging/Logger.h"

#include<cstring>
#include<string>

boost::scoped_ptr<webServer::LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
  g_logFile->append(msg, len);
}

void flushFunc()
{
  g_logFile->flush();
}

int main(int argc, char* argv[])
{
  char name[256];
  strncpy(name, argv[0], 256);
  g_logFile.reset(new webServer::LogFile(::basename(name), 256*1024));
  webServer::Logger::setOutput(outputFunc);
  webServer::Logger::setFlush(flushFunc);

  std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for (int i = 0; i < 10000; ++i)
  {
    LOG_INFO << line;

    usleep(1000);
  }
}
