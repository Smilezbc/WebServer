#include "../../logging/Logger.h"
#include "../../logging/LogFile.h"
#include "../../Timestamp.h"
#include <stdio.h>

long g_total;
FILE* g_file;
boost::scoped_ptr<webServer::LogFile> g_logFile;

void dummyOutput(const char* msg, int len)
{
  g_total += len;
  if (g_file)
  {
    fwrite(msg, 1, len, g_file);
  }
  else if (g_logFile)
  {
    g_logFile->append(msg, len);
  }
}

void bench()
{
  webServer::Logger::setOutput(dummyOutput);
  webServer::Timestamp start(webServer::Timestamp::now());
  g_total = 0;

  const int batch = 1000*1000;
  const bool kLongLog = false;
  std::string empty = " ";
  std::string longStr(3000, 'X');
  longStr += " ";

  for (int i = 0; i < batch; ++i)
  {
    LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
             << (kLongLog ? longStr : empty)
             << i;
  }
  webServer::Timestamp end(webServer::Timestamp::now());
  double seconds = timeDifference(end, start);
  printf("%f seconds, %ld bytes, %.2f msg/s, %.2f MiB/s\n",
         seconds, g_total, batch / seconds, g_total / seconds / 1024 / 1024);
}

int main()
{
  getppid(); // for ltrace and strace
  LOG_TRACE << "trace";
  LOG_DEBUG << "debug";
  LOG_INFO << "Hello";
  LOG_WARN << "World";
  LOG_ERROR << "Error";
  LOG_INFO << sizeof(webServer::Logger);
  LOG_INFO << sizeof(webServer::LogStream);
  LOG_INFO << sizeof(webServer::Fmt);
  LOG_INFO << sizeof(webServer::LogStream::Buffer);

  bench();

  char buffer[64*1024];

  g_file = fopen("/dev/null", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench();
  fclose(g_file);

  g_file = fopen("/tmp/log", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench();
  fclose(g_file);

  g_file = NULL;
  g_logFile.reset(new webServer::LogFile("test_log", 500*1000*1000));
  bench();

  g_logFile.reset(new webServer::LogFile("test_log_mt", 500*1000*1000, true));
  bench();
  g_logFile.reset();
}
