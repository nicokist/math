// these tests can only be compiled and executed with availability of
// MPI
#ifdef STAN_MPI

#include <stan/math/prim/arr.hpp>
#include <test/unit/math/prim/mat/functor/mpi_test_env.hpp>

#include <iostream>

MPI_TEST(mpi_cluster, chunk_mapping) {
  if (rank != 0)
    return;

  std::vector<int> small_load = stan::math::mpi_map_chunks(world_size - 1, 1);

  EXPECT_EQ(world_size, small_load.size());

  if (world_size > 1)
    EXPECT_EQ(0, small_load[0]);

  for (std::size_t i = 1; i < world_size; ++i)
    EXPECT_EQ(1, small_load[i]);

  std::vector<int> med_load = stan::math::mpi_map_chunks(world_size + 1, 2);

  EXPECT_EQ(world_size, med_load.size());

  if (world_size > 1) {
    EXPECT_EQ(2, med_load[0]);
    EXPECT_EQ(4, med_load[1]);
    for (std::size_t i = 2; i < world_size; ++i)
      EXPECT_EQ(2, med_load[i]);
  } else {
    EXPECT_EQ(4, med_load[0]);
  }
}

MPI_TEST(mpi_cluster, listen_state) {
  if (rank != 0)
    return;

  EXPECT_TRUE(stan::math::mpi_cluster::is_listening());
}

/**
 * simple distributed MPI apply (uses mpi_command implicitly) which
 * performs the basic communications which we need
 * (broadcast/scatter/gather).
 */
struct make_secret {
  static void distributed_apply() {
    boost::mpi::communicator world;
    double common;
    boost::mpi::broadcast(world, common, 0);
    double worker_value;
    boost::mpi::scatter(world, worker_value, 0);
    double worker_secret = common * worker_value;
    std::vector<double> secrets;
    boost::mpi::gather(world, worker_secret, secrets, 0);
  }
};

// register worker command
STAN_REGISTER_MPI_DISTRIBUTED_APPLY(make_secret)

MPI_TEST(mpi_cluster, communication) {
  if (rank != 0)
    return;

  boost::mpi::communicator world;

  std::unique_lock<std::mutex> cluster_lock;
  EXPECT_NO_THROW((cluster_lock = stan::math::mpi_broadcast_command<
                       stan::math::mpi_distributed_apply<make_secret>>()));

  EXPECT_TRUE(cluster_lock.owns_lock());

  // this one will fail
  std::unique_lock<std::mutex> cluster_lock_fail;
  EXPECT_THROW((cluster_lock_fail = stan::math::mpi_broadcast_command<
                    stan::math::mpi_distributed_apply<make_secret>>()),
               stan::math::mpi_is_in_use);

  EXPECT_FALSE(cluster_lock_fail.owns_lock());

  double common = 3.14;
  boost::mpi::broadcast(world, common, 0);

  std::vector<double> msgs(world_size, 0.0);

  for (std::size_t i = 0; i < world_size; ++i)
    msgs[i] = i;

  double root_value = -1;

  boost::mpi::scatter(world, msgs, root_value, 0);

  EXPECT_DOUBLE_EQ(msgs[0], root_value);

  double root_secret = common * root_value;

  std::vector<double> secrets(world_size);

  boost::mpi::gather(world, root_secret, secrets, 0);

  for (std::size_t i = 0; i < world_size; ++i)
    EXPECT_DOUBLE_EQ(common * msgs[i], secrets[i]);
}

#endif
