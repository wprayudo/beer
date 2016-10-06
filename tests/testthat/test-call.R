test_that("call method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")

    beer <- new(Bee)
    testthat::expect_true(beer$ping())

    res <- beer$call("box.info", NULL)
    testthat::expect_true(is.list(res))
    testthat::expect_gt(length(res), 0)

    res <- beer$call("add_two_numbers", list(1, 2))
    testthat::expect_true(is.list(res))
    testthat::expect_equal(length(res), 1)
    testthat::expect_equal(length(res[[1]]), 1)
    testthat::expect_equal(res[[1]][[1]], 3)

    testthat::expect_error(beer$call("some.unknown.function", NULL))

    system("beectl eval example cleanup.lua")
})
