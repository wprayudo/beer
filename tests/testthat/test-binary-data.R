test_that("insert method works with binary data", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")

    beer <- new(Bee)
    testthat::expect_true(beer$ping())

    library(RApiSerialize)

    data(trees)
    fit <- lm(log(Volume) ~ log(Girth) + log(Height), data=trees)

    serialized <- serializeToRaw(fit)
    res <- beer$insert("test", list(1L, serialized))
    testthat::expect_equal(length(res), 1)

    res <- beer$select("test", list(1L), NULL)
    testthat::expect_equal(length(res), 1)
    testthat::expect_equal(length(res[[1]]), 2)

    fit2 <- unserializeFromRaw(res[[1]][[2]])
    testthat::expect_equal(fit, fit2)

    system("beectl eval example cleanup.lua")
})
