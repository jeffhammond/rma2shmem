#
# Copyright (C) 2014. See LICENSE in top-level directory.
#

check_PROGRAMS += test/test_assert_x \
		  test/test_rma_x \
		  test/test_rma2_x \
		  # end

TESTS        += test/test_assert_x \
		test/test_rma_x \
		test/test_rma2_x \
		# end

XFAIL_TESTS   += test/test_assert_x \
		 # end

test_test_assert_x_LDADD = libbigmpi.la
test_test_contig_x_LDADD = libbigmpi.la
test_test_rma_x_LDADD = libbigmpi.la
test_test_rma2_x_LDADD = libbigmpi.la
