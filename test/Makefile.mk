#
# Copyright (C) 2014. See LICENSE in top-level directory.
#

check_PROGRAMS += test/test_assert \
		  test/test_rma \
		  test/test_rma2 \
		  # end

TESTS        += test/test_assert \
		test/test_rma \
		test/test_rma2 \
		# end

XFAIL_TESTS   += test/test_assert \
		 # end

test_test_assert_LDADD = libbigmpi.la
test_test_rma_LDADD = libbigmpi.la
test_test_rma2_LDADD = libbigmpi.la
