/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/
#include "hphp/util/numa.h"

#ifdef HAVE_NUMA
#include "hphp/util/portability.h"
#include <folly/Bits.h>
extern "C" {
HHVM_ATTRIBUTE_WEAK extern void numa_init();
}
#endif

namespace HPHP {

// Treat the system as having a single NUMA node if HAVE_NUMA not defined.
// Otherwise, initNuma() will calculate the number of allowed NUMA nodes.
uint32_t numa_num_nodes = 1;

#ifdef HAVE_NUMA

uint32_t numa_node_set;
uint32_t numa_node_mask;
std::vector<bitmask*> node_to_cpu_mask;
bool use_numa = false;

void initNuma() {
  if (getenv("HHVM_DISABLE_NUMA")) {
    return;
  }
  // When linked dynamically numa_init() is called before JEMallocInitializer()
  // numa_init is not exported by libnuma.so so it will be NULL
  // however when linked statically numa_init() is not guaranteed to be called
  // before JEMallocInitializer(), so call it here.
  if (&numa_init) {
    numa_init();
  }
  if (numa_available() < 0) return;

  // set interleave for early code. we'll then force interleave for a few
  // regions, and switch to local for the threads
  numa_set_interleave_mask(numa_all_nodes_ptr);

  int max_node = numa_max_node();
  if (max_node >= 32) return;

  bool ret = true;
  bitmask* run_nodes = numa_get_run_node_mask();
  bitmask* mem_nodes = numa_get_mems_allowed();
  numa_num_nodes = 0;
  for (int i = 0; i <= max_node; i++) {
    if (!numa_bitmask_isbitset(run_nodes, i) ||
        !numa_bitmask_isbitset(mem_nodes, i)) {
      // Only deal with the case of a contiguous set of nodes where we can
      // run/allocate memory on each node.
      ret = false;
      break;
    }
    numa_node_set |= 1u << i;
    numa_num_nodes++;
  }
  numa_bitmask_free(run_nodes);
  numa_bitmask_free(mem_nodes);

  if (!ret || numa_num_nodes <= 1) return;

  numa_node_mask = folly::nextPowTwo(numa_num_nodes) - 1;
}

uint32_t next_numa_node(std::atomic<uint32_t>& curr_node) {
  if (!use_numa) return 0;
  uint32_t node;
  do {
    node = curr_node.fetch_add(1u, std::memory_order_relaxed);
    node &= numa_node_mask;
  } while (!((numa_node_set >> node) & 1));
  return node;
}

void numa_interleave(void* start, size_t size) {
  if (!use_numa) return;
  numa_interleave_memory(start, size, numa_all_nodes_ptr);
}

void numa_bind_to(void* start, size_t size, uint32_t node) {
  if (!use_numa) return;
  numa_tonode_memory(start, size, (int)node);
}

#endif
}
