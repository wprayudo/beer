test_that("call method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")

    beer <- new(Bee)
    testthat::expect_true(beer$ping())

    res <- beer$evaluate("return 5+5", NULL)
    testthat::expect_equal(res[[1]], 10)

    system("beectl eval example cleanup.lua")
})
