// __BEGIN_LICENSE__
// Copyright (C) 2006-2009 United States Government as represented by
// the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
// __END_LICENSE__

#include <gtest/gtest.h>
#include <vw/Math/MatrixSparseSkyline.h>

using namespace vw;
using namespace vw::math;

static const double DELTA = 1e-5;

Vector<uint> create_test_skyline(unsigned size, int max_offset ) {
  Vector<uint> result(size);
  for ( uint i = 0; i < size; ++i ) {
    int offset = int(i) - rand()%max_offset;
    if ( offset < 0 ) offset = 0;
    result[i] = offset;
  }
  return result;
}

template <class VectorT>
void fill_vector(VectorT& b) {
  for ( unsigned i = 0; i < b.size(); ++i )
    b[i] = lround(double(random())/(pow(2,31)-1)*100)+1;
}

template <class MatrixT>
void fill_symmetric_matrix(MatrixT& A, Vector<uint> const& skyline ) {
  for (unsigned i = 0; i < A.rows(); ++i)
    for (unsigned j = skyline[i]; j < std::min(i+1,A.cols()); ++j) {
      A(i,j) = lround(double(random())/(pow(2,31)-1)*100)+1;
      A(j,i) = A(i,j);
    }
}

// Unoptimized LDLT decomposition
template <class MatrixT>
void ldl_decomposition(MatrixT& A) {
  VW_ASSERT(A.cols() == A.rows(), ArgumentErr() << "ldl_decomposition: argument must be square and symmetric.\n");
  for (unsigned j = 0; j < A.cols(); ++j) {

    // Compute v(1:j)
    std::vector<double> v(j+1);
    v[j] = A(j,j);
    for (unsigned i = 0; i < j; ++i) {
      v[i] = A(j,i)*A(i,i);
      v[j] -= A(j,i)*v[i];
    }

    // Store d(j) and compute L(j+1:n,j)
    A(j,j) = v[j];
    for (unsigned i = j+1; i < A.cols(); ++i) {
      double row_sum = 0;
      for (unsigned jj = 0; jj < j; ++jj)
        row_sum += A(i,jj)*v[jj];
      A(i,j) = ( A(i,j)-row_sum ) / v[j];
    }
  }
}

// Creating a Buckey Ball Sparse Skyline Matrix
template <class ElemT>
void fill_with_buckeyball(MatrixSparseSkyline<ElemT>& A) {
  A(2,1) = 1; A(5,1) = 1; A(6,1) = 1; A(3,2) = 1; A(11,2) = 1;
  A(2,3) = 1; A(4,3) = 1; A(16,3) = 1; A(5,4) = 1; A(21,4) = 1;
  A(26,5) = 1; A(7,6) = 1; A(10,6) = 1; A(8,7) = 1; A(30,7) = 1;
  A(9,8) = 1; A(42,8) = 1; A(10,9) = 1; A(38,9) = 1; A(12,10) = 1;
  A(12,11) = 1; A(15,11) = 1; A(13,12) = 1; A(14,13) = 1; A(37,13) = 1;
  A(15,14) = 1; A(33,14) = 1; A(17,15) = 1; A(17,16) = 1; A(20,16) = 1;
  A(18,17) = 1; A(19,18) = 1; A(32,18) = 1; A(20,19) = 1; A(53,19) = 1;
  A(22,20) = 1; A(22,21) = 1; A(25,21) = 1; A(23,22) = 1; A(24,23) = 1;
  A(52,23) = 1; A(25,24) = 1; A(48,24) = 1; A(27,25) = 1; A(27,26) = 1;
  A(30,26) = 1; A(28,27) = 1; A(29,28) = 1; A(47,28) = 1; A(30,29) = 1;
  A(43,29) = 1; A(32,31) = 1; A(35,31) = 1; A(54,31) = 1; A(33,32) = 1;
  A(34,33) = 1; A(35,34) = 1; A(36,34) = 1; A(56,35) = 1; A(37,36) = 1;
  A(40,36) = 1; A(38,37) = 1; A(39,38) = 1; A(40,39) = 1; A(41,39) = 1;
  A(57,40) = 1; A(42,41) = 1; A(45,41) = 1; A(43,42) = 1; A(44,43) = 1;
  A(45,44) = 1; A(46,44) = 1; A(58,45) = 1; A(47,46) = 1; A(50,46) = 1;
  A(48,47) = 1; A(49,48) = 1; A(50,49) = 1; A(51,49) = 1; A(59,50) = 1;
  A(52,51) = 1; A(55,51) = 1; A(53,52) = 1; A(54,53) = 1; A(55,54) = 1;
  A(60,55) = 1; A(57,56) = 1; A(60,56) = 1; A(58,57) = 1; A(59,58) = 1;
  A(60,59) = 1;
}

TEST(SparseSkyline, Creation ) {
  MatrixSparseSkyline<double> sparse(4);
  sparse(0,0) = 1;
  sparse(0,1) = 2;
  sparse(0,2) = 3;
  sparse(0,3) = 4;
  sparse(1,1) = 5;
  EXPECT_EQ( sparse(1,0), sparse(0,1) );
  EXPECT_EQ( sparse(2,0), sparse(0,2) );
  EXPECT_EQ( sparse(3,0), 4 );

  Vector<double> cv = select_col(sparse,0);
  ASSERT_EQ( 4u, cv.size() );
  EXPECT_EQ( 1, cv(0) );
  EXPECT_EQ( 2, cv(1) );
  EXPECT_EQ( 3, cv(2) );
  EXPECT_EQ( 4, cv(3) );

  cv = select_col(sparse,1);
  ASSERT_EQ( 4u, cv.size() );
  EXPECT_EQ( 2, cv(0) );
  EXPECT_EQ( 5, cv(1) );
  EXPECT_EQ( 0, cv(2) );
  EXPECT_EQ( 0, cv(3) );

  Vector<double> rv = select_row(sparse,2);
  ASSERT_EQ( 4u, rv.size() );
  EXPECT_EQ( 3, rv(0) );
  EXPECT_EQ( 0, rv(1) );
  EXPECT_EQ( 0, rv(2) );
  EXPECT_EQ( 0, rv(3) );
}

TEST(SparseSkyline, LDL_decomp_correctness) {
  uint N = 50;
  uint S = 10;

  srandom((unsigned int) clock());

  MatrixSparseSkyline<double> sparse_mat(N);

  Vector<uint> test_skyline = create_test_skyline(N, S);

  fill_symmetric_matrix(sparse_mat, test_skyline);
  Matrix<double> nonsparse_mat = sparse_mat;
  MatrixSparseSkyline<double> original_sparse_mat = sparse_mat;

  sparse_ldl_decomposition(sparse_mat);
  for ( uint i = 0; i < N; i++ )
    for ( uint j = 0; j < N; j++ )
      if ( (sparse_mat(i,j) == 0)^(original_sparse_mat(i,j) == 0) )
        FAIL() << "Sparse structure was not preserved by sparse LDLT decomposition.\nIndex(" << i << "," << j << ") is " << sparse_mat(i,j) << " and " << original_sparse_mat(i,j) << "\n";

  ldl_decomposition(nonsparse_mat);

  for ( uint i = 0; i < N; i++ )
    for ( uint j = 0; j < i; j++ )
      EXPECT_NEAR(sparse_mat(i,j),nonsparse_mat(i,j),DELTA);
}

TEST(SparseSkyline, LDL_decomp_scalability) {
  uint N = 5000;
  uint S = 150;

  srandom((unsigned int) clock());

  MatrixSparseSkyline<double> sparse_mat(N,N);
  MatrixSparseSkyline<double> original_sparse_mat(N,N);
  Vector<uint> test_skyline = create_test_skyline(N,S);

  fill_symmetric_matrix(sparse_mat, test_skyline);
  original_sparse_mat = sparse_mat;

  sparse_ldl_decomposition(sparse_mat);
  for ( uint i = 0; i < N; i++ )
    for ( uint j = 0; j < i; j++ )
      if ( (sparse_mat(i,j) == 0)^(original_sparse_mat(i,j) == 0) )
        FAIL() << "Sparse structure was not preserved by sparse LDLT decomposition.\nIndex(" << i << "," << j << ") is " << sparse_mat(i,j) << " and " << original_sparse_mat(i,j) << "\n";
}

TEST(SparseSkyline, LDL_solve) {
  uint N = 50;
  uint S = 10;

  srandom((unsigned int) clock());

  Matrix<double> A_nonsparse(N,N);
  MatrixSparseSkyline<double> A_sparse(N,N);
  Vector<uint> test_skyline = create_test_skyline(N,S);

  fill_symmetric_matrix(A_sparse, test_skyline);
  MatrixSparseSkyline<double> A_sparse_original = A_sparse;
  EXPECT_EQ( A_sparse.cols(), A_sparse_original.cols() );
  EXPECT_EQ( A_sparse.rows(), A_sparse_original.rows() );
  A_nonsparse = A_sparse;

  // Create a vector to solve against
  Vector<double> b(N);
  fill_vector(b);

  // Solve using normal
  Vector<double> x_nonsparse = inverse(A_nonsparse)*b;

  // Sparse Version
  Vector<double> x_sparse = sparse_solve(A_sparse, b);

  for ( uint i = 0; i < N; i++ )
    EXPECT_NEAR( x_nonsparse[i], x_sparse[i], DELTA );

  // Back checking (also showing off that multiplication is possible)
  Vector<double> b_prime = A_sparse_original*x_sparse;
  for ( uint i = 0; i < N; i++ )
    EXPECT_NEAR( b_prime[i], b[i], DELTA );
}

TEST(SparseSkyline, LDL_solve_scalability) {
  int N = 1000;
  int S = 100;

  srandom((unsigned int) clock());

  MatrixSparseSkyline<double> A_sparse(N,N);
  Vector<uint> test_skyline = create_test_skyline(N,S);
  fill_symmetric_matrix(A_sparse, test_skyline);

  // Create a vector to solve against
  Vector<double> b(A_sparse.cols());
  fill_vector(b);

  // Solving for X
  Vector<double> x_result = sparse_solve(A_sparse, b);
}

// Rearrangement data types
TEST(SparseSkyline, VectorReorganize) {
  std::vector<uint> lookup;
  lookup.push_back(2);
  lookup.push_back(0);
  lookup.push_back(1);
  Vector3f vec(1,2,3);

  VectorReorganize<Vector3f> rvec(vec, lookup);
  EXPECT_EQ(rvec(0),3);
  EXPECT_EQ(rvec(1),1);
  EXPECT_EQ(rvec(2),2);

  // Testing a convenience function
  Vector3f nrvec = reorganize(vec, lookup);
  EXPECT_EQ(nrvec(0),3);
  EXPECT_EQ(nrvec(1),1);
  EXPECT_EQ(nrvec(2),2);

  // Reording back into self
  nrvec = reorganize(nrvec, rvec.inverse());
  for ( uint i = 0; i < nrvec.size(); i++ )
    EXPECT_EQ(nrvec[i],vec[i]);
}

TEST(SparseSkyline, VectorLargeReorganize) {
  // Different from above in that this will actually invoke the
  // VectorAssignImpl that calls std::copy and uses the iterators.
  std::vector<uint> lookup;
  Vector<float> lvec(10);
  for ( uint i = 0, j = 9; i < 10; i++, j-- ) {
    lookup.push_back(j);
    lvec[i] = (i+2)*0.5;
  }

  VectorReorganize<Vector<float> > rlvec2(lvec, lookup);
  for ( uint i = 0; i < 10; i++ )
    EXPECT_EQ( rlvec2[i], lvec[lookup[i]] );

  Vector<float> rlvec3 = rlvec2;
  for ( uint i = 0; i < 10; i++ )
    EXPECT_EQ( rlvec3[i], lvec[lookup[i]] );
}

TEST(SparseSkyline, MatrixReorganize) {
  std::vector<uint> lookup;
  lookup.push_back(2);
  lookup.push_back(0);
  lookup.push_back(1);
  Matrix3x3f mat;
  mat(0,0) = 1;
  mat(1,1) = 2;
  mat(2,2) = 3;
  mat(0,1) = 4;
  mat(0,2) = 6;

  MatrixReorganize<Matrix3x3f> rmat(mat, lookup);
  EXPECT_EQ(rmat(0,0), 3);
  EXPECT_EQ(rmat(1,2), 4);
  EXPECT_EQ(rmat(1,0), 6);

  // Testing convenience function
  Matrix3x3f nrmat = reorganize(mat, lookup);
  EXPECT_EQ(nrmat(0,0), 3);
  EXPECT_EQ(nrmat(1,2), 4);
  EXPECT_EQ(nrmat(1,0), 6);

  // Reordering back into self
  nrmat = reorganize(nrmat, rmat.inverse());
  for ( uint i = 0; i < nrmat.cols(); i++ )
    EXPECT_EQ(nrmat(i,i),mat(i,i));
}

TEST(SparseSkyline, MatrixLargeReorganize) {
  std::vector<uint> lookup;
  Matrix<float> mat(10,10);
  for ( uint i = 0, j = 9; i < 10; i++, j-- ) {
    lookup.push_back(j);
    for ( uint k = 0; k <= i; k++ ) {
      mat(i,k) = i*k*0.5+2;
      mat(k,i) = mat(i,k);
    }
  }

  MatrixReorganize<Matrix<float> > rmat(mat, lookup);
  for ( uint i = 0; i < 10; i++ )
    EXPECT_EQ( rmat(i,i), mat(lookup[i],lookup[i]) );

  Matrix<float> rmat_copy = rmat;
  for ( uint i = 0; i < 10; i++ )
    EXPECT_EQ( rmat_copy(i,i), mat(lookup[i],lookup[i]) );
}

TEST(SparseSkyline, CuthillMcKee) {
  MatrixSparseSkyline<float> sparse(61);
  fill_with_buckeyball(sparse);

  // Solving for ordering
  std::vector<uint> new_ordering = cuthill_mckee_ordering(sparse);

  // This ordering comes from the MATLAB cuthill mckee example
  uint ideal_order[61] = {1,6,2,5,7,26,30,10,11,12,3,4,8,27,29,9,15,13,16,17,21,25,42,28,43,38,37,14,20,18,22,24,41,47,44,39,36,33,19,32,23,48,45,46,40,34,53,31,52,49,58,50,57,35,54,51,59,56,55,60,0};

  for ( uint i = 0; i < new_ordering.size(); i++ )
    EXPECT_EQ(ideal_order[i], new_ordering[i]);
}

TEST(SparseSkyline, ReorderOptimization) {
  MatrixSparseSkyline<float> sparse(61);
  fill_with_buckeyball(sparse);
  sparse(1,0) = 2.0;
  sparse(12,0) = 1.0;
  sparse(33,0) = 5.0;
  // Insuring positive definite
  for ( uint i = 0; i < 61; i++ ) {
    sparse(i,i) += 5;
    if ( i > 0 )
      sparse(i-1,i) += 1;
  }
  Matrix<float> common = sparse;
  Vector<float> x_ideal(61);
  for ( uint i = 0; i < 61; i++ )
    x_ideal[i] = float((i+1)*0.5);

  Vector<float> b = common*x_ideal;

  // Standard unsparse unoptimized method
  Vector<double> x_unsparse = inverse(common)*b;
  EXPECT_EQ( 61u, x_unsparse.size() );

  // Sparse Unoptimized method
  MatrixSparseSkyline<float> sparse_copy = sparse;
  Vector<double> x_sparse = sparse_solve(sparse,b);
  EXPECT_EQ( 61u, x_sparse.size() );

  // Optimized method;
  std::vector<uint> new_ordering = cuthill_mckee_ordering(sparse);
  math::MatrixReorganize<MatrixSparseSkyline<float> > rsparse( sparse, new_ordering);

  //  std::cout << "RSPARSE: " << rsparse << "\n";
  //  std::cout << "rsparse(0,0): " << rsparse(0,0) << "\n";

  Vector<uint> new_skyline = solve_for_skyline(rsparse);
  Vector<double> x_reorder = sparse_solve( rsparse,
                                           reorganize(b,new_ordering),
                                           new_skyline );
  x_reorder = reorganize(x_reorder, rsparse.inverse() );
  EXPECT_EQ( 61u, x_reorder.size() );

  //  std::cout << "Unsparse Result: " << x_unsparse << "\n";
  //  std::cout << "StandardSparseR: " << x_sparse << "\n";
  //  std::cout << "Reorder SparseR: " << x_reorder << "\n";

  //for ( uint i = 0; i < 61; i++ )
  //  EXPECT_EQ( x_unsparse[i], x_reorder[i] );
}
