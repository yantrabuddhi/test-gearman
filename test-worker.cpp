//#include "gear_config.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <signal.h>

#include <libgearman/gearman.h>
//#include <boost/program_options.hpp>
static gearman_return_t worker_function(gearman_job_st *job, void *context)
{
  const char *workload= (const char *)gearman_job_workload(job);
  const size_t workload_size= gearman_job_workload_size(job);

  std::cout << "Recieved " << workload_size << " bytes" << std::endl;
////  
  char *result=new char[workload_size];
  memcpy(result,workload,workload_size);
  
  /*
   *if (gearman_failed(gearman_job_send_data(job, &result[y], 1)))
      {
        return GEARMAN_ERROR;
      }
    if (gearman_failed(gearman_job_send_status(job, (uint32_t)y, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }

      // Notice that we send based on y divided by zero.
      if (gearman_failed(gearman_job_send_status(job, (uint32_t)workload_size, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }
   */
    if (gearman_failed(gearman_job_send_data(job, &result[0], workload_size)))
    {
      return GEARMAN_ERROR;
    }
   
}

int main()
{
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    std::cerr << "signal:" << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }
  gearman_worker_st *worker;
  if ((worker= gearman_worker_create(NULL)) == NULL)
  {
    std::cerr << "Memory allocation failure on worker creation." << std::endl;
    return EXIT_FAILURE;
  }

  //if (unique)
  //{
    gearman_worker_add_options(worker, GEARMAN_WORKER_GRAB_UNIQ);
  //}

  int timeout=-1;
  //
  if (timeout >= 0)
  {
    gearman_worker_set_timeout(worker, timeout);
  }

  if (gearman_failed(gearman_worker_add_server(worker, "localhost", GEARMAN_DEFAULT_TCP_PORT)))
  {
    std::cerr << gearman_worker_error(worker) << std::endl;
    return EXIT_FAILURE;
  }
  if (gearman_failed(gearman_worker_set_identifier(worker, "worker1", strlen("worker1"))))
  {
    std::cerr << gearman_worker_error(worker) << std::endl;
    return EXIT_FAILURE;
  }
  gearman_function_t worker_fn= gearman_function_create(worker_function);
  if (gearman_failed(gearman_worker_define_function(worker,
                                                    gearman_literal_param("make_call"),
                                                    worker_fn,
                                                    0, 
                                                    NULL/*&options*/)))
  {
    std::cerr << gearman_worker_error(worker) << std::endl;
    return EXIT_FAILURE;
  }

  int limit=0;
  // Add one if count is not zero
  if (limit != 0)
  {
    limit++;
  }

  while (--limit)
  {
    if (gearman_failed(gearman_worker_work(worker)))
    {
      std::cerr << gearman_worker_error(worker) << std::endl;
      break;
    }
  }

  gearman_worker_free(worker);

  return EXIT_SUCCESS;

}
