//#include "gear_config.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <libgearman/gearman.h>

//#include "util/string.hpp"
//#include "gearmand/error.hpp"
//#define std::cerr<< std::cerr<<

int main()
{
  gearman_client_st client;
  if (gearman_client_create(&client) == NULL)
  {
    std::cerr << "Memory allocation failure on client creation" << std::endl;
    return EXIT_FAILURE;
  }
  //
  int timeout=-1;
  //
  if (timeout >= 0)
  {
    gearman_client_set_timeout(&client, timeout);
  }

  gearman_return_t ret= gearman_client_add_server(&client, "localhost", GEARMAN_DEFAULT_TCP_PORT);
  if (ret != GEARMAN_SUCCESS)
  {
    std::cerr<<(gearman_client_error(&client));
    return EXIT_FAILURE;
  }
  if (gearman_failed(gearman_client_set_identifier(&client, "client1", strlen("client1"))))
  {
    std::cerr<<(gearman_client_error(&client));
    return EXIT_FAILURE;
  }
  int exit_code= EXIT_SUCCESS;
  int count=1; //if status is to be returned then don't change count on status receive
  do
  {
    size_t result_size;
    char *result;
    result= (char *)gearman_client_do(&client, "make_call", NULL,
                                      "scheme-string", strlen("scheme-string"),
                                      &result_size, &ret);
    if (ret == GEARMAN_WORK_DATA)
    {
      std::cout.write(result, result_size);

      free(result);
      continue;
    }
    else if (ret == GEARMAN_WORK_STATUS)
    {
      uint32_t numerator;
      uint32_t denominator;

      gearman_client_do_status(&client, &numerator, &denominator);
      std::clog << "Status: " << numerator << "/" << denominator << std::endl;
      continue;
    }
    else if (ret == GEARMAN_SUCCESS)
    {
      std::cout.write(result, result_size);
      free(result);
    }
    else if (ret == GEARMAN_WORK_FAIL)
    {
      std::cerr<<("Work failed");
      exit_code= EXIT_FAILURE;
      break;
    }
    else
    {
      std::cerr<<(gearman_client_error(&client));
      exit_code= EXIT_FAILURE;
      break;
    }

    --count;

  } while (count);

  gearman_client_free(&client);

  return exit_code;
}
